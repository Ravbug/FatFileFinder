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
	typedef time_t fileTime;
	typedef size_t fileSize;
	#include <boost/filesystem.hpp>
	#include <boost/range/iterator_range.hpp>
	using namespace boost::filesystem;
	#define ofstream_scope boost::filesystem
#else
	#include <filesystem>
	using namespace std::filesystem;
	typedef file_time_type fileTime;
	typedef unitmax_t fileSize;
#endif

//structure definitions
struct FileData{
	path Path;
	
	//see typedefs for platform-specific types
	fileSize size = 0;
	fileTime modifyDate;

	//constructors
	FileData(){}
	FileData(path inPath, fileSize inSize){
		Path = inPath;
		size = inSize;
	}
	//for back navigation
	FileData* parent = NULL;
	virtual ~FileData(){}
};
struct FolderData : FileData{
	unsigned long files_size = 1;
	unsigned long num_items = 0;
	vector<FolderData*> subFolders;
	vector<FileData*> files;
	//destructor
	virtual ~FolderData(){
		//deallocate each of the files
		for(FileData* file : files){
			delete file;
		}
		//deallocate each of the subfolders
		for(FolderData* folder : subFolders){
			delete folder;
		}
		subFolders.clear();
		files.clear();
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
