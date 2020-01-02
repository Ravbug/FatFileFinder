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

	statusBar = this->CreateStatusBar( 1, wxSTB_DEFAULT_STYLE, wxID_ANY );
	wxMenuBar* menuBar;
	menuBar = new wxMenuBar( 0 );
	wxMenu* menuFile;
	menuFile = new wxMenu();
	wxMenuItem* openFolderMenu;
	openFolderMenu = new wxMenuItem( menuFile, wxID_OPEN, wxString( wxT("Open Folder") ) + wxT('\t') + wxT("Ctrl-O"), wxT("Calculate the size a folder, and its sub folders"), wxITEM_NORMAL );
	menuFile->Append( openFolderMenu );

	wxMenuItem* reloadFolderMenu;
	reloadFolderMenu = new wxMenuItem( menuFile, wxID_REFRESH, wxString( wxT("Reload Folder") ) + wxT('\t') + wxT("Ctrl-R"), wxT("Recalculate the selected folder's size"), wxITEM_NORMAL );
	menuFile->Append( reloadFolderMenu );

	wxMenuItem* stopSizingMenu;
	stopSizingMenu = new wxMenuItem( menuFile, wxID_STOP, wxString( wxT("Stop Sizing Folder") ) , wxT("Stop the current size calculation"), wxITEM_NORMAL );
	menuFile->Append( stopSizingMenu );

	menuBar->Append( menuFile, wxT("File") );

	wxMenu* menuWindow;
	menuWindow = new wxMenu();
	wxMenuItem* menuAbout;
	menuAbout = new wxMenuItem( menuWindow, wxID_ABOUT, wxString( wxT("About FatFileFinder") ) , wxT("Show information about this application"), wxITEM_NORMAL );
	menuWindow->Append( menuAbout );

	wxMenuItem* menuQuit;
	menuQuit = new wxMenuItem( menuWindow, wxID_EXIT, wxString( wxT("Close") ) + wxT('\t') + wxT("Ctrl-W"), wxT("Close FatFileFilder"), wxITEM_NORMAL );
	menuWindow->Append( menuQuit );

	menuBar->Append( menuWindow, wxT("Window") );

	wxMenu* menuHelp;
	menuHelp = new wxMenu();
	wxMenuItem* menuGit;
	menuGit = new wxMenuItem( menuHelp, wxID_INDENT, wxString( wxT("GitHub Repository") ) , wxT("View the source code for this application"), wxITEM_NORMAL );
	menuHelp->Append( menuGit );

	wxMenuItem* menuUpdates;
	menuUpdates = new wxMenuItem( menuHelp, wxID_UP, wxString( wxT("Check for Updates") ) , wxT("Visit the updates page"), wxITEM_NORMAL );
	menuHelp->Append( menuUpdates );

	menuToggleSidebar = new wxMenuItem( menuHelp, wxID_INFO, wxString( wxT("Hide Sidebar") ) + wxT('\t') + wxT("Ctrl-I"), wxEmptyString, wxITEM_NORMAL );
	menuHelp->Append( menuToggleSidebar );

	menuToggleLog = new wxMenuItem( menuHelp, wxID_JUSTIFY_CENTER, wxString( wxT("Show Log") ) + wxT('\t') + wxT("Ctrl-L"), wxEmptyString, wxITEM_NORMAL );
	menuHelp->Append( menuToggleLog );

	menuBar->Append( menuHelp, wxT("Help") );

	this->SetMenuBar( menuBar );

	wxFlexGridSizer* mainSizer;
	mainSizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	mainSizer->AddGrowableCol( 0 );
	mainSizer->AddGrowableRow( 1 );
	mainSizer->SetFlexibleDirection( wxBOTH );
	mainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxGridBagSizer* toolbarSizer;
	toolbarSizer = new wxGridBagSizer( 0, 0 );
	toolbarSizer->SetFlexibleDirection( wxBOTH );
	toolbarSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	openFolderBtn = new wxButton( this, wxID_OPEN, wxT("📂"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxBU_EXACTFIT );
	toolbarSizer->Add( openFolderBtn, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_BOTTOM, 5 );

	reloadFolderBtn = new wxButton( this, wxID_REFRESH, wxT("🔄"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxBU_EXACTFIT );
	toolbarSizer->Add( reloadFolderBtn, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_BOTTOM, 5 );

	stopSizeBtn = new wxButton( this, wxID_STOP, wxT("🛑"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxBU_EXACTFIT );
	toolbarSizer->Add( stopSizeBtn, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_BOTTOM, 5 );

	progressBar = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	progressBar->SetValue( 0 );
	toolbarSizer->Add( progressBar, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_BOTTOM, 5 );


	toolbarSizer->AddGrowableCol( 3 );

	mainSizer->Add( toolbarSizer, 1, wxEXPAND|wxALIGN_BOTTOM, 5 );

	mainSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxSP_PERMIT_UNSPLIT|wxSP_THIN_SASH );
	mainSplitter->SetSashGravity( 1 );
	mainSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrameBase::mainSplitterOnIdle ), NULL, this );
	mainSplitter->SetMinimumPaneSize( 220 );

	wxPanel* directoryPanel;
	directoryPanel = new wxPanel( mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* browserSizer;
	browserSizer = new wxBoxSizer( wxVERTICAL );

	browserSplitter = new wxSplitterWindow( directoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxSP_PERMIT_UNSPLIT|wxSP_THIN_SASH );
	browserSplitter->SetSashGravity( 1 );
	browserSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrameBase::browserSplitterOnIdle ), NULL, this );
	browserSplitter->SetMinimumPaneSize( 150 );

	wxPanel* browserPanel;
	browserPanel = new wxPanel( browserSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* browserSizerInner;
	browserSizerInner = new wxBoxSizer( wxVERTICAL );

	fileBrowser = new wxTreeListCtrl( browserPanel, TREELIST, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE|wxTL_SINGLE );
	fileBrowser->AppendColumn( wxT("File Name"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE );
	fileBrowser->AppendColumn( wxT("Percentage"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER, wxCOL_RESIZABLE|wxCOL_SORTABLE );
	fileBrowser->AppendColumn( wxT("Size"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_RIGHT, wxCOL_RESIZABLE|wxCOL_SORTABLE );

	browserSizerInner->Add( fileBrowser, 1, wxEXPAND | wxALL, 5 );


	browserPanel->SetSizer( browserSizerInner );
	browserPanel->Layout();
	browserSizerInner->Fit( browserPanel );
	logPanel = new wxPanel( browserSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridBagSizer* logSizer;
	logSizer = new wxGridBagSizer( 0, 0 );
	logSizer->SetFlexibleDirection( wxBOTH );
	logSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	logCtrl = new wxTextCtrl( logPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP|wxTE_MULTILINE|wxTE_READONLY );
	logSizer->Add( logCtrl, wxGBPosition( 0, 0 ), wxGBSpan( 1, 3 ), wxEXPAND|wxTOP|wxRIGHT, 5 );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( logPanel, wxID_ANY, wxT("Error Log"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	logSizer->Add( m_staticText1, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	wxButton* m_button6;
	m_button6 = new wxButton( logPanel, wxID_ANY, wxT("Copy Log"), wxDefaultPosition, wxDefaultSize, 0 );
	logSizer->Add( m_button6, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_button7 = new wxButton( logPanel, wxID_ANY, wxT("Clear Log"), wxDefaultPosition, wxDefaultSize, 0 );
	logSizer->Add( m_button7, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	logSizer->AddGrowableCol( 0 );
	logSizer->AddGrowableRow( 0 );

	logPanel->SetSizer( logSizer );
	logPanel->Layout();
	logSizer->Fit( logPanel );
	browserSplitter->SplitHorizontally( browserPanel, logPanel, 800 );
	browserSizer->Add( browserSplitter, 1, wxEXPAND, 5 );


	directoryPanel->SetSizer( browserSizer );
	directoryPanel->Layout();
	browserSizer->Fit( directoryPanel );
	propertyPanel = new wxPanel( mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridBagSizer* propertySizer;
	propertySizer = new wxGridBagSizer( 0, 0 );
	propertySizer->SetFlexibleDirection( wxBOTH );
	propertySizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	propertyList = new wxDataViewListCtrl( propertyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	PLPropertyCol = propertyList->AppendTextColumn( wxT("Property"), wxDATAVIEW_CELL_INERT, 150, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	PLValueCol = propertyList->AppendTextColumn( wxT("Value"), wxDATAVIEW_CELL_ACTIVATABLE, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	propertySizer->Add( propertyList, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	copyPathBtn = new wxButton( propertyPanel, COPYPATH, wxT("Copy File Path"), wxDefaultPosition, wxDefaultSize, 0 );
	propertySizer->Add( copyPathBtn, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	revealBtn = new wxButton( propertyPanel, wxID_FIND, wxT("Reveal In Explorer"), wxDefaultPosition, wxDefaultSize, 0 );
	propertySizer->Add( revealBtn, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );


	propertySizer->AddGrowableCol( 0 );
	propertySizer->AddGrowableRow( 0 );

	propertyPanel->SetSizer( propertySizer );
	propertyPanel->Layout();
	propertySizer->Fit( propertyPanel );
	mainSplitter->SplitVertically( directoryPanel, propertyPanel, 700 );
	mainSizer->Add( mainSplitter, 1, wxEXPAND, 5 );


	this->SetSizer( mainSizer );
	this->Layout();

	this->Centre( wxBOTH );
}

MainFrameBase::~MainFrameBase()
{
}
