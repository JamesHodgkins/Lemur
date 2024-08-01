#ifndef LEMUR_MAIN_WINDOW_H
#define LEMUR_MAIN_WINDOW_H

/**************************************************************************************
* Lemur:        Main Application Window Class                                         *
*-------------------------------------------------------------------------------------*
* Filename:     window_main.h                                                         *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A class for the main application window                                           *
***************************************************************************************/


#include "core.h"
#include "window.h"


namespace Lemur
{


	class MainWindow : public Window
	{
	protected:

		// Load required resources
		void loadResources();

	public:

		// Constructor
		MainWindow(int aWidth, int aHeight, const char* aTitle);

		// Destructor
		~MainWindow() override;

		// Delete copy constructor
		MainWindow(const MainWindow& aOther) = delete;

		// Delete move constructor
		MainWindow(MainWindow&& aOther) = delete;

		bool operator==(const MainWindow& other) const = default;

		// Initialize the window and UI Components
		void initialise() override;

		// Render the window and UI Components
		virtual void onFrame(NVGcontext* NULLREF = nullptr) override;

		// Action window events
		void actionEvents(InputMap* aInput);

	};

}// namespace Lemur

#endif // !LEMUR_MAIN_WINDOW_H