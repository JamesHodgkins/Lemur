#ifndef LEMUR_APPLICATION_H
#define LEMUR_APPLICATION_H

/**************************************************************************************
* Lemur:        System Application Class                                              *
*-------------------------------------------------------------------------------------*
* Filename:     Application.h                                                         *
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



#include <iostream>
#include "core.h"					// Include Core Utilities
#include "window_main.h"			// Include Main Window Class
#include "resource_manager.h"		// Include Resource Manager


namespace Lemur
{
	// Application Class
	class Application
	{
		Application(Application& other) = delete;
		void operator=(const Application&) = delete;
		
	protected:
		// Constructor
		Application();

	private:

		// Static reference to the application instance
		static Application* instance_;		// Pointer to the application instance

		bool running = false;					// Flag to indicate if the application is still running
		MainWindow* mainWindow = nullptr;		// Pointer to the main window
		ResourceManager* resManager;			// Pointer to the resource manager
			

	public:

		// Get the application instance
		static Application* getInstance();

		// Destructor
		~Application();

		// Initialize the application
		void initialise();

		// Check if the application is still running
		bool isRunning();

		// Update the application for the next frame
		void update();

		// Get the main window
		MainWindow* getMainWindow();

	};

}// namespace Lemur


#endif // !LEMUR_APPLICATION_H
