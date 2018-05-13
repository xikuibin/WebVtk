#pragma once

// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkActor.h>
// headers needed for this example
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>

#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkImageActor.h"
#include "vtkImageProperty.h"
#include "vtkSmartPointer.h"
#include "vtkImageReader2.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageMapper3D.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkDICOMImageReader.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkImageMapToColors.h"
class MPRView
{
public:
	enum operations
	{
		opZoom,
		opSlice,
		opWL,
		opUnknown
	};

private:

	vtkSmartPointer<vtkPNGWriter> writer;
	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter;
	//vtkSmartPointer<vtkResliceImageViewer> riw;
	std::vector<unsigned char> png_in_memory_ ;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkImageReslice> reslice;

	vtkSmartPointer<vtkImageMapToColors> color;
	int m_colorWindow;
	int m_colorLevel;

	void moveSlice(Wt::Coordinates prepos, Wt::Coordinates curpos);

	void adjustWL(Wt::Coordinates prepos, Wt::Coordinates curpos);
	void zoomImage(Wt::Coordinates prepos, Wt::Coordinates curpos);

public:
	MPRView();

	~MPRView(); 

	std::vector<unsigned char>& getImageData();
	int createRender(int MPRmode);
	void ReadResultPNG();


	void mouseAction(MPRView::operations curop, Wt::Coordinates prepos, Wt::Coordinates curpos );
};




