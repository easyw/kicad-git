/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@verizon.net>
 *
 * Copyright (C) 1992-2020 KiCad Developers, see change_log.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <confirm.h>
#include <pcbnew.h>
#include <pcb_edit_frame.h>
#include <widgets/msgpanel.h>
#include <board.h>
#include <footprint.h>
#include <track.h>
#include <pcb_shape.h>
#include <pad.h>
#include <board_commit.h>
#include <connectivity/connectivity_data.h>
#include <widgets/progress_reporter.h>

#include "ar_autoplacer.h"
#include "ar_matrix.h"
#include <memory>
#include <ratsnest/ratsnest_data.h>

#define AR_GAIN            16
#define AR_KEEPOUT_MARGIN  500
#define AR_ABORT_PLACEMENT -1

#define STEP_AR_MM 1.0

/* Bits characterizing cell */
#define CELL_IS_EMPTY  0x00
#define CELL_IS_HOLE   0x01   /* a conducting hole or obstacle */
#define CELL_IS_MODULE 0x02   /* auto placement occupied by a module */
#define CELL_IS_EDGE   0x20   /* Area and auto-placement: limiting cell contour (Board, Zone) */
#define CELL_IS_FRIEND 0x40   /* Area and auto-placement: cell part of the net */
#define CELL_IS_ZONE   0x80   /* Area and auto-placement: cell available */


/* Penalty (cost) for CntRot90 and CntRot180:
 * CntRot90 and CntRot180 are from 0 (rotation allowed) to 10 (rotation not allowed)
 */
static const double OrientationPenalty[11] =
{
    2.0,        // CntRot = 0 rotation prohibited
    1.9,        // CntRot = 1
    1.8,        // CntRot = 2
    1.7,        // CntRot = 3
    1.6,        // CntRot = 4
    1.5,        // CntRot = 5
    1.4,        // CntRot = 5
    1.3,        // CntRot = 7
    1.2,        // CntRot = 8
    1.1,        // CntRot = 9
    1.0         // CntRot = 10 rotation authorized, no penalty
};


AR_AUTOPLACER::AR_AUTOPLACER( BOARD* aBoard )
{
    m_board = aBoard;
    m_connectivity = std::make_unique<CONNECTIVITY_DATA>( );

    for( MODULE* footprint : m_board->Footprints() )
        m_connectivity->Add( footprint );

    m_gridSize = Millimeter2iu( STEP_AR_MM );
    m_progressReporter = nullptr;
    m_refreshCallback = nullptr;
    m_minCost = 0.0;
}


void AR_AUTOPLACER::placeFootprint( MODULE* aFootprint, bool aDoNotRecreateRatsnest,
                                    const wxPoint& aPos )
{
    if( !aFootprint )
        return;

    aFootprint->SetPosition( aPos );
    m_connectivity->Update( aFootprint );
}


int AR_AUTOPLACER::genPlacementRoutingMatrix()
{
    m_matrix.UnInitRoutingMatrix();

    EDA_RECT bbox = m_board->GetBoardEdgesBoundingBox();

    if( bbox.GetWidth() == 0 || bbox.GetHeight() == 0 )
        return 0;

    // Build the board shape
    m_board->GetBoardPolygonOutlines( m_boardShape /*, aErrorText, aErrorLocation*/ );
    m_topFreeArea = m_boardShape;
    m_bottomFreeArea = m_boardShape;

    m_matrix.ComputeMatrixSize( bbox );
    int nbCells = m_matrix.m_Ncols * m_matrix.m_Nrows;

    // Choose the number of board sides.
    m_matrix.m_RoutingLayersCount = 2;
    m_matrix.InitRoutingMatrix();
    m_matrix.m_routeLayerBottom = B_Cu;
    m_matrix.m_routeLayerTop = F_Cu;

    // Fill (mark) the cells inside the board:
    fillMatrix();

    // Other obstacles can be added here:
    for( auto drawing : m_board->Drawings() )
    {
        switch( drawing->Type() )
        {
        case PCB_SHAPE_T:
            if( drawing->GetLayer() != Edge_Cuts )
            {
                m_matrix.TraceSegmentPcb( (PCB_SHAPE*) drawing, CELL_IS_HOLE | CELL_IS_EDGE,
                                          m_matrix.m_GridRouting, AR_MATRIX::WRITE_CELL );
            }
            break;

        default:
            break;
        }
    }

    // Initialize top layer. to the same value as the bottom layer
    if( m_matrix.m_BoardSide[AR_SIDE_TOP] )
        memcpy( m_matrix.m_BoardSide[AR_SIDE_TOP], m_matrix.m_BoardSide[AR_SIDE_BOTTOM],
                nbCells * sizeof(AR_MATRIX::MATRIX_CELL) );

    return 1;
}


