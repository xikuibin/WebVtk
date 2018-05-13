In progress ...

# WebVtk
Demo application show how to implement web DICOM application based on VTK


An example show how to use vtk and wt implementing an web based PACS. Maindevelopment language is C++. A little html CSS js knowledge Will be helpful.

Dicom Image are loaded and rendered on server. Only result image are sent to client. Any popular web browser could support due to design of wt.

his exapmle only implement a MPR window. Any one familiar to VTk could add more

load blanace

Two layers load balance could be used on product environment. On single server wt use deticated-process mode. A main process will be run as entry point. If an request arrives, an independent work subprocess started. It will hold aVTk session which execute all rendering. When multiple servers are necessary, haproxy could be used as frontend. It has native session-sticky feature which could help to make all requests from same client will be sent to same wt process. This could reduce the overload loading vtk data multiple times.

Acknowledgement

wt vtk

