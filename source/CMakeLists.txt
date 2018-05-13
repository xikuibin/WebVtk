CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

Project(WebVtk)
SET (TARGET_NAME "WebVtk")

# Change according to local enviornment
#SET (WT_SDK_DIR "/home/ubuser/wtvtk/wt/install" CACHE PATH "")
#SET (VTK_LOCALINSTALL_DIR "/home/ubuser/vtkHome/install" CACHE PATH "")
#SET (VTK_DIR ${VTK_LOCALINSTALL_DIR}/lib/cmake/vtk-7.0)
SET (WT_SDK_DIR "C:/Develop/Wt-3.3.6-msvs2015-Windows-x64-SDK" CACHE PATH "")
SET (VTK_LOCALINSTALL_DIR "C:/Develop/VTK7.0.Opengl" CACHE PATH "")
SET (VTK_DIR "${VTK_LOCALINSTALL_DIR}/lib/cmake/vtk-7.0" CACHE PATH "")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})#

if(UNIX)
    SET (USE_PREBUILD_WT_SDK OFF CACHE BOOL "use widnows prebuild wt binary" )
ELSE()
    SET (USE_PREBUILD_WT_SDK ON CACHE BOOL "use widnows prebuild wt binary" )
ENDIF()


#prebuild wt sdk includes necessary boost components
IF(NOT USE_PREBUILD_WT_SDK)
    #find boost package
    set(Boost_USE_STATIC_LIBS        OFF) # only find static libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    OFF)
    find_package(Boost 1.35 COMPONENTS 
        date_time
        regex
        program_options
        signals
        system
        filesystem
        thread
        random
        REQUIRED
        )
ENDIF(NOT USE_PREBUILD_WT_SDK)

INCLUDE_DIRECTORIES(
	${WT_SDK_DIR}/include
	${VTK_LOCALINSTALL_DIR}/include/vtk-7.0
	)
LINK_DIRECTORIES(
	${WT_SDK_DIR}/lib
	${VTK_LOCALINSTALL_DIR}/lib
	)

ADD_EXECUTABLE(${TARGET_NAME}
    WebVtk.C
    SingleThreadedApplication.C
    MPRWidget.cpp
	MPRView.cpp

	MPRView.h
	MPRWidget.h
	SingleThreadedApplication.h
)



TARGET_LINK_LIBRARIES(${TARGET_NAME}
    debug wtd      optimized wt
    debug wthttpd  optimized wthttp
    
    vtkCommonCore-7.0
    vtkCommonMisc-7.0
    vtkCommonExecutionModel-7.0
    vtkCommonDataModel-7.0
    vtkRenderingCore-7.0
    vtkFiltersSources-7.0
    vtkCommonExecutionModel-7.0
    vtkImagingCore-7.0
    vtkCommonMath-7.0
    vtkInteractionStyle-7.0
    vtkCommonComputationalGeometry-7.0
    vtkFiltersCore-7.0
    vtkFiltersGeneral-7.0
    vtkImagingGeneral-7.0
    vtkCommonTransforms-7.0
    vtkRenderingAnnotation-7.0
    vtkImagingHybrid-7.0
    vtkRenderingVolume-7.0
    vtkInteractionImage-7.0
    vtkIOImage-7.0
    vtkRenderingFreeType-7.0
    vtkImagingColor-7.0
    vtkChartsCore-7.0

 )

#IF(WIN32)
#    TARGET_LINK_LIBRARIES(${TARGET_NAME}
#    vtkRenderingOpenGL2-7.0
#    vtkRenderingVolumeOpenGL2-7.0
#    )
#ELSE() # unbuntu 14 do not supoort VTK opengl 2
    TARGET_LINK_LIBRARIES(${TARGET_NAME}
        vtkRenderingOpenGL-7.0
        vtkRenderingVolumeOpenGL-7.0
    )
#ENDIF()

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

 IF(MSVC)
    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "/W3 /wd4251 /wd4275") # /Wp64
  ENDIF(MSVC) 

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${TARGET_NAME} ${Boost_LIBRARIES})
endif()

IF(WIN32)
  ADD_DEFINITIONS(
      -D_CRT_SECURE_NO_WARNINGS
      -D_SCL_SECURE_NO_WARNINGS
      )
ENDIF(WIN32)



