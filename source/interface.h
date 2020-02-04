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
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/splitter.h>
#include <wx/dataview.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define COPYPATH 1000
#define FDISP 1001

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrameBase
///////////////////////////////////////////////////////////////////////////////
class MainFrameBase : public wxFrame
{
	private:

	protected:
		wxStatusBar* statusBar;
		wxMenuItem* menuToggleSidebar;
		wxMenuItem* menuToggleLog;
		wxButton* openFolderBtn;
		wxButton* reloadFolderBtn;
		wxButton* stopSizeBtn;
		wxGauge* progressBar;
		wxSplitterWindow* mainSplitter;
		wxSplitterWindow* browserSplitter;
		wxScrolledWindow* scrollView;
		wxGridBagSizer* scrollSizer;
		wxPanel* logPanel;
		wxTextCtrl* logCtrl;
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

		void browserSplitterOnIdle( wxIdleEvent& )
		{
			browserSplitter->SetSashPosition( 800 );
			browserSplitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainFrameBase::browserSplitterOnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class FolderDisplayBase
///////////////////////////////////////////////////////////////////////////////
class FolderDisplayBase : public wxPanel
{
	private:

	protected:
		wxStaticText* ItemName;
		wxDataViewListCtrl* ListCtrl;
		wxDataViewColumn* nameCol;
		wxDataViewColumn* percentCol;
		wxDataViewColumn* sizeCol;

	public:

		FolderDisplayBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~FolderDisplayBase();

};

