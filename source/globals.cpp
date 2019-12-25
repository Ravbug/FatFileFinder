//
//  globals.cpp
//
//	Place global constants or functions without classes in this file.
//	Then #include "globals.cpp" in files that need them
//
//  Copyright © 2019 Ravbug. All rights reserved.
//
#include <wx/wx.h>
#pragma mark Shared functions
static inline const std::string AppName = "FatFileFinder";
static inline const std::string AppVersion = "0.1b";

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

/**
 Calls stat on a path
 @param path the path to get stat for
 @return a stat struct representing the path
 */
inline struct stat get_stat(const std::string& path){
	struct stat buf;
	stat(path.c_str(),&buf);
	return buf;
}


#pragma mark Windows functions
#if defined _WIN32
//place windows-specific globals here

//windows.h must be the first include, and must be placed before using namespace std.
//if they are not in this order, the compiler will not be able to resolve byte
//ensure globals.cpp is the first include in every file
#include <windows.h>
#include <winnt.h>
#include <array>
#include <filesystem>

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

/**
 Determines if a path is too long. On Windows using Win32 APIs, the maxiumum length of the entire path is 260 bytes, but to be safe this program reduces it to 247.
 @param inPath the path to the file
 @return true if path is too long, false otherwise
 */
static inline bool path_too_long(const std::string& inPath){
	return inPath.size() > 247;
}

static inline bool is_writable(const std::string& inPath) {
	mode_t mode = get_stat(inPath).st_mode;
	return mode & _S_IWRITE;
}

static inline bool is_executable(const std::string& inPath) {
	mode_t mode = get_stat(inPath).st_mode;
	return mode & _S_IEXEC && mode & _S_IFDIR;
}

static inline bool is_hidden(const std::string& inPath) {
	DWORD attributes = GetFileAttributesA(inPath.c_str());
	return attributes & FILE_ATTRIBUTE_HIDDEN;
}

static inline void reveal(const std::filesystem::path& fspath) {
	wxExecute(wxT("C:\\Windows\\explorer.exe \"" + fspath.string() + "\""), wxEXEC_ASYNC);

}

/**
Get the attributes for a file (Windows)
@param path the path to the file
@return a boolean array representing the different file properties
*/
static inline std::array<bool, 13> file_attributes_for(const std::string& path) {
	DWORD attributes = GetFileAttributesA(path.c_str());
	int attr_const[] = {FILE_ATTRIBUTE_ARCHIVE, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED, FILE_ATTRIBUTE_INTEGRITY_STREAM, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, FILE_ATTRIBUTE_NO_SCRUB_DATA, FILE_ATTRIBUTE_OFFLINE, FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS, FILE_ATTRIBUTE_RECALL_ON_OPEN, FILE_ATTRIBUTE_REPARSE_POINT, FILE_ATTRIBUTE_SPARSE_FILE, FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_VIRTUAL};

	std::array<bool, 13> attr;

	for (int i = 0; i < attr.size(); i++) {
		attr[i] = attributes & attr_const[i];
	}

	return attr;
}

#pragma mark macOS functions
#elif defined __APPLE__
	#include <boost/filesystem.hpp>
	#include <boost/range/iterator_range.hpp>
	#include <sys/statvfs.h>
	using namespace boost::filesystem;
//place macOS-specific globals here

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

/**
 Determines if a path is too long to process. On macOS (HFS/APFS), the file path length is unlimited, but the filename limit is 255 characters.
 @note Uses statvfs to query the correct maximum path.
 @param inPath the path to the file
 @return true if the path is too long, false if not
 */
static inline bool path_too_long(const std::string& inPath){
	path p = path(inPath);
	struct statvfs buf;
	statvfs(inPath.c_str(),&buf);
	return p.leaf().string().size() > buf.f_namemax;
}

/**
 Determines if an item is accessible using boost::filesystem
 @param s the file_status object
 @return true if accessible, false otherwise
*/
static inline bool can_access(const file_status& s) {
	return (s.permissions() & perms::others_read) != perms::no_perms || (s.permissions() & perms::owner_read) != perms::no_perms;
}

#pragma mark Linux functions
#elif defined __linux__
#include <limits.h>
#include <filesystem>
#include <sys/statvfs.h>
/**
 Determines if a path is too long to process. On Linux, a file path cannot exceed 4096 characters, and a filename cannot exceed 255 bytes.
 @param inPath the path to the file
 @return true if path is too long, false otherwise
 */
