

#pragma warning (disable : 4251 4275 4503 4996) // soma warning about boost

#include <Wt/WCssDecorationStyle>
#include <Wt/WColor>
#include <Wt/WContainerWidget>
#include <Wt/WEvent>
#include <Wt/WPainter>
#include <Wt/WPaintedWidget>
#include <Wt/WPainterPath>
#include <Wt/WPen>
#include <Wt/WPointF>
#include <Wt/WPushButton>
#include <Wt/WRectF>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WToolBar>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WGLWidget>
#include <Wt/WFileResource>
#include <Wt/WMemoryResource>

#include "MPRView.h"

#include <sstream>

const std::string DICOM_DATA_FOLDER = "C:\\Develop\\SampleDICOMData";

int MPRView::createRender(int MPRmode)
{
	std::string folder = DICOM_DATA_FOLDER;
	
	// Read all the DICOM files in the specified directory.
	vtkSmartPointer<vtkDICOMImageReader> reader =
		vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName(folder.c_str());
	reader->Update();

	// Calculate the center of the volume
	reader->Update();
	int extent[6];
	double spacing[3];
	double origin[3];


	reader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
	reader->GetOutput()->GetSpacing(spacing);
	reader->GetOutput()->GetOrigin(origin);

	double center[3];
	center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
	center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
	center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);

	// Matrices for axial, coronal, sagittal, oblique view orientations
	static double axialElements[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	static double coronalElements[16] = {
		1, 0, 0, 0,
		0, 0, -1, 0, //
		0, -1, 0, 0, 
		0, 0, 0, 1 };

	static double sagittalElements[16] = {
		0, 0,1, 0,
		-1, 0, 0, 0,
		0,-1, 0, 0,
		0, 0, 0, 1 };

	//static double obliqueElements[16] = {
	//         1, 0, 0, 0,
	//         0, 0.866025, -0.5, 0,
	//         0, 0.5, 0.866025, 0,
	//         0, 0, 0, 1 };

	// Set the slice orientation
	vtkSmartPointer<vtkMatrix4x4> resliceAxes =
		vtkSmartPointer<vtkMatrix4x4>::New();


	if(MPRmode == 0)
	{
		resliceAxes->DeepCopy(axialElements);
	}
	else if(MPRmode == 1)
	{
		resliceAxes->DeepCopy(coronalElements); 
	}
	else
	{
		resliceAxes->DeepCopy(sagittalElements); 
	}

	// Set the point through which to slice
	resliceAxes->SetElement(0, 3, center[0]);
	resliceAxes->SetElement(1, 3, center[1]);
	resliceAxes->SetElement(2, 3, center[2]);


	// Extract a slice in the desired orientation
	//vtkSmartPointer<vtkImageReslice> reslice =
	reslice =	vtkSmartPointer<vtkImageReslice>::New();
	reslice->SetInputConnection(reader->GetOutputPort());
	reslice->SetOutputDimensionality(2);
	reslice->SetResliceAxes(resliceAxes); 
	reslice->AutoCropOutputOn();
	reslice->SetInterpolationModeToLinear();
	reslice->SetInterpolationModeToCubic();

	// Create a greyscale lookup table
	vtkSmartPointer<vtkLookupTable> table =
		vtkSmartPointer<vtkLookupTable>::New();
	//table->SetRange(0, 2000); // image intensity range
	table->SetRange(m_colorLevel - 0.5*m_colorWindow, m_colorLevel + 0.5*m_colorWindow); // image intensity range
	table->SetValueRange(0.0, 1.0); // from black to white
	table->SetSaturationRange(0.0, 0.0); // no color saturation
	table->SetRampToLinear();
	table->Build();

	// Map the image through the lookup table
	//vtkSmartPointer<vtkImageMapToColors> color =
	color =	vtkSmartPointer<vtkImageMapToColors>::New();
	color->SetLookupTable(table);
	color->SetInputConnection(reslice->GetOutputPort());

	// Display the image
	vtkSmartPointer<vtkImageActor> actor =
		vtkSmartPointer<vtkImageActor>::New();
	actor->GetMapper()->SetInputConnection(color->GetOutputPort());

	//vtkSmartPointer<vtkRenderer> renderer =
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);

	vtkSmartPointer<vtkRenderWindow> window =
		vtkSmartPointer<vtkRenderWindow>::New();
	window->SetSize(512,512);
	// 
	window->AddRenderer(renderer);

	renderer->GetActiveCamera()->SetParallelProjection(1);
	renderer->ResetCamera();
	renderer->GetActiveCamera()->SetParallelScale(0.5*renderer->GetActiveCamera()->GetParallelScale());

	// Set up the interaction
	vtkSmartPointer<vtkInteractorStyleImage> imageStyle =
		vtkSmartPointer<vtkInteractorStyleImage>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetInteractorStyle(imageStyle);
	window->SetInteractor(interactor);
	window->Render();


	windowToImageFilter = 
		vtkSmartPointer<vtkWindowToImageFilter>::New();
	
	windowToImageFilter->SetInput(window);	
	//windowToImageFilter->SetInput(riw->GetRenderWindow());
	windowToImageFilter->Update();

	writer = 
		vtkSmartPointer<vtkPNGWriter>::New();
	writer->WriteToMemoryOn();
	//writer->SetFileName("screenshot.png");
	writer->SetInputConnection(windowToImageFilter->GetOutputPort());
	//writer->Write();

	windowToImageFilter->Modified();

	writer->Write();

	ReadResultPNG();
	return 0;
}

