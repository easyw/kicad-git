/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 1992-2011 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef PCB_TARGET_H
#define PCB_TARGET_H

#include <board_item.h>


class EDA_RECT;
class LINE_READER;


class PCB_TARGET : public BOARD_ITEM
{
    int     m_shape;            // bit 0 : 0 = draw +, 1 = draw X
    int     m_size;
    int     m_lineWidth;
    wxPoint m_pos;

public:
    PCB_TARGET( BOARD_ITEM* aParent );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    PCB_TARGET( BOARD_ITEM* aParent, int aShape, PCB_LAYER_ID aLayer,
                const wxPoint& aPos, int aSize, int aWidth );

    // Do not create a copy constructor & operator=.
    // The ones generated by the compiler are adequate.

    ~PCB_TARGET();

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_TARGET_T == aItem->Type();
    }

    void SetPosition( const wxPoint& aPos ) override { m_pos = aPos; }
    wxPoint GetPosition() const override { return m_pos; }

    void SetShape( int aShape )     { m_shape = aShape; }
    int GetShape() const            { return m_shape; }

    void SetSize( int aSize )       { m_size = aSize; }
    int GetSize() const             { return m_size; }

    void SetWidth( int aWidth )     { m_lineWidth = aWidth; }
    int GetWidth() const            { return m_lineWidth; }

    void Move( const wxPoint& aMoveVector ) override
    {
        m_pos += aMoveVector;
    }

    void Rotate( const wxPoint& aRotCentre, double aAngle ) override;

    void Flip( const wxPoint& aCentre, bool aFlipLeftRight ) override;

    wxString GetClass() const override
    {
        return wxT( "PCB_TARGET" );
    }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    // Virtual function
    const EDA_RECT GetBoundingBox() const override;

    std::shared_ptr<SHAPE> GetEffectiveShape( PCB_LAYER_ID aLayer ) const override;

    wxString GetSelectMenuText( EDA_UNITS aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

    virtual void SwapData( BOARD_ITEM* aImage ) override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif
};


#endif
