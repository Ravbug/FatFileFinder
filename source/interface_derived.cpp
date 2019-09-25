//
//  interface.cpp
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the implementation for the main GUI.
// Place constructors and function definitons here.

#include "interface_derived.h"
#include "globals.cpp"
#include <wx/generic/aboutdlgg.h>
#include <wx/aboutdlg.h>

//include the icon file on linux
#ifdef __linux
#include "wxlin.xpm"
#endif

#define PROGEVT 2001
#define RELOADEVT 2002

wxDEFINE_EVENT(progEvt, wxCommandEvent);

//Declare event mapping here
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_OPEN,MainFrame::OnOpenFolder)
EVT_COMMAND(PROGEVT, progEvt, MainFrame::OnUpdateUI)
EVT_COMMAND(RELOADEVT,progEvt, MainFrame::OnUpdateReload)
EVT_BUTTON(wxID_OPEN, MainFrame::OnOpenFolder)
EVT_BUTTON(COPYPATH, MainFrame::OnCopy)
EVT_MENU(wxID_REFRESH,MainFrame::OnReloadFolder)
EVT_BUTTON(wxID_REFRESH,MainFrame::OnReloadFolder)
EVT_TREELIST_ITEM_EXPANDING(TREELIST,MainFrame::OnListExpanding)
EVT_TREELIST_SELECTION_CHANGED(TREELIST,MainFrame::OnListSelection)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent) : MainFrameBase( parent )
{
	//perform any additional setup here
	
	//set the icon and fix scaling issues(windows and linux only)
	#ifdef _WIN32
		//name is the same as the one used in the resource file definition
		//the resource file definition must have the same ending as the name of the icon file itself
		//in this case, the icon file is wxwin.ico, so the definition is IDI_WXWIN
		SetIcon(wxIcon("IDI_WXWIN"));
	
		//fix DPI scaling
		dpi_scale(this);
	#elif __linux
		SetIcon(wxIcon(wxICON(wxlin)));
	#endif
	
	//set up the default values for the left side table
	string properties[] = {"Name","Size","Type","Items","Modified","Is System","Is Hidden", "Is Read Only"};
	for (const string& p : properties){
		//pairs, because 2 columns
		wxVector<wxVariant> items;
		items.push_back(p);
		items.push_back("");
		//add to the view
		propertyList->AppendItem(items);
	}
	
	//create the sorting object
	fileBrowser->SetItemComparator(new sizeComparator());
}

/**
 Size a folder on a background thread
 @param folder the path to the folder to size
 */
void MainFrame::SizeRootFolder(const string& folder){
	//deallocate existing data
	delete folderData;
	progIndex = 0;
	loaded.clear();
	//clear old cells
	fileBrowser->DeleteAllItems();
	
	worker = thread([&](string folder){
		//called on progress updates
		progCallback callback = [&](float progress, FolderData* data){			
			wxCommandEvent event(progEvt);
			event.SetId(PROGEVT);
			event.SetInt(progress * 100);
			event.SetClientData(data);
			
			//invoke event to notify needs to update UI
			wxPostEvent(this, event);
		};
		sizer.SizeFolder(folder, callback);
	},folder);
	worker.detach();
}

/**
 Populates a tree item's immediate sub-items
 @param item the tree item to add sub items
 @param data the FolderData pointer representing the data for the item
 */
void MainFrame::AddSubItems(const wxTreeListItem& item,FolderData* data){
	for(FolderData* d : data->subFolders){
		//add the item, with its client data pointer
		wxTreeListItem added = fileBrowser->AppendItem(item,FolderIcon + "\t" + d->Path.leaf().string(),wxTreeListCtrl::NO_IMAGE,wxTreeListCtrl::NO_IMAGE,new StructurePtrData(d));
		//set the other strings on the item
		if (d->total_size > 0){
			fileBrowser->SetItemText(added, 2, folderSizer::sizeToString(d->total_size));
			fileBrowser->SetItemText(added,1,folderSizer::percentOfParent(d));
		}
		else{
			fileBrowser->SetItemText(added,2,"[sizing]");
			fileBrowser->SetItemText(added,1,"[waiting]");
		}
		
		//add placeholder to get disclosure triangle if there are sub items to show
		if(d->subFolders.size() > 0 || d->files.size() > 0){
			fileBrowser->AppendItem(added, "");
		}
		//add this item's files
		AddFiles(added,d);
	}
}
/**
 Populate the sidebar with info about a particular item in the tree
 @param spd the StructurePtrData object stored in the tree cell
 */
