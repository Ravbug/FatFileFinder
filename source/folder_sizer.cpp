//
//  folder_sizer.cpp
//  mac
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "folder_sizer.hpp"
#include "globals.cpp"

//constructor and destructor
folderSizer::folderSizer(){}
folderSizer::~folderSizer(){}

/**
 Calculate the size of a folder, including the size of subfolders
 @param folder the path to the folder to size
 @param progress the std::function to call with progress updates
 @return the FolderData structure representing the entire directory tree
 */
FolderData* folderSizer::SizeFolder(const string& folder, const progCallback& progress){
	if (folder.size() > maxPathLength){
		return NULL;
	}
	
	FolderData* fd = new FolderData{};
	fd->Path = path(folder);
	
	vector<path> folders;
	// iterate through the items in the folder
	try{
		for(auto& p : directory_iterator(folder)){
			//is the item a folder? if so, defer sizing it
			if (is_directory(p)){
				folders.push_back(p);
			}
			else{
				//size the file, add its details to the structure
				FileData file = {p,file_size(p)};
				fd->files_size += file.size;
				fd->files.push_back(file);
			}
		}
	}
	catch(exception){
		//notify user?
		return NULL;
	}
	fd->total_size = fd->files_size;
	fd->num_items = fd->files.size();
	
	//recursively size the folders in the folder
	float num = 0;
	for(path& p : folders){
		FolderData* d = SizeFolder(p.string(), nullptr);
		if (d != NULL){
			fd->num_items += d->num_items + 1;
			fd->total_size += d->total_size;
			num++;
			if (progress != nullptr){
				progress(num / folders.size(),fd);
			}
			fd->subFolders.push_back(*d);
		}
		//clean up unused data
		delete d;
	}

	return fd;
}
