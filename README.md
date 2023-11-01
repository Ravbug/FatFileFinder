# FatFileFinder

## Information
This cross-platform app sweeps a folder or drive and displays the size of each item in the folder, to assist the user in cleaning their drive.
This version is a rewrite of my [Windows-only version](https://github.com/Ravbug/FatFileFinder) in C++, using the [wxWidgets](https://www.wxwidgets.org) library for GUI. 
Similar to [WinDirStat](https://windirstat.net/), but lighter and faster.

## Building
Use CMake:
```sh
mkdir -p build && cd build
cmake -G ..
cmake --build . --config Release --target install
```

## Reporting bugs
To report a bug, use the [Issues](https://github.com/Ravbug/FatFileFinderCPP/issues) tab on this github page.
For crashes, please run the program in a debugger and tell me which line the exception breakpoint triggers, and under which conditions, 
so that I can more easily and more quickly fix the issue.

