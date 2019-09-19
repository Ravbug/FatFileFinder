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

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MainFrame *frame = new MainFrame( );
    frame->Show( true );
    return true;
}
