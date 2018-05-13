#pragma once

#include "MPRView.h"

class MPRWidget : public Wt::WPaintedWidget
{
public:
	MPRWidget(int width, int height, Wt::WContainerWidget *parent = 0);
	virtual ~MPRWidget();

	void clear();
	void setColor(const Wt::WColor& c);

	void createRenderHandler(int rendermode);

protected:
	virtual void paintEvent(Wt::WPaintDevice *paintDevice);
	void DrawResultImage(Wt::WPainter &painter);

private:
	Wt::WPainterPath path_;
	Wt::WColor color_;
	MPRView* prender_;
	Wt::Coordinates last_mouse_pos_;
	Wt::WMemoryResource *pImageMemory;
	MPRView::operations curOperation_;

	void mouseDown(const Wt::WMouseEvent& e);
	void mouseDrag(const Wt::WMouseEvent& e);
	void touchStart(const Wt::WTouchEvent& e);
	void touchMove(const Wt::WTouchEvent& e);
};

Wt::WWidget *createMPRWidget(MPRWidget*& canvas); 