void MainFrame::PopulateSidebar(StructurePtrData* spd){
	if (spd == NULL){return;}
	path p;
	if (spd->fileData != NULL){
		p = spd->fileData->Path;
		propertyList->SetTextValue(folderSizer::sizeToString(spd->fileData->size), 1, 1);
		string ext = p.extension().string();
		//special case for files with no extension
		propertyList->SetTextValue(iconForExtension(ext) + " " + (ext.size() == 0? "binary" : ext.substr(1)) + " File", 2, 1);
		propertyList->SetTextValue("",3,1);
	}
	else{
		p = spd->folderData->Path;
		propertyList->SetTextValue(folderSizer::sizeToString(spd->folderData->total_size), 1, 1);
		propertyList->SetTextValue(FolderIcon + " Folder", 2, 1);
		propertyList->SetTextValue(to_string(spd->folderData->num_items),3,1);
		
	}
	propertyList->SetTextValue(p.leaf().string(), 0, 1);
		
	//modified date
	tm* time;
	time_t tm = last_write_time(p);
	time = localtime(&tm);
	char dateString[100];
	strftime(dateString,50,"%x %X",time);
	propertyList->SetTextValue(dateString,4,1);
	
	//also show selected item in the status bar
	statusBar->SetStatusText(p.string());
	
}

/**
 Add the file listings for a folder in the tree view
 @param root the folder item to add the files to
 @param data the backing structure to set on each file
 */
void MainFrame::AddFiles(wxTreeListItem root, FolderData* data){
	//populate files
	for(FileData* f : data->files){
		wxTreeListItem fileItem = fileBrowser->AppendItem(root,iconForExtension(f->Path.extension().string()) + "\t" + f->Path.leaf().string(),wxTreeListCtrl::NO_IMAGE,wxTreeListCtrl::NO_IMAGE,new StructurePtrData(f));
		fileBrowser->SetItemText(fileItem, 2, folderSizer::sizeToString(f->size));
		fileBrowser->SetItemText(fileItem, 1,folderSizer::percentOfParent(f));
	}
}

/**
 Refresh the UI on progress updates
 @param event the command event from the sender. Must have client data set on it.
 */
void MainFrame::OnUpdateUI(wxCommandEvent& event){
	//update pointer
	FolderData* fd = (FolderData*)event.GetClientData();
	folderData = fd;
	//update progress
	int prog = event.GetInt();
	progressBar->SetValue(prog);
	
	//if this is first progress update, populate the whole table
	if(progIndex == 0){
		AddSubItems(fileBrowser->GetRootItem(), fd);
		lastUpdateItem = fileBrowser->GetFirstChild(fileBrowser->GetRootItem());
		
		AddFiles(fileBrowser->GetRootItem(),fd);
	}
	else{
		//update the most recent leaf
		lastUpdateItem = fileBrowser->GetNextSibling(lastUpdateItem);
		unsigned long totalSize = fd->subFolders[progIndex]->total_size;
		fileBrowser->SetItemText(lastUpdateItem, 2, folderSizer::sizeToString(totalSize));
		fileBrowser->SetItemData(lastUpdateItem, new StructurePtrData(fd->subFolders[progIndex]));
		if (fd->subFolders[progIndex]->num_items > 0){
			fileBrowser->AppendItem(lastUpdateItem, "");
		}
	}
	progIndex++;
	//set titlebar
	SetTitle(AppName + " - Sizing " + to_string(prog) + "% " + fd->Path.string() + " [" + folderSizer::sizeToString(fd->total_size) + "]");
	
	//set percentage on completion
	if(progIndex == fd->subFolders.size()){
		wxTreeListItem item = fileBrowser->GetFirstChild(fileBrowser->GetRootItem());
		for(FolderData* data : fd->subFolders){
			fileBrowser->SetItemText(item, 1,folderSizer::percentOfParent(data));
			item = fileBrowser->GetNextSibling(item);
		}
		//sort the collumns descending
		fileBrowser->SetSortColumn(1,false);
	}
}

/**
 Called when a reload folder operation finishes
 */
void MainFrame::OnUpdateReload(wxCommandEvent& event){
	StructurePtrData* wrapper = (StructurePtrData*)event.GetClientData();
	//set the item's client data
	fileBrowser->SetClientData(wrapper->folderData);
	
	//hook up the new pointer
	wrapper->folderData->parent = wrapper->reloadData->parent;
	wrapper->reloadData = wrapper->folderData;
	
	//get the super folders that need to check for moves
	vector<FolderData*> superItems = sizer.getSuperFolders(wrapper->reloadData);
	
	//update file sizes
	for(FolderData* i : superItems){
		sizer.sizeImmediate(i,true);
	}
	
	//recalculate the items, size values
	folderSizer::recalculateStats(folderData);
	
	//redraw the view
	fileBrowser->DeleteAllItems();
	
	//clear the cache
	loaded.clear();
	
	AddSubItems(fileBrowser->GetRootItem(),folderData);
	AddFiles(fileBrowser->GetRootItem(),folderData);
	
	//get percents
	wxTreeListItem item = fileBrowser->GetFirstChild(fileBrowser->GetRootItem());
	for(FolderData* data : folderData->subFolders){
		fileBrowser->SetItemText(item, 1,folderSizer::percentOfParent(data));
		item = fileBrowser->GetNextSibling(item);
	}
}

