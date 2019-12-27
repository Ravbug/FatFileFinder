///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/button.h>
#include <wx/gauge.h>
#include <wx/gbsizer.h>
#include <wx/treelist.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/splitter.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_INFO 1000
#define TREELIST 1001
#define COPYPATH 1002

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrameBase
///////////////////////////////////////////////////////////////////////////////
class MainFrameBase : public wxFrame
{
	private:

	protected:
		wxStatusBar* statusBar;
		wxMenuItem* menuToggleSidebar;
		wxButton* openFolderBtn;
		wxButton* reloadFolderBtn;
		wxButton* stopSizeBtn;
		wxGauge* progressBar;
		wxSplitterWindow* mainSplitter;
		wxTreeListCtrl* fileBrowser;
		wxPanel* propertyPanel;
		wxDataViewListCtrl* propertyList;
		wxDataViewColumn* PLPropertyCol;
		wxDataViewColumn* PLValueCol;
		wxButton* copyPathBtn;
		wxButton* revealBtn;

	public:

		MainFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Fat File Finder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 720,500 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrameBase();

		void mainSplitterOnIdle( wxIdleEvent& )
		{
			mainSplitter->SetSashPosition( 700 );
			mainSplitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainFrameBase::mainSplitterOnIdle ), NULL, this );
		}

};

