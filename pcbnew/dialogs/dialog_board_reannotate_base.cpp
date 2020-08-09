///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx_html_report_panel.h"

#include "dialog_board_reannotate_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_BOARD_REANNOTATE_BASE::DIALOG_BOARD_REANNOTATE_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bmainSizer;
	bmainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bupperSizer;
	bupperSizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_StandardOptions = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_StandardOptions, wxID_ANY, _("Sorting") ), wxHORIZONTAL );

	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 11, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_Down_Right = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Down_Right->SetValue( true );
	fgSizer5->Add( m_Down_Right, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_down_right_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_down_right_bitmap->SetToolTip( _("Horizontally: top left to bottom right") );

	fgSizer5->Add( reannotate_down_right_bitmap, 0, wxALL, 5 );


	fgSizer5->Add( 80, 0, 1, wxEXPAND, 5 );

	m_Right_Down = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Right_Down, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_right_down_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_right_down_bitmap->SetToolTip( _("Horizontally: top right to bottom left") );

	fgSizer5->Add( reannotate_right_down_bitmap, 0, wxALL, 5 );


	fgSizer5->Add( 80, 0, 1, wxEXPAND, 5 );

	m_Down_Left = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Down_Left, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_down_left_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_down_left_bitmap->SetToolTip( _("Horizontally: bottom left to top right") );

	fgSizer5->Add( reannotate_down_left_bitmap, 0, wxALL, 5 );


	fgSizer5->Add( 80, 0, 1, wxEXPAND, 5 );

	m_Left_Down = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Left_Down, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_left_down_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_left_down_bitmap->SetToolTip( _("Horizontally:: bottom right to top left") );

	fgSizer5->Add( reannotate_left_down_bitmap, 0, wxALL, 5 );

	m_Up_Right = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Up_Right, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_up_right_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_up_right_bitmap->SetToolTip( _("Vertically: top left to bottom right") );

	fgSizer5->Add( reannotate_up_right_bitmap, 0, wxALL, 5 );


	fgSizer5->Add( 50, 0, 1, wxEXPAND, 5 );

	m_Right_Up = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Right_Up, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_right_up_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_right_up_bitmap->SetToolTip( _("Vertically: top right to bottom left") );

	fgSizer5->Add( reannotate_right_up_bitmap, 0, wxALL, 5 );


	fgSizer5->Add( 50, 0, 1, wxEXPAND, 5 );

	m_Up_Left = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Up_Left, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_up_left_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_up_left_bitmap->SetToolTip( _("Vertically: bottom left to top right") );

	fgSizer5->Add( reannotate_up_left_bitmap, 0, wxALL, 5 );


	fgSizer5->Add( 50, 0, 1, wxEXPAND, 5 );

	m_Left_Up = new wxRadioButton( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_Left_Up, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	reannotate_left_up_bitmap = new wxStaticBitmap( sbSizer3->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 48,48 ), 0 );
	reannotate_left_up_bitmap->SetToolTip( _("Vertically: bottom right to top left") );

	fgSizer5->Add( reannotate_left_up_bitmap, 0, wxALL, 5 );


	sbSizer3->Add( fgSizer5, 1, wxEXPAND, 5 );


	bSizer4->Add( sbSizer3, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_StandardOptions, wxID_ANY, wxEmptyString ), wxVERTICAL );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_UpdateSchematic = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, _("Update schematic"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UpdateSchematic->SetValue(true);
	fgSizer11->Add( m_UpdateSchematic, 0, wxALIGN_CENTER|wxALL, 5 );


	fgSizer11->Add( 100, 0, 1, wxEXPAND, 5 );

	m_SortGridText = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Round to:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SortGridText->Wrap( -1 );
	m_SortGridText->SetToolTip( _("Component position will be rounded\nto this grid before sorting.\nThis helps with misaligned parts.") );

	fgSizer11->Add( m_SortGridText, 0, wxALIGN_CENTER|wxALL, 5 );

	wxArrayString m_GridChoiceChoices;
	m_GridChoice = new wxChoice( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_GridChoiceChoices, 0 );
	m_GridChoice->SetSelection( 0 );
	m_GridChoice->SetToolTip( _("Component position will be rounded\nto this grid before sorting.\nThis helps with misaligned parts.") );
	m_GridChoice->SetMinSize( wxSize( 300,-1 ) );

	fgSizer11->Add( m_GridChoice, 0, wxALL, 5 );


	sbSizer4->Add( fgSizer11, 0, wxEXPAND|wxALL, 0 );

	wxFlexGridSizer* fgSizer331;
	fgSizer331 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer331->SetFlexibleDirection( wxBOTH );
	fgSizer331->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_SortOn = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Sort by:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SortOn->Wrap( -1 );
	m_SortOn->SetMinSize( wxSize( 100,-1 ) );

	fgSizer331->Add( m_SortOn, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_VERTICAL, 0 );

	m_SortOnModules = new wxRadioButton( sbSizer4->GetStaticBox(), wxID_ANY, _("Footprints"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_SortOnModules->SetValue( true );
	m_SortOnModules->SetToolTip( _("Annotation based on footprint coordinates") );

	fgSizer331->Add( m_SortOnModules, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_SortOnReference = new wxRadioButton( sbSizer4->GetStaticBox(), wxID_ANY, _("References"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_SortOnReference->SetToolTip( _("Annotation based on reference designator coordinates") );
	m_SortOnReference->SetMinSize( wxSize( 130,-1 ) );

	fgSizer331->Add( m_SortOnReference, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sbSizer4->Add( fgSizer331, 1, wxALL|wxEXPAND, 5 );


	bSizer4->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer31;
	sbSizer31 = new wxStaticBoxSizer( new wxStaticBox( m_StandardOptions, wxID_ANY, wxEmptyString ), wxVERTICAL );

	wxFlexGridSizer* fgSizer6111;
	fgSizer6111 = new wxFlexGridSizer( 0, 7, 0, 0 );
	fgSizer6111->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer6111->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );

	AnnotateLabel = new wxStaticText( sbSizer31->GetStaticBox(), wxID_ANY, _("Annotate:"), wxDefaultPosition, wxDefaultSize, 0 );
	AnnotateLabel->Wrap( -1 );
	AnnotateLabel->SetMinSize( wxSize( 100,-1 ) );

	fgSizer6111->Add( AnnotateLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_AnnotateAll = new wxRadioButton( sbSizer31->GetStaticBox(), wxID_ANY, _("All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AnnotateAll->SetValue( true );
	fgSizer6111->Add( m_AnnotateAll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_AnnotateFront = new wxRadioButton( sbSizer31->GetStaticBox(), wxID_ANY, _("Front"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6111->Add( m_AnnotateFront, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_AnnotateBack = new wxRadioButton( sbSizer31->GetStaticBox(), wxID_ANY, _("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6111->Add( m_AnnotateBack, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_AnnotateSelection = new wxRadioButton( sbSizer31->GetStaticBox(), wxID_ANY, _("Selection"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6111->Add( m_AnnotateSelection, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sbSizer31->Add( fgSizer6111, 1, wxEXPAND, 5 );


	bSizer4->Add( sbSizer31, 0, wxALL|wxEXPAND, 5 );


	m_StandardOptions->SetSizer( bSizer4 );
	m_StandardOptions->Layout();
	bSizer4->Fit( m_StandardOptions );
	m_notebook->AddPage( m_StandardOptions, _("Standard Options"), true );
	m_Advanced = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 6, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	fgSizer3->Add( 48, 0, 1, wxEXPAND, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer5->Add( fgSizer3, 0, wxEXPAND, 0 );

	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 5, 5 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_FrontRefDesStartText = new wxStaticText( m_Advanced, wxID_ANY, _("Front reference start:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrontRefDesStartText->Wrap( -1 );
	m_FrontRefDesStartText->SetToolTip( _("Starting reference designation for front.") );

	gbSizer1->Add( m_FrontRefDesStartText, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_FrontRefDesStart = new wxTextCtrl( m_Advanced, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrontRefDesStart->SetToolTip( _("Default is 1") );

	gbSizer1->Add( m_FrontRefDesStart, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_BottomRefDesStartText = new wxStaticText( m_Advanced, wxID_ANY, _("Back reference start:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_BottomRefDesStartText->Wrap( -1 );
	m_BottomRefDesStartText->SetToolTip( _("Blank continues from front or enter a number greater than the highest reference designation on the front.") );

	gbSizer1->Add( m_BottomRefDesStartText, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_BackRefDesStart = new wxTextCtrl( m_Advanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_BackRefDesStart->SetToolTip( _("Leave blank or zero, or enter a number greater than the highest reference designation on the front.") );

	gbSizer1->Add( m_BackRefDesStart, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_RemoveFrontPrefix = new wxCheckBox( m_Advanced, wxID_ANY, _("Remove front prefix"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RemoveFrontPrefix->SetToolTip( _("If checked will remove the front side prefix\nin the front prefix box if present") );

	gbSizer1->Add( m_RemoveFrontPrefix, wxGBPosition( 2, 0 ), wxGBSpan( 1, 2 ), wxALL, 5 );

	m_RemoveBackPrefix = new wxCheckBox( m_Advanced, wxID_ANY, _("Remove back prefix"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RemoveBackPrefix->SetToolTip( _("If checked will remove the Back side prefix\nin the back prefix box if present") );

	gbSizer1->Add( m_RemoveBackPrefix, wxGBPosition( 2, 2 ), wxGBSpan( 1, 2 ), wxALL, 5 );

	m_FrontPrefixText = new wxStaticText( m_Advanced, wxID_ANY, _("Front prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FrontPrefixText->Wrap( -1 );
	m_FrontPrefixText->SetToolTip( _("Optional prefix for component side reference designations (i.e. F_)") );

	gbSizer1->Add( m_FrontPrefixText, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_FrontPrefix = new wxTextCtrl( m_Advanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_FrontPrefix->SetToolTip( _("Optional prefix for component side reference designations (i.e. F_)") );

	gbSizer1->Add( m_FrontPrefix, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_BackPrefixText = new wxStaticText( m_Advanced, wxID_ANY, _("Back prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_BackPrefixText->Wrap( -1 );
	m_BackPrefixText->SetToolTip( _("Optional prefix for solder side reference designations (i.e. B_)") );

	gbSizer1->Add( m_BackPrefixText, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_BackPrefix = new wxTextCtrl( m_Advanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_BackPrefix->SetToolTip( _("Optional prefix for solder side reference designations (i.e. B_)") );

	gbSizer1->Add( m_BackPrefix, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_ExcludeLocked = new wxCheckBox( m_Advanced, wxID_ANY, _("Exclude locked footprints"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ExcludeLocked->SetToolTip( _("Locked footprints will not be reannotated") );

	gbSizer1->Add( m_ExcludeLocked, wxGBPosition( 3, 0 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_ExcludeListText = new wxStaticText( m_Advanced, wxID_ANY, _("Exclude references:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ExcludeListText->Wrap( -1 );
	m_ExcludeListText->SetToolTip( _("Do not re-annotate this type \nof reference (R means R*)") );
	m_ExcludeListText->SetMinSize( wxSize( 150,-1 ) );

	gbSizer1->Add( m_ExcludeListText, wxGBPosition( 3, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_ExcludeList = new wxTextCtrl( m_Advanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_ExcludeList->SetMinSize( wxSize( 153,-1 ) );

	gbSizer1->Add( m_ExcludeList, wxGBPosition( 3, 3 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	bSizer5->Add( gbSizer1, 1, wxALL|wxEXPAND, 5 );


	m_Advanced->SetSizer( bSizer5 );
	m_Advanced->Layout();
	bSizer5->Fit( m_Advanced );
	m_notebook->AddPage( m_Advanced, _("Reference Designators"), false );

	bupperSizer->Add( m_notebook, 1, wxALL|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_MessageWindow = new WX_HTML_REPORT_PANEL( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxVSCROLL );
	m_MessageWindow->SetMinSize( wxSize( -1,150 ) );

	bSizer6->Add( m_MessageWindow, 1, wxEXPAND|wxLEFT|wxRIGHT, 5 );


	bupperSizer->Add( bSizer6, 1, wxALL|wxEXPAND, 5 );


	bmainSizer->Add( bupperSizer, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 6 );

	wxBoxSizer* m_buttonsSizer;
	m_buttonsSizer = new wxBoxSizer( wxHORIZONTAL );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	m_buttonsSizer->Add( m_sdbSizer, 1, wxALL|wxEXPAND, 5 );


	bmainSizer->Add( m_buttonsSizer, 0, wxEXPAND|wxLEFT, 5 );


	this->SetSizer( bmainSizer );
	this->Layout();

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_BOARD_REANNOTATE_BASE::OnClose ) );
	m_FrontPrefix->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::FilterFrontPrefix ), NULL, this );
	m_BackPrefix->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::FilterBackPrefix ), NULL, this );
	m_sdbSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::OnCloseClick ), NULL, this );
	m_sdbSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::OnApplyClick ), NULL, this );
}

DIALOG_BOARD_REANNOTATE_BASE::~DIALOG_BOARD_REANNOTATE_BASE()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_BOARD_REANNOTATE_BASE::OnClose ) );
	m_FrontPrefix->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::FilterFrontPrefix ), NULL, this );
	m_BackPrefix->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::FilterBackPrefix ), NULL, this );
	m_sdbSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::OnCloseClick ), NULL, this );
	m_sdbSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_BOARD_REANNOTATE_BASE::OnApplyClick ), NULL, this );

}
