//
//  DirectoryData.cpp
//  mac
//
//  Created by Main on 1/13/20.
//  Copyright © 2020 Ravbug. All rights reserved.
//

#include "DirectoryData.hpp"
#include <filesystem>
using namespace filesystem;

/**
 Clear variables, including deallocating all sub-objects stored in vectors
 */
void DirectoryData::resetStats(){
		//deallocate each of the files
		for (DirectoryData* file : files) {
			delete file;
			file = nullptr;
		}
		//deallocate each of the subfolders
		for (DirectoryData* folder : subFolders) {
			delete folder;
			folder = nullptr;
		}
		subFolders.clear();
		files.clear();
		size = 0;
		num_items = 0;
   }

/**
 Back-propagate changes made to child objects anywhere in the hierarchy into the parent object
 Does not make filesystem calls, instead uses only the data in the FolderData struct.
 Modifies the properties of the struct.
 */
void DirectoryData::recalculateStats(){
	if (subFolders.size() > 0){
		size = 1;
		num_items = files.size();
		//calculate file size
		for (DirectoryData* file : files){
			size += file->size;
		}
		
		for(DirectoryData* sub : subFolders){
			//error handle
			if (sub == nullptr) {continue;}
			sub->recalculateStats();
			num_items += sub->num_items + 1;
			size += sub->size;
		}
	}
}

/**
Find all the single super-items on this tree for this node
@returns vector of all the pointers that make up a single chain to data
*/
vector<DirectoryData*> DirectoryData::getSuperFolders(){
	vector<DirectoryData*> folders;
	//recursively advance up the hierarchy
	function<DirectoryData*(DirectoryData*)> recurse = [&](DirectoryData* d) -> DirectoryData* {
		if (d->parent != nullptr){
			folders.push_back((DirectoryData*)d->parent);
			return recurse((DirectoryData*)d->parent);
		}
		else{
			return d;
		}
	};
	recurse(this);
	
	return folders;
}

/**
Return a string representing the item's % size of the superitem
@returns size rounded to 1 decimal place e.g (5.2%)
*/
long double DirectoryData::percentOfParent() const{
//	if (parent == nullptr){return "[waiting]";}
//	//round to 2 decimal places, then attach unit
//	char buffer[10];
//	sprintf(buffer,"%.1Lf",(long double)size / (long double)parent->size * 100);
//	return string(buffer) + "%";
	
	return (long double)size / (long double)parent->size * 100;
}
