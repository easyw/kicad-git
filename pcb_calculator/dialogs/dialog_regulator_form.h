/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 1992-2011 jean-pierre.charras
 * Copyright (C) 1992-2021 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file dialog_regulator_form.h
 */

#ifndef DIALOG_REGULATOR_FORM_H
#define DIALOG_REGULATOR_FORM_H

#include "dialog_regulator_form_base.h"

class PCB_CALCULATOR_FRAME;
class REGULATOR_DATA;


/**
 * Subclass of DIALOG_REGULATOR_FORM_BASE, which is generated by wxFormBuilder.
 *
 * Dialog used to add / edit regulators to the list on the regulator tab of the pcb calculator.
 */
class DIALOG_REGULATOR_FORM : public DIALOG_REGULATOR_FORM_BASE
{
public:
    DIALOG_REGULATOR_FORM( PCB_CALCULATOR_FRAME* parent, const wxString& aRegName )
            : DIALOG_REGULATOR_FORM_BASE( parent )
    {
        m_textCtrlName->SetValue( aRegName );
        m_textCtrlName->Enable( aRegName.IsEmpty() );
        UpdateDialog();

        m_sdbSizerOK->SetDefault();

        // Now all widgets have the size fixed, call FinishDialogSettings
        finishDialogSettings();
    }

    ~DIALOG_REGULATOR_FORM(){};

    /**
     * @return true if regulator parameters are acceptable.
     */
    bool TransferDataFromWindow();

    /**
     * Transfer data from dialog to aItem.
     *
     * @param aItem is the #REGULATOR_DATA to copy to dialog.
     */
    void CopyRegulatorDataToDialog( REGULATOR_DATA* aItem );

    /**
     * Create a new #REGULATOR_DATA from dialog data.
     *
     * @return a the new #REGULATOR_DATA.
     */
    REGULATOR_DATA* BuildRegulatorFromData();

    /**
     * Enable/disable Iadj related widgets, according to the regulator type.
     */
    void UpdateDialog();

    /**
     * Called when the current regulator type is changed.
     */
    void OnRegTypeSelection( wxCommandEvent& event ) override
    {
        UpdateDialog();
    }
};

#endif // DIALOG_REGULATOR_FORM_H
