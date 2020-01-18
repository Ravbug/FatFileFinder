//
//  interface.cpp
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
// This file contains the implementation for the main GUI.
// Place constructors and function definitons here.

#include "interface_derived.h"
#include <wx/generic/aboutdlgg.h>
#include <wx/aboutdlg.h>
#include <wx/gdicmn.h>
using namespace std::filesystem;

//include the icon file on linux
#ifdef __linux
#include "wxlin.xpm"
#include "wxlin_s.xpm"
#endif

//Declare event mapping here
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_OPEN, MainFrame::OnOpenFolder)
EVT_MENU(wxID_INDENT, MainFrame::OnSourceCode)
EVT_MENU(wxID_UP, MainFrame::OnUpdates)
EVT_MENU(wxID_INFO, MainFrame::OnToggleSidebar)
EVT_MENU(wxID_JUSTIFY_CENTER, MainFrame::OnToggleLog)
EVT_COMMAND(PROGEVT, progEvt, MainFrame::OnUpdateUI)
EVT_COMMAND(RELOADEVT, progEvt, MainFrame::OnUpdateReload)
EVT_COMMAND(LOGEVT, progEvt, MainFrame::OnLog)
EVT_BUTTON(wxID_OPEN, MainFrame::OnOpenFolder)
EVT_BUTTON(COPYPATH, MainFrame::OnCopy)
EVT_BUTTON(wxID_FIND, MainFrame::OnReveal)
EVT_BUTTON(wxID_STOP, MainFrame::OnAbort)
EVT_BUTTON(wxID_CLEAR, MainFrame::OnClearLog)
EVT_BUTTON(wxID_COPY, MainFrame::OnCopyLog)
EVT_MENU(wxID_REFRESH,MainFrame::OnReloadFolder)
EVT_BUTTON(wxID_REFRESH,MainFrame::OnReloadFolder)
EVT_TREELIST_ITEM_EXPANDING(TREELIST,MainFrame::OnListExpanding)
EVT_TREELIST_SELECTION_CHANGED(TREELIST,MainFrame::OnListSelection)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent) : MainFrameBase( parent )
{
	//perform any additional setup here
	SetLabel(AppName + " v" + AppVersion);
	
	//set the icon and fix scaling issues(windows and linux only)
	#ifdef _WIN32
		//name is the same as the one used in the resource file definition
		//the resource file definition must have the same ending as the name of the icon file itself
		//in this case, the icon file is wxwin.ico, so the definition is IDI_WXWIN
		SetIcon(wxIcon("IDI_WXWIN"));
	
		//fix DPI scaling
		dpi_scale(this);

		//set color
		SetBackgroundColour(*wxWHITE);

		//update names (no unicode support for now)
		openFolderBtn->SetLabel("Open");
		reloadFolderBtn->SetLabel("Reload");
		stopSizeBtn->SetLabel("Stop");

	#elif __linux
		SetIcon(wxIcon(wxICON(wxlin_s)));
	#elif __APPLE__
		//reveal button label lists Finder instead of Explorer
		revealBtn->SetLabel("Reveal in Finder");
	#endif
	
	//set up the default values for the left side table
#if defined __APPLE__ || defined __linux__
	string properties[] = {"Name","Size","Type","Items","Modified","Created","Accessed","Is Hidden", "Is Read Only","Is Executable","Is Symbolic Link", "mode_t types","Permissions","Size on Disk"};
#elif defined _WIN32
	string properties[] = {"Name","Size","Type","Items","Modified","Created","Accessed","Is Hidden", "Is Read Only","Is Executable", "Is Symbolic Link", "Is Archive", "Is Compressed", "Is Encrypted", "Integrity Stream", "No Indexing", "No Scrubbing", "Is Offline", "Recall on Access", "Is Reparse Point", "Is Sparse File", "Is System", "Is Temporary", "Is Virtual"};
#endif
	for (const string& p : properties){
		//pairs, because 2 columns
		wxVector<wxVariant> items;
		items.push_back(p);
		items.push_back("");
		//add to the view
		propertyList->AppendItem(items);
	}
	
	PLPropertyCol->SetWidth(wxCOL_WIDTH_AUTOSIZE);
	//create the sorting object
	fileBrowser->SetItemComparator(new sizeComparator());
	
	// default unsplit
	browserSplitter->Unsplit();
}