bool AR_AUTOPLACER::fillMatrix()
{
    std::vector <int> x_coordinates;
    bool success = true;
    int step = m_matrix.m_GridRouting;
    wxPoint coord_orgin = m_matrix.GetBrdCoordOrigin(); // Board coordinate of matruix cell (0,0)

    // Create a single board outline:
    SHAPE_POLY_SET brd_shape = m_boardShape;
    brd_shape.Fracture( SHAPE_POLY_SET::PM_FAST );
    const SHAPE_LINE_CHAIN& outline = brd_shape.Outline(0);
    const BOX2I& rect = outline.BBox();

    // Creates the horizontal segments
    // Calculate the y limits of the area
    for( int refy = rect.GetY(), endy = rect.GetBottom(); refy < endy; refy += step )
    {
        // The row index (vertical position) of current line scan inside the placement matrix
        int idy = (refy - coord_orgin.y) / step;

        // Ensure we are still inside the placement matrix
        if( idy >= m_matrix.m_Nrows )
            break;

        // Ensure we are inside the placement matrix
        if( idy <= 0 )
            continue;

        // find all intersection points of an infinite line with polyline sides
        x_coordinates.clear();

        for( int v = 0; v < outline.PointCount(); v++ )
        {

            int seg_startX = outline.CPoint( v ).x;
            int seg_startY = outline.CPoint( v ).y;
            int seg_endX   = outline.CPoint( v + 1 ).x;
            int seg_endY   = outline.CPoint( v + 1 ).y;

            /* Trivial cases: skip if ref above or below the segment to test */
            if( ( seg_startY > refy ) && ( seg_endY > refy ) )
                continue;

            // segment below ref point, or its Y end pos on Y coordinate ref point: skip
            if( ( seg_startY <= refy ) && (seg_endY <= refy ) )
                continue;

            /* at this point refy is between seg_startY and seg_endY
             * see if an horizontal line at Y = refy is intersecting this segment
             */
            // calculate the x position of the intersection of this segment and the
            // infinite line this is more easier if we move the X,Y axis origin to
            // the segment start point:

            seg_endX -= seg_startX;
            seg_endY -= seg_startY;
            double newrefy = (double) ( refy - seg_startY );
            double intersec_x;

            if ( seg_endY == 0 )    // horizontal segment on the same line: skip
                continue;

            // Now calculate the x intersection coordinate of the horizontal line at
            // y = newrefy and the segment from (0,0) to (seg_endX,seg_endY) with the
            // horizontal line at the new refy position the line slope is:
            // slope = seg_endY/seg_endX; and inv_slope = seg_endX/seg_endY
            // and the x pos relative to the new origin is:
            // intersec_x = refy/slope = refy * inv_slope
            // Note: because horizontal segments are already tested and skipped, slope
            // exists (seg_end_y not O)
            double inv_slope = (double) seg_endX / seg_endY;
            intersec_x = newrefy * inv_slope;
            x_coordinates.push_back( (int) intersec_x + seg_startX );
        }

        // A line scan is finished: build list of segments

        // Sort intersection points by increasing x value:
        // So 2 consecutive points are the ends of a segment
        std::sort( x_coordinates.begin(), x_coordinates.end() );

        // An even number of coordinates is expected, because a segment has 2 ends.
        // An if this algorithm always works, it must always find an even count.
        if( ( x_coordinates.size() & 1 ) != 0 )
        {
            success = false;
            break;
        }

        // Fill cells having the same Y coordinate
        int iimax = x_coordinates.size() - 1;

        for( int ii = 0; ii < iimax; ii += 2 )
        {
            int seg_start_x = x_coordinates[ii] - coord_orgin.x;
            int seg_end_x = x_coordinates[ii + 1] - coord_orgin.x;
            // Fill cells at y coord = idy,
            // and at x cood >= seg_start_x and <= seg_end_x

            for( int idx = seg_start_x / step; idx < m_matrix.m_Ncols; idx++ )
            {
                if( idx * step > seg_end_x )
                    break;

                if( idx * step >= seg_start_x )
                    m_matrix.SetCell( idy, idx, AR_SIDE_BOTTOM, CELL_IS_ZONE );
            }

        }
    }   // End examine segments in one area

    return success;
}


