//
//  DirectoryData.hpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#pragma once
using namespace std;

typedef int64_t fileSize;

class DirectoryData{
public:
	string Path;
	//see typedefs for platform-specific types
	fileSize size = 0;
	fileSize files_size = 1;
	unsigned long num_items = 0;
	bool isFolder = false;
	bool isSymlink = false;
	
	//for back navigation
	DirectoryData* parent = NULL;
	
	//for holding items
	vector<DirectoryData*> subFolders;
	vector<DirectoryData*> files;
	
	DirectoryData(const string& inPath, bool folder){
		Path = inPath;
		isFolder = folder;
	}
	DirectoryData(const string& inPath, fileSize inSize){
		Path = inPath;
		size = inSize;
		isFolder = false;
	}
	//destructor
	~DirectoryData(){
		resetStats();
	}
	
	void resetStats();
	void recalculateStats();
	vector<DirectoryData*> getSuperFolders();
	string percentOfParent();
};
