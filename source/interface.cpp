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

	mainSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DSASH|wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxSP_PERMIT_UNSPLIT|wxSP_THIN_SASH );
	mainSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrameBase::mainSplitterOnIdle ), NULL, this );

	m_panel1 = new wxPanel( mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_dataViewListCtrl3 = new wxDataViewListCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES );
	m_dataViewListColumn1 = m_dataViewListCtrl3->AppendTextColumn( wxT("Property"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	m_dataViewListColumn2 = m_dataViewListCtrl3->AppendTextColumn( wxT("Value"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	bSizer3->Add( m_dataViewListCtrl3, 0, wxALL|wxEXPAND, 5 );


	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_treeListCtrl3 = new wxTreeListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE );

	bSizer2->Add( m_treeListCtrl3, 1, wxEXPAND | wxALL, 5 );


	m_panel2->SetSizer( bSizer2 );
	m_panel2->Layout();
	bSizer2->Fit( m_panel2 );
	mainSplitter->SplitVertically( m_panel1, m_panel2, 0 );
	baseSizer->Add( mainSplitter, 1, wxEXPAND, 5 );


	this->SetSizer( baseSizer );
	this->Layout();
	statusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	menuBar = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	wxMenuItem* menuAbout;
	menuAbout = new wxMenuItem( menuFile, wxID_ABOUT, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	menuFile->Append( menuAbout );

	wxMenuItem* menuQuit;
	menuQuit = new wxMenuItem( menuFile, wxID_EXIT, wxString( wxT("Close") ) + wxT('\t') + wxT("Ctrl-W"), wxEmptyString, wxITEM_NORMAL );
	menuFile->Append( menuQuit );

	menuBar->Append( menuFile, wxT("File") );

	this->SetMenuBar( menuBar );


	this->Centre( wxBOTH );
}

MainFrameBase::~MainFrameBase()
{
}