void AR_AUTOPLACER::rotateFootprint( MODULE* aFootprint, double angle, bool incremental )
{
    if( aFootprint == NULL )
        return;

    if( incremental )
        aFootprint->SetOrientation( aFootprint->GetOrientation() + angle );
    else
        aFootprint->SetOrientation( angle );


    m_board->GetConnectivity()->Update( aFootprint );
}


void AR_AUTOPLACER::addFpBody( wxPoint aStart, wxPoint aEnd, LSET aLayerMask )
{
    // Add a polygonal shape (rectangle) to m_fpAreaFront and/or m_fpAreaBack
    if( aLayerMask[ F_Cu ] )
    {
        m_fpAreaTop.NewOutline();
        m_fpAreaTop.Append( aStart.x, aStart.y );
        m_fpAreaTop.Append( aEnd.x, aStart.y );
        m_fpAreaTop.Append( aEnd.x, aEnd.y );
        m_fpAreaTop.Append( aStart.x, aEnd.y );
    }
    if( aLayerMask[ B_Cu ] )
    {
        m_fpAreaBottom.NewOutline();
        m_fpAreaBottom.Append( aStart.x, aStart.y );
        m_fpAreaBottom.Append( aEnd.x, aStart.y );
        m_fpAreaBottom.Append( aEnd.x, aEnd.y );
        m_fpAreaBottom.Append( aStart.x, aEnd.y );
    }
}

void AR_AUTOPLACER::addPad( PAD* aPad, int aClearance )
{
    // Add a polygonal shape (rectangle) to m_fpAreaFront and/or m_fpAreaBack
    EDA_RECT bbox = aPad->GetBoundingBox();
    bbox.Inflate( aClearance );

    if( aPad->IsOnLayer( F_Cu ) )
    {
        m_fpAreaTop.NewOutline();
        m_fpAreaTop.Append( bbox.GetLeft(), bbox.GetTop() );
        m_fpAreaTop.Append( bbox.GetRight(), bbox.GetTop() );
        m_fpAreaTop.Append( bbox.GetRight(), bbox.GetBottom() );
        m_fpAreaTop.Append( bbox.GetLeft(), bbox.GetBottom() );
    }
    if( aPad->IsOnLayer( B_Cu ) )
    {
        m_fpAreaBottom.NewOutline();
        m_fpAreaBottom.Append( bbox.GetLeft(), bbox.GetTop() );
        m_fpAreaBottom.Append( bbox.GetRight(), bbox.GetTop() );
        m_fpAreaBottom.Append( bbox.GetRight(), bbox.GetBottom() );
        m_fpAreaBottom.Append( bbox.GetLeft(), bbox.GetBottom() );
    }
}


void AR_AUTOPLACER::buildFpAreas( MODULE* aFootprint, int aFpClearance )
{
    m_fpAreaTop.RemoveAllContours();
    m_fpAreaBottom.RemoveAllContours();

    aFootprint->BuildPolyCourtyards();
    m_fpAreaTop = aFootprint->GetPolyCourtyardFront();
    m_fpAreaBottom = aFootprint->GetPolyCourtyardBack();

    LSET layerMask;

    if( aFootprint->GetLayer() == F_Cu )
        layerMask.set( F_Cu );

    if( aFootprint->GetLayer() == B_Cu )
        layerMask.set( B_Cu );

    EDA_RECT fpBBox = aFootprint->GetBoundingBox();

    fpBBox.Inflate( ( m_matrix.m_GridRouting / 2 ) + aFpClearance );

    // Add a minimal area to the fp area:
    addFpBody( fpBBox.GetOrigin(), fpBBox.GetEnd(), layerMask );

    // Trace pads + clearance areas.
    for( PAD* pad : aFootprint->Pads() )
    {
        int margin = (m_matrix.m_GridRouting / 2) + pad->GetOwnClearance( pad->GetLayer() );
        addPad( pad, margin );
    }
}


