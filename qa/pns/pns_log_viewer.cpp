/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 KiCad Developers.
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


// WARNING - this Tom's crappy PNS hack tool code. Please don't complain about its quality
// (unless you want to improve it).

#include <wx/clipbrd.h>

#include <qa_utils/utility_registry.h>
#include <pgm_base.h>

#include <profile.h>

#include <view/view_overlay.h>

#include "pns_log.h"
#include "router/pns_diff_pair.h"
#include "router/pns_utils.h"

#include "pns_log_viewer_frame.h"

#include "qa/drc_proto/drc_proto.h"

#define TOM_EXTRA_DEBUG

class LABEL_MANAGER
{
public:
    struct LABEL
    {
        COLOR4D     m_color;
        std::string m_msg;
        VECTOR2I    m_target;
        BOX2I    m_bbox;
    };

    LABEL_MANAGER( KIGFX::GAL* aGal ) :
        m_gal( aGal ) {};


    void Add( VECTOR2I target, std::string msg, COLOR4D color )
    {
        LABEL lbl;

        lbl.m_target = target;
        lbl.m_msg = msg;
        lbl.m_color = color;
        m_gal->SetGlyphSize( VECTOR2D(m_textSize, m_textSize) );

        VECTOR2I textDims = m_gal->GetTextLineSize( msg );

        lbl.m_bbox.SetOrigin( lbl.m_target - textDims - VECTOR2I( m_textSize, m_textSize ) );
        lbl.m_bbox.SetSize( textDims );
        m_labels.push_back( lbl );
    }

    void Add( const SHAPE_LINE_CHAIN& aL, COLOR4D color )
    {
        for( int i = 0; i < aL.PointCount(); i++ )
        {
            char msg[1024];
            snprintf(msg, sizeof(msg), "%d", i );
            Add( aL.CPoint(i), msg, color );
        }
    }

    void Redraw( std::shared_ptr<KIGFX::VIEW_OVERLAY> aOvl )
    {
        recalculate();
        for ( auto & lbl : m_labels )
        {
            //printf("Draw lbl %d %d '%s'\n", lbl.m_bbox.GetOrigin().x, lbl.m_bbox.GetOrigin().y, lbl.m_msg.c_str() );
            aOvl->SetIsFill( false );
            aOvl->SetIsStroke( true );
            aOvl->SetLineWidth( 10000 );
            aOvl->SetStrokeColor( lbl.m_color.Brighten( 0.7 ) );
            aOvl->Rectangle( lbl.m_bbox.GetOrigin(), lbl.m_bbox.GetEnd() );
            aOvl->BitmapText( lbl.m_msg, lbl.m_bbox.Centre(), 0.0 );
            VECTOR2I nearest = nearestBoxCorner( lbl.m_bbox, lbl.m_target );
            aOvl->Line( lbl.m_target, nearest );
        }
    }

private:

    VECTOR2I nearestBoxCorner( BOX2I b, VECTOR2I p )
    {
        VECTOR2I ptest[4] =
        {
            b.GetPosition(),
            b.GetPosition() + VECTOR2I( b.GetWidth(), 0 ),
            b.GetPosition() + VECTOR2I( b.GetWidth(), b.GetHeight() ),
            b.GetPosition() + VECTOR2I( 0, b.GetHeight() )
        };

        int bestDist = INT_MAX;
        VECTOR2I rv;

        for(int i = 0; i< 4;i++)
        {
            int dist = ( ptest[i] - p ).EuclideanNorm();
            if(dist < bestDist)
            {
                bestDist = dist;
                rv = ptest[i];
            }
        }

        return rv;
    }

