//
//  folder_sizer.hpp
//  mac
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <functional>

#ifdef __APPLE__
	#include <boost/filesystem.hpp>
	#include <boost/range/iterator_range.hpp>
	using namespace boost::filesystem;
	#define ofstream_scope boost::filesystem
#endif


using namespace std;

//structure definitions
struct FileData{
	path Path;
	unsigned long size;
};
struct FolderData{
	path Path;
	unsigned long files_size = 0;
	unsigned long num_items = 0;
	unsigned long total_size = 0;
	vector<FolderData> subFolders;
	vector<FileData> files;
};

//callback definitions
typedef function<void(float progress, FolderData* data)> progCallback;

class folderSizer{
public:
	folderSizer();
	~folderSizer();
	FolderData* SizeFolder(const string& folder, const progCallback& progress);
};
