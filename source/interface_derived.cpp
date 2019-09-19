//
//  interface.cpp
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the implementation for the main GUI.
// Place constructors and function definitons here.

#include "interface_derived.h"
#include "globals.cpp"


//include the icon file on linux
#ifdef __linux
#include "wxlin.xpm"
#endif

//Declare event mapping here
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent) : MainFrameBase( parent )
{
	//perform any additional setup here
	
	//set the icon and fix scaling issues(windows and linux only)
	#ifdef _WIN32
		//name is the same as the one used in the resource file definition
		//the resource file definition must have the same ending as the name of the icon file itself
		//in this case, the icon file is wxwin.ico, so the definition is IDI_WXWIN
		SetIcon(wxIcon("IDI_WXWIN"));
	
		//fix DPI scaling
		dpi_scale(this);
	#elif __linux
		SetIcon(wxIcon(wxICON(wxlin)));
	#endif

}

//definitions for the events
void MainFrame::OnExit(wxCommandEvent& event)
{
	Close( true );
}
void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox( "This is a wxWidgets' Hello world sample", "About Hello World", wxOK | wxICON_INFORMATION );
}
void MainFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
}