void AR_AUTOPLACER::genModuleOnRoutingMatrix( MODULE* Module )
{
    int         ox, oy, fx, fy;
    LSET        layerMask;
    EDA_RECT    fpBBox = Module->GetBoundingBox();

    fpBBox.Inflate( m_matrix.m_GridRouting / 2 );
    ox  = fpBBox.GetX();
    fx  = fpBBox.GetRight();
    oy  = fpBBox.GetY();
    fy  = fpBBox.GetBottom();

    if( ox < m_matrix.m_BrdBox.GetX() )
        ox = m_matrix.m_BrdBox.GetX();

    if( ox > m_matrix.m_BrdBox.GetRight() )
        ox = m_matrix.m_BrdBox.GetRight();

    if( fx < m_matrix.m_BrdBox.GetX() )
        fx = m_matrix.m_BrdBox.GetX();

    if( fx > m_matrix.m_BrdBox.GetRight() )
        fx = m_matrix.m_BrdBox.GetRight();

    if( oy < m_matrix.m_BrdBox.GetY() )
        oy = m_matrix.m_BrdBox.GetY();

    if( oy > m_matrix.m_BrdBox.GetBottom() )
        oy = m_matrix.m_BrdBox.GetBottom();

    if( fy < m_matrix.m_BrdBox.GetY() )
        fy = m_matrix.m_BrdBox.GetY();

    if( fy > m_matrix.m_BrdBox.GetBottom() )
        fy = m_matrix.m_BrdBox.GetBottom();

    if( Module->GetLayer() == F_Cu )
        layerMask.set( F_Cu );

    if( Module->GetLayer() == B_Cu )
        layerMask.set( B_Cu );

    m_matrix.TraceFilledRectangle( ox, oy, fx, fy, layerMask,
                          CELL_IS_MODULE, AR_MATRIX::WRITE_OR_CELL );

    // Trace pads + clearance areas.
    for( PAD* pad : Module->Pads() )
    {
        int margin = (m_matrix.m_GridRouting / 2) + pad->GetOwnClearance( pad->GetLayer() );
        m_matrix.PlacePad( pad, CELL_IS_MODULE, margin, AR_MATRIX::WRITE_OR_CELL );
    }

    // Trace clearance.
    int margin = ( m_matrix.m_GridRouting * Module->GetPadCount() ) / AR_GAIN;
    m_matrix.CreateKeepOutRectangle( ox, oy, fx, fy, margin, AR_KEEPOUT_MARGIN , layerMask );

    // Build the footprint courtyard
    buildFpAreas( Module, margin );

    // Substract the shape to free areas
    m_topFreeArea.BooleanSubtract( m_fpAreaTop, SHAPE_POLY_SET::PM_FAST );
    m_bottomFreeArea.BooleanSubtract( m_fpAreaBottom, SHAPE_POLY_SET::PM_FAST );
}


/* Test if the rectangular area (ux, ux .. y0, y1):
 * - is a free zone (except OCCUPED_By_MODULE returns)
 * - is on the working surface of the board (otherwise returns OUT_OF_BOARD)
 *
 * Returns OUT_OF_BOARD, or OCCUPED_By_MODULE or FREE_CELL if OK
 */
int AR_AUTOPLACER::testRectangle( const EDA_RECT& aRect, int side )
{
    EDA_RECT rect = aRect;

    rect.Inflate( m_matrix.m_GridRouting / 2 );

    wxPoint start   = rect.GetOrigin();
    wxPoint end     = rect.GetEnd();

    start   -= m_matrix.m_BrdBox.GetOrigin();
    end     -= m_matrix.m_BrdBox.GetOrigin();

    int row_min = start.y / m_matrix.m_GridRouting;
    int row_max = end.y / m_matrix.m_GridRouting;
    int col_min = start.x / m_matrix.m_GridRouting;
    int col_max = end.x / m_matrix.m_GridRouting;

    if( start.y > row_min * m_matrix.m_GridRouting )
        row_min++;

    if( start.x > col_min * m_matrix.m_GridRouting )
        col_min++;

    if( row_min < 0 )
        row_min = 0;

    if( row_max >= ( m_matrix.m_Nrows - 1 ) )
        row_max = m_matrix.m_Nrows - 1;

    if( col_min < 0 )
        col_min = 0;

    if( col_max >= ( m_matrix.m_Ncols - 1 ) )
        col_max = m_matrix.m_Ncols - 1;

    for( int row = row_min; row <= row_max; row++ )
    {
        for( int col = col_min; col <= col_max; col++ )
        {
            unsigned int data = m_matrix.GetCell( row, col, side );

            if( ( data & CELL_IS_ZONE ) == 0 )
                return AR_OUT_OF_BOARD;

            if( (data & CELL_IS_MODULE) )
                return AR_OCCUIPED_BY_MODULE;
        }
    }

    return AR_FREE_CELL;
}


/* Calculates and returns the clearance area of the rectangular surface
 * aRect):
 * (Sum of cells in terms of distance)
 */