/**
 Size a folder on a background thread
 @param folder the path to the folder to size
 */
void MainFrame::SizeRootFolder(const string& folder){
	//deallocate existing data
	delete folderData;
	//clear the log
	logCtrl->SetValue("");
	//hide the log
	if (browserSplitter->IsSplit()) {
		wxCommandEvent e;
		OnToggleLog(e);
	}
	progIndex = 0;
	loaded.clear();
	//clear old cells
	fileBrowser->DeleteAllItems();
	//enable sizing 
	sizer.abort = false;
	userClosedLog = false;
	
	worker = thread([&](string folder){
		//called on progress updates
		progCallback callback = [&](float progress, DirectoryData* data){
			wxCommandEvent event(progEvt);
			event.SetId(PROGEVT);
			event.SetInt(progress * 100);
			event.SetClientData(data);
			
			//invoke event to notify needs to update UI
			wxPostEvent(this, event);
		};
		//ensure callback gets invoked for items with 0 subfolders
		DirectoryData* fin = sizer.SizeFolder(folder, callback);
		if (fin->subFolders.size() == 0){
			callback(1,fin);
		}
	},folder);
	worker.detach();
}

/**
 Populates a tree item's immediate sub-items
 @param item the tree item to add sub items
 @param data the FolderData pointer representing the data for the item
 */
void MainFrame::AddSubItems(const wxTreeListItem& item,DirectoryData* data){
	for(DirectoryData* d : data->subFolders){
		//add the item, with its client data pointer
		wxTreeListItem added = fileBrowser->AppendItem(item,FolderIcon + "\t" + path(d == nullptr? "[NULL]" : d->Path).filename().string(),wxTreeListCtrl::NO_IMAGE,wxTreeListCtrl::NO_IMAGE,new StructurePtrData(d));
		//set the other strings on the item
		if (d == nullptr) {
			fileBrowser->SetItemText(added, 2, "[not loaded]");
			fileBrowser->SetItemText(added, 1, "[not loaded]");
		}
		else {
			if (d->size > 0) {
				fileBrowser->SetItemText(added, 2, folderSizer::sizeToString(d->size));
				fileBrowser->SetItemText(added, 1, d->percentOfParent());
			}
			else {
				fileBrowser->SetItemText(added, 2, "[sizing]");
				fileBrowser->SetItemText(added, 1, "[waiting]");
			}
			//add placeholder to get disclosure triangle if there are sub items to show
			if (d->subFolders.size() > 0 || d->files.size() > 0) {
				fileBrowser->AppendItem(added, "Placeholder");
			}
			//add this item's files
			AddFiles(added, d);
		}
	}
}
/**
 Populate the sidebar with info about a particular item in the tree
 @param spd the StructurePtrData object stored in the tree cell
 */
