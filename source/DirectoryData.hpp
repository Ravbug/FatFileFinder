//
//  DirectoryData.hpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#pragma once
#include "globals.h"
using namespace std;

class DirectoryData{
public:
	string Path;
	//see typedefs for platform-specific types
	fileSize size;
	fileSize files_size;
	unsigned long num_items;
	bool isFolder;
	bool isSymlink;
	
	//for back navigation
	DirectoryData* parent = nullptr;
	
	//for holding items
	vector<DirectoryData*> subFolders;
	vector<DirectoryData*> files;
	
	DirectoryData(const string& inPath, bool folder){
		Path = inPath;
		isFolder = folder;
		size = 0;
		files_size = 1;
		num_items = 0;
		isSymlink = false;
	}
	DirectoryData(const string& inPath, fileSize inSize) : DirectoryData(inPath, false){
		size = inSize;
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
