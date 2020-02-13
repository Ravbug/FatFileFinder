//
//  FolderDisplay.hpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#pragma once
#include "interface.h"
#include "DirectoryData.hpp"
#include <filesystem>
#include <unordered_map>
#include <thread>

//callback definitions
typedef function<void(float progress, DirectoryData* data)> progCallback;

class FolderDisplay : public FolderDisplayBase{
public:
	DirectoryData* data;
	
	FolderDisplay(wxWindow*,wxWindow*, DirectoryData*);
	~FolderDisplay(){
		abort = true;
		//Log("Resize operation has stopped because the view was closed. This folder will need to be manually resized.");
	}
	
	void Size(FolderDisplay*, wxDataViewItem);
	
	/**
	 Blanks the display. Use display() to show items again.
	 */
	void Clear(){
		ListCtrl->DeleteAllItems();
	}
	wxDataViewItem GetCurrentItem(){
		return ListCtrl->GetSelection();
	}
	void SetItemData(const wxDataViewItem& item, DirectoryData* data){
		ListCtrl->SetItemData(item, (uintptr_t)data);
	}
	void SetItemText(const wxString& str, unsigned int row, unsigned int col){
		ListCtrl->SetTextValue(str, row, col);
	}
	int ItemToRow(const wxDataViewItem& item){
		return ListCtrl->ItemToRow(item);
	}
	
	void display();
	static string sizeToString(const fileSize&);
	bool abort = true;
private:
	wxWindow* eventManager = nullptr;
	std::thread worker;
	int displayStartIndex = 0;
	
	FolderDisplay* reloadParent = nullptr;
	wxDataViewItem updateItem;
	
	/**
	Display a message in the log
	@param msg the string to display
	*/
	void Log(const string& msg) {
		wxCommandEvent* evt = new wxCommandEvent(progEvt, LOGEVT);
		evt->SetString(msg);
		if (eventManager != nullptr){
			eventManager->GetEventHandler()->QueueEvent(evt);
		}
	}
	DirectoryData* SizeItem(const string&, const progCallback&);
	void sizeImmediate(DirectoryData*, const bool& skipFolders = false);
	void AddItem(DirectoryData*);
	
	//event handlers
	void OnSelectionChanged(wxDataViewEvent&);
	void OnSelectionActivated(wxDataViewEvent&);
	void OnUpdateUI(wxCommandEvent&);
	
	
	/**
	 Sets the label in this display to the size of the item
	 @param isSizing pass true if the size operation is in progress, false otherwise
	 @note If the size of the current DirectoryData is 0, the item's size will display as Needs reload because the minimum size FatFileFinder reports is 1 byte.
	 */
	void UpdateTitle(bool isSizing = false){
		ItemName->SetLabel((isSizing? "(Sizing) " : "") + std::filesystem::path(data->Path).filename().string() + " - " + (data->size == 0? "Needs reload" : sizeToString(data->size)));
	}
	wxDECLARE_EVENT_TABLE();
	
public:
	#if defined __APPLE__ || defined __linux__
		/**
		 Return the icon for a file type
		 @param data the directorydata of the file
		 @returns an emoji representing the file type
		 */
		static wxString iconForExtension(const DirectoryData* data){
			//for drawing icons next to items in the list
			static const std::unordered_map<string,wxString> icons = {
				{"exe", L"💾" },{"dll", L"💾" },{"bat", L"💾" },{"jar", "💾" },
				{"iso", L"💿" },{"bin", L"💿"},
				{"ai",L"🎨" },{"bmp", L"🎨" },{"gif", L"🎨" },{"ico", L"🎨" },{"jpeg", L"🎨" },{"jpg", L"🎨" },{"png",L"🎨" },{"psd",L"🎨" },{"svg",L"🎨" },{"tif",L"🎨" },
				{"mp3",L"🎵" },{"aif", L"🎵" },{"ogg", L"🎵" },{"wav", L"🎵" },{"wma", L"🎵" },{"m4a", L"🎵" },
				{ "mp4",L"🎞"},{ "avi", L"🎞"},{ "flv", L"🎞"},{ "h264", L"🎞"},{ "m4v", L"🎞"},{ "mkv", L"🎞"},{ "mov",L"🎞"},{ "mpg",L"🎞"},{ "wmv",L"🎞"},
				{ "7z",L"📦"},{ "arj", L"📦"},{ "pkg", L"📦"},{ "rar", L"📦"},{ "rpm", L"📦"},{ "tar.gz", L"📦"},{ "z", L"📦"},{ "zip",L"📦"},
				{ "doc",L"📝"},{ "docx", L"📝"},{ "odt", L"📝"},{ "pdf", L"📝"},{ "rtf", L"📝"},{ "tex", L"📝"}
			};
			static const wxString FolderIcon = L"📁";
			//avoid crash checking unordered map for empty string
			if (!data->isFolder){
				string extension = std::filesystem::path(data->Path).extension();
				if (extension.size() == 0){
					return L"📟";
				}
				try{
					extension = extension.substr(1,extension.size());
					return icons.at(extension);
				}
				catch(exception e){
					return L"📄";
				}
			}
			else{
				return FolderIcon;
			}
		}
	#elif defined _WIN32
		//on Windows, unicode is not supported (for now)
		static const wxString FolderIcon = "";
		static wxString iconForExtension(const DirectoryData* data) {
			return "";
		}
	#endif
};
