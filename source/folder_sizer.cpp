//
//  folder_sizer.cpp
//  mac
//
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "globals.h"
#include "folder_sizer.hpp"
#include <filesystem>
#include <array>

using namespace std::filesystem;


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
	
	if (abort || path_too_long(folder)) {
		return fd;
	}
	
	//skip symbolic links
	std::error_code ec;
	if (is_symlink(path(folder),ec)){
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
		if (fd->subFolders[i] != nullptr){
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
			if (/*!is_symlink(s) &&*/ can_access(s))
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
					//cout << file->Path << ": " << file->size << endl;
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
	int size = 1000;		//MB = 1000, MiB = 1024
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

