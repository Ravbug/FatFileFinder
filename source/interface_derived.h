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
#include <unordered_map>
#include <wx/treebase.h>

using namespace std;

class StructurePtrData;

/**
 Defines the main window and all of its behaviors and members.
 */
class MainFrame : public MainFrameBase
{
public:
	MainFrame(wxWindow* parent = NULL);
private:
	folderSizer sizer;
	FolderData* folderData = NULL;
	thread worker;
	unordered_set<string> loaded;
	int progIndex = 0;
	wxTreeListItem lastUpdateItem;

	string GetPathFromDialog(const string& message);
	void AddSubItems(const wxTreeListItem& item,FolderData* data);
	void SizeRootFolder(const string& folder);
	void PopulateSidebar(StructurePtrData* data);
	void AddFiles(wxTreeListItem root, FolderData* data);
	
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnOpenFolder(wxCommandEvent& event);
	void OnStopSizing(wxCommandEvent& event);
	void OnReloadFolder(wxCommandEvent& event);
	void OnUpdateUI(wxCommandEvent& event);
	void OnUpdateReload(wxCommandEvent& event);
	void OnListExpanding(wxTreeListEvent& event);
	void OnListSelection(wxTreeListEvent& event);
	void OnCopy(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
	
	//for drawing icons next to items in the list
	unordered_map<string,string> icons = {
		{".exe", "💾" },{".dll", "💾" },{".bat", "💾" },{".jar", "💾" },
		{".iso", "💿" },{".bin", "💿"},
		{".ai","🎨" },{".bmp","🎨" },{".gif","🎨" },{".ico","🎨" },{".jpeg","🎨" },{".jpg","🎨" },{".png","🎨" },{".psd","🎨" },{".svg","🎨" },{".tif","🎨" },
		{".mp3","🎵" },{".aif","🎵" },{".ogg","🎵" },{".wav","🎵" },{".wma","🎵" },
		{ ".mp4","🎞"},{ ".avi","🎞"},{ ".flv","🎞"},{ ".h264","🎞"},{ ".m4v","🎞"},{ ".mkv","🎞"},{ ".mov","🎞"},{ ".mpg","🎞"},{ ".wmv","🎞"},
		{ ".7z","📦"},{ ".arj","📦"},{ ".pkg","📦"},{ ".rar","📦"},{ ".rpm","📦"},{ ".tar.gz","📦"},{ ".z","📦"},{ ".zip","📦"},
		{ ".doc","📝"},{ ".docx","📝"},{ ".odt","📝"},{ ".pdf","📝"},{ ".rtf","📝"},{ ".tex","📝"}
	};
	const string FolderIcon = "📁";
	/**
	 Return the icon for a file type
	 @param extension the extension of the file
	 @returns an emoji representing the file type
	 */
	string iconForExtension(const string& extension){
		//avoid crash checking unordered map for empty string
		if (extension.size() == 0){
			return "📟";
		}
		try{
			return icons.at(extension);
		}
		catch(exception e){
			return "📄";
		}
	}
};

/**
 Class for storing data within the tree.
 This class does not delete its pointers when it is deallocated. Those must be deleted manually.
 */
class StructurePtrData : public wxTreeItemData{
public:
	FolderData* folderData = NULL;
	FolderData* reloadData = NULL;
	FileData* fileData = NULL;
	//constructors
	StructurePtrData(FolderData* data):wxTreeItemData(){
		folderData = data;
	}
	StructurePtrData(FileData* data):wxTreeItemData(){
		fileData = data;
	}
};

/**
 Class for defining custom sorting in a wxTreeListCtrl.
 Overrides the Compare method.
 */
class sizeComparator : public wxTreeListItemComparator{
public:
	int Compare(wxTreeListCtrl *treelist, unsigned column, wxTreeListItem first, wxTreeListItem second){
		//get client data
		StructurePtrData* item1 = (StructurePtrData*)treelist->GetItemData(first);
		StructurePtrData* item2 = (StructurePtrData*)treelist->GetItemData(first);
		
		//error checking
		if (!item1 || !item2){
			return 0;
		}
		
		//get the size to use
		unsigned long size1;
		unsigned long size2;
		if (item1->folderData != NULL){
			size1 = item1->folderData->total_size;
		}
		else{
			size1 = item1->fileData->size;
		}
		if (item2->folderData != NULL){
			size2 = item2->folderData->total_size;
		}
		else{
			size2 = item2->fileData->size;
		}
		
		//negative = sort first, positive = sort second
		return (size1 > size2)? 1 : -1;
	}
};
