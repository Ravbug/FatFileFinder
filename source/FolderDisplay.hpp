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

class FolderDisplay : public FolderDisplayBase{
public:
	DirectoryData* data;
	
	FolderDisplay(wxWindow*,wxWindow*, DirectoryData*);
	~FolderDisplay(){
		
	}
	
	void display();
	static string sizeToString(const fileSize&);
private:
	wxWindow* eventManager = nullptr;
	
	//event handlers
	void OnSelectionChanged(wxDataViewEvent&);
	void OnSelectionActivated(wxDataViewEvent&);
	wxDECLARE_EVENT_TABLE();
	
	#if defined __APPLE__ || defined __linux__
		//for drawing icons next to items in the list
		const std::unordered_map<string,wxString> icons = {
			{".exe", L"💾" },{".dl", L"💾" },{".bat", L"💾" },{".jar", "💾" },
			{".iso", L"💿" },{".bin", L"💿"},
			{".ai",L"🎨" },{".bmp", L"🎨" },{".gif", L"🎨" },{".ico", L"🎨" },{".jpeg", L"🎨" },{".jpg", L"🎨" },{".png",L"🎨" },{".psd",L"🎨" },{".svg",L"🎨" },{".tif",L"🎨" },
			{".mp3",L"🎵" },{".aif", L"🎵" },{".ogg", L"🎵" },{".wav", L"🎵" },{".wma", L"🎵" },
			{ ".mp4",L"🎞"},{ ".avi", L"🎞"},{ ".flv", L"🎞"},{ ".h264", L"🎞"},{ ".m4v", L"🎞"},{ ".mkv", L"🎞"},{ ".mov",L"🎞"},{ ".mpg",L"🎞"},{ ".wmv",L"🎞"},
			{ ".7z",L"📦"},{ ".arj", L"📦"},{ ".pkg", L"📦"},{ ".rar", L"📦"},{ ".rpm", L"📦"},{ ".tar.gz", L"📦"},{ ".z", L"📦"},{ ".zip",L"📦"},
			{ ".doc",L"📝"},{ ".docx", L"📝"},{ ".odt", L"📝"},{ ".pdf", L"📝"},{ ".rtf", L"📝"},{ ".tex", L"📝"}
		};
		const wxString FolderIcon = L"📁";
public:
		/**
		 Return the icon for a file type
		 @param data the directorydata of the file
		 @returns an emoji representing the file type
		 */
		wxString iconForExtension(const DirectoryData* data){
			//avoid crash checking unordered map for empty string
			if (!data->isFolder){
				string extension = std::filesystem::path(data->Path).extension();
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
			else{
				return FolderIcon;
			}
		}
	#elif defined _WIN32
		//on Windows, unicode is not supported (for now)
		const wxString FolderIcon = "";
		static wxString iconForExtension(const string& extension) {
			return "";
		}
	#endif
};
