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

