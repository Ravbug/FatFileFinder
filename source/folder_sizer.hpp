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
using namespace std;

#ifdef __APPLE__
	#include <boost/filesystem.hpp>
	#include <boost/range/iterator_range.hpp>
	using namespace boost::filesystem;
	#define ofstream_scope boost::filesystem
#else
#include <filesystem>
using namespace std::filesystem;
#endif



struct FolderData;

//structure definitions
struct FileData{
	path Path;
	
#ifdef __APPLE__
	unsigned long size = 0;
	time_t modifyDate;
#else
	uintmax_t size = 0;
	file_time_type modifyDate;
#endif
	//for back navigation
	FolderData* parent = NULL;
	~FileData(){
		parent = NULL;
	}
};
struct FolderData : FileData{
	unsigned long files_size = 1;
	unsigned long num_items = 0;
	vector<FolderData*> subFolders;
	vector<FileData*> files;
	//destructor
	~FolderData(){
		//deallocate each of the files
		for(FileData* file : files){
			delete file;
		}
		//deallocate each of the subfolders
		for(FolderData* folder : subFolders){
			delete folder;
		}
	}
};

//callback definitions
typedef function<void(float progress, FolderData* data)> progCallback;

/**
 Class that calculates the sizes of folders.
 */
class folderSizer{
public:
	folderSizer();
	~folderSizer();
	FolderData* SizeFolder(const string& folder, const progCallback& progress);
	void sizeImmediate(FolderData* data, const bool& skipFolders = false);
	vector<FolderData*> getSuperFolders(FolderData* data);
	static string percentOfParent(FileData* data);
	static string sizeToString(const unsigned long& fileSize);
	static void recalculateStats(FolderData* data);
};
