#ifndef LEMUR_WINDOW_H
#define LEMUR_WINDOW_H

/**************************************************************************************
* Lemur:        GUI Window Class                                                      *
*-------------------------------------------------------------------------------------*
* Filename:     Window.h                                                              *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI base window class                                                           *
***************************************************************************************/

#include <vector>
#include <iostream>

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Component.h"

namespace Lemur
{
	class Window : public Component
	{
	protected:
		GLFWwindow* glfwHandle = nullptr;       // Handle to the GLFW window
		struct NVGcontext* context = nullptr;   // NanoVG context

		// Update properties following initialization or resize
		void updateProperties();

		// Load required resources
		void loadResources();


	public:

		InputMap input;                       // Input map for storing user input


		// Constructor
		Window(int aWidth, int aHeight, const char* aTitle);

		// Set the window size
		void setSize(int aWidth, int aHeight);

		// Get the window size
		float getWidth();
		float getHeight();

		// Check if the window is still running
		bool isRunning();

		// Set the current context
		void makeCurrentContext();

		// Get the NanoVG context
		NVGcontext* getContext();

		// Reset the NanoVG context
		void resetContext();

		// Returns 0,0 as window will always be at the root of the Component tree
		Vector2 getRelativeLocation();

		// Close the window and clean up resources
		void close();

		// Initialize the window and UI Components
		virtual void initialise();

		// Render the window and UI Components
		virtual void onFrame(NVGcontext* context) override;

		// End the frame
		void endFrame();

		// Action window events
		void actionEvents(InputMap* aInput);

		// Process events for UI Components
		void triggerEventsChain();
		void closeEvents();

		// Callback function for mouse movement
		static void mousePositionEventCallback(GLFWwindow* aWindow, double aPositionX, double aPositionY);

		// Callback function for window resize
		static void windowResizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight);

		// Callback function for mouse clicks
		static void mouseClickEventCallback(GLFWwindow* aWindow, int aButton, int aAction, int aMods);

		// Callback function for mouse scroll
		static void mouseScrollEventCallback(GLFWwindow* aWindow, double aOffsetX, double aOffsetY);

		// Callback function for key events
		static void keyEventCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aMods);

		// Callback function for cursor entering/leaving window
		static void cursorEnterEventCallback(GLFWwindow* aWindow, int aEntered);


	};

} // namespace Lemur

#endif // !LEMUR_WINDOW_H