void MainFrame::OnCopy(wxCommandEvent& event){
	//platform specific code
}

/**
 Called when the reload button or reload menu is selected
 @param event (unused) command event from sender
 */
void MainFrame::OnReloadFolder(wxCommandEvent& event){
	//Get selected item
	wxTreeListItem selected = fileBrowser->GetSelection();
	
	//error check
	if (!selected.IsOk()){
		return;
	}
	
	StructurePtrData* ptr = (StructurePtrData*)fileBrowser->GetItemData(selected);
	
	//only resize folders, if a file is selected, stop
	if (ptr == NULL || ptr->folderData == NULL){return;}
	
	//check if the folder still exists
	if (exists(ptr->folderData->Path)){
		//Prepend item with same name, but [sizing for folder]
		wxTreeListItem replaced = fileBrowser->InsertItem(fileBrowser->GetItemParent(selected),selected, fileBrowser->GetItemText(selected));
		fileBrowser->SetItemText(replaced, 2, "[sizing]");
		//remove selected item
		fileBrowser->DeleteItem(selected);
		//select the new item
		fileBrowser->Select(replaced);
		
		//create a thread to size that folder, return when finished
		worker = thread([&](string folder,FolderData* oldptr){
			//called on progress update
			progCallback callback = [&](float progress, FolderData* data){
				//check that the item still exists
				if (int(progress*100) == 100 && replaced && replaced.IsOk()){
					//set up event event
					wxCommandEvent event(progEvt);
					event.SetId(RELOADEVT);
					event.SetInt(progress * 100);
					//needs to pass both the list item and the updated client data
					StructurePtrData* dataWrap = new StructurePtrData(data);
					dataWrap->reloadData = oldptr;
					event.SetClientData(dataWrap);
					
					//invoke event to notify needs to update UI
					wxPostEvent(this, event);
				}
			};
			sizer.SizeFolder(folder, callback);
		},ptr->folderData->Path.string(),ptr->folderData);
		worker.detach();

	}
	else{
		//remove the item if it does not exist
		fileBrowser->DeleteItem(selected);
		//recalculate the items, size values
		folderSizer::recalculateStats(ptr->folderData);
	}
}

/**
 Called when a folder item is expanding. This will load the immediate sub items for that folder.
 @param event the wxTreeListEvent sent by the item that is expanding
 */
void MainFrame::OnListExpanding(wxTreeListEvent& event){
	wxTreeListItem item = event.GetItem();
	// add this item's immediate sub-items to the list
	StructurePtrData* spd = (StructurePtrData*)fileBrowser->GetItemData(item);
	FolderData* data = spd->folderData;
	string key = data->Path.string();
	//prevent trying to load already loaded items
	if (loaded.find(key) == loaded.end()){
		//remove the placeholder item
		wxTreeListItem placeholder = fileBrowser->GetFirstChild(item);
		if (placeholder.IsOk()){
			fileBrowser->DeleteItem(placeholder);
		}
		AddSubItems(item,data);
	}
	//mark as loaded
	loaded.insert(key);
}

/**
 Called when an item in the list is selected. Populates the sidebar.
 @param event the wxTreeListEvent sent by the selected item
 */
void MainFrame::OnListSelection(wxTreeListEvent& event){
	//get selection information
	wxTreeListItem item = event.GetItem();
	// add this item's immediate sub-items to the list
	StructurePtrData* spd = (StructurePtrData*)fileBrowser->GetItemData(item);
	//populate the sidebar
	PopulateSidebar(spd);
	
}

/** Brings up a folder selection dialog with a prompt
 * @param message the prompt for the user
 * @return path selected, or an empty string if nothing chosen
 */
string MainFrame::GetPathFromDialog(const string& message)
{
	//present the dialog
	wxDirDialog dlg(NULL, message, "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() == wxID_CANCEL) {
		return string("");
	}
	//get the path and return the standard string version
	wxString path = dlg.GetPath();
	return path.ToStdString();
}


//definitions for the events
/**
 Called when the open menu or button is selected
 @param event (unused) event from sender
 */
void MainFrame::OnOpenFolder(wxCommandEvent& event){
	string path = GetPathFromDialog("Select a folder to size");
	if (path != ""){
		//begin sizing folder
		SizeRootFolder(path);
	}
}
/**
 Closes the app
 */
void MainFrame::OnExit(wxCommandEvent& event)
{
	Close( true );
}
/**
 Shows an about dialog with app details
 */
void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName(AppName);
	aboutInfo.SetVersion(AppVersion);
	aboutInfo.SetDescription(_("Fill about info here."));
	aboutInfo.SetCopyright("(C) 2019");
	aboutInfo.SetWebSite("https://github.com/ravbug/FatFileFinderCPP");
	aboutInfo.AddDeveloper("Ravbug");
	wxAboutBox(aboutInfo);
}
/**
 Aborts the background sizing operation
 */
void MainFrame::OnStopSizing(wxCommandEvent& event){
	//update the UI
}
