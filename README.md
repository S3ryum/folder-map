# Folder Map

A read-only C++ command-line report for the number and size of files in a folder, grouped by file extension.

## Requirements

A C++17 compiler.

## Build and run

~~~sh
g++ -std=c++17 -O2 folder_map.cpp -o folder-map
./folder-map .
~~~

On Windows, run folder-map.exe with a folder path. The report skips folders it cannot read, does not follow directory symlinks, and never changes files.
