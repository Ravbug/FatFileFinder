#pragma once

//
//  interface_derived.h
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the definition for GUI events and methods
// Define the user interface elements with wxFormBuilder

#include "globals.cpp"
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
	/**
	Log a message to the console
	@param msg the string to log
	*/
	void Log(const string& msg) {
		wxCommandEvent event(progEvt, LOGEVT);
		event.SetString(msg);
		wxPostEvent(this, event);
	}
private:
	folderSizer sizer;
	DirectoryData* folderData = NULL;
	thread worker;
	unordered_set<string> loaded;
	int progIndex = 0;
	wxTreeListItem lastUpdateItem;
	bool userClosedLog = false;

	string GetPathFromDialog(const string&);
	void AddSubItems(const wxTreeListItem&, DirectoryData*);
	void SizeRootFolder(const string&);
	void PopulateSidebar(StructurePtrData*);
	void AddFiles(wxTreeListItem root, DirectoryData*);
	
	void OnExit(wxCommandEvent&);
	void OnAbout(wxCommandEvent&);
	void OnOpenFolder(wxCommandEvent&);
	void OnReloadFolder(wxCommandEvent&);
	void OnUpdateUI(wxCommandEvent&);
	void OnUpdateReload(wxCommandEvent&);
	void OnListExpanding(wxTreeListEvent&);
	void OnListSelection(wxTreeListEvent&);
	void OnCopy(wxCommandEvent&);
	void OnReveal(wxCommandEvent&);
	void OnLog(wxCommandEvent& evt) {
		logCtrl->AppendText((evt.GetString()) + "\n");
		if (!userClosedLog && !browserSplitter->IsSplit()) {
			OnToggleLog(evt);
		}
	}
	void OnSourceCode(wxCommandEvent&){
		wxLaunchDefaultBrowser("https://github.com/ravbug/FatFileFinderCPP/");
	}
	void OnUpdates(wxCommandEvent& event){
		wxLaunchDefaultBrowser("https://github.com/ravbug/FatFileFinderCPP/releases/latest");
	}
	void OnToggleSidebar(wxCommandEvent& event){
		if (!mainSplitter->IsSplit()){
			mainSplitter->SplitVertically(mainSplitter->GetWindow1(),propertyPanel);
			mainSplitter->SetSashPosition(mainSplitter->GetSize().x * 3.0/4);
			mainSplitter->UpdateSize();
			menuToggleSidebar->SetItemLabel("Hide Sidebar\tCtrl-I");
		}
		else{
			userClosedLog = true;
			mainSplitter->Unsplit();
			mainSplitter->UpdateSize();
			menuToggleSidebar->SetItemLabel("Show Sidebar\tCtrl-I");
		}
	}
	void OnToggleLog(wxCommandEvent& event){
		if (!browserSplitter->IsSplit()){
			browserSplitter->SplitHorizontally(browserSplitter->GetWindow1(),logPanel);
			browserSplitter->SetSashPosition(browserSplitter->GetSize().x * 3.0/4);
			browserSplitter->UpdateSize();
			menuToggleLog->SetItemLabel("Hide Log\tCtrl-L");
		}
		else{
			browserSplitter->Unsplit();
			browserSplitter->UpdateSize();
			menuToggleLog->SetItemLabel("Show Log\tCtrl-L");
		}
	}
	void OnAbort(wxCommandEvent& event) {
		if (!sizer.abort) {
			wxMessageBox("Stopped Sizing");
			sizer.abort = true;
		}
	}
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
	//attribution: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}
	int Compare(wxTreeListCtrl *treelist, unsigned column, wxTreeListItem first, wxTreeListItem second){
		//get client data
		StructurePtrData* item1 = (StructurePtrData*)treelist->GetItemData(first);
		StructurePtrData* item2 = (StructurePtrData*)treelist->GetItemData(second);
		
		//error checking
		if (!item1 || !item2){
			return 0;
		}

		//return -1, 0, or 1 
		return sgn(item1->folderData->size - item2->folderData->size);
	}
};
