/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 CERN
 * Copyright (C) 2019 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <tool/tool_action.h>
#include <bitmaps.h>
#include <eda_item.h>
#include <drawing_sheet/ds_data_item.h>

#include "tools/pl_actions.h"

// Actions, being statically-defined, require specialized I18N handling.  We continue to
// use the _() macro so that string harvesting by the I18N framework doesn't have to be
// specialized, but we don't translate on initialization and instead do it in the getters.

#undef _
#define _(s) s


// PL_DRAWING_TOOLS
//
TOOL_ACTION PL_ACTIONS::drawLine( "plEditor.InteractiveDrawing.drawLine",
        AS_GLOBAL, 0, "",
        _( "Add Line" ), _( "Add a line" ),
        BITMAPS::add_graphical_segments, AF_ACTIVATE, (void*) DS_DATA_ITEM::DS_SEGMENT );

TOOL_ACTION PL_ACTIONS::drawRectangle( "plEditor.InteractiveDrawing.drawRectangle",
        AS_GLOBAL, 0, "",
        _( "Add Rectangle" ), _( "Add a rectangle" ),
        BITMAPS::add_rectangle, AF_ACTIVATE, (void*) DS_DATA_ITEM::DS_RECT );

TOOL_ACTION PL_ACTIONS::placeText( "plEditor.InteractiveDrawing.placeText",
        AS_GLOBAL, 0, "",
        _( "Add Text" ), _( "Add a text item" ),
        BITMAPS::text, AF_ACTIVATE, (void*) DS_DATA_ITEM::DS_TEXT );

TOOL_ACTION PL_ACTIONS::placeImage( "plEditor.InteractiveDrawing.placeImage",
        AS_GLOBAL, 0, "",
        _( "Add Bitmap" ), _( "Add a bitmap image" ),
        BITMAPS::image, AF_ACTIVATE, (void*) DS_DATA_ITEM::DS_BITMAP );


// PL_EDIT_TOOL
//
TOOL_ACTION PL_ACTIONS::move( "plEditor.InteractiveMove.move",
        AS_GLOBAL,
        'M', LEGACY_HK_NAME( "Move Item" ),
        _( "Move" ), _( "Moves the selected item(s)" ),
        BITMAPS::move, AF_ACTIVATE );

TOOL_ACTION PL_ACTIONS::appendImportedDrawingSheet( "plEditor.InteractiveEdit.appendWorksheet",
        AS_GLOBAL, 0, "",
        _( "Append Existing Drawing Sheet..." ),
        _( "Append an existing drawing sheet file to current file" ),
        BITMAPS::import, AF_ACTIVATE );


// PL_EDITOR_CONTROL
//
TOOL_ACTION PL_ACTIONS::showInspector( "plEditor.EditorControl.ShowInspector",
        AS_GLOBAL, 0, "",
        _( "Show Design Inspector" ), _( "Show the list of items in the drawing sheet" ),
        BITMAPS::spreadsheet );

TOOL_ACTION PL_ACTIONS::previewSettings( "plEditor.EditorControl.PreviewSettings",
        AS_GLOBAL, 0, "",
        _( "Page Preview Settings..." ), _( "Edit preview data for page size and title block" ),
        BITMAPS::sheetset );

TOOL_ACTION PL_ACTIONS::layoutNormalMode( "plEditor.EditorControl.LayoutNormalMode",
        AS_GLOBAL, 0, "",
        _( "Show title block in preview mode" ),
        _( "Show title block in preview mode:\n"
           "text placeholders will be replaced with preview data." ),
        BITMAPS::pagelayout_normal_view_mode );

TOOL_ACTION PL_ACTIONS::layoutEditMode( "plEditor.EditorControl.LayoutEditMode",
        AS_GLOBAL, 0, "",
        _( "Show title block in edit mode" ),
        _( "Show title block in edit mode:\n"
           "text placeholders are shown as ${keyword} tokens." ),
        BITMAPS::pagelayout_special_view_mode  );


// PL_SELECTION_TOOL
//
TOOL_ACTION PL_ACTIONS::selectionActivate( "plEditor.InteractiveSelection",
        AS_GLOBAL, 0, "", "", "",       // No description, not shown anywhere
        BITMAPS::INVALID_BITMAP, AF_ACTIVATE );

TOOL_ACTION PL_ACTIONS::selectionMenu( "plEditor.InteractiveSelection.SelectionMenu",
        AS_GLOBAL );

TOOL_ACTION PL_ACTIONS::addItemToSel( "plEditor.InteractiveSelection.AddItemToSel",
        AS_GLOBAL );

TOOL_ACTION PL_ACTIONS::addItemsToSel( "plEditor.InteractiveSelection.AddItemsToSel",
        AS_GLOBAL );

TOOL_ACTION PL_ACTIONS::removeItemFromSel( "plEditor.InteractiveSelection.RemoveItemFromSel",
        AS_GLOBAL );

TOOL_ACTION PL_ACTIONS::removeItemsFromSel( "plEditor.InteractiveSelection.RemoveItemsFromSel",
        AS_GLOBAL );

TOOL_ACTION PL_ACTIONS::clearSelection( "plEditor.InteractiveSelection.ClearSelection",
        AS_GLOBAL );


