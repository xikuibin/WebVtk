
#pragma warning (disable : 4251 4275 4503 4996) // soma warning about boost

#include <iostream>

#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WHBoxLayout>
#include <Wt/WGridLayout>
#include <Wt/WBootstrapTheme>
#include <Wt/WCssTheme>
#include <Wt/WPainterPath>
#include <Wt/WPaintedWidget>
#include "SingleThreadedApplication.h"

#include "MPRWidget.h"
#include "MPRView.h"

using namespace Wt;


class WebVtkApplication : public SingleThreadedApplication
{
public:
  WebVtkApplication(const WEnvironment& env);

protected:
  virtual void create();
  virtual void destroy();
  void InitPaintApp(const Wt::WEnvironment &env, Wt::WApplication* app);

private:
 
  MPRWidget *pmprwidget_[3];
 
};

#ifndef WIN32
#include <sys/syscall.h>

pid_t mygettid()
{
     return syscall(SYS_gettid);
}
#endif

void WebVtkApplication::InitPaintApp(const Wt::WEnvironment &env, Wt::WApplication* app)
{
	// Choice of theme: defaults to bootstrap3 but can be overridden using
	// a theme parameter (for testing)
	const std::string *themePtr = env.getParameter("theme");
	std::string theme;
	if (!themePtr)
		theme = "bootstrap3";
	else
		theme = *themePtr;

	if (theme == "bootstrap3") {
		Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(app);
		bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
		bootstrapTheme->setResponsive(true);
		app->setTheme(bootstrapTheme);

		// load the default bootstrap3 (sub-)theme
		app->useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");
	} else if (theme == "bootstrap2") {
		Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(app);
		bootstrapTheme->setResponsive(true);
		app->setTheme(bootstrapTheme);
	} else
		app->setTheme(new Wt::WCssTheme(theme));

	app->setTitle("Wt single MPR window");


	Wt::WContainerWidget *container = new Wt::WContainerWidget(app->root());
	container->setWidth(1040);



 	Wt::WGridLayout *layout = new Wt::WGridLayout(container);
 	layout->setContentsMargins(0, 0, 0, 0);
 
 	layout->setRowResizable(0, false, 60);

#ifdef WIN32
	WString pInfo("");
#else
	WString pInfo("Information: ");
	pInfo += " \n parent process id=" + std::to_string((int)::getppid()) +
		" \n process id=" + std::to_string((int)::getpid()) +
		" \n pthread_self=" + std::to_string((int)pthread_self()) +
		" \n thread id=" + std::to_string((int)mygettid());
#endif

 	layout->addWidget(createMPRWidget(pmprwidget_[0]), 0, 0);
 	layout->addWidget(createMPRWidget(pmprwidget_[1]), 0, 1);	
 	layout->addWidget(createMPRWidget(pmprwidget_[2]), 1, 0);
 
 	layout->addWidget(new Wt::WText(pInfo), 1, 1, 1, 1, Wt::AlignCenter | Wt::AlignMiddle);

  	pmprwidget_[0]->createRenderHandler(0);
  	pmprwidget_[1]->createRenderHandler(1);	
 	pmprwidget_[2]->createRenderHandler(2);

}


WebVtkApplication::WebVtkApplication(const WEnvironment& env)
  : SingleThreadedApplication(env)
{
  /* Intentionally left blank! Actual construction happens in create() */
}

void WebVtkApplication::create()
{

  InitPaintApp(environment(), this);
   
}

void WebVtkApplication::destroy()
{
  root()->clear();
}


WApplication *createApplication(const WEnvironment& env)
{
	return new WebVtkApplication(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createApplication);
}

