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
	DirectoryData* folderData = NULL;
	thread worker;
	unordered_set<string> loaded;
	int progIndex = 0;
	wxTreeListItem lastUpdateItem;

	string GetPathFromDialog(const string& message);
	void AddSubItems(const wxTreeListItem& item, DirectoryData* data);
	void SizeRootFolder(const string& folder);
	void PopulateSidebar(StructurePtrData* data);
	void AddFiles(wxTreeListItem root, DirectoryData* data);
	
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
	void OnReveal(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
	
#if defined __APPLE__ || defined __linux__
	//for drawing icons next to items in the list
	const unordered_map<string,wxString> icons = {
		{".exe", L"💾" },{".dl", L"💾" },{".bat", L"💾" },{".jar", "💾" },
		{".iso", L"💿" },{".bin", L"💿"},
		{".ai",L"🎨" },{".bmp", L"🎨" },{".gif", L"🎨" },{".ico", L"🎨" },{".jpeg", L"🎨" },{".jpg", L"🎨" },{".png",L"🎨" },{".psd",L"🎨" },{".svg",L"🎨" },{".tif",L"🎨" },
		{".mp3",L"🎵" },{".aif", L"🎵" },{".ogg", L"🎵" },{".wav", L"🎵" },{".wma", L"🎵" },
		{ ".mp4",L"🎞"},{ ".avi", L"🎞"},{ ".flv", L"🎞"},{ ".h264", L"🎞"},{ ".m4v", L"🎞"},{ ".mkv", L"🎞"},{ ".mov",L"🎞"},{ ".mpg",L"🎞"},{ ".wmv",L"🎞"},
		{ ".7z",L"📦"},{ ".arj", L"📦"},{ ".pkg", L"📦"},{ ".rar", L"📦"},{ ".rpm", L"📦"},{ ".tar.gz", L"📦"},{ ".z", L"📦"},{ ".zip",L"📦"},
		{ ".doc",L"📝"},{ ".docx", L"📝"},{ ".odt", L"📝"},{ ".pdf", L"📝"},{ ".rtf", L"📝"},{ ".tex", L"📝"}
	};
	const wxString FolderIcon = L"📁";
	/**
	 Return the icon for a file type
	 @param extension the extension of the file
	 @returns an emoji representing the file type
	 */
	wxString iconForExtension(const string& extension){
		//avoid crash checking unordered map for empty string
		if (extension.size() == 0){
			return L"📟";
		}
		try{
			return icons.at(extension);
		}
		catch(exception e){
			return L"📄";
		}
	}
#elif defined _WIN32
	//on Windows, unicode is not supported (for now)
	const wxString FolderIcon = "";
	wxString iconForExtension(const string& extension) {
		return "";
	}
#endif
};

/**
 Class for storing data within the tree.
 This class does not delete its pointers when it is deallocated. Those must be deleted manually.
 */
class StructurePtrData : public wxTreeItemData{
public:
	DirectoryData* folderData = NULL;
	DirectoryData* reloadData = NULL;
	//constructors
	StructurePtrData(DirectoryData* data):wxTreeItemData(){
		folderData = data;
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
		StructurePtrData* item2 = (StructurePtrData*)treelist->GetItemData(second);
		
		//error checking
		if (!item1 || !item2){
			return 0;
		}
		
		//get needed data
		double size1 = 0;
		double size2 = 0;
		//calculate the percentages
		size1 = (double)item1->folderData->size;
	
		size2 = (double)item2->folderData->size;
	
		//return the difference
		return size1 - size2;
	}
};
