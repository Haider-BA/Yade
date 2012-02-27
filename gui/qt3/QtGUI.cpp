/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"QtGUI.hpp"
#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<yade/gui-qt3/GLViewer.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/bind.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
//#ifdef HAVE_CONFIG_H
//	#include <config.h>
//#endif
//#ifdef Q_WS_X11
	#include<X11/Xlib.h>
//#endif
#include<qapplication.h>

//#include<yade/gui-py/PythonUI.hpp>
#include<boost/thread.hpp>
#include<stdexcept>

QtGUI* QtGUI::self=NULL;
QApplication* QtGUI::app=NULL;

QtGUI::QtGUI(){self=this; mainWindowHidden=false; }
QtGUI::~QtGUI(){}

CREATE_LOGGER(QtGUI);

void QtGUI::help(){
	cerr<<"Qt3 GUI\n=======\n\t-h to get this help\n"<<endl;
}


int QtGUI::run(int argc, char *argv[])
{
	throw std::runtime_error("QtGUI::run should not be called!");
#if 0
	int ch;
	while( ( ch = getopt(argc,argv,"+hw") ) != -1)
		switch(ch){
			case 'w'	: mainWindowHidden=true; break;
			case 'h' : help(); return 0; break;
			default: break;	
		}
	if(optind<argc){ // process non-option arguments
		if(boost::algorithm::ends_with(string(argv[optind]),string(".xml")) ||
			boost::algorithm::ends_with(string(argv[optind]),string(".xml.gz")) ||
			boost::algorithm::ends_with(string(argv[optind]),string(".xml.bz2"))) Omega::instance().setSimulationFileName(string(argv[optind]));
		else if(boost::algorithm::ends_with(string(argv[optind]),string(".py"))) PythonUI::runScript=string(argv[optind]);
		else optind--;
	}
	for (int index=optind+1; index<argc; index++) {
		LOG_DEBUG("Adding script parameter `"<<argv[index]<<"' from the command line.");
		PythonUI::scriptArgs.push_back(string(argv[index]));
		if(PythonUI::runScript.empty()) LOG_WARN("Got parameter `"<<argv[index]<<"', but no .py script to be run!");
	}
	XInitThreads();
   app=new QApplication(argc,argv);
	mainWindow=new YadeQtMainWindow();
	mainWindow->show();
	app->setMainWidget(mainWindow);
	LOG_DEBUG("Launching Python thread now...");
	//PyEval_InitThreads();
	boost::thread pyThread(boost::function0<void>(&PythonUI::pythonSession));
	int res =  app->exec();
	delete mainWindow;
	return res;
#endif
}

bool QtGUI::checkDisplay(bool quiet){
	bool ret=(XOpenDisplay(NULL)!=0);
	if(!ret && !quiet) LOG_ERROR("Unable to open display `"<<getenv("DISPLAY")<<"'.");
	return ret;
}

bool QtGUI::runNaked(){
	if(!app){ // no app existing yet
		if(getenv("DISPLAY")==0){
			LOG_ERROR("$DISPLAY environment var not set, not starting qt3 gui.");
			return false;
		};
		LOG_DEBUG("Creating QApplication");
		XInitThreads();
		int argc=0;
	   app=new QApplication(argc,(char**)NULL);
		if(!YadeQtMainWindow::self){
			YadeQtMainWindow::guiMayDisappear=true;
			mainWindow=new YadeQtMainWindow();
			mainWindow->hide();
		} else { LOG_ERROR("Main window was there, but not QtGUI::app??"); }
		boost::thread appThread(boost::bind(&QApplication::exec,app));
	}
	return true;
}