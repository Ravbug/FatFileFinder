#pragma once

//
//  interface_derived.h
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the definition for GUI events and methods
// Define the user interface elements with wxFormBuilder

#include "globals.h"
#include "interface.h"
#include "folder_sizer.hpp"
#include "FolderDisplay.hpp"
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <wx/treebase.h>
#include <wx/clipbrd.h>

using namespace std;

class StructurePtrData;

/**
 Defines the main window and all of its behaviors and members.
 */
class MainFrame : public MainFrameBase
{
public:
	MainFrame(wxWindow* parent = nullptr);
	/**
	Log a message to the console
	@param msg the string to log
	*/
	void Log(const wxString& msg) {
		logCtrl->AppendText(msg + "\n");
		if (!userClosedLog && !browserSplitter->IsSplit()) {
			auto e = wxCommandEvent();
			OnToggleLog(e);
		}
	}
	void OnLog(wxCommandEvent& evt) {
		Log(evt.GetString());
	}
	void PopulateSidebar(DirectoryData*);
	
	void ChangeSelection(DirectoryData*);
	
	FolderDisplay* AddDisplay(DirectoryData* model){
		FolderDisplay* f = new FolderDisplay(scrollView,this,model);
		scrollSizer->SetCols(scrollSizer->GetCols()+1);
		scrollSizer->Add((wxPanel*)f,wxALL|wxEXPAND);
		scrollSizer->Layout();
		currentDisplay.push_back(f);
		return f;
	}
	
private:
	folderSizer sizer = folderSizer(this);
	DirectoryData* folderData = nullptr;
	thread worker;
	unordered_set<string> loaded;
	int progIndex = 0;
	wxDataViewItem lastUpdateItem;
	bool userClosedLog = false;

	string GetPathFromDialog(const string&);
	void SizeRootFolder(const string&);
	
	vector<FolderDisplay*> currentDisplay;
	
	void OnExit(wxCommandEvent&);
	void OnAbout(wxCommandEvent&);
	void OnOpenFolder(wxCommandEvent&);
	void OnReloadFolder(wxCommandEvent&);
	void OnUpdateUI(wxCommandEvent&);
	void OnUpdateReload(wxCommandEvent&);
	void OnCopy(wxCommandEvent&);
	void OnToggleSidebar(wxCommandEvent&);
	void OnToggleLog(wxCommandEvent&);
	void OnReveal(wxCommandEvent&);


	void OnSourceCode(wxCommandEvent&){
		wxLaunchDefaultBrowser("https://github.com/ravbug/FatFileFinderCPP/");
	}
	void OnUpdates(wxCommandEvent& event){
		wxLaunchDefaultBrowser("https://github.com/ravbug/FatFileFinderCPP/releases/latest");
	}
	void OnAbort(wxCommandEvent& event) {
		if (!sizer.abort) {
			wxMessageBox("File percentage calculations will be incorrect.\nReload an individual item to size it, or Open this folder and resize it to calculate all items.","Stopped Sizing");
			sizer.abort = true;
		}
	}
	void OnClearLog(wxCommandEvent& event) {
		logCtrl->SetValue("");
	}
	void OnCopyLog(wxCommandEvent& event) {
		//copy values to the clipboard
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(logCtrl->GetValue()));
			wxTheClipboard->Flush();
			wxTheClipboard->Close();
		}
	}
	void UpdateTitlebar(int prog, const string& size) {
		SetTitle(AppName + " v" + AppVersion + " - Sizing " + to_string(prog) + "% " + folderData->Path + " [" + size + "]");
	}
	wxDECLARE_EVENT_TABLE();
	
	/**
	 Disables sorting the tree view
	 @param direction (modify) the boolean to store the direction of the sort
	 @return the column that was set to unsort
	 */
	unsigned DisableSorting(bool& direction){
		unsigned uCol = -1;
//		if (fileBrowser->GetSortColumn(&uCol,&direction)) {
//			wxDataViewCtrl* pView = fileBrowser->GetDataView();
//			wxDataViewColumn* pCol = pView->GetColumn(uCol);
//			pCol->UnsetAsSortKey();
//		}
		return uCol;
	}
	
	/**
	 Re-enables sorting
	 @param col the column to use as the sort column
	 @param direction true to sort ascending, false to sort descending
	 */
	void EnableSorting(const unsigned& col, const bool& direction){
		if (col != -1){
//			fileBrowser->SetSortColumn(col, direction);
		}
	}
	
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
	DirectoryData* folderData = nullptr;
	DirectoryData* reloadData = nullptr;
	//constructors
	StructurePtrData(DirectoryData* data):wxTreeItemData(){
		folderData = data;
	}
};

/**
 Class for defining custom sorting in a wxTreeListCtrl.
 Overrides the Compare method.
 */
//class sizeComparator : public wxTreeListItemComparator{
//public:
//	//attribution: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
//	template <typename T> int sgn(T val) {
//		return (T(0) < val) - (val < T(0));
//	}
//	int Compare(wxTreeListCtrl *treelist, unsigned column, wxTreeListItem first, wxTreeListItem second){
//		//get client data
//		StructurePtrData* item1 = nullptr;
//		StructurePtrData* item2 = nullptr;
//		
//		item1 = (StructurePtrData*)treelist->GetItemData(first);
//		item2 = (StructurePtrData*)treelist->GetItemData(second);
//		
//		//error checking
//		if (!item1 || !item2){
//			return 0;
//		}
//
//		//return -1, 0, or 1 
//		return sgn(item1->folderData->size - item2->folderData->size);
//	}
//};