unsigned int AR_AUTOPLACER::calculateKeepOutArea( const EDA_RECT& aRect, int side )
{
    wxPoint start   = aRect.GetOrigin();
    wxPoint end     = aRect.GetEnd();

    start   -= m_matrix.m_BrdBox.GetOrigin();
    end     -= m_matrix.m_BrdBox.GetOrigin();

    int row_min = start.y / m_matrix.m_GridRouting;
    int row_max = end.y / m_matrix.m_GridRouting;
    int col_min = start.x / m_matrix.m_GridRouting;
    int col_max = end.x / m_matrix.m_GridRouting;

    if( start.y > row_min * m_matrix.m_GridRouting )
        row_min++;

    if( start.x > col_min * m_matrix.m_GridRouting )
        col_min++;

    if( row_min < 0 )
        row_min = 0;

    if( row_max >= ( m_matrix.m_Nrows - 1 ) )
        row_max = m_matrix.m_Nrows - 1;

    if( col_min < 0 )
        col_min = 0;

    if( col_max >= ( m_matrix.m_Ncols - 1 ) )
        col_max = m_matrix.m_Ncols - 1;

    unsigned int keepOutCost = 0;

    for( int row = row_min; row <= row_max; row++ )
    {
        for( int col = col_min; col <= col_max; col++ )
        {
            // m_matrix.GetDist returns the "cost" of the cell
            // at position (row, col)
            // in autoplace this is the cost of the cell, if it is
            // inside aRect
            keepOutCost += m_matrix.GetDist( row, col, side );
        }
    }

    return keepOutCost;
}


/* Test if the module can be placed on the board.
 * Returns the value TstRectangle().
 * Module is known by its bounding box
 */
int AR_AUTOPLACER::testFootprintOnBoard( MODULE* aFootprint, bool TstOtherSide,
                                         const wxPoint& aOffset )
{
    int side = AR_SIDE_TOP;
    int otherside = AR_SIDE_BOTTOM;

    if( aFootprint->GetLayer() == B_Cu )
    {
        side = AR_SIDE_BOTTOM; otherside = AR_SIDE_TOP;
    }

    EDA_RECT    fpBBox = aFootprint->GetFootprintRect();
    fpBBox.Move( -aOffset );

    buildFpAreas( aFootprint, 0 );

    int diag = //testModuleByPolygon( aFootprint, side, aOffset );
        testRectangle( fpBBox, side );

    if( diag != AR_FREE_CELL )
        return diag;

    if( TstOtherSide )
    {
        diag = //testModuleByPolygon( aFootprint, otherside, aOffset );
                testRectangle( fpBBox, otherside );

        if( diag != AR_FREE_CELL )
            return diag;
    }

    int marge = ( m_matrix.m_GridRouting * aFootprint->GetPadCount() ) / AR_GAIN;

    fpBBox.Inflate( marge );
    return calculateKeepOutArea( fpBBox, side );
}


