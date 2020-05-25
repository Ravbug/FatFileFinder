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

	model = new FileSizeModel();

	ListCtrl->AssociateModel(model.get());
	//add rows here
	ListCtrl->AppendTextColumn("File Name",wxDATAVIEW_CELL_INERT,wxCOL_WIDTH_AUTOSIZE,static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE );
	ListCtrl->AppendProgressColumn("Percent",wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_CENTER), wxDATAVIEW_COL_SORTABLE );
	ListCtrl->AppendTextColumn("File Size",wxDATAVIEW_CELL_INERT, 100, static_cast<wxAlignment>(wxALIGN_RIGHT), 0 );

	//fix color on Windows
#if defined _WIN32
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ));
#endif
}

FolderDisplay::~FolderDisplay()
{
	abort = true;
	//Log("Resize operation has stopped because the view was closed. This folder will need to be manually resized.");
}

/**
Activated when the selection in the view is changed
@param event the event raised by the dataview
*/
void FolderDisplay::OnSelectionChanged(wxDataViewEvent& event){
	//notify parent to update sidebar display
	wxCommandEvent* evt = new wxCommandEvent(progEvt, SELEVT);
	//pass along the address to the DirectoryData to the event
	auto a = event.GetItem();
	if (a.IsOk()){
		uintptr_t* addr = new uintptr_t((ListCtrl->GetItemData(a)));
		evt->SetClientData(addr);
		eventManager->GetEventHandler()->QueueEvent(evt);
		event.Skip();
	}
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
	UpdateTitle();
	auto addItems = [&](vector<DirectoryData*>& list){
		for (DirectoryData* folder : list){
			AddItem(folder);
		}
	};
	addItems(data->subFolders);
	addItems(data->files);
	
	//set sort descending
	ListCtrl->GetColumn(1)->SetSortOrder(false);
	ListCtrl->GetColumn(0)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
	ListCtrl->GetColumn(2)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
	
	//fix size and force redraw
	SetClientSize(ListCtrl->GetSize());
	GetParent()->Layout();
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
	auto max = suffix.size();

	for (int i = 0; i < max; i++)
	{
		double compare = pow(size, i);
		if (fileSize <= compare || i == max - 1)
		{
			int minus = 0;
			if (i > 0)
			{
				minus = 1;
			}
			
			if (fileSize > compare){
				minus = 0;
			}
			
			//round to 2 decimal places (except for bytes), then attach unit
			char buffer[10];
			const string format = (i - minus == 0) ? "%.0f" : "%.2f";
			sprintf(buffer,format.c_str(),fileSize / pow(size,i-minus));
			formatted = string(buffer) + suffix[i - minus];
			break;
		}
	}

	if (formatted == ""){
		return "??";
	}
	return formatted;
}

/**
Calculate the size of a folder, including the size of subfolders. Does not allocate a new root.
@param fd the DirectoryData to size
@param progress the std::function to call with progress updates
*/
void FolderDisplay::SizeItem(DirectoryData* fd, const progCallback& progress){
	if (abort || path_too_long(fd->Path)) {
		return;
	}
	
	//skip symbolic links
	std::error_code ec;
	if (is_symlink(fd->Path,ec)){
		fd->size = 1;
		fd->isSymlink = true;
		return;
	}
	
	//calculate the size of the immediate files in the folder
	try{
		sizeImmediate(fd);
	}
	catch(const filesystem_error& e){
		//notify user
		Log("Error sizing directory" + fd->Path + "\n" + e.what());
		return;
	}
	
	fd->num_items = fd->files.size();
	
	//recursively size the folders in the folder
	bool calledback = false;
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
			calledback = true;
			progress((float)(i + 1) / fd->subFolders.size(), fd);
		}
	}
	//ensure callback is called for folders with no sub-items
	if (progress != nullptr && !calledback){
		progress(1, fd);
	}
}

/**
 Calculate the size of a folder, including the size of subfolders. Allocates a new DirectoryData.
 @param folder the path to the folder to size
 @param progress the std::function to call with progress updates
 */
DirectoryData* FolderDisplay::SizeItem(const string& folder, const progCallback& progress){
	DirectoryData* fd = new DirectoryData(folder, true);
	
	SizeItem(fd, progress);
	
	return fd;
}

/**
 Calculate the size of the immediate files in the folder
 @param data the FolderData struct to calculate
 */
void FolderDisplay::sizeImmediate(DirectoryData* data, const bool& skipFolders){
	//clear to prevent dupes
	data->files.clear();
	if (!skipFolders){
		data->subFolders.clear();
	}
	// iterate through the items in the folder
	for(auto& p : directory_iterator(data->Path,directory_options::skip_permission_denied)){
		//is the item a folder? if so, defer sizing it
		//check if can read the file
		try {
			file_status s = status(p.path());
			if (can_access(s))
			{
				if (is_directory(p)) {
					if (!skipFolders) {
						string str = p.path().string();
						DirectoryData* sub = new DirectoryData(str, true);
						data->subFolders.push_back(sub);
					}
				}
				else {
					//size the file, add its details to the structure

					string str = p.path().string();
					auto stat = stat_file_size(str);
					DirectoryData* file = new DirectoryData(str, stat);
					data->size += file->size;
					file->parent = data;
					data->files.push_back(file);
					//cout << file->Path << ": " << file->size << endl;
				}
			}
		}
		catch (const filesystem_error& e) {
			Log("Error sizing file " + p.path().string() + "\n" + e.what());
		}
		catch (const system_error& e) {
			Log(string("Error sizing item in directory ") + data->Path + "\n" + e.what());
		}
	}
}

