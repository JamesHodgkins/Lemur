#ifndef LEMUR_APPLICATION_CPP
#define LEMUR_APPLICATION_CPP

/**************************************************************************************
* Lemur:        System Application Class                                              *
*-------------------------------------------------------------------------------------*
* Filename:     Application.cpp                                                       *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Root application class                                                            *
*                                                                                     *
* Notes:                                                                              *
*   This class is a singleton, which is currently not thread safe.                    *
***************************************************************************************/


#include "application.h"

namespace Lemur
{
	Application* Application::getInstance()
	{
		// Check if the application instance has already been created
		if (instance_ != nullptr)
			return instance_;

		// Create the application instance
		instance_ = new Application();

		// Initialize the application
		instance_->initialise();
	}

	Application::Application() {}


	Application::~Application()
	{
		mainWindow->close();
		delete resManager;
		delete mainWindow;
	}

	void Application::initialise()
	{
		// Initialize the application
		resManager = new ResourceManager();
		mainWindow = new MainWindow(1280, 720, "OpenDraft");
		mainWindow->resourceManager = resManager;
		mainWindow->initialise();
		running = true;
	}


	bool Application::isRunning()
	{
		return running;
	}


	// Update the application frame
	void Application::update()
	{
		if (!mainWindow->isRunning())
		{
			running = false;
			return;
		}

		mainWindow->triggerEventsChain();
		mainWindow->resetContext();
		mainWindow->onFrame();
		mainWindow->endFrame();
	}

	// Get the main window instance
	MainWindow* Application::getMainWindow()
	{
		return mainWindow;
	}


	// Static reference to the application instance
	Application* Application::instance_ = nullptr;

	

} // namespace Lemur

#endif // !LEMUR_APPLICATION_CPP