    VECTOR2I boxMtv( BOX2I b1, BOX2I b2 )
    {
        VECTOR2I rv(0, 0);

        b1.Normalize();
        b2.Normalize();

        if( !b1.Intersects(b2) )
            return rv;

        int bestDist = INT_MAX;

        VECTOR2I p[4] =
        {
            b2.GetPosition(),
            b2.GetPosition() + VECTOR2I( b2.GetWidth(), 0 ),
            b2.GetPosition() + VECTOR2I( b2.GetWidth(), b2.GetHeight() ),
            b2.GetPosition() + VECTOR2I( 0, b2.GetHeight() )
        };

        for (int i = 0; i < 4 ; i++ )
        {
            if ( b1.Contains( p[i] ))
            {
             //   printf("CONT %d\n", i );
                VECTOR2I dp[4] = 
                {
                    VECTOR2I( b1.GetEnd().x - p[i].x + 1, 0 ),
                    VECTOR2I( b1.GetPosition().x - p[i].x - 1, 0 ),
                    VECTOR2I( 0, b1.GetEnd().y - p[i].y + 1  ),
                    VECTOR2I( 0, b1.GetPosition().y - p[i].y - 1 )
                };

                for(int j = 0; j < 4; j++ )
                {
                    BOX2I btest(b2);
                    btest.Move( dp[j] );
                    if( !b1.Intersects(btest) )
                    {
                        int dist = dp[j].EuclideanNorm();
                        if( dist < bestDist )
                        {
                            bestDist = dist;
                            rv = dp[j];
                        }
                    }
                }
            }
        }

        return rv;
    }

    void recalculate()
    {
        int iterLimit = 5;
        while ( iterLimit > 0 )
        {
            printf("Iter %d\n", iterLimit );
            bool collisionsFound = false;
            for(int i = 0; i < m_labels.size(); i++ )
            {
                for(int j = 0; j < m_labels.size(); j++ )
                {
                    if (i == j )
                        continue;


                    auto bb_i = m_labels[i].m_bbox;
                    auto bb_j = m_labels[j].m_bbox;

                    bb_i.Inflate(100000);
                    bb_j.Inflate(100000);
                    VECTOR2I mtv = boxMtv(bb_i, bb_j);


                    if( mtv.x || mtv.y )
                    {
//                        printf("%d %d mtv %d %d\n", i, j, mtv.x, mtv.y );

                        m_labels[i].m_bbox.Move( -mtv );
                        collisionsFound = true;
                    }
                }
            }

            if( ! collisionsFound )
                break;

            iterLimit--;
        }
    }

    KIGFX::GAL* m_gal;
    int m_textSize = 100000;
    std::vector<LABEL> m_labels;
};




class WX_SHAPE_TREE_ITEM_DATA : public wxClientData
{
public:
    WX_SHAPE_TREE_ITEM_DATA( PNS_TEST_DEBUG_DECORATOR::DEBUG_ENT* item ) : m_item( item ){};

    PNS_TEST_DEBUG_DECORATOR::DEBUG_ENT* m_item;
};

void PNS_LOG_VIEWER_FRAME::createUserTools()
{

}



PNS_TEST_DEBUG_DECORATOR::STAGE* PNS_LOG_VIEWER_FRAME::getCurrentStage()
{
    PNS_TEST_DEBUG_DECORATOR* dbgd = m_env->GetDebugDecorator();
    int                       count = dbgd->GetStageCount();

    int iter = m_rewindIter;

    if( count <= 0 )
        return nullptr;

    if( iter < 0 )
        iter = 0;

    if( iter >= count )
        iter = count - 1;

    return dbgd->GetStage( iter );
}

