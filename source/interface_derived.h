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
#include <unordered_set>
#include <wx/treebase.h>

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
	unordered_set<string> loaded;
	void AddSubItems(const wxTreeListItem& item,FolderData* data);
	void SizeRootFolder(const string& folder);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnOpenFolder(wxCommandEvent& event);
	void OnStopSizing(wxCommandEvent& event);
	void OnUpdateUI(wxCommandEvent& event);
	void OnListExpanding(wxTreeListEvent& event);
	wxDECLARE_EVENT_TABLE();
};

/**
 Class for storing data within the tree.
 This class does not delete its pointers when it is deallocated. Those must be deleted manually.
 */
class StructurePtrData : public wxTreeItemData{
public:
	FolderData* folderData = NULL;
	FileData* fileData = NULL;
	//constructors
	StructurePtrData(FolderData* data):wxTreeItemData(){
		folderData = data;
	}
	StructurePtrData(FileData* data):wxTreeItemData(){
		fileData = data;
	}
};
