#pragma once

//
//  interface_derived.h
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the definition for GUI events and methods
// Define the user interface elements with wxFormBuilder

#include "interface.h"
#include "folder_sizer.hpp"

using namespace std;

class MainFrame : public MainFrameBase
{
public:
	MainFrame(wxWindow* parent = NULL);
private:
	folderSizer sizer;
	string GetPathFromDialog(const string& message);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnOpenFolder(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