void PNS_LOG_VIEWER_FRAME::drawLoggedItems( int iter )
{
    if( !m_env )
        return;

    m_overlay = m_galPanel->DebugOverlay();
    m_overlay->Clear();

    PNS_TEST_DEBUG_DECORATOR::STAGE* st = getCurrentStage();

    if( !st )
        return;

    LABEL_MANAGER labelMgr( m_galPanel->GetGAL() );

    auto drawShapes = [&]( PNS_TEST_DEBUG_DECORATOR::DEBUG_ENT* ent ) -> bool {
        bool isEnabled = ent->IsVisible();
        bool isSelected = false;
        if( !isEnabled )
            return true;

        for( auto& sh : ent->m_shapes )
        {
            COLOR4D color = ent->m_color;
            int lineWidth = ent->m_width;

            m_overlay->SetIsStroke( true );
            m_overlay->SetIsFill( false );

            if( isSelected )
            {
                color = COLOR4D( 1.0, 1.0, 1.0, 1.0 );
                lineWidth *= 2;
            }


            m_overlay->SetStrokeColor( color );
            m_overlay->SetLineWidth( ent->m_width );

            switch( sh->Type() )
            {
            case SH_CIRCLE:
            {
                auto cir = static_cast<SHAPE_CIRCLE*>( sh );
                m_overlay->Circle( cir->GetCenter(), cir->GetRadius() );

                break;
            }
            case SH_RECT:
            {
                auto rect = static_cast<SHAPE_RECT*>( sh );
                m_overlay->Rectangle( rect->GetPosition(), rect->GetPosition() + rect->GetSize() );

                break;
            }
            case SH_LINE_CHAIN:
            {
                auto lc = static_cast<SHAPE_LINE_CHAIN*>( sh );

                for( int i = 0; i < lc->SegmentCount(); i++ )
                {
                    auto s = lc->CSegment( i );
                    m_overlay->Line( s.A, s.B );
                }

                if( ent->m_hasLabels)
                    labelMgr.Add( *lc, color );

                break;

            }
            default: break;
            }
        }


        return true;
    };

    st->m_entries->IterateTree( drawShapes );

    labelMgr.Redraw( m_overlay );

    m_galPanel->GetView()->MarkDirty();
    m_galPanel->GetParent()->Refresh();
}


static BOARD* loadBoard( const std::string& filename )
{
    PLUGIN::RELEASER pi( new PCB_IO );
    BOARD*           brd = nullptr;

    try
    {
        brd = pi->Load( wxString( filename.c_str() ), NULL, NULL );
    }
    catch( const IO_ERROR& ioe )
    {
        wxString msg = wxString::Format( _( "Error loading board.\n%s" ),
                ioe.Problem() );

        printf( "Board Loading Error: '%s'\n", (const char*) msg.mb_str() );
        return nullptr;
    }

    return brd;
}


void PNS_LOG_VIEWER_FRAME::SetLogFile( PNS_LOG_FILE* aLog )
{
    m_logFile.reset( aLog );

    SetBoard( m_logFile->GetBoard() );

    m_env.reset( new PNS_TEST_ENVIRONMENT );

    m_env->SetMode( PNS::PNS_MODE_ROUTE_SINGLE );
    m_env->ReplayLog( m_logFile.get() );

    auto dbgd = m_env->GetDebugDecorator();
    int  n_stages = dbgd->GetStageCount();
    m_rewindSlider->SetMax( n_stages - 1 );
    m_rewindSlider->SetValue( n_stages - 1 );
    m_rewindIter = n_stages - 1;

    auto extents = m_board->GetBoundingBox();


    BOX2D bbd;
    bbd.SetOrigin( extents.GetOrigin() );
    bbd.SetWidth( extents.GetWidth() );
    bbd.SetHeight( extents.GetHeight() );
    bbd.Inflate( std::min( bbd.GetWidth(), bbd.GetHeight() ) / 5 );

    m_galPanel->GetView()->SetViewport( bbd );

    drawLoggedItems( m_rewindIter );
    updateDumpPanel( m_rewindIter );
}

void PNS_LOG_VIEWER_FRAME::onReload( wxCommandEvent& event )
{
    event.Skip();
}

void PNS_LOG_VIEWER_FRAME::onExit( wxCommandEvent& event )
{
    event.Skip();
}

void PNS_LOG_VIEWER_FRAME::onListChecked( wxCommandEvent& event )
{
    syncModel();
    drawLoggedItems( m_rewindIter );
}


void PNS_LOG_VIEWER_FRAME::onRewindScroll( wxScrollEvent& event )
{
    m_rewindIter = event.GetPosition();
    drawLoggedItems( m_rewindIter );
    updateDumpPanel( m_rewindIter );
    char str[128];
    sprintf( str, "%d", m_rewindIter );
    m_rewindPos->SetValue( str );
    event.Skip();
}

