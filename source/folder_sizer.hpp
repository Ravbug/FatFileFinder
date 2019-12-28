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
typedef int64_t fileSize;


//structure definitions
struct DirectoryData{
	string Path;
	//see typedefs for platform-specific types
	fileSize size = 0;
	time_t modifyDate;
	
	bool isFolder;
	
	DirectoryData(const string& inPath, bool folder){
		Path = inPath;
		isFolder = folder;
	}
	DirectoryData(const string& inPath, fileSize inSize){
		Path = inPath;
		size = inSize;
		isFolder = false;
	}

	//for back navigation
	DirectoryData* parent = NULL;
	
	fileSize files_size = 1;
	unsigned long num_items = 0;
	vector<DirectoryData*> subFolders;
	vector<DirectoryData*> files;
	//destructor
	~DirectoryData(){
		//deallocate each of the files
		for(DirectoryData* file : files){
			delete file;
		}
		//deallocate each of the subfolders
		for(DirectoryData* folder : subFolders){
			delete folder;
		}
		subFolders.clear();
		files.clear();
	}
	
};

//callback definitions
typedef function<void(float progress, DirectoryData* data)> progCallback;

/**
 Class that calculates the sizes of folders.
 */
class folderSizer{
public:
	folderSizer();
	~folderSizer();
	DirectoryData* SizeFolder(const string&, const progCallback&);
	void sizeImmediate(DirectoryData*, const bool& skipFolders = false);
	vector<DirectoryData*> getSuperFolders(DirectoryData*);
	static string percentOfParent(DirectoryData*);
	static string sizeToString(const fileSize&);
	static void recalculateStats(DirectoryData*);
};