int AR_AUTOPLACER::getOptimalFPPlacement( MODULE* aFootprint )
{
    int     error = 1;
    wxPoint lastPosOK;
    double  min_cost, curr_cost, Score;
    bool    testOtherSide;

    aFootprint->CalculateBoundingBox();

    lastPosOK = m_matrix.m_BrdBox.GetOrigin();

    wxPoint  fpPos = aFootprint->GetPosition();
    EDA_RECT fpBBox  = aFootprint->GetFootprintRect();

    // Move fpBBox to have the footprint position at (0,0)
    fpBBox.Move( -fpPos );
    wxPoint fpBBoxOrg = fpBBox.GetOrigin();

    // Calculate the limit of the footprint position, relative to the routing matrix area
    wxPoint xylimit = m_matrix.m_BrdBox.GetEnd() - fpBBox.GetEnd();

    wxPoint initialPos = m_matrix.m_BrdBox.GetOrigin() - fpBBoxOrg;

    // Stay on grid.
    initialPos.x    -= initialPos.x % m_matrix.m_GridRouting;
    initialPos.y    -= initialPos.y % m_matrix.m_GridRouting;

    m_curPosition = initialPos;
    wxPoint fpOffset = fpPos - m_curPosition;

    // Examine pads, and set testOtherSide to true if a footprint has at least 1 pad through.
    testOtherSide = false;

    if( m_matrix.m_RoutingLayersCount > 1 )
    {
        LSET other( aFootprint->GetLayer() == B_Cu ? F_Cu : B_Cu );

        for( PAD* pad : aFootprint->Pads() )
        {
            if( !( pad->GetLayerSet() & other ).any() )
                continue;

            testOtherSide = true;
            break;
        }
    }

    fpBBox.SetOrigin( fpBBoxOrg + m_curPosition );

    min_cost = -1.0;
//    m_frame->SetStatusText( wxT( "Score ??, pos ??" ) );


    for( ; m_curPosition.x < xylimit.x; m_curPosition.x += m_matrix.m_GridRouting )
    {
        m_curPosition.y = initialPos.y;

        for( ; m_curPosition.y < xylimit.y; m_curPosition.y += m_matrix.m_GridRouting )
        {

            fpBBox.SetOrigin( fpBBoxOrg + m_curPosition );
            fpOffset = fpPos - m_curPosition;
            int keepOutCost = testFootprintOnBoard( aFootprint, testOtherSide, fpOffset );

            if( keepOutCost >= 0 )    // i.e. if the footprint can be put here
            {
                error = 0;
                // m_frame->build_ratsnest_module( aFootprint ); // fixme
                curr_cost   = computePlacementRatsnestCost( aFootprint, fpOffset );
                Score       = curr_cost + keepOutCost;

                if( (min_cost >= Score ) || (min_cost < 0 ) )
                {
                    lastPosOK   = m_curPosition;
                    min_cost    = Score;
                    wxString msg;
/*                    msg.Printf( wxT( "Score %g, pos %s, %s" ),
                                min_cost,
                                GetChars( ::CoordinateToString( LastPosOK.x ) ),
                                GetChars( ::CoordinateToString( LastPosOK.y ) ) );
                    m_frame->SetStatusText( msg );*/
                }
            }
        }
    }

    // Regeneration of the modified variable.
    m_curPosition = lastPosOK;

    m_minCost = min_cost;
    return error;
}


const PAD* AR_AUTOPLACER::nearestPad( MODULE *aRefFP, PAD* aRefPad, const wxPoint& aOffset)
{
    const PAD* nearest = nullptr;
    int64_t    nearestDist = INT64_MAX;

    for ( MODULE* footprint : m_board->Footprints() )
    {
        if ( footprint == aRefFP )
            continue;

        if( !m_matrix.m_BrdBox.Contains( footprint->GetPosition() ) )
            continue;

        for( PAD* pad: footprint->Pads() )
        {
            if( pad->GetNetCode() != aRefPad->GetNetCode() || pad->GetNetCode() <= 0 )
                continue;

            auto dist = (VECTOR2I( aRefPad->GetPosition() - aOffset ) - VECTOR2I( pad->GetPosition() ) ).EuclideanNorm();

            if ( dist < nearestDist )
            {
                nearestDist = dist;
                nearest = pad;
            }
        }
    }

    return nearest;
}


double AR_AUTOPLACER::computePlacementRatsnestCost( MODULE *aFootprint, const wxPoint& aOffset )
{
    double  curr_cost;
    VECTOR2I start;      // start point of a ratsnest
    VECTOR2I end;        // end point of a ratsnest
    int     dx, dy;

    curr_cost = 0;

    for ( PAD* pad : aFootprint->Pads() )
    {
        const PAD* nearest = nearestPad( aFootprint, pad, aOffset );

        if( !nearest )
            continue;

        start   = VECTOR2I( pad->GetPosition() ) - VECTOR2I(aOffset);
        end     = VECTOR2I( nearest->GetPosition() );

        //m_overlay->SetIsStroke( true );
        //m_overlay->SetStrokeColor( COLOR4D(0.0, 1.0, 0.0, 1.0) );
        //m_overlay->Line( start, end );

        // Cost of the ratsnest.
        dx  = end.x - start.x;
        dy  = end.y - start.y;

        dx  = abs( dx );
        dy  = abs( dy );

        // ttry to have always dx >= dy to calculate the cost of the rastsnet
        if( dx < dy )
            std::swap( dx, dy );

        // Cost of the connection = length + penalty due to the slope
        // dx is the biggest length relative to the X or Y axis
        // the penalty is max for 45 degrees ratsnests,
        // and 0 for horizontal or vertical ratsnests.
        // For Horizontal and Vertical ratsnests, dy = 0;
        double conn_cost = hypot( dx, dy * 2.0 );
        curr_cost += conn_cost;    // Total cost = sum of costs of each connection
    }

    return curr_cost;
}


