## Purpose
Demo application show how to implement web DICOM application based on VTK

An example show how to use vtk and wt implementing an web based PACS. Maindevelopment language is C++. A little html CSS js knowledge Will be helpful.

Dicom Image are loaded and rendered on server. Only result image are sent to client. Any popular web browser could support due to design of wt.

This exapmle only implement a MPR window. Any one familiar to VTk could add more funtions.


## Build instruction

This project is based on two open source project:
- The Visualization Toolkit (VTK) is an open-source, freely available software system for 3D computer graphics, image processing, and visualization. Refer to https://www.vtk.org/
- Wt is a web GUI library in modern C++. Quickly develop highly interactive web UIs with widgets, without having to write a single line of JavaScript. Wt handles all request handling and page rendering for you, so you can focus on functionality. Refer to https://www.webtoolkit.eu/wt

#### Windows

###### Prerequist

- OS: Windows 10 tested. Windows & or later should work.
- Compiler: Microsoft Visual Studio 2015
- CMake: CMake 3.11 used. Any CMake 3.x should work.
- Wt: Wt 3.3.6 prebuild binary with VS 2015 used. Any 3.3.x should be OK. Suppose the zip extracted to C:\Develop\Wt-3.3.6-msvs2015-Windows-x64-SDK, named as WT_SDK_DIR
- Vtk: Vtk 7.x build as vtk official document. All files in C:\Develop\VTK7.0.Opengl named as VTK_LOCALINSTALL_DIR.

The Wt and Vtk path is used in CMake files.

###### Build 
0. Change the DICOM_DATA_FOLDER in MPRView.cpp. Specify a folder contain a CT or MR DICOM series.
1. Use Cmake to generate VS solution. Specify Visusal Studio 2015 win64.
2. Build solution by VS 2005

###### Running
1. Add  VTK_LOCALINSTALL_DIR/bin and WT_SDK_DIR/bin to the Path enviorment variable. Or copy dll from two directories to executable path.
2. Specify command line arguments: ` --docroot docroot  --http-port 8080 --http-addr 0.0.0.0 `.  --docroot is used as web server root. All files are in *docroot* folder.
3. Run command, and access *host_ip:8080*， 3 MPR windows with different orientation will be show.
4. Only zoom, WW/WL, slice functions are implemented for demo.

There is WebVtk.bat to show how to direct use the result. Please read Wektoolkit document for more description about how to used command line arguments.

#### Ubuntu

###### Prerequist

- OS: Ubuntu 14. Ubuntu is the best choice for Vtk. Desktop compnent is necessary for Opengl.
- Compiler: gcc 4.8
- CMake: CMake 3.2 used. Any CMake 3.x should work.
- Wt: Wt 3.3.6 build on same machine. Saved in WT_SDK_DIR
- Vtk: Vtk 7.x build as vtk official document. All files are saved in  VTK_LOCALINSTALL_DIR.

The Wt and Vtk path is used in CMake files.

###### Build 
0. Change the DICOM_DATA_FOLDER in MPRView.cpp. Specify a folder contain a CT or MR DICOM series.
1. Use Cmake to generate unix makefiles. 
2. run `make' in build directory.

###### Running
1. Add  VTK_LOCALINSTALL_DIR/lib and WT_SDK_DIR/lib to the library path. For example, `export LD_LIBRARY_PATH=../Wt/lib:../VTK/lib:$LD_LIBRARY_PATH`
2. Specify command line arguments: ` --docroot docroot  --http-port 8080 --http-addr 0.0.0.0 `.  --docroot is used as web server root. All files are in *docroot* folder.
3. Run command, and access *host_ip:8080* from browser. 3 MPR windows with different orientation will be show.
4. Only zoom, WW/WL, slice functions are implemented for demo.

## load blanace

Two layers load balance could be used on product environment. On single server wt use deticated-process mode. A main process will be run as entry point. If an request arrives, an independent work subprocess started. It will hold a VTk session which execute all rendering. 

When multiple servers are necessary, haproxy could be used as frontend. It has native session-sticky feature which could help to make all requests from same client will be sent to same wt process. This could reduce the overload loading vtk data multiple times.

To implement this, please read document about Wt configuration and haproxy.

## Acknowlegement