static inline bool path_too_long(const std::string& inPath){
	std::filesystem::path p(inPath);
	struct statvfs buf;
	statvfs(inPath.c_str(),&buf);
	if (p.filename().string().size() > buf.f_namemax){
		return true;
	}
	return p.stem().string().size() > PATH_MAX;
}

/**
 * The Reveal feature is not available on Linux because there is no standard file browser nor standard terminal, nor a standard way to interface with either.
 */
static inline void reveal(const std::string& path){}

#else
//place globals for systems here

#endif


#pragma mark Win-Linux functions
//globals for both linux and windows
#if defined __linux__ || defined _WIN32
#define leaf() filename()

/**
 Determines if an item is accessible using std::filesystem
 @param s the file_status object 
 @return true if accessible, false otherwise
*/
static inline bool can_access(const std::filesystem::file_status& s) {
	return (s.permissions() & std::filesystem::perms::others_read) != std::filesystem::perms::none || (s.permissions() & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
}
#endif

#pragma mark Unix-like functions
#if defined __APPLE__ || defined __linux__
#include <sys/param.h>
#include <unistd.h>


/**
 Determines if an item is write-able using stat
 @param path the path to the file
 @return true if the file can be written to, false otherwise
 */
static inline bool is_writable(const std::string& path){
	mode_t mode = get_stat(path).st_mode;
	//owner or others can write
	return mode & S_IWUSR || mode & S_IWOTH;
}

/**
 Determines if an item is executable using stat
 @param path the path to the file
 @return true if file is executable, false otherwise
 */
static inline bool is_executable(const std::string& path){
	mode_t mode = get_stat(path).st_mode;
	//owner can exeucte or others can execute if the item is not a folder
	return (mode & S_IXUSR || mode & S_IXOTH) && !(mode & S_IFDIR);
}

/**
 Gets a permissions string based on stat
 @param path the path to the item
 @return string representing the permissions
 */
static inline std::string permstr_for(const std::string& path){
	mode_t perm = get_stat(path).st_mode;
	char perms[10];
	perms[0] = (perm & S_IRUSR) ? 'r' : '-';
    perms[1] = (perm & S_IWUSR) ? 'w' : '-';
    perms[2] = (perm & S_IXUSR) ? 'x' : '-';
    perms[3] = (perm & S_IRGRP) ? 'r' : '-';
    perms[4] = (perm & S_IWGRP) ? 'w' : '-';
    perms[5] = (perm & S_IXGRP) ? 'x' : '-';
    perms[6] = (perm & S_IROTH) ? 'r' : '-';
    perms[7] = (perm & S_IWOTH) ? 'w' : '-';
    perms[8] = (perm & S_IXOTH) ? 'x' : '-';
    perms[9] = '\0';
	
	return std::string(perms);
}

/**
 Determines if a file is hidden
 @param strpath the path to the file
 @return true if file is hidden (path starts with '.'), false otherwise
 */
static inline bool is_hidden(const std::string& strpath){
	std::filesystem::path p(strpath);
	//true if path name starts with '.'
	return p.leaf().string()[0] == '.';
}

/**
 Determines the st_mode type(s) of a path
 @param path the path to the file
 @return a string for the type
 */
static inline std::string modet_type_for(const std::string& path){
	mode_t perm = get_stat(path).st_mode;
	std::string types[] = {"symbolic link", "regular file", "block device", "directory", "character device", "FIFO"};
	int perms[] = {S_IFLNK, S_IFREG, S_IFBLK, S_IFDIR, S_IFCHR, S_IFIFO};
	
	std::string result;
	for (int i = 0; i < sizeof(perms)/sizeof(int); i++){
		if (perm & perms[i]){
			result += types[i] + ", ";
		}
	}
	//omit trailing comma
	return result.substr(0,result.length()-2);
}

/**
 Converts a time_t to a formatted date string
 @param inTime the time_t to convert
 @return formatted date string
 */
static inline std::string timeToString(time_t& inTime) {
	tm* time;
	time_t tm = inTime;
	time = localtime(&tm);
	if (time != NULL){
		char dateString[100];
		strftime(dateString, 50, "%x %X", time);
		return dateString;
	}
	return "Unavailable";
}

/**
 Returns the size of the file on disk, based on the number of blocks it consumes
 @param path the path to the file
 @return number of bytes representing the file	on disk
 */
static inline long long size_on_disk(const std::string& path){
	return get_stat(path).st_blocks * DEV_BSIZE;
}

static inline time_t file_modify_time(const std::string& path){
	return get_stat(path).st_mtime;
}

#endif