// Sort routines
static bool sortFootprintsByComplexity( MODULE* ref, MODULE* compare )
{
    double ff1, ff2;

    ff1 = ref->GetArea() * ref->GetPadCount();
    ff2 = compare->GetArea() * compare->GetPadCount();

    return ff2 < ff1;
}


static bool sortFootprintsByRatsnestSize( MODULE* ref, MODULE* compare )
{
    double ff1, ff2;

    ff1 = ref->GetArea() * ref->GetFlag();
    ff2 = compare->GetArea() * compare->GetFlag();
    return ff2 < ff1;
}


MODULE* AR_AUTOPLACER::pickFootprint( )
{
    std::vector<MODULE*> fpList;


    for( MODULE* footprint : m_board->Footprints() )
    {
        footprint->CalculateBoundingBox();
        fpList.push_back( footprint );
    }

    sort( fpList.begin(), fpList.end(), sortFootprintsByComplexity );

    for( unsigned kk = 0; kk < fpList.size(); kk++ )
    {
        MODULE* footprint = fpList[kk];
        footprint->SetFlag( 0 );

        if( !footprint->NeedsPlaced() )
            continue;

        m_connectivity->Update( footprint );
    }

    m_connectivity->RecalculateRatsnest();

    for( unsigned kk = 0; kk < fpList.size(); kk++ )
    {
        MODULE* footprint = fpList[kk];

        auto edges = m_connectivity->GetRatsnestForComponent( footprint, true );

        footprint->SetFlag( edges.size() ) ;
    }

    sort( fpList.begin(), fpList.end(), sortFootprintsByRatsnestSize );

    // Search for "best" module.
    MODULE* bestModule  = nullptr;
    MODULE* altModule   = nullptr;

    for( unsigned ii = 0; ii < fpList.size(); ii++ )
    {
        MODULE* footprint = fpList[ii];

        if( !footprint->NeedsPlaced() )
            continue;

        altModule = footprint;

        if( footprint->GetFlag() == 0 )
            continue;

        bestModule = footprint;
        break;
    }

    if( bestModule )
        return bestModule;
    else
        return altModule;
}


void AR_AUTOPLACER::drawPlacementRoutingMatrix( )
{
    // Draw the board free area
    m_overlay->Clear();
    m_overlay->SetIsFill( true );
    m_overlay->SetIsStroke( false );

    SHAPE_POLY_SET freeArea = m_topFreeArea;
    freeArea.Fracture( SHAPE_POLY_SET::PM_FAST );

    // Draw the free polygon areas, top side:
    if( freeArea.OutlineCount() > 0 )
    {
        m_overlay->SetIsFill( true );
        m_overlay->SetIsStroke( false );
        m_overlay->SetFillColor( COLOR4D(0.7, 0.0, 0.1, 0.2) );
        m_overlay->Polygon( freeArea );
    }

    freeArea = m_bottomFreeArea;
    freeArea.Fracture( SHAPE_POLY_SET::PM_FAST );

    // Draw the free polygon areas, bottom side:
    if( freeArea.OutlineCount() > 0 )
    {
        m_overlay->SetFillColor( COLOR4D(0.0, 0.7, 0.0, 0.2) );
        m_overlay->Polygon( freeArea );
    }
}


