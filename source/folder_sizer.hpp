//
//  folder_sizer.hpp
//  mac
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <functional>
#include <vector>
#include <wx/event.h>
#include "DirectoryData.hpp"
using namespace std;

#ifdef __APPLE__
//	#include <boost/filesystem.hpp>
//	#include <boost/range/iterator_range.hpp>
//	using namespace boost::filesystem;
//	#define ofstream_scope boost::filesystem
#else
#endif

//callback definitions
typedef function<void(float progress, DirectoryData* data)> progCallback;

/**
 Class that calculates the sizes of folders.
 */
class folderSizer{
public:
	bool abort = true;
	wxWindow* ParentWindow;

	folderSizer(wxWindow*);
	~folderSizer();
	DirectoryData* SizeFolder(const string&, const progCallback&);
	void sizeImmediate(DirectoryData*, const bool& skipFolders = false);

	/**
	Display a message in the log
	@param msg the string to display
	*/
	void Log(const string& msg) {
		wxCommandEvent* evt = new wxCommandEvent(progEvt, LOGEVT);
		evt->SetString(msg);
		ParentWindow->GetEventHandler()->QueueEvent(evt);
	}

};