void MainFrame::PopulateSidebar(StructurePtrData* spd){
	if (spd == nullptr){return;}
	
	//make sure item still exists
	DirectoryData* ptr;
	path p = spd->folderData->Path;
	if (!spd->folderData->isFolder){
		propertyList->SetTextValue(folderSizer::sizeToString(spd->folderData->size), 1, 1);
		string ext = p.extension().string();
		//special case for files with no extension
		propertyList->SetTextValue(iconForExtension(ext) + " " + (ext.size() == 0? "extensionless" : ext.substr(1)) + " File", 2, 1);
		propertyList->SetTextValue("",3,1);
	}
	else{
		propertyList->SetTextValue(folderSizer::sizeToString(spd->folderData->size), 1, 1);
		propertyList->SetTextValue(FolderIcon + " Folder", 2, 1);
		propertyList->SetTextValue(to_string(spd->folderData->num_items),3,1);
	}
	ptr = spd->folderData;
	propertyList->SetTextValue(p.filename().string(), 0, 1);
	
	//modified date
	propertyList->SetTextValue(timeToString(file_modify_time(ptr->Path)),4,1);
	propertyList->SetTextValue(timeToString(file_create_time(ptr->Path)), 5, 1);
	propertyList->SetTextValue(timeToString(file_access_time(ptr->Path)), 6, 1);
	
	//Is read only
	propertyList->SetTextValue(is_writable(ptr->Path)? "No" : "Yes", 8, 1);
	
	//Is executable
	propertyList->SetTextValue(is_executable(ptr->Path)? "Yes" : "No", 9, 1);
	
	//is symbolic link
	propertyList->SetTextValue(ptr->isSymlink? "Yes" : "No", 10, 1);
	
	//Is Hidden
	propertyList->SetTextValue(is_hidden(ptr->Path)? "Yes" : "No", 7, 1);
	
#if defined __APPLE__ || defined __linux__
	//mode_t
	propertyList->SetTextValue(modet_type_for(ptr->Path), 11, 1);

	//perms string
	propertyList->SetTextValue(permstr_for(ptr->Path), 12, 1);
	
	//Size on disk
	propertyList->SetTextValue(!ptr->isFolder? folderSizer::sizeToString(size_on_disk(ptr->Path)) : "-", 13, 1);
	
# elif defined _WIN32

	//file args windows
	auto args = file_attributes_for(ptr->Path);

	for (int i = 0; i < args.size(); i++) {
		propertyList->SetTextValue(args[i] ? "Yes" : "No", 11+i, 1);
	}

#endif
	//fix width
	PLValueCol->SetWidth(wxCOL_WIDTH_AUTOSIZE);
	
	//also show selected item in the status bar
	statusBar->SetStatusText(p.string());
}

/**
 Add the file listings for a folder in the tree view
 @param root the folder item to add the files to
 @param data the backing structure to set on each file
 */
void MainFrame::AddFiles(wxTreeListItem root, DirectoryData* data){
	//populate files
	for(DirectoryData* f : data->files){
		wxTreeListItem fileItem = fileBrowser->AppendItem(root, iconForExtension(path(f->Path).extension().string()) + "\t" + path(f->Path).filename().string(), wxTreeListCtrl::NO_IMAGE, wxTreeListCtrl::NO_IMAGE, new StructurePtrData(f));
		fileBrowser->SetItemText(fileItem, 2, folderSizer::sizeToString(f->size));
		fileBrowser->SetItemText(fileItem, 1, f->percentOfParent());
	}
}

/**
 Refresh the UI on progress updates
 @param event the command event from the sender. Must have client data set on it.
 */