void PNS_LOG_VIEWER_FRAME::onBtnRewindLeft( wxCommandEvent& event )
{
    if( m_rewindIter > 0 )
    {
        m_rewindIter--;
        drawLoggedItems( m_rewindIter );
        updateDumpPanel( m_rewindIter );
        char str[128];
        sprintf( str, "%d", m_rewindIter );
        m_rewindPos->SetValue( str );
    }
}

void PNS_LOG_VIEWER_FRAME::onBtnRewindRight( wxCommandEvent& event )
{
    auto dbgd = m_env->GetDebugDecorator();
    int  count = dbgd->GetStageCount();

    if( m_rewindIter < count )
    {
        m_rewindIter++;
        drawLoggedItems( m_rewindIter );
        updateDumpPanel( m_rewindIter );
        char str[128];
        sprintf( str, "%d", m_rewindIter );
        m_rewindPos->SetValue( str );
    }
}

void PNS_LOG_VIEWER_FRAME::onRewindCountText( wxCommandEvent& event )
{
    if( !m_env )
        return;

    int val = wxAtoi( m_rewindPos->GetValue() );

    auto dbgd = m_env->GetDebugDecorator();
    int  count = dbgd->GetStageCount();

    if( val < 0 )
        val = 0;

    if( val >= count )
        val = count - 1;

    m_rewindIter = val;
    m_rewindSlider->SetValue( m_rewindIter );
    drawLoggedItems( m_rewindIter );
    updateDumpPanel( m_rewindIter );

    event.Skip();
}

void PNS_LOG_VIEWER_FRAME::syncModel()
{
    for( wxTreeListItem item = m_itemList->GetFirstItem(); item.IsOk();
         item = m_itemList->GetNextItem( item ) )
    {
        WX_SHAPE_TREE_ITEM_DATA* idata =
                static_cast<WX_SHAPE_TREE_ITEM_DATA*>( m_itemList->GetItemData( item ) );
        if( idata )
        {
            bool checked = m_itemList->GetCheckedState( item ) == wxCHK_CHECKED;
            bool selected = m_itemList->IsSelected( item );
            idata->m_item->m_visible = checked || selected;
            idata->m_item->m_selected = selected;
        }
    }

}

void PNS_LOG_VIEWER_FRAME::onListRightClick( wxMouseEvent& event )
{
    auto sel = m_itemList->GetPopupMenuSelectionFromUser( *m_listPopupMenu );

    switch( sel )
    {
    case ID_LIST_SHOW_NONE:
        m_itemList->CheckItemRecursively( m_itemList->GetRootItem(), wxCHK_UNCHECKED );
        syncModel();
        drawLoggedItems( m_rewindIter );
        break;
    case ID_LIST_SHOW_ALL:
        m_itemList->CheckItemRecursively( m_itemList->GetRootItem(), wxCHK_CHECKED );
        syncModel();
        drawLoggedItems( m_rewindIter );
        break;
    case ID_LIST_COPY:
    {
        wxString s;

        PNS_TEST_DEBUG_DECORATOR::STAGE* st = getCurrentStage();

        if( !st )
            return;

        auto formatShapes = [&]( PNS_TEST_DEBUG_DECORATOR::DEBUG_ENT* ent ) -> bool {
            if( ent->m_selected )
            {
                //printf("Ent %p\n", ent );
                for( auto sh : ent->m_shapes )
                {
                    //printf("sh %p\n", sh );
                    //printf("%s\n", sh->Format().c_str() );
                    s += "// " + ent->m_name + "\n " + sh->Format() + "; \n";
                }
            }
            return true;
        };

        st->m_entries->IterateTree( formatShapes );

        if( wxTheClipboard->Open() )
        {
            // This data objects are held by the clipboard,
            // so do not delete them in the app.
            wxTheClipboard->SetData( new wxTextDataObject( s ) );
            wxTheClipboard->Flush(); // Allow data to be available after closing KiCad
            wxTheClipboard->Close();
        }
        return;
    }
    }
}

