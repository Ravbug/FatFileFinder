//
//  FolderDisplay.cpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#include "FolderDisplay.hpp"
#include <array>

wxBEGIN_EVENT_TABLE(FolderDisplay, wxPanel)
EVT_DATAVIEW_SELECTION_CHANGED(FDISP, FolderDisplay::OnSelectionChanged)
EVT_DATAVIEW_ITEM_ACTIVATED(FDISP, FolderDisplay::OnSelectionActivated)
wxEND_EVENT_TABLE()

using namespace std;
using namespace std::filesystem;

/**
 Constructs a FolderDisplay given an event parent and a model
 @param parentWindow the parent wxWindow
 @param eventWindow the wxWindow to send events to
 @param contents the FolderData to represent in this FolderDisplay
 @note Must call display() to display the contents (or update them)
 */
FolderDisplay::FolderDisplay(wxWindow* parentWindow, wxWindow* eventWindow, DirectoryData* contents) : FolderDisplayBase(parentWindow){
	eventManager = eventWindow;
	data = contents;
}

/**
Activated when the selection in the view is changed
@param event the event raised by the dataview
*/
void FolderDisplay::OnSelectionChanged(wxDataViewEvent& event){
	//notify parent to update sidebar display
	wxCommandEvent* evt = new wxCommandEvent(progEvt, SELEVT);
	//pass along the address to the DirectoryData to the event
	uintptr_t* addr = new uintptr_t((ListCtrl->GetItemData(event.GetItem())));
	evt->SetClientData(addr);
	eventManager->GetEventHandler()->QueueEvent(evt);
	event.Skip();
}

/**
Activated when the selection in the view is activated (double clicked or enter pressed, etc)
@param event the event raised by the dataview
*/
void FolderDisplay::OnSelectionActivated(wxDataViewEvent& event){
	wxCommandEvent* evt = new wxCommandEvent(progEvt, ACTEVT);
	auto item = event.GetItem();
	if (item != nullptr){
		uintptr_t* addr = new uintptr_t((ListCtrl->GetItemData(item)));
		evt->SetClientData(addr);
		eventManager->GetEventHandler()->QueueEvent(evt);
	}
	
	event.Skip();
}

/**
 Displays the items in the DirectoryData in the data grid
 @pre data must not be nullptr
 */
void FolderDisplay::display(){
	ItemName->SetLabel(path(data->Path).filename().string() + " - " + sizeToString(data->size));
	auto addItems = [&](vector<DirectoryData*>& list){
		for (DirectoryData* folder : list){
			//no parent set? use the current associated model as the parent
			if (folder->parent == nullptr){
				folder->parent = data;
			}
			wxVector<wxVariant> items(3);
			path p = path(folder->Path);
			items[0] = iconForExtension(folder) + p.filename().string();
			items[1] = wxAny((long)(folder->percentOfParent()));
			items[2] = sizeToString(folder->size);
			//store the address that the pointer is referencing as the client data for the item
			//wxUIntPtr clientdata((uintptr_t)(folder));
			ListCtrl->AppendItem(items,(uintptr_t)(folder));
		}
	};
	addItems(data->subFolders);
	addItems(data->files);
}

/**
 Formats a raw file size to a string with a unit
 @param fileSize the size of the item in bytes
 @returns unitized string, example "12 KB"
 */
string FolderDisplay::sizeToString(const fileSize& fileSize){
	string formatted = "";
	int size = 1000;		//MB = 1000, MiB = 1024
	array<string,5> suffix { " bytes", " KB", " MB", " GB", " TB" };

	for (int i = 0; i < suffix.size(); i++)
	{
		double compare = pow(size, i);
		if (fileSize <= compare)
		{
			int minus = 0;
			if (i > 0)
			{
				minus = 1;
			}
			//round to 2 decimal places, then attach unit
			char buffer[10];
			sprintf(buffer,"%.2f",fileSize / pow(size,i-minus));
			formatted = string(buffer) + suffix[i - minus];
			break;
		}
	}

	return formatted;
}
