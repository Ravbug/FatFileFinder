//
//  globals.cpp
//
//	Place global constants or functions without classes in this file.
//	Then #include "globals.cpp" in files that need them
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
#include <wx/wx.h>

static inline const std::string AppName = "FatFileFinder";
static inline const std::string AppVersion = "1.0";

/**
Fits a wxWindow to its contents, and then sets that size as the window's minimum size.
This function ignores and overwrites the window's previous size or size hints.
@param window the wxWindow to apply size changes
*/
inline void fitWindowMinSize(wxWindow* window) {
	//fit size to children
	window->Fit();
	
	//constrain minimum size to the minimum fitting size
	wxSize size = window->GetSize();
	window->SetSizeHints(size);
}


#if defined _WIN32
//place windows-specific globals here

//windows.h must be the first include, and must be placed before using namespace std.
//if they are not in this order, the compiler will not be able to resolve byte
//ensure globals.cpp is the first include in every file
#include <windows.h>
#include <chrono>
#include <ctime>
#include <filesystem>
static inline const int maxPathLength = 247;

/**
@return the calculated display scale factor using GDI+
*/
inline float get_WIN_dpi_multiple() {
	FLOAT dpiX;
	HDC screen = GetDC(0);
	dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);
	return dpiX / 96;
}

/**
Scales a wxWindow's minimum size to the correct size using the monitor's DPI factor (Windows only)
This does not preserve the defined size of the window. To simply fit the window to contents, regardless
of DPI, use fitWindowMinSize.
@param window the wxWindow to scale
*/
inline void dpi_scale_minimum(wxWindow* window) {
	//fit size to children
	window->Fit();
	
	//calculate the scaled min size
	float fac = get_WIN_dpi_multiple();
	float minh = window->GetMinHeight() * fac;
	float minw = window->GetMinWidth() * fac;
	//set the minimum size
	window->SetSizeHints(wxSize(minw,minh));
}

/**
Scales a wxWindow's current size to the correct size using the monitor's DPI factor (Windows only)
This preserves the defined size of the window. To simply fit the window to contents, regardless
of DPI, use fitWindowMinSize.
@param window the wxWindow to scale
*/
inline void dpi_scale(wxWindow* window) {
	float fac = get_WIN_dpi_multiple();
	wxSize size = window->GetSize();
	window->SetSize(wxSize(size.GetWidth() * fac,size.GetHeight()*fac));
}

static inline std::string timeToString(std::filesystem::file_time_type inTime) {
	//time_t cftime = decltype(inTime)::clock::to_time_t(inTime);
	//return std::asctime(std::localtime(&cftime));
	//std::chrono::system_clock::to_time_t((std::chrono::time_point<std::chrono::time_point)inTime);
	return "Test";
}

#elif defined __APPLE__
	#include <boost/filesystem.hpp>
	#include <boost/range/iterator_range.hpp>
	using namespace boost::filesystem;
//place macOS-specific globals here
//skip paths longer than this length to avoid errors
static inline const int maxPathLength = 260;

static inline std::string timeToString(time_t& inTime) {
	tm* time;
	time_t tm = inTime;
	time = localtime(&tm);
	char dateString[100];
	strftime(dateString, 50, "%x %X", time);
	return dateString;
}

/**
 Reveal a folder or file in the Finder
 @param fspath the path to the file or folder
 */
static inline void reveal(const path& fspath){
	path p = fspath;
	if (! is_directory(p)){
		p = fspath.parent_path();
	}
	wxExecute(wxT("open \"" + p.string() + "\""),wxEXEC_ASYNC);
}

#elif defined __linux__
//place linux-specific globals here
static inline std::string timeToString(std::filesystem::file_time_type inTime) {
	return "test";
}
#else
//place globals for systems here

#endif


//globals for both linux and windows
#if defined __linux__ || defined _WIN32
#define leaf() filename()
#endif
