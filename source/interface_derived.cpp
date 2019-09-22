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

wxDEFINE_EVENT(progEvt, wxCommandEvent);
wxDEFINE_EVENT(errorEvt, wxCommandEvent);

//Declare event mapping here
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_OPEN,MainFrame::OnOpenFolder)
EVT_COMMAND(wxID_ANY, progEvt, MainFrame::OnUpdateUI)
EVT_BUTTON(wxID_OPEN, MainFrame::OnOpenFolder)
EVT_TREELIST_ITEM_EXPANDING(TREELIST,MainFrame::OnListExpanding)
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
	string properties[] = {"Name","Size","Type","Items","Modified","Created","Is System","Is Hidden", "Is Read Only", "Full Path"};
	for (const string& p : properties){
		//pairs, because 2 columns
		wxVector<wxVariant> items;
		items.push_back(p);
		items.push_back("");
		//add to the view
		propertyList->AppendItem(items);
	}
	
}

/**
 Size a folder on a background thread
 @param folder the path to the folder to size
 */
void MainFrame::SizeRootFolder(const string& folder){
	//deallocate existing data
	delete folderData;
	
	//clear old cells
	fileBrowser->DeleteAllItems();
	
	worker = thread([&](string folder){
		//called on progress updates
		progCallback callback = [&](float progress, FolderData* data){			
			wxCommandEvent event(progEvt);
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
 Refresh the UI on progress updates
 */
void MainFrame::OnUpdateUI(wxCommandEvent& event){
	//update pointer
	FolderData* fd = (FolderData*)event.GetClientData();
	folderData = fd;
	//update progress
	int prog = event.GetInt();
	progressBar->SetValue(prog);
	if (prog == 100){
		cout << fd->total_size << endl;
		
		//update tree view (make this not refresh every time)
		fileBrowser->DeleteAllItems();
		
		AddSubItems(fileBrowser->GetRootItem(),fd);
		
	}
	//set titelbar
	SetTitle(AppName + " - Sizing " + to_string(prog) + "% " + fd->Path.string());
}

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

void MainFrame::AddSubItems(const wxTreeListItem& item,FolderData* data){
	for(FolderData* d : data->subFolders){
		//add the item, with its client data pointer
		wxTreeListItem added = fileBrowser->AppendItem(item,FolderIcon + "\t" + d->Path.leaf().string(),wxTreeListCtrl::NO_IMAGE,wxTreeListCtrl::NO_IMAGE,new StructurePtrData(d));
		//set the other strings on the item
		fileBrowser->SetItemText(added, 1, folderSizer::sizeToString(d->total_size));
		
		//add placeholder to get disclosure triangle if there are sub items to show
		if(d->subFolders.size() > 0 || d->files.size() > 0){
			fileBrowser->AppendItem(added, "placeholder");
		}
//
		//add this items' files
		for(FileData* f : d->files){
			wxTreeListItem fileItem = fileBrowser->AppendItem(added,iconForExtension(f->Path.extension().string()) + "\t" + f->Path.leaf().string(),wxTreeListCtrl::NO_IMAGE,wxTreeListCtrl::NO_IMAGE,new StructurePtrData(f));
			fileBrowser->SetItemText(fileItem, 1, folderSizer::sizeToString(f->size));
		}
	}
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
