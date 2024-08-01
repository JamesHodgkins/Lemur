#ifndef LEMUR_MAIN_WINDOW_CPP
#define LEMUR_MAIN_WINDOW_CPP

/**************************************************************************************
* Lemur:        Main Application Window Class                                         *
*-------------------------------------------------------------------------------------*
* Filename:     window_main.cpp                                                       *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A class for the main application window                                           *
***************************************************************************************/



#include <chrono>
#include "window_main.h"
#include "application.h"
#include "components.h"


namespace Lemur
{

	// Load required resources
	void MainWindow::loadResources()
	{
		resourceManager->importFontFromFile(context, "sans", "..\\resources\\fonts\\OpenSans.ttf");

	}


	MainWindow::MainWindow(int aWidth, int aHeight, const char* aTitle) : Window(aWidth, aHeight, aTitle) {}

	MainWindow::~MainWindow() = default;


	// Initialize the window and UI Components
	void MainWindow::initialise()
	{
		loadResources();

		backColour.setRGB(31, 39, 48);


		// Create a new UI Panel Component
		Panel* panel1 = new Panel(50, 50, 140, 200);
		panel1->setName("My Panel 1");
		panel1->backColour = Colour(255, 255, 0, 255);
		addChildControl(panel1);
		panel1->setAnchor(Anchor::Direction::Left, true);
		panel1->setAnchor(Anchor::Direction::Right,true);

		// Create second panel
		Panel* panel2 = new Panel(50, 120, 80, 70);
		panel2->setName("My Panel 2");
		panel2->backColour = Colour(255, 0, 255, 255);
		//panel2->setAnchor(Anchor::Direction::Left, true);
		panel2->setAnchor(Anchor::Direction::Right, true);
		panel1->addChildControl(panel2);

		// Create a new UI Component
		Button* btn1 = new Button(10, 10, 100, 50, "Button 1");
		btn1->setName("My Button 1");
		panel1->addChildControl(btn1);

		// Create a new UI Component
		Button* btn2 = new Button(120, 10, 100, 50, "Button 2");
		btn1->setName("My Button 2");
		panel1->addChildControl(btn2);

		// Create a new UI Component
		Button* btn3 = new Button(10, 10, 100, 50, "Button 3");
		btn3->setName("My Button 3");
		panel2->addChildControl(btn3);

	}


	// Render the window and UI Components
	void MainWindow::onFrame(NVGcontext* NULLREF)
	{
		Application* app = Application::getInstance();

		
	}


	// Action main window events
	void MainWindow::actionEvents(InputMap* aInput)
	{

	}

}// namespace Lemur

#endif // !LEMUR_MAIN_WINDOW_CPP