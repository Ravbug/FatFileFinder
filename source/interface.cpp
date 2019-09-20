///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "interface.h"

///////////////////////////////////////////////////////////////////////////

MainFrameBase::MainFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* baseSizer;
	baseSizer = new wxBoxSizer( wxVERTICAL );

	mainSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxSP_PERMIT_UNSPLIT|wxSP_THIN_SASH );
	mainSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrameBase::mainSplitterOnIdle ), NULL, this );

	propertyPanel = new wxPanel( mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridBagSizer* propertySizer;
	propertySizer = new wxGridBagSizer( 0, 0 );
	propertySizer->SetFlexibleDirection( wxBOTH );
	propertySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	propertyList = new wxDataViewListCtrl( propertyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES );
	PLPropertyCol = propertyList->AppendTextColumn( wxT("Property"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	PLValueCol = propertyList->AppendTextColumn( wxT("Value"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	propertySizer->Add( propertyList, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );


	propertySizer->AddGrowableCol( 0 );
	propertySizer->AddGrowableRow( 0 );

	propertyPanel->SetSizer( propertySizer );
	propertyPanel->Layout();
	propertySizer->Fit( propertyPanel );
	directoryPanel = new wxPanel( mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* browserSizer;
	browserSizer = new wxBoxSizer( wxVERTICAL );

	fileBrowser = new wxTreeListCtrl( directoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE|wxTL_SINGLE );

	browserSizer->Add( fileBrowser, 1, wxEXPAND | wxALL, 5 );


	directoryPanel->SetSizer( browserSizer );
	directoryPanel->Layout();
	browserSizer->Fit( directoryPanel );
	mainSplitter->SplitVertically( propertyPanel, directoryPanel, 0 );
	baseSizer->Add( mainSplitter, 1, wxEXPAND, 5 );


	this->SetSizer( baseSizer );
	this->Layout();
	statusBar = this->CreateStatusBar( 1, wxSTB_DEFAULT_STYLE, wxID_ANY );
	menuBar = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	wxMenuItem* openFolderMenu;
	openFolderMenu = new wxMenuItem( menuFile, wxID_OPEN, wxString( wxT("Open Folder") ) + wxT('\t') + wxT("Ctrl-O"), wxT("Size a folder"), wxITEM_NORMAL );
	menuFile->Append( openFolderMenu );

	menuBar->Append( menuFile, wxT("File") );

	menuWindow = new wxMenu();
	wxMenuItem* menuAbout;
	menuAbout = new wxMenuItem( menuWindow, wxID_ABOUT, wxString( wxT("About FatFileFinder") ) , wxT("Show information about this application"), wxITEM_NORMAL );
	menuWindow->Append( menuAbout );

	wxMenuItem* menuQuit;
	menuQuit = new wxMenuItem( menuWindow, wxID_EXIT, wxString( wxT("Close") ) + wxT('\t') + wxT("Ctrl-W"), wxT("Close FatFileFilder"), wxITEM_NORMAL );
	menuWindow->Append( menuQuit );

	menuBar->Append( menuWindow, wxT("Window") );

	this->SetMenuBar( menuBar );


	this->Centre( wxBOTH );
}

MainFrameBase::~MainFrameBase()
{
}
