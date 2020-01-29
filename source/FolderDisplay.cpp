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
 @param parentWindow the wxWindow to send events to
 @param contents the FolderData to represent in this FolderDisplay
 @note nullptr is passed to the base class constructor
 @note Must call display() to display the contents (or update them)
 */
FolderDisplay::FolderDisplay(wxWindow* parentWindow, const DirectoryData* contents) : FolderDisplayBase(nullptr){
	eventManager = parentWindow;
	ItemName->SetLabel(path(contents->Path).filename().string());
}

/**
Activated when the selection in the view is changed
@param event the event raised by the dataview
*/
void FolderDisplay::OnSelectionChanged(wxDataViewEvent& event){
	//notify parent to update sidebar display
	event.Skip();
}

/**
Activated when the selection in the view is activated (double clicked or enter pressed, etc)
@param event the event raised by the dataview
*/
void FolderDisplay::OnSelectionActivated(wxDataViewEvent& event){

	event.Skip();
}

/**
 Displays the items in the DirectoryData in the data grid
 @pre data must not be nullptr
 */
void FolderDisplay::display(){
	for (DirectoryData* folder : data->subFolders){
		wxVector<wxVariant> items(3);
		items[0] = path(folder->Path).filename().string();
		items[1] = folder->percentOfParent();
		items[2] = sizeToString(folder->size);
	}
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
