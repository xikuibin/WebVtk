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

#include "MPRWidget.h"

#include "MPRView.h"

using namespace Wt;


namespace {

extern 
Wt::WPushButton *createColorToggle(const char *className, const Wt::WColor& color,
				   MPRWidget *canvas)
{
    Wt::WPushButton *button = new Wt::WPushButton();
    button->setTextFormat(Wt::XHTMLText);
    button->setText("&nbsp;");
    button->setCheckable(true);
    button->addStyleClass(className);
    button->setWidth(60);
    button->checked().connect(std::bind([=] () {
	canvas->setColor(color);
    }));

    return button;
}

}


/* Approximate bootstrap standard colors */
const Wt::WColor bt_blue(0, 110, 204);    // btn-primary
const Wt::WColor bt_red(218, 81, 76);     // btn-danger
const Wt::WColor bt_green(59, 195, 95);   // btn-success
const Wt::WColor bt_orange(250, 168, 52); // btn-warning
const Wt::WColor bt_black = Wt::black;    // btn-inverse
const Wt::WColor bt_gray(210, 210, 210);  // (default)
const Wt::WColor buttoncolors[] = {
	bt_blue, bt_red, bt_green, bt_orange, bt_black, bt_gray
};

Wt::WWidget * createMPRWidget(MPRWidget*& canvas) {


	Wt::WContainerWidget* result = new Wt::WContainerWidget();

	canvas = new MPRWidget(516, 516);
	canvas->setColor(bt_blue);
	canvas->decorationStyle().setBorder
		(Wt::WBorder(Wt::WBorder::Solid, Wt::WBorder::Medium, Wt::darkBlue));

	std::vector<Wt::WPushButton *> colorButtons;
	colorButtons.push_back(createColorToggle("btn-primary", bt_blue, canvas));
	colorButtons.push_back(createColorToggle("btn-danger", bt_red, canvas));
	colorButtons.push_back(createColorToggle("btn-success", bt_green, canvas));

	colorButtons[MPRView::opZoom]->setText("Zoom");
	colorButtons[MPRView::opSlice]->setText("Slice");
	colorButtons[MPRView::opWL]->setText("W/L");

	Wt::WToolBar *toolBar = new Wt::WToolBar();

	for (unsigned i = 0; i < colorButtons.size(); ++i) {
		Wt::WPushButton *button = colorButtons[i];

		button->setChecked(i == 0);
		toolBar->addButton(button);

		// Implement a radio button group
		for (unsigned j = 0; j < colorButtons.size(); ++j) {
		if (i != j) {
			Wt::WPushButton * const other = colorButtons[j];
			button->checked().connect(other, &Wt::WPushButton::setUnChecked);
		}
		}
	}

	result->addWidget(toolBar);
	result->addWidget(canvas);

	return result; 
} 



void MPRWidget::touchMove(const Wt::WTouchEvent& e)
{
	Wt::Coordinates c = e.touches()[0].widget();
	path_.lineTo(c.x, c.y);

	if(prender_) 	{
		prender_->mouseAction(curOperation_, last_mouse_pos_, c);
	}
	last_mouse_pos_ = c;

	update(Wt::PaintUpdate);
}

void MPRWidget::touchStart(const Wt::WTouchEvent& e)
{
	Wt::Coordinates c = e.touches()[0].widget();
	path_ = Wt::WPainterPath(Wt::WPointF(c.x, c.y));
	last_mouse_pos_ = c;
}

void MPRWidget::mouseDrag(const Wt::WMouseEvent& e)
{
	Wt::Coordinates c = e.widget();
	path_.lineTo(c.x, c.y);

	if(prender_) 	{
		prender_->mouseAction(curOperation_, last_mouse_pos_, c);
	}
	last_mouse_pos_ = c;


	update(Wt::PaintUpdate);
}

void MPRWidget::mouseDown(const Wt::WMouseEvent& e)
{
	Wt::Coordinates c = e.widget();
	path_ = Wt::WPainterPath(Wt::WPointF(c.x, c.y));
	last_mouse_pos_ = c;
}

void MPRWidget::paintEvent(Wt::WPaintDevice *paintDevice)
{
	Wt::WPainter painter(paintDevice);
	DrawResultImage(painter);

//	painter.setRenderHint(Wt::WPainter::Antialiasing);
//	Wt::WPen pen;
//	pen.setWidth(3);
//	pen.setColor(color_);
//	pen.setCapStyle(Wt::FlatCap);
//	pen.setJoinStyle(Wt::MiterJoin);
//	painter.setPen(pen);
//	painter.drawPath(path_);

}

void MPRWidget::setColor(const Wt::WColor& c)
{
	color_ = c;
	
	curOperation_ = MPRView::opUnknown;
	for(int op = 0; op < (int)MPRView::opUnknown; op++)
	{
		if( buttoncolors[op] == c)  {
			curOperation_ = (MPRView::operations)op;
			break;
		}
	}

}

void MPRWidget::clear()
{
	update();
}

MPRWidget::MPRWidget(int width, int height, WContainerWidget *parent /*= 0*/) 
	: Wt::WPaintedWidget(parent), prender_(0), last_mouse_pos_(0, 0), pImageMemory(0), curOperation_(MPRView::opUnknown)
{
	resize(width, height);

	decorationStyle().setCursor("icons/pencil.cur", Wt::CrossCursor);

	mouseDragged().connect(this, &MPRWidget::mouseDrag);
	mouseWentDown().connect(this, &MPRWidget::mouseDown);
	touchStarted().connect(this, &MPRWidget::touchStart);
	touchMoved().connect(this, &MPRWidget::touchMove);
	touchMoved().preventDefaultAction();

	color_ = Wt::black;
}

void MPRWidget::DrawResultImage(Wt::WPainter &painter)
{
	if(!pImageMemory) 
	{
		pImageMemory = new Wt::WMemoryResource("image/png", this);
	}
	pImageMemory->suggestFileName("data.png");
	pImageMemory->setData(prender_->getImageData());

	Wt::WPainter::Image image(pImageMemory->url(), 512, 512);
	painter.drawImage(0, 0, image);

}


void MPRWidget::createRenderHandler(int rendermode)
{
	prender_ = new MPRView();
	prender_ -> createRender(rendermode);
}

MPRWidget::~MPRWidget()
{
	if(prender_) delete prender_;
}
