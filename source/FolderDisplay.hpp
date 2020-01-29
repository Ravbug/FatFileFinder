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

class FolderDisplay : FolderDisplayBase{
public:
	FolderDisplay(wxWindow*, const DirectoryData*);
	
	void display();
	static string sizeToString(const fileSize&);
private:
	wxWindow* eventManager = nullptr;
	DirectoryData* data;
	
	//event handlers
	void OnSelectionChanged(wxDataViewEvent&);
	void OnSelectionActivated(wxDataViewEvent&);
	wxDECLARE_EVENT_TABLE();
};
