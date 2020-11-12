/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <unit_test_utils/unit_test_utils.h>

#include <board.h>
#include <footprint.h>
#include <pad_naming.h>

struct PAD_FIXTURE
{
    PAD_FIXTURE() : m_board(), m_module( &m_board )
    {
    }

    PAD MakeNPTH()
    {
        PAD pad( &m_module );

        pad.SetAttribute( PAD_ATTRIB_NPTH );
        pad.SetLayerSet( PAD::UnplatedHoleMask() );

        return pad;
    }

    PAD MakeAperture()
    {
        PAD pad( &m_module );

        pad.SetAttribute( PAD_ATTRIB_PTH );
        pad.SetLayerSet( PAD::ApertureMask() );

        return pad;
    }

    PAD MakeSmd()
    {
        PAD pad( &m_module );

        pad.SetAttribute( PAD_ATTRIB_SMD );
        pad.SetLayerSet( PAD::SMDMask() );

        return pad;
    }

    BOARD  m_board;
    MODULE m_module;
};


BOOST_FIXTURE_TEST_SUITE( PadNaming, PAD_FIXTURE )

/**
 * Check what gets names and what doesn't
 */
BOOST_AUTO_TEST_CASE( CanName )
{
    auto npth = MakeNPTH();
    BOOST_CHECK_EQUAL( false, PAD_NAMING::PadCanHaveName( npth ) );

    auto aperture = MakeAperture();
    BOOST_CHECK_EQUAL( false, PAD_NAMING::PadCanHaveName( aperture ) );

    auto smd = MakeSmd();
    BOOST_CHECK_EQUAL( true, PAD_NAMING::PadCanHaveName( smd ) );
}


BOOST_AUTO_TEST_SUITE_END()