AR_RESULT AR_AUTOPLACER::AutoplaceFootprints( std::vector<MODULE*>& aFootprints,
                                              BOARD_COMMIT* aCommit,
                                              bool aPlaceOffboardModules )
{
    wxPoint memopos;
    int     error;
    bool    cancelled = false;

    memopos = m_curPosition;

    m_matrix.m_GridRouting = m_gridSize; //(int) m_frame->GetScreen()->GetGridSize().x;

    // Ensure Board.m_GridRouting has a reasonable value:
    if( m_matrix.m_GridRouting < Millimeter2iu( 0.25 ) )
        m_matrix.m_GridRouting = Millimeter2iu( 0.25 );

    // Compute module parameters used in auto place
    if( genPlacementRoutingMatrix( ) == 0 )
        return AR_FAILURE;

    int placedCount = 0;

    for( MODULE* footprint : m_board->Footprints() )
        footprint->SetNeedsPlaced( false );

    std::vector<MODULE*> offboardMods;

    if( aPlaceOffboardModules )
    {
        for( MODULE* footprint : m_board->Footprints() )
        {
            if( !m_matrix.m_BrdBox.Contains( footprint->GetPosition() ) )
                offboardMods.push_back( footprint );
        }
    }

    for( MODULE* footprint : aFootprints )
    {
        footprint->SetNeedsPlaced( true );
        aCommit->Modify( footprint );
    }

    for( MODULE* footprint : offboardMods )
    {
        footprint->SetNeedsPlaced( true );
        aCommit->Modify( footprint );
    }

    for( MODULE* footprint : m_board->Footprints() )
    {
        if( footprint->NeedsPlaced() )    // Erase from screen
            placedCount++;
        else
            genModuleOnRoutingMatrix( footprint );
    }


    int         cnt = 0;
    wxString    msg;

    if( m_progressReporter )
    {
        m_progressReporter->Report( _( "Autoplacing components..." ) );
        m_progressReporter->SetMaxProgress( placedCount );
    }

    drawPlacementRoutingMatrix();

    if( m_refreshCallback )
        m_refreshCallback( nullptr );

    MODULE* footprint;

    while( ( footprint = pickFootprint() ) != nullptr )
    {
        // Display some info about activity, footprint placement can take a while:
        //m_frame->SetStatusText( msg );

        if( m_progressReporter )
            m_progressReporter->SetTitle( wxString::Format(
                    _( "Autoplacing %s" ), footprint->GetReference() ) );

        double initialOrient = footprint->GetOrientation();

        error = getOptimalFPPlacement( footprint );
        double bestScore = m_minCost;
        double bestRotation = 0.0;
        int rotAllowed;

        if( error == AR_ABORT_PLACEMENT )
            goto end_of_tst;

        // Try orientations 90, 180, 270 degrees from initial orientation
        rotAllowed = footprint->GetPlacementCost180();

        if( rotAllowed != 0 )
        {
            rotateFootprint( footprint, 1800.0, true );
            error   = getOptimalFPPlacement( footprint );
            m_minCost *= OrientationPenalty[rotAllowed];

            if( bestScore > m_minCost )    // This orientation is better.
            {
                bestScore   = m_minCost;
                bestRotation = 1800.0;
            }
            else
            {
                rotateFootprint( footprint, initialOrient, false );
            }

            if( error == AR_ABORT_PLACEMENT )
                goto end_of_tst;
        }

        // Determine if the best orientation of a footprint is 90.
        rotAllowed = footprint->GetPlacementCost90();

        if( rotAllowed != 0 )
        {
            rotateFootprint( footprint, 900.0, true );
            error   = getOptimalFPPlacement( footprint );
            m_minCost *= OrientationPenalty[rotAllowed];

            if( bestScore > m_minCost )    // This orientation is better.
            {
                bestScore   = m_minCost;
                bestRotation = 900.0;
            }
            else
            {
                rotateFootprint( footprint, initialOrient, false );
            }

            if( error == AR_ABORT_PLACEMENT )
                goto end_of_tst;
        }

        // Determine if the best orientation of a footprint is -90.
        if( rotAllowed != 0 )
        {
            rotateFootprint( footprint, 2700.0, true );
            error = getOptimalFPPlacement( footprint );
            m_minCost *= OrientationPenalty[rotAllowed];

            if( bestScore > m_minCost )    // This orientation is better.
            {
                bestScore   = m_minCost;
                bestRotation = 2700.0;
            }
            else
            {
                rotateFootprint( footprint, initialOrient, false );
            }

            if( error == AR_ABORT_PLACEMENT )
                goto end_of_tst;
        }

end_of_tst:

        if( error == AR_ABORT_PLACEMENT )
            break;


        bestRotation += initialOrient;

        if( bestRotation != footprint->GetOrientation() )
        {
            rotateFootprint( footprint, bestRotation, false );
        }

        // Place footprint.
        placeFootprint( footprint, true, m_curPosition );

        footprint->CalculateBoundingBox();
        genModuleOnRoutingMatrix( footprint );
        footprint->SetIsPlaced( true );
        footprint->SetNeedsPlaced( false );
        drawPlacementRoutingMatrix();

        if( m_refreshCallback )
            m_refreshCallback( footprint );

        if( m_progressReporter )
        {
            m_progressReporter->AdvanceProgress();

            if ( !m_progressReporter->KeepRefreshing( false ) )
            {
                cancelled = true;
                break;
            }
        }
        cnt++;
    }

    m_curPosition = memopos;

    m_matrix.UnInitRoutingMatrix();

    for( MODULE* fp : m_board->Footprints() )
        fp->CalculateBoundingBox();

    return cancelled ? AR_CANCELLED : AR_COMPLETED;
}
