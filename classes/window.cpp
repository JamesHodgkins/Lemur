#ifndef LEMUR_WINDOW_CPP
#define LEMUR_WINDOW_CPP

/**************************************************************************************
* Lemur:        GUI Window Class                                                      *
*-------------------------------------------------------------------------------------*
* Filename:     Window.cpp                                                            *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI base window class                                                           *
***************************************************************************************/



#define GLEW_STATIC						// Use static glew library (glew32s.lib)
#include "GL/glew.h"					// Include glew

#include "nanovg.h"						// Include core nanovg library

#ifndef NANOVG_GL3_IMPLEMENTATION
	#define NANOVG_GL3_IMPLEMENTATION	// Use GL3.
	#include "nanovg_gl.h"				// Include nanovg opengl3 implementation
#endif


#include "window.h"
#include "application.h"


namespace Lemur
{
	void Window::updateProperties()
	{
		int width = 0, height = 0;
		glfwGetWindowSize(glfwHandle, &width, &height);
		setSize(width, height);
	}

	void Window::loadResources()
	{
		// To be overridden by derived classes
	}

	Window::Window(int aWidth, int aHeight, const char* aTitle)
	{
		setSize(aWidth, aHeight);
		input = InputMap();
		text = aTitle;

		// Initialize GLFW
		if (!glfwInit())
			return;

		// Static cast parameters to float
		int w = static_cast<int>(aWidth);
		int h = static_cast<int>(aHeight);

		// Create window with graphics context
		glfwHandle = glfwCreateWindow(w, h, aTitle, nullptr, nullptr);
		if (glfwHandle == nullptr)
			return;

		glfwMakeContextCurrent(glfwHandle);       // Set context as current
		glfwSwapInterval(1);                      // Enable vsync
		glewInit();                               // Initialize glew

		// Initialize NanoVG context (OpenGL backend)
		//context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
		context = nvgCreateGL3(0);
		if (context == nullptr) {
			return;
		}

		// Bind this instance of GrWindow to the glfw window instance for static callbacks
		glfwSetWindowUserPointer(glfwHandle, this);

		// Set the callback function for mouse movement
		glfwSetCursorPosCallback(glfwHandle, mousePositionEventCallback);
		glfwSetMouseButtonCallback(glfwHandle, mouseClickEventCallback);
		glfwSetScrollCallback(glfwHandle, mouseScrollEventCallback);
		glfwSetKeyCallback(glfwHandle, keyEventCallback);
		glfwSetWindowSizeCallback(glfwHandle, windowResizeCallback);
		glfwSetCursorEnterCallback(glfwHandle, cursorEnterEventCallback);

		// Capture caps lock state
		glfwSetInputMode(glfwHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

		// Update some properties following intitialisation
		updateProperties();
	}

	void Window::mousePositionEventCallback(GLFWwindow* aWindow, double aPositionX, double aPositionY)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(aWindow));
		if (windowInstance) {
			// Access the instance and store the mouse position
			windowInstance->input.mouse.position.x = static_cast<int>(aPositionX);
			windowInstance->input.mouse.position.y = static_cast<int>(aPositionY);
		}
	}

	void Window::windowResizeCallback(GLFWwindow* aWindow, int aWidth, int aHeight)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(aWindow));
		if (windowInstance) {
			// Access the instance and update size
			windowInstance->updateProperties();
			windowInstance->onFrame(windowInstance->context);
		}
	}

	void Window::mouseClickEventCallback(GLFWwindow* aWindow, int aButton, int aAction, int aMods)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(aWindow));
		if (windowInstance) {
			switch (aButton) {
			case 0:
				if (aAction == GLFW_PRESS)
					windowInstance->input.mouse.leftButton.changeState(true);
				if (aAction == GLFW_RELEASE)
					windowInstance->input.mouse.leftButton.changeState(false);
				break;

			case 1:
				if (aAction == GLFW_PRESS)
					windowInstance->input.mouse.rightButton.changeState(true);
				if (aAction == GLFW_RELEASE)
					windowInstance->input.mouse.rightButton.changeState(false);
				break;

			case 2:
				if (aAction == GLFW_PRESS)
					windowInstance->input.mouse.middleButton.changeState(true);
				if (aAction == GLFW_RELEASE)
					windowInstance->input.mouse.middleButton.changeState(false);
				break;
			}
		}
	}

	// Callback function for mouse scroll
	void Window::mouseScrollEventCallback(GLFWwindow* aWindow, double aOffsetX, double aOffsetY)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(aWindow));
		if (windowInstance) {
			// Access the instance and store the mouse scroll offset
			windowInstance->input.mouse.scroll = static_cast<int>(aOffsetY);
		}
	}

	void Window::keyEventCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aMods)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(aWindow));
		if (windowInstance) {
			InputMap* map = &windowInstance->input;

			if (map->keys.find(aKey) == map->keys.end())
				return;

			// Change button state to down
			if (aAction == GLFW_PRESS)
				map->keys[aKey].changeState(true);

			// Change button state to up
			if (aAction == GLFW_RELEASE)
				map->keys[aKey].changeState(false);


			if (aMods & GLFW_MOD_CAPS_LOCK)
				map->capsLock = true;
			else
				map->capsLock = false;
				

		}
	}

	void Window::cursorEnterEventCallback(GLFWwindow* aWindow, int aEntered)
	{
		Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(aWindow));
		
		if (aEntered == GLFW_TRUE)
			windowInstance->mouseOver = true;
		else
			windowInstance->mouseOver = false;

	}

	void Window::setSize(int aWidth, int aHeight)
	{
		size.x = aWidth;
		size.y = aHeight;

		if (glfwHandle)
			glfwSetWindowSize(glfwHandle, aWidth, aHeight);
	}

	float Window::getWidth()
	{
		return static_cast<float>(size.x);
	}

	float Window::getHeight()
	{
		return static_cast<float>(size.y);
	}

	bool Window::isRunning()
	{
		return !glfwWindowShouldClose(glfwHandle);
	}

	void Window::makeCurrentContext()
	{
		glfwMakeContextCurrent(glfwHandle);
	}

	NVGcontext* Window::getContext()
	{
		return context;
	}

	void Window::resetContext()
	{
		// If the context is not null, reset it
		if (context != nullptr)
		{
			nvgReset(context);

			glClearColor(
				backColour.getRedNorm(),
				backColour.getGreenNorm(),
				backColour.getBlueNorm(),
				1.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Cast window size
			float w = getWidth();
			float h = getHeight();

			glViewport(0, 0, w, h);

			nvgBeginFrame(context, w, h, 1);
		}
	}

	Vector2 Window::getRelativeLocation()
	{
		return Vector2(0, 0);
	}

	void Window::close()
	{
		glfwDestroyWindow(glfwHandle);
		glfwTerminate();

		childComponents.clear();

		nvgDeleteGL3(context);
	}

	void Window::initialise()
	{

	}

	void Window::onFrame(NVGcontext* context)
	{
		glfwPollEvents();
	}

	void Window::endFrame()
	{
		if (context == nullptr)
			return;

		// Draw child UI Components
		drawChildComponents(context);

		nvgEndFrame(context);

		closeEvents();

		glfwPollEvents();
		glfwSwapBuffers(glfwHandle);
	}

	void Window::actionEvents(InputMap* aInput)
	{

	}

	void Window::triggerEventsChain()
	{
		// Get mouse position from input
		int mousePosX = static_cast<int>(input.mouse.position.x);
		int mousePosY = static_cast<int>(input.mouse.position.y);


		if (mouseOver && !mouseEnter)
			mouseEnter = true;

		else if (!mouseOver && !mouseLeave)
			mouseLeave = true;


		if (mouseOver && input.mouse.leftButton.isPressDown())
		{
			mousePressDown = true;
			active = true;
		}

		// Check if mouse was pressed down
		if (!input.mouse.leftButton.isDown())
			mousePressDown = false;

		// Check if mouse was released
		if (mouseOver && input.mouse.leftButton.isPressUp())
			mousePressUp = true;
		else
			mousePressUp = false;

		if (!mouseOver && input.mouse.leftButton.isPressUp())
			active = false;




		for (std::shared_ptr<Component> control : childComponents)
		{
			control->processEvents(&input);
		}

		Application* app = Application::getInstance();
	}

	void Window::closeEvents()
	{
		input.mouse.leftButton.changeState(input.mouse.leftButton.isDown());
		input.mouse.middleButton.changeState(input.mouse.middleButton.isDown());
		input.mouse.rightButton.changeState(input.mouse.rightButton.isDown());
		input.mouse.scroll = 0;

		for (auto& pair : input.keys) {
			int key = pair.first;
			input.keys[key].changeState(input.keys[key].isDown());
		}

	}
} // namespace Lemur

#endif // !LEMUR_WINDOW_CPP
