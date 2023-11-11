# WslAssociator

Make it easy to associate graphical Linux applications with file types in Windows Explorer. The associated Linux application will be
started via [WSLg](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps). A unixifed path will be passed to the application
as a single command-line argument, and the application will be started in the directory of that argument.

## Build/install
1. Install Visual Studio 2022 or higher with C++ support.
1. Modify the map at the top of WslAssociator.cpp to associate file extensions with Linux commands.
1. Build.
1. Copy the .exe somewhere convenient.
1. Associate files with it in Windows Explorer.
