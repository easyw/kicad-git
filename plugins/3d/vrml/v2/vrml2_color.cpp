/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 Cirilo Bernardo <cirilo.bernardo@gmail.com>
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


#include <iostream>
#include <sstream>
#include <wx/log.h>

#include "vrml2_base.h"
#include "vrml2_color.h"


WRL2COLOR::WRL2COLOR() : WRL2NODE()
{
    m_Type = WRL2NODES::WRL2_COLOR;
    return;
}


WRL2COLOR::WRL2COLOR( WRL2NODE* aParent ) : WRL2NODE()
{
    m_Type = WRL2NODES::WRL2_COLOR;
    m_Parent = aParent;

    if( NULL != m_Parent )
        m_Parent->AddChildNode( this );

    return;
}


WRL2COLOR::~WRL2COLOR()
{
    #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 2 )
    wxLogTrace( MASK_VRML, " * [INFO] Destroying Color node\n" );
    #endif

    return;
}


bool WRL2COLOR::isDangling( void )
{
    // this node is dangling unless it has a parent of type WRL2_INDEXEDFACESET

    if( NULL == m_Parent || m_Parent->GetNodeType() != WRL2NODES::WRL2_INDEXEDFACESET )
        return true;

    return false;
}


bool WRL2COLOR::AddRefNode( WRL2NODE* aNode )
{
    // this node may not own or reference any other node

    #ifdef DEBUG_VRML2
    do {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [BUG] AddRefNode is not applicable";
        wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
    } while( 0 );
    #endif

    return false;
}


bool WRL2COLOR::AddChildNode( WRL2NODE* aNode )
{
    // this node may not own or reference any other node

    #ifdef DEBUG_VRML2
    do {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [BUG] AddChildNode is not applicable";
        wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
    } while( 0 );
    #endif

    return false;
}


bool WRL2COLOR::Read( WRLPROC& proc, WRL2BASE* aTopNode )
{
    size_t line, column;
    proc.GetFilePosData( line, column );

    char tok = proc.Peek();

    if( proc.eof() )
    {
        #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 1 )
        do {
            std::ostringstream ostr;
            ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            ostr << " * [INFO] bad file format; unexpected eof at line ";
            ostr << line << ", column " << column;
            wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
        } while( 0 );
        #endif

        return false;
    }

    if( '{' != tok )
    {
        #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 1 )
        do {
            std::ostringstream ostr;
            ostr << proc.GetError() << "\n";
            ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            ostr << " * [INFO] bad file format; expecting '{' but got '" << tok;
            ostr  << "' at line " << line << ", column " << column;
            wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
        } while( 0 );
        #endif

        return false;
    }

    proc.Pop();
    std::string glob;

    if( proc.Peek() == '}' )
    {
        proc.Pop();
        return true;
    }

    if( !proc.ReadName( glob ) )
    {
        #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 1 )
        do {
            std::ostringstream ostr;
            ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            ostr << proc.GetError();
            wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
        } while( 0 );
        #endif

        return false;
    }

    proc.GetFilePosData( line, column );

    // expecting 'color'
    if( !glob.compare( "color" ) )
    {
        if( !proc.ReadMFVec3f( colors ) )
        {
            #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 1 )
            do {
                std::ostringstream ostr;
                ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                ostr << " * [INFO] invalid color set at line " << line << ", column ";
                ostr << column << "\n";
                ostr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                ostr << " * [INFO] message: '" << proc.GetError() << "'";
                wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
            } while( 0 );
            #endif

            return false;
        }
    }
    else
    {
        #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 1 )
        do {
            std::ostringstream ostr;
            ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            ostr << " * [INFO] bad Color at line " << line << ", column ";
            ostr << column << "\n";
            ostr << " * [INFO] file: '" << proc.GetFileName() << "'";
            wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
        } while( 0 );
        #endif

        return false;
    }

    if( proc.Peek() == '}' )
    {
        proc.Pop();
        return true;
    }

    proc.GetFilePosData( line, column );

    #if defined( DEBUG_VRML2 ) && ( DEBUG_VRML2 > 1 )
    do {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [INFO] bad Color at line " << line << ", column ";
        ostr << column << " (no closing brace)\n";
        ostr << " * [INFO] file: '" << proc.GetFileName() << "'";
        wxLogTrace( MASK_VRML, "%s\n", ostr.str().c_str() );
    } while( 0 );
    #endif

    return false;
}


SGNODE* WRL2COLOR::TranslateToSG( SGNODE* aParent )
{
    // any data manipulation must be performed by the parent node
    return NULL;
}


bool WRL2COLOR::HasColors( void )
{
    if( colors.empty() )
        return false;

    return true;
}


void WRL2COLOR::GetColor( int aIndex, float& red, float& green, float& blue )
{
    if( aIndex < 0 || aIndex >= (int)colors.size() )
    {
        red = 0.8f;
        green = 0.8f;
        blue = 0.8f;
        return;
    }

    red = colors[aIndex].x;
    green = colors[aIndex].y;
    blue = colors[aIndex].z;

    return;
}


void WRL2COLOR::GetColors( WRLVEC3F*& aColorList, size_t& aListSize)
{
    if( colors.empty() )
    {
        aColorList = NULL;
        aListSize = 0;
        return;
    }

    aColorList = &colors[0];
    aListSize = colors.size();

    return;
}
