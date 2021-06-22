/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2004-2021 KiCad Developers, see change_log.txt for contributors.
 * Copyright (C) 2019 CERN
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

#ifndef LIB_ARC_H
#define LIB_ARC_H

#include <lib_item.h>
#include <math/vector2d.h>

class TRANSFORM;


class LIB_ARC : public LIB_ITEM
{
public:
    LIB_ARC( LIB_SYMBOL* aParent );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~LIB_ARC() { }

    wxString GetClass() const override
    {
        return wxT( "LIB_ARC" );
    }

    wxString GetTypeName() const override
    {
        return _( "Arc" );
    }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    const EDA_RECT GetBoundingBox() const override;

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    int GetPenWidth() const override;

    void BeginEdit( const wxPoint& aStartPoint ) override;
    void CalcEdit( const wxPoint& aPosition ) override;
    void SetEditState( int aState ) { m_editState = aState; }

    void Offset( const wxPoint& aOffset ) override;

    void MoveTo( const wxPoint& aPosition ) override;

    wxPoint GetPosition() const override { return m_Pos; }

    void MirrorHorizontal( const wxPoint& aCenter ) override;
    void MirrorVertical( const wxPoint& aCenter ) override;
    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true ) override;

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform ) const override;

    int GetWidth() const override { return m_Width; }
    void SetWidth( int aWidth ) override { m_Width = aWidth; }

    void SetRadius( int aRadius ) { m_Radius = aRadius; }
    int GetRadius() const { return m_Radius; }

    void SetFirstRadiusAngle( int aAngle ) { m_t1 = aAngle; }
    int GetFirstRadiusAngle() const { return m_t1; }

    void SetSecondRadiusAngle( int aAngle ) { m_t2 = aAngle; }
    int GetSecondRadiusAngle() const { return m_t2; }

    wxPoint GetStart() const { return m_ArcStart; }
    void SetStart( const wxPoint& aPoint ) { m_ArcStart = aPoint; }

    wxPoint GetEnd() const { return m_ArcEnd; }
    void SetEnd( const wxPoint& aPoint ) { m_ArcEnd = aPoint; }

    /**
     * Calculate the arc mid point using the arc start and end angles and radius length.
     *
     * @note This is not a general purpose trigonometric arc midpoint calculation.  It is
     *       limited to the less than 180.0 degree arcs used in symbols.
     *
     * @return A #VECTOR2I object containing the midpoint of the arc.
     */
    VECTOR2I CalcMidPoint() const;

    /**
     * Calculate the radius and angle of an arc using the start, end, and center points.
     */
    void CalcRadiusAngles();


    wxString GetSelectMenuText( EDA_UNITS aUnits ) const override;

    BITMAPS GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

private:
    void print( const RENDER_SETTINGS* aSettings, const wxPoint& aOffset, void* aData,
                const TRANSFORM& aTransform ) override;

    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The arc specific sort order is as follows:
     *      - Arc horizontal (X) position.
     *      - Arc vertical (Y) position.
     *      - Arc start angle.
     *      - Arc end angle.
     */
    int compare( const LIB_ITEM& aOther,
            LIB_ITEM::COMPARE_FLAGS aCompareFlags = LIB_ITEM::COMPARE_FLAGS::NORMAL ) const override;

    enum SELECT_T               // When creating an arc: status of arc
    {
        ARC_STATUS_START,
        ARC_STATUS_END,
        ARC_STATUS_OUTLINE,
    };

    int      m_Radius;
    int      m_t1;              // First radius angle of the arc in 0.1 degrees.
    int      m_t2;              /* Second radius angle of the arc in 0.1 degrees. */
    wxPoint  m_ArcStart;
    wxPoint  m_ArcEnd;          /* Arc end position. */
    wxPoint  m_Pos;             /* Radius center point. */
    int      m_Width;           /* Line width */
    int      m_editState;
};


#endif    // LIB_ARC_H