void MPRView::ReadResultPNG()
{
	vtkUnsignedCharArray* png_memory = writer->GetResult();
	int size = png_memory->GetNumberOfTuples();
	
	unsigned char* p = png_memory->GetPointer(0);
	png_in_memory_.insert(png_in_memory_.begin(), p, p+size );
	png_in_memory_.resize(size);

}

void MPRView::mouseAction(MPRView::operations curop, Wt::Coordinates prepos, Wt::Coordinates curpos ) 
{
	if(curop == opSlice )
	{
		moveSlice(prepos, curpos);
	}
	else if (curop == opWL )
	{
		adjustWL(prepos, curpos);
	}
	else if (curop == opZoom)
	{
		zoomImage(prepos, curpos);
	}
	else
	{
		//nothing
	}
}
	



void MPRView::moveSlice(Wt::Coordinates prepos, Wt::Coordinates curpos)
{


	// Increment slice position by deltaY of mouse
	int deltaY = (prepos.y - curpos.y)/3; // reduce the change ratio

	reslice->Update();
	double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
	vtkMatrix4x4 *matrix = reslice->GetResliceAxes();
	// move the center point that we are slicing through
	double point[4];
	double center[4];
	point[0] = 0.0;
	point[1] = 0.0;
	point[2] = sliceSpacing * deltaY;
	point[3] = 1.0;
	matrix->MultiplyPoint(point, center);
	matrix->SetElement(0, 3, center[0]);
	matrix->SetElement(1, 3, center[1]);
	matrix->SetElement(2, 3, center[2]);

	//riw->Render();

	windowToImageFilter->Modified();
	writer->Write();
	ReadResultPNG();
}

void MPRView::adjustWL(Wt::Coordinates prepos, Wt::Coordinates curpos)
{
	m_colorLevel += curpos.y - prepos.y;
	m_colorWindow += curpos.x - prepos.x;

	vtkScalarsToColors* plookuptale = color->GetLookupTable();
	plookuptale->SetRange(m_colorLevel - 0.5*m_colorWindow, m_colorLevel + 0.5*m_colorWindow);

	renderer->Render();
	windowToImageFilter->Modified();
	writer->Write();
	ReadResultPNG();
}

void MPRView::zoomImage(Wt::Coordinates prepos, Wt::Coordinates curpos)
{
	double scale = renderer->GetActiveCamera()->GetParallelScale(); 

	scale += scale * 0.01 * (curpos.y - prepos.y);
	if (scale < 1) scale = 1;

	renderer->GetActiveCamera()->SetParallelScale(scale);

	renderer->Render();
	windowToImageFilter->Modified();
	writer->Write();
	ReadResultPNG();
}

std::vector<unsigned char>& MPRView::getImageData()
{
	return png_in_memory_;
}

MPRView::MPRView() : m_colorWindow(400), m_colorLevel(40)
{

}

MPRView::~MPRView()
{
}