/**
 Size the model representing this display on a background thread
 @param parent the item that owns this item
 @param updateItem the item in the parent that needs to be updated
 */
void FolderDisplay::Size(FolderDisplay* parent, wxDataViewItem updateItem){
	//reset items
	displayStartIndex = 0;
	ListCtrl->DeleteAllItems();
	abort = false;
	
	//reset / deallocate
	data->resetStats();
	
	if (parent != nullptr){
		reloadParent = parent;
		this->updateItem = updateItem;
	}
	
	worker = thread([&](FolderDisplay* parent){
		auto uicallback = [&](float prog, DirectoryData* updated){
			wxCommandEvent event(progEvt);
			event.SetId(PROGEVT);
			event.SetInt(prog * 100);
			event.SetClientData(updated);
			
			//invoke event to notify needs to update UI
			wxPostEvent(this, event);
			
		};
		//called on progress updates
		SizeItem(data, uicallback);
	},parent);
	worker.detach();
}

void FolderDisplay::OnUpdateUI(wxCommandEvent& event){
	//update pointer
	UpdateTitle(true);
	DirectoryData* fd = (DirectoryData*)event.GetClientData();
	//add the current folder
	if (fd->subFolders.size() > displayStartIndex){
		AddItem(fd->subFolders[displayStartIndex]);
	}
	
	//add files once
	if (displayStartIndex == 0){
		for (DirectoryData* file : fd->files){
			AddItem(file);
		}
		//fit
		ListCtrl->GetColumn(0)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
	}
	
	++displayStartIndex;
	//update progress
	int prog = event.GetInt();
		
		auto old_parent = data->parent;
		//update size in parent
		if (old_parent != nullptr){
			old_parent->size -= data->size;
			//deallocate old data
			//delete data;
			data = fd;
			old_parent->size += data->size;
			
			//reconnect item in SubFolders of parent
			for(int i = 0; i < old_parent->subFolders.size(); i++){
				DirectoryData* parentItem = old_parent->subFolders[i];
				if (parentItem->Path == data->Path){
					old_parent->subFolders[i] = data;
					break;
				}
			}
		}
		else{
			data = fd;
		}
		data->parent = old_parent;
		
		UpdateTitle(false);
	if (prog == 100){
		//update percents
		for (int i = 0; i < ListCtrl->GetItemCount(); i++){
			DirectoryData* d = (DirectoryData*)(ListCtrl->GetItemData(ListCtrl->RowToItem(i)));
			wxVariant v = wxAny((long)(d->percentOfParent()));
			ListCtrl->SetValue(v, i, 1);
		}
		
		//reconnect if applicable
		if (reloadParent != nullptr && updateItem.IsOk()){
			reloadParent->SetItemData(updateItem, fd);
			data->recalculateStats();
			reloadParent->SetItemText(sizeToString(fd->size), reloadParent->ItemToRow(updateItem), 2);
			reloadParent = nullptr;
		}
		abort = true;
		
		//set sort descending
		ListCtrl->GetColumn(1)->SetSortOrder(false);
	}
	
	//invoke main UI event
	//notify parent to connect new pointer to display
	
	wxCommandEvent* evt = new wxCommandEvent(progEvt, RESEVT);
	//pass along the address to the DirectoryData to the event
	uintptr_t* addr = new uintptr_t(prog);
	evt->SetClientData(addr);
	eventManager->GetEventHandler()->QueueEvent(evt);
}

#if defined _WIN32
wxString FolderDisplay::GetFileDescription(const string& path)
{
	/*auto pos = path.rfind('.');
	string extension;
	if (pos != string::npos) {
		extension = path.substr(pos,path.size());
	}
	else {
		extension = "";
	}*/

	SHFILEINFOW fileInfo;
	UINT sizeFile = sizeof(fileInfo);
	UINT Flags = SHGFI_TYPENAME | SHGFI_EXETYPE;

	SHGetFileInfoW(LPCWSTR(path.c_str()), 0, &fileInfo, sizeFile, Flags);

	return fileInfo.szTypeName;
}
#elif defined __APPLE__ || defined __linux__
/**
 Gets the description of the file using the File command
 @param path the path to the file
 @returns the string returned by file command
 */
wxString FolderDisplay::GetFileDescription(const string& path){
	char buffer[64];
    string result = "";
	auto command = string("file -b \"" + path + "\"");
	//open the pipe
	FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
		//if cannot read, return unknown type
		result = "Unknown";
    }
    pclose(pipe);
    return result;
}
#endif
