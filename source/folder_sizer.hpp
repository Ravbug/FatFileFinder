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


/**
 Class that calculates the sizes of folders.
 */
class folderSizer{
public:
	bool abort = true;
	wxWindow* ParentWindow;

	folderSizer(wxWindow*);
	~folderSizer();


};
