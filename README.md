# FatFileFinder C++ edition

## Information
This cross-platform app sweeps a folder or drive and displays the size of each item in the folder, to assist the user in cleaning their drive.
This version is a rewrite of my [Windows-only version](https://github.com/Ravbug/FatFileFinder) in C++, using the [wxWidgets](https://www.wxwidgets.org) library for GUI. 
Similar to [WinDirStat](https://windirstat.net/), but lighter and uses a background thread.

Visit [here](https://github.com/Ravbug/FatFileFinder) to use the legacy Windows-only version.

## How to Install
1. Ensure your system meets the minimum requirements:
   - macOS 10.15 or higher (I cannot support 10.14 or lower because of unsupported STL features, sorry!)
   - Windows 8 or higher (Windows 7 and lower are untested)
   - 64 bit linux with GTK+ (32 bit users must compile their own copy)
1. Download the executable from the [releases](https://github.com/Ravbug/FatFileFinderCPP/releases/latest) tab.
2. The program is a self-contained executable. Store it anywhere on your disk. 
-  Recommended locations:
   - macOS: `/Applications`
   - Windows: `\Program Files`
   - Linux: `/usr/bin`
2. Run the program by double-clicking it.

## How to Size a Folder
1. Click the folder button (📁) in the toolbar to choose a folder or drive.
2. The program will begin sizing the folder and updating the view.
3. To stop sizing early, press the 🛑 button at the bottom of the window. 

## Other Usage info
* Single click rows in the table to view their properties in the sidebar. Click a disclosure triangle to open a folder.
You can also use the up and down arrows to move the selection. Note that expanding a folder with a lot of immediate sub-items can take a moment.
* To refresh a folder, select it in the table and press the refresh button (🔁). The program will refresh the contents of that folder.
If you want to reload the root folder, you will have to re-size it using the 📁 button.
* To view an item in your system's file browser, select it in the view and press `Reveal in Explorer/Finder` in the sidebar.
* To copy the full path to an item, select it in the view and press `Copy Path` in the sidebar.

Note: Clipboard is currently not available on macOS. The sidebar in the Windows version is different from that on macOS and Linux. 
The Windows version currently does not support the emoji icons. 

## Screenshots
![Main Window](https://user-images.githubusercontent.com/22283943/72393521-111f9980-3701-11ea-8885-992f2852a19f.png)

## Compiling it yourself
Download a copy of the repository, then
- **macOS**: Open `mac.xcodeproj` in Xcode and press Build.
- **Windows**: Open `windows.sln` in Visual Studio 2019 or later, select your target, and press Build.
- **Linux**: cd to the root folder and run `make` in your command line. If you are missing packages, the build system will alert you. Visit [this guide](https://github.com/Ravbug/wxWidgetsTemplate/wiki/Building-the-Projects#linux)
if problems persist. The executable will be located in `linux-build/`.

The compile can take a while! On all systems, the compiler is configured to use all of your system's cores.

## Reporting bugs
To report a bug, use the [Issues](https://github.com/Ravbug/FatFileFinderCPP/issues) tab on this github page.
For crashes, please run the program in a debugger and tell me which line the exception breakpoint triggers, and under which conditions, 
so that I can more easily and more quickly fix the issue.