void MainFrame::OnUpdateUI(wxCommandEvent& event){
	//update pointer
	DirectoryData* fd = (DirectoryData*)event.GetClientData();
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
		if (lastUpdateItem.IsOk()){
			lastUpdateItem = fileBrowser->GetNextSibling(lastUpdateItem);
			if (fd->subFolders[progIndex] != nullptr){
				auto totalSize = fd->subFolders[progIndex]->size;
				fileBrowser->SetItemText(lastUpdateItem, 2, folderSizer::sizeToString(totalSize));
				fileBrowser->SetItemData(lastUpdateItem, new StructurePtrData(fd->subFolders[progIndex]));
				//add placeholder if one is not already there
				if ((fd->subFolders[progIndex]->num_items > 0) && !fileBrowser->GetFirstChild(lastUpdateItem).IsOk()){
					fileBrowser->AppendItem(lastUpdateItem, "[Placeholder]");
				}
			}
		}
	}
	progIndex++;
	//set titlebar
	{
		string sizestr = folderSizer::sizeToString(fd->size);
		UpdateTitlebar(prog, sizestr);
	}
	
	//set percentage on completion
	if(progIndex == fd->subFolders.size()){
		wxTreeListItem item = fileBrowser->GetFirstChild(fileBrowser->GetRootItem());
		for(DirectoryData* data : fd->subFolders){
			if (data->size == 0) {
				fileBrowser->SetItemText(item, 1, "???");
				fileBrowser->SetItemText(item, 2, "[read error]");
			}
			else {
				fileBrowser->SetItemText(item, 1, data->percentOfParent());
			}
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
	
	DirectoryData* old = wrapper->reloadData;
	DirectoryData* repl = wrapper->folderData;
	
	//hook up the new pointers
	repl->parent = old->parent;
	*old = *repl;
	
	//recalculate the items, size values
	folderData->recalculateStats();
	
	//redraw the view
	fileBrowser->DeleteAllItems();
	
	//clear the cache
	loaded.clear();
	
	AddSubItems(fileBrowser->GetRootItem(),folderData);
	AddFiles(fileBrowser->GetRootItem(),folderData);
	
	//get percents
	wxTreeListItem item = fileBrowser->GetFirstChild(fileBrowser->GetRootItem());
	for(DirectoryData* data : folderData->subFolders){
		if (data->size == 0) {
			fileBrowser->SetItemText(item, 1, "???");
			fileBrowser->SetItemText(item, 2, "[read error]");
		}
		else {
			fileBrowser->SetItemText(item, 1, data->percentOfParent());
		}
		item = fileBrowser->GetNextSibling(item);
	}
	
	//update titlebar
	UpdateTitlebar(100,folderSizer::sizeToString(folderData->size));
}

void MainFrame::OnCopy(wxCommandEvent& event){
	//Get selected item
	wxTreeListItem selected = fileBrowser->GetSelection();
	
	//error check
	if (!selected.IsOk()){
		return;
	}
	
	StructurePtrData* ptr = (StructurePtrData*)fileBrowser->GetItemData(selected);
	
	//get the pointer to use
	DirectoryData* data = ptr->folderData;
	
	if (data == nullptr){
		//if no pointer, don't try to copy
		return;
	}
	//copy values to the clipboard
	if(wxTheClipboard->Open()){
		 wxTheClipboard->SetData( new wxTextDataObject(data->Path) );
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
	}
}

void MainFrame::OnReveal(wxCommandEvent& event){
	//Get selected item
	wxTreeListItem selected = fileBrowser->GetSelection();
	
	//error check
	if (!selected.IsOk()){
		return;
	}
	
	StructurePtrData* ptr = (StructurePtrData*)fileBrowser->GetItemData(selected);
	
	//get the pointer to use
	DirectoryData* data = ptr->folderData;
	if (ptr == nullptr){
		//if no pointer, don't try to open
		return;
	}
	reveal(data->Path);
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
	if (ptr == nullptr || ptr->folderData == nullptr || !ptr->folderData->isFolder){return;}
	
	//check if the folder still exists
	if (exists(ptr->folderData->Path)){
		//enable sizing 
		sizer.abort = false;
		userClosedLog = false;

		//Prepend item with same name, but [sizing for folder]
		wxTreeListItem replaced;
		try {
			replaced = fileBrowser->InsertItem(fileBrowser->GetItemParent(selected), selected, fileBrowser->GetItemText(selected));
		}
		catch (exception) {
			wxMessageBox("This item is not fully loaded.\nSize a folder completely before refreshing.", "Unable to size");
			return;
		}
		fileBrowser->SetItemText(replaced, 2, "[sizing]");
		
		//create a thread to size that folder, return when finished
		worker = thread([&](string folder,DirectoryData* oldptr){
			//called on progress update
			progCallback callback = [&](float progress, DirectoryData* data){
				//check that the item still exists
				if (int(progress*100) == 100 && replaced && replaced.IsOk()){
					//get the super folders that need to check for moves
					vector<DirectoryData*> superItems = oldptr->getSuperFolders();
					
					//update file sizes (modifying the data structure on multiple threads is a bad idea, but in this case it should be fine)
					for(DirectoryData* i : superItems){
						sizer.sizeImmediate(i,true);
					}
					
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
			DirectoryData* fin = sizer.SizeFolder(folder, callback);
			//ensure callback gets invoked for items with 0 subfolders
			if (fin->subFolders.size() == 0){
				callback(1,fin);
			}
		},ptr->folderData->Path,ptr->folderData);
		worker.detach();
		
		//remove selected item (do this after creating thread to avoid thread collisions)
		fileBrowser->DeleteItem(selected);
		//select the new item
		fileBrowser->Select(replaced);

	}
	else{
		//subtract from the size
		ptr->folderData->parent->size -= ptr->folderData->size;

		ptr->folderData->resetStats();
		
		//recalculate the items, size values
		ptr->folderData->recalculateStats();

		UpdateTitlebar(100, folderSizer::sizeToString(folderData->size));


		//remove the item if it does not exist
		fileBrowser->DeleteItem(selected);
	}
}

/**
 Called when a folder item is expanding. This will load the immediate sub items for that folder.
 @param event the wxTreeListEvent sent by the item that is expanding
 */
void MainFrame::OnListExpanding(wxTreeListEvent& event){
	
	//set cursor
	wxSetCursor(wxCURSOR_WAIT);
	
	wxTreeListItem item = event.GetItem();
	// add this item's immediate sub-items to the list
	StructurePtrData* spd = (StructurePtrData*)fileBrowser->GetItemData(item);
	DirectoryData* data = spd->folderData;
	string key = data->Path;
	//prevent trying to load already loaded items
	if (loaded.find(key) == loaded.end()){
		//remove the placeholder item
		wxTreeListItem placeholder = fileBrowser->GetFirstChild(item);
		if (placeholder.IsOk()){
			fileBrowser->DeleteItem(placeholder);
		}
		AddSubItems(item,data);
		if (data->parent == folderData && data->subFolders.size() == 0){
			AddFiles(item, data);
		}
	}
	//mark as loaded
	loaded.insert(key);
	
	//reset cursor
	wxSetCursor(wxNullCursor);
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
	wxDirDialog dlg(nullptr, message, "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
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
	//deallocate structure
	delete folderData;
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
	aboutInfo.SetCopyright("(C) 2019 Ravbug");
#if defined _WIN32
	aboutInfo.SetIcon(wxIcon("IDI_WXWIN"));
#elif defined __linux__
	aboutInfo.SetIcon(wxICON(wxlin_s));
#endif
	wxAboutBox(aboutInfo);
}

/**
Toggle the info sidebar
Called when the menu is activated
@param event the event from the caller (unused)
*/
void MainFrame::OnToggleSidebar(wxCommandEvent& event) {
	if (!mainSplitter->IsSplit()) {
		mainSplitter->SplitVertically(mainSplitter->GetWindow1(), propertyPanel);
		mainSplitter->SetSashPosition(mainSplitter->GetSize().x * 3.0 / 4);
		mainSplitter->UpdateSize();
		menuToggleSidebar->SetItemLabel("Hide Sidebar\tCtrl-I");
	}
	else {
		userClosedLog = true;
		mainSplitter->Unsplit();
		mainSplitter->UpdateSize();
		menuToggleSidebar->SetItemLabel("Show Sidebar\tCtrl-I");
	}
}

/**
Toggle the log panel
Called when the menu is activated
@param event the event from the caller (unused)
*/
void MainFrame::OnToggleLog(wxCommandEvent& event) {
	if (!browserSplitter->IsSplit()) {
		browserSplitter->SplitHorizontally(browserSplitter->GetWindow1(), logPanel);
		browserSplitter->SetSashPosition(browserSplitter->GetSize().x * 3.0 / 4);
		browserSplitter->UpdateSize();
		menuToggleLog->SetItemLabel("Hide Log\tCtrl-L");
	}
	else {
		browserSplitter->Unsplit();
		browserSplitter->UpdateSize();
		menuToggleLog->SetItemLabel("Show Log\tCtrl-L");
	}
}
