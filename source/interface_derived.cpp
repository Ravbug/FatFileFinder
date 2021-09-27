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
using namespace std;
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
EVT_MENU(wxID_PROPERTIES, MainFrame::OnToggleSidebar)
EVT_MENU(wxID_JUSTIFY_FILL, MainFrame::OnToggleLog)
EVT_COMMAND(LOGEVT, progEvt, MainFrame::OnLog)
EVT_BUTTON(wxID_OPEN, MainFrame::OnOpenFolder)
EVT_BUTTON(COPYPATH, MainFrame::OnCopy)
EVT_BUTTON(wxID_FIND, MainFrame::OnReveal)
EVT_BUTTON(wxID_STOP, MainFrame::OnAbort)
EVT_BUTTON(wxID_CLEAR, MainFrame::OnClearLog)
EVT_BUTTON(wxID_COPY, MainFrame::OnCopyLog)
EVT_MENU(wxID_REFRESH,MainFrame::OnReloadFolder)
EVT_BUTTON(wxID_REFRESH,MainFrame::OnReloadFolder)
EVT_BUTTON(wxID_JUSTIFY_FILL,MainFrame::OnToggleLog)
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
		closeLogBtn->SetLabel("Close");

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
	//fileBrowser->SetItemComparator(new sizeComparator());
	
	// default unsplit
	browserSplitter->Unsplit();
	AddDisplay(nullptr);
}

/**
 Size a folder on a background thread
 @param folder the path to the folder to size
 */
void MainFrame::SizeRootFolder(const string& folder){
	//deallocate existing data
	currentDisplay[0]->Dealloc();
	//clear the log
	logCtrl->SetValue("");
	//hide the log
	if (browserSplitter->IsSplit()) {
		wxCommandEvent e;
		OnToggleLog(e);
	}

	userClosedLog = false;
	selected = nullptr;
	
	progCallback callback = [&](float progress, DirectoryData* data){
		wxCommandEvent event(progEvt);
		event.SetId(PROGEVT);
		event.SetInt(progress * 100);
		event.SetClientData(data);
		
		//invoke event to notify needs to update UI
		wxPostEvent(this, event);
	};
	currentDisplay[0]->Clear();
	currentDisplay[0]->SetData(new DirectoryData(folder, true));
	wxDataViewItem i;
	
	//reset viewing area
	for (int i = 1; i < currentDisplay.size(); i++){
        currentDisplay[i]->Dealloc();
		currentDisplay[i]->Destroy();
	}
	currentDisplay.erase(currentDisplay.begin()+1,currentDisplay.end());
	//update sizer size
	scrollSizer->SetCols(1);
	//force-update the scrolled window
	wxSize size = scrollView->GetBestVirtualSize();
	scrollView->SetVirtualSize( size );
	
	//start size
	currentDisplay[0]->Size(nullptr,i);
}

/**
 Populate the sidebar with info about a particular item in the tree
 @param ptr the DirectoryData object stored to load properties for
 */
void MainFrame::PopulateSidebar(DirectoryData* ptr){
	path p = ptr->Path;
	propertyList->SetTextValue(p.filename().string(), 0, 1);
	//make sure it exists
	if (!exists(ptr->Path)){
		for (int i = 1; i < propertyList->GetItemCount(); i++){
			propertyList->SetTextValue("[Deleted]", i, 1);
		}
		return;
	}
	
	propertyList->SetTextValue(ptr->isFolder? to_string(ptr->num_items) : "", 3, 1);
	propertyList->SetTextValue(FolderDisplay::sizeToString(ptr->size), 1, 1);

	string ext = p.extension().string();
	//special case for files with no extension
	propertyList->SetTextValue(FolderDisplay::GetFileDescription(ptr->Path), 2, 1);
	
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
	propertyList->SetTextValue(!ptr->isFolder? FolderDisplay::sizeToString(size_on_disk(ptr->Path)) : "-", 13, 1);
	
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

void MainFrame::OnCopy(wxCommandEvent& event){
	if (selected == nullptr){
		//if no pointer, don't try to copy
		return;
	}
	//copy values to the clipboard
	if(wxTheClipboard->Open()){
		wxTheClipboard->SetData( new wxTextDataObject(selected->Path) );
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
	}
}

void MainFrame::OnReveal(wxCommandEvent& event){
	//Get selected item

	//get the pointer to use
	if (selected == nullptr){
		//if no pointer, don't try to open
		return;
	}
	reveal(selected->Path);
}
/**
 Called when the reload button or reload menu is selected
 @param event (unused) command event from sender
 */
void MainFrame::OnReloadFolder(wxCommandEvent& event){
	//get the folder data that was last selected
	
	if (selected == nullptr || !(selected->isFolder)){return;}
	
	FolderDisplay* toReload = nullptr;
	int index = 0;
	int parent_idx = 0;
	for(FolderDisplay* disp : currentDisplay){
		if (selected->parent != nullptr && disp->GetPath() == selected->Path){
			parent_idx = index;
		}
		if (disp->GetPath() == selected->Path){
			toReload = disp;
			break;
		}
		index++;
	}
	
	//not opened? open it first
	if (toReload == nullptr){
		toReload = ChangeSelection(selected);
		toReload->SetData(selected);
	}
	
	FolderDisplay* fdisp = currentDisplay[parent_idx];
	auto item = fdisp->GetCurrentItem();
	
	//signal it to size again
	toReload->Size(fdisp, item);
	
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
    currentDisplay[0]->Dealloc();
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
	aboutInfo.SetCopyright("(C) 2020 Ravbug");
	aboutInfo.SetDescription(wxString::Format("wxWidgets %d.%d.%d.%d", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, wxSUBRELEASE_NUMBER));
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

/**
 Call to update the selected item. This will update the display area.
 @param sender the currently selected item.
 */
FolderDisplay* MainFrame::ChangeSelection(DirectoryData* sender){
	//find where the sender is in the list
	int idx;
	for (idx = 0; idx < currentDisplay.size(); idx++){
		if (currentDisplay[idx]->GetPath() == sender->parent->Path){
			break;
		}
	}
	//at end? only need to add another display
	if (idx < currentDisplay.size()-1){
		//remove from current display by deallocating
		for (int i = idx+1; i < currentDisplay.size(); i++){
			currentDisplay[i]->Destroy();
		}
		//update vector size
		currentDisplay.erase(currentDisplay.begin()+idx+1,currentDisplay.end());
		//update sizer size
		scrollSizer->SetCols(idx+1);
	}
	FolderDisplay* f = AddDisplay(sender);
	f->display();
	return f;
}