void PNS_LOG_VIEWER_FRAME::onListSelect( wxCommandEvent& event )
{
    syncModel();
    drawLoggedItems( m_rewindIter );
}


void PNS_LOG_VIEWER_FRAME::buildListTree( wxTreeListItem                       item,
                                          PNS_TEST_DEBUG_DECORATOR::DEBUG_ENT* ent, int depth )
{
#ifdef EXTRA_VERBOSE
    for( int i = 0; i < depth * 2; i++ )
        printf( " " );

    if( ent->m_msg.length() )
        printf( "MSG: %s\n", ent->m_msg.c_str() );
    else
        printf( "SHAPES: %s [%d]\n", ent->m_name.c_str(), ent->m_children.size() );
#endif

    wxTreeListItem ritem;

    printf("depth %d\n", depth );

    if( ent->m_msg.length() )
    {
        ritem = m_itemList->AppendItem( item, "Child" );
        m_itemList->SetItemText( ritem, 0, "Message" );
        m_itemList->SetItemText( ritem, 1, ent->m_msg );
    }
    else
    {
        ritem = m_itemList->AppendItem( item, "Child" );
        m_itemList->SetItemText( ritem, 0, "Shapes" );
        m_itemList->SetItemText( ritem, 1, ent->m_name );
    }

    m_itemList->SetItemText( ritem, 2, wxFileNameFromPath( ent->m_srcLoc.fileName ) );
    m_itemList->SetItemText( ritem, 3, ent->m_srcLoc.funcName );
    m_itemList->SetItemText( ritem, 4, wxString::Format("%d", ent->m_srcLoc.line ) );

    m_itemList->SetItemData( ritem, new WX_SHAPE_TREE_ITEM_DATA( ent ) );

    if( !ent->m_children.size() )
        return;

    for( auto child : ent->m_children )
    {
        buildListTree( ritem, child, depth + 1 );
    }
}


static void expandAllChildren( wxTreeListCtrl* tree )
{
    wxTreeListItem child = tree->GetFirstItem ();
    while (child.IsOk())
    {
        tree->Expand ( child );
        child = tree->GetNextItem( child );
    }
}

void PNS_LOG_VIEWER_FRAME::updateDumpPanel( int iter )
{
    if( !m_env )
        return;

    auto dbgd = m_env->GetDebugDecorator();
    int  count = dbgd->GetStageCount();

    wxArrayString dumpStrings;

    if( count <= 0 )
        return;

    if( iter < 0 )
        iter = 0;

    if( iter >= count )
        iter = count - 1;


    auto st = dbgd->GetStage( iter );
    auto rootItem = m_itemList->GetRootItem();

    m_itemList->DeleteAllItems();
    buildListTree( rootItem, st->m_entries );
    m_itemList->CheckItemRecursively( rootItem, wxCHK_UNCHECKED );

    expandAllChildren( m_itemList );

    m_itemList->Refresh();
}


int replay_main_func( int argc, char* argv[] )
{
    auto frame = new PNS_LOG_VIEWER_FRAME( nullptr );

    //  drcCreateTestsProviderClearance();
    //    drcCreateTestsProviderEdgeClearance();


    if( argc >= 2 && std::string( argv[1] ) == "-h" )
    {
        printf( "PNS Log (Re)player. Allows to step through the log written by the ROUTER_TOOL "
                "in debug Kicad builds. " );
        printf( "Requires a board file with UUIDs and a matching log file. Both are written to "
                "/tmp when you press '0' during routing." );
        return 0;
    }

    if( argc < 3 )
    {
        printf( "Expected parameters: log_file.log board_file.dump\n" );
        return 0;
    }

    PNS_LOG_FILE* logFile = new PNS_LOG_FILE;
    logFile->Load( argv[1], argv[2] );

    frame->SetLogFile( logFile );
    //SetTopFrame( frame );      // wxApp gets a face.

    return 0;
}

static bool registered2 = UTILITY_REGISTRY::Register( {
        "replay",
        "PNS Log Player",
        replay_main_func,
} );

