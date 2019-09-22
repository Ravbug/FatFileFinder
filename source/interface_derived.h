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
#include <thread>

using namespace std;

class MainFrame : public MainFrameBase
{
public:
	MainFrame(wxWindow* parent = NULL);
private:
	folderSizer sizer;
	FolderData* folderData = NULL;
	string GetPathFromDialog(const string& message);
	thread worker;
	void SizeRootFolder(const string& folder);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnOpenFolder(wxCommandEvent& event);
	void OnStopSizing(wxCommandEvent& event);
	void OnUpdateUI(wxCommandEvent& event);
	void OnListExpanding(wxTreeListEvent& event);
	wxDECLARE_EVENT_TABLE();
};
