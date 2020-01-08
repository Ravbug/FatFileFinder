//
//  folder_sizer.cpp
//  mac
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "globals.cpp"
#include "folder_sizer.hpp"
#include <array>

//constructor and destructor
folderSizer::folderSizer(wxWindow* Parent)
{
	ParentWindow = Parent;
}
folderSizer::~folderSizer(){}

/**
 Calculate the size of a folder, including the size of subfolders
 @param folder the path to the folder to size
 @param progress the std::function to call with progress updates
 @return the FolderData structure representing the entire directory tree
 */
DirectoryData* folderSizer::SizeFolder(const string& folder, const progCallback& progress){
	DirectoryData* fd = new DirectoryData(folder, true);
	if (path_too_long(folder)){
		return fd;
	}

	if (abort) {
		return fd;
	}
	
	if (is_symlink(path(folder))){
		fd->size = 1;
		fd->isSymlink = true;
		return fd;
	}
	
	//calculate the size of the immediate files in the folder
	try{
		sizeImmediate(fd);
	}
	catch(filesystem_error e){
		//notify user
		Log("Error sizing directory" + folder + "\n" + e.what());
		return fd;
	}
	
	fd->size = fd->files_size;
	fd->num_items = fd->files.size();
	
	//recursively size the folders in the folder
	for (int i = 0; i < fd->subFolders.size(); i++){
		fd->subFolders[i] = SizeFolder(fd->subFolders[i]->Path, nullptr);
		
		//update parent
		if (fd->subFolders[i] != NULL){
			fd->subFolders[i]->parent = fd;
			fd->num_items += fd->subFolders[i]->num_items + 1;
			fd->size += fd->subFolders[i]->size;
			//check for zero size
			if (fd->size == 0){
				fd->size = 1;
			}
		}
		if (progress != nullptr) {
			progress((float)(i + 1) / fd->subFolders.size(), fd);
		}
	}

	return fd;
}

/**
 Calculate the size of the immediate files in the folder
 @param data the FolderData struct to calculate
 */
void folderSizer::sizeImmediate(DirectoryData* data, const bool& skipFolders){
	//clear to prevent dupes
	data->files.clear();
	data->files_size = 1;
	if (!skipFolders){
		data->subFolders.clear();
	}
	// iterate through the items in the folder
	for(auto& p : directory_iterator(data->Path,directory_options::skip_permission_denied)){
		//is the item a folder? if so, defer sizing it
		//check if can read the file
		try {
			file_status s = status(p.path());
			if (is_symlink(s)){
				//skip symbolic link
				continue;
			}
			if (can_access(s))
			{
				if (is_directory(p)) {
					if (!skipFolders) {
						DirectoryData* sub = new DirectoryData(p.path().string(), true);
						data->subFolders.push_back(sub);
					}
				}
				else {
					//size the file, add its details to the structure
					DirectoryData* file = new DirectoryData(p.path().string(), stat_file_size(p.path().string()));
					data->files_size += file->size;
					file->parent = data;
					data->files.push_back(file);

				}
			}
		}
		catch (filesystem_error e) {
			Log("Error sizing file " + p.path().string() + "\n" + e.what());
		}
	}
}

/**
 Formats a raw file size to a string with a unit
 @param fileSize the size of the item in bytes
 @returns unitized string, example "12 KB"
 */
string folderSizer::sizeToString(const fileSize& fileSize){
	string formatted = "";
	int size = 1000;		//MB = 100, MiB = 1024
	array<string,5> suffix { " bytes", " KB", " MB", " GB", " TB" };

	for (int i = 0; i < suffix.size(); i++)
	{
		double compare = pow(size, i);
		if (fileSize <= compare)
		{
			int minus = 0;
			if (i > 0)
			{
				minus = 1;
			}
			//round to 2 decimal places, then attach unit
			char buffer[10];
			sprintf(buffer,"%.2f",fileSize / pow(size,i-minus));
			formatted = string(buffer) + suffix[i - minus];
			break;
		}
	}

	return formatted;
}

/**
 Recalculate the size and the number of items in a FolderData struct.
 Does not make filesystem calls, instead uses only the data in the FolderData struct.
 Modifies the properties of the struct.
 @param data the FolderData to resize
 */
void folderSizer::recalculateStats(DirectoryData* data){
	//error handle
	if (data == NULL){return;}
	
	if (data->subFolders.size() > 0){
		data->num_items = data->files.size();
		//calculate file size
		data->files_size = 1;
		for (DirectoryData* file : data->files){
			data->files_size += file->size;
		}
		data->size = data->files_size;
		
		for(DirectoryData* sub : data->subFolders){
			//error handle
			if (sub == NULL) {continue;}
			folderSizer::recalculateStats(sub);
			data->num_items += sub->num_items + 1;
			data->size += sub->size;
			data->files_size += sub->files_size;
		}
	}
}

/**
 Find all the single super-items on this tree
 @param data the node to find the super items for
 @returns vector of all the pointers that make up a single chain to data
 */
vector<DirectoryData*> folderSizer::getSuperFolders(DirectoryData* data){
	vector<DirectoryData*> folders;
	//recursively advance up the hierarchy
	function<DirectoryData*(DirectoryData*)> recurse = [&](DirectoryData* d) -> DirectoryData* {
		if (d->parent != NULL){
			folders.push_back((DirectoryData*)d->parent);
			return recurse((DirectoryData*)d->parent);
		}
		else{
			return d;
		}
	};
	recurse(data);
	
	return folders;
}

/**
 Return a string representing the item's % size of the superitem
 @param data the sub item to calculate
 @returns size rounded to 1 decimal place e.g (5.2%)
 */
string folderSizer::percentOfParent(DirectoryData* data){
	if (data == NULL || data->parent == NULL){return "[waiting]";}
	//round to 2 decimal places, then attach unit
	char buffer[10];
	sprintf(buffer,"%.1Lf",(long double)data->size / (long double)data->parent->size * 100);
	return string(buffer) + "%";
}
