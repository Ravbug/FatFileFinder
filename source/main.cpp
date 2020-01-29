//
//  main.cpp
//  wxWidgetsTemplate
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the definition for a wxWidgets application. It configures it and launches it.
// There is no int main function.

// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "interface_derived.h"

class FatFileFinder: public wxApp
{
public:
    virtual bool OnInit();
    virtual int FilterEvent(wxEvent&);
    MainFrame* frame;
};

wxIMPLEMENT_APP(FatFileFinder);
bool FatFileFinder::OnInit()
{
    frame = new MainFrame( );
    frame->Show( true );
    return true;
}

//for catching global events
int FatFileFinder::FilterEvent(wxEvent& event) {
	//logging
    if (event.GetId() == LOGEVT && event.IsCommandEvent()) {
        wxCommandEvent* ce = (wxCommandEvent*)(event.Clone());
        frame->OnLog(*ce);
        delete ce;
        return true;
    }
	//on selection change
	else if (event.GetId() == SELEVT && event.IsCommandEvent()){
		wxCommandEvent* ce = (wxCommandEvent*)(event.Clone());
		//extract the pointer from the client data
		uintptr_t* ptr = (uintptr_t*)(ce->GetClientData());
		DirectoryData* data = (DirectoryData*)*ptr;
		frame->PopulateSidebar(data);
		delete ce;
		return true;
	}
    return -1;
}
