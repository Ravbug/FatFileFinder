//
//  FolderDisplay.cpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#include "FolderDisplay.hpp"
#include <array>
#include <thread>

wxBEGIN_EVENT_TABLE(FolderDisplay, wxPanel)
EVT_DATAVIEW_SELECTION_CHANGED(FDISP, FolderDisplay::OnSelectionChanged)
EVT_DATAVIEW_ITEM_ACTIVATED(FDISP, FolderDisplay::OnSelectionActivated)
EVT_COMMAND(PROGEVT, progEvt, FolderDisplay::OnUpdateUI)
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
			AddItem(folder);
		}
	};
	addItems(data->subFolders);
	addItems(data->files);
}

/**
 Add an item to the display
 @param folder the item to add to the display
 */
void FolderDisplay::AddItem(DirectoryData* folder){
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

/**
 Calculate the size of a folder, including the size of subfolders
 @param folder the path to the folder to size
 @param progress the std::function to call with progress updates
 */
DirectoryData* FolderDisplay::SizeItem(const string& folder, const progCallback& progress){
	DirectoryData* fd = new DirectoryData(folder, true);
	
	if (abort || path_too_long(folder)) {
		return fd;
	}
	
	//skip symbolic links
	std::error_code ec;
	if (is_symlink(path(folder),ec)){
		fd->size = 1;
		fd->isSymlink = true;
		return fd;
	}
	
	//calculate the size of the immediate files in the folder
	try{
		sizeImmediate(fd);
	}
	catch(filesystem_error e){
		//notify user
		Log("Error sizing directory" + folder + "\n" + e.what());
		return fd;
	}
	
	fd->size = fd->files_size;
	fd->num_items = fd->files.size();
	
	//recursively size the folders in the folder
	for (int i = 0; i < fd->subFolders.size(); i++){
		fd->subFolders[i] = SizeItem(fd->subFolders[i]->Path, nullptr);
		
		//update parent
		if (fd->subFolders[i] != nullptr){
			fd->subFolders[i]->parent = fd;
			fd->num_items += fd->subFolders[i]->num_items + 1;
			fd->size += fd->subFolders[i]->size;
			//check for zero size
			if (fd->size == 0){
				fd->size = 1;
			}
		}
		if (progress != nullptr) {
			progress((float)(i + 1) / fd->subFolders.size(), fd);
		}
	}

	return fd;
}

/**
 Calculate the size of the immediate files in the folder
 @param data the FolderData struct to calculate
 */
void FolderDisplay::sizeImmediate(DirectoryData* data, const bool& skipFolders){
	//clear to prevent dupes
	data->files.clear();
	data->files_size = 1;
	if (!skipFolders){
		data->subFolders.clear();
	}
	// iterate through the items in the folder
	for(auto& p : directory_iterator(data->Path,directory_options::skip_permission_denied)){
		//is the item a folder? if so, defer sizing it
		//check if can read the file
		try {
			file_status s = status(p.path());
			if (/*!is_symlink(s) &&*/ can_access(s))
			{
				if (is_directory(p)) {
					if (!skipFolders) {
						DirectoryData* sub = new DirectoryData(p.path().string(), true);
						data->subFolders.push_back(sub);
					}
				}
				else {
					//size the file, add its details to the structure
					DirectoryData* file = new DirectoryData(p.path().string(), stat_file_size(p.path().string()));
					data->files_size += file->size;
					file->parent = data;
					data->files.push_back(file);
					//cout << file->Path << ": " << file->size << endl;
				}
			}
		}
		catch (filesystem_error e) {
			Log("Error sizing file " + p.path().string() + "\n" + e.what());
		}
	}
}

/**
 Size the model representing this display on a background thread
 @param callback the function to call for progress updates
 */
void FolderDisplay::Size(const progCallback& callback){
	//reset items
	displayStartIndex = 0;
	ListCtrl->DeleteAllItems();
	abort = false;
	
	worker = thread([&](){
		auto uicallback = [&](float prog, DirectoryData* updated){
			wxCommandEvent event(progEvt);
			event.SetId(PROGEVT);
			event.SetInt(prog * 100);
			event.SetClientData(updated);
			
			//invoke event to notify needs to update UI
			wxPostEvent(this, event);
		};
		//called on progress updates
		SizeItem(data->Path, uicallback);
	});
	worker.detach();
}

void FolderDisplay::OnUpdateUI(wxCommandEvent& event){
	//update pointer
	DirectoryData* fd = (DirectoryData*)event.GetClientData();
	data = fd;
	//add the current folder
	AddItem(fd->subFolders[displayStartIndex]);
	
	
	++displayStartIndex;
	//update progress
	int prog = event.GetInt();
	
	cout << prog << endl;
	
	//add files once
	if (prog == 100){
		//update percents
		auto count = data->subFolders.size();
		for (int i = 0; i < count; i++){
			wxVariant v = wxAny((long)(data->subFolders[i]->percentOfParent()));
			ListCtrl->SetValue(v, i, 1);
		}
		for (DirectoryData* file : fd->files){
			AddItem(file);
		}
	}
	
	//progressBar->SetValue(prog);
}
