#ifndef LEMUR_INPUT_H
#define LEMUR_INPUT_H

/**************************************************************************************
* Lemur:        UI Input Class                                                        *
*-------------------------------------------------------------------------------------*
* Filename:     Input.h                                                             *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Inptut map to store the state of inputs against a glfw window instance            *
***************************************************************************************/



#include <unordered_map>
#include "glfw/glfw3.h"
#include <iostream>


namespace Lemur
{
	class KeyInput
	{
	private:
		bool down = false;
		bool pressed = false;
		bool released = false;

	public:

		void changeState(bool aState)
		{
			if (!aState && !down)
				released = false;

			pressed = (!down && aState);
			released = (down && !aState);
			down = aState;
		}

		bool isDown() const { return down; }
		bool isPressDown() const { return pressed; }
		bool isPressUp() const { return released; }
	};


	class InputMap
	{
	private:
		struct PositionStruct
		{
			int y;
			int x;
		} position;


	public:

		std::unordered_map<int, KeyInput>keys;
		struct MouseStruct
		{
			KeyInput leftButton;
			KeyInput middleButton;
			KeyInput rightButton;
			PositionStruct position;
			int scroll;
		} mouse;

		bool capsLock = false;


		InputMap()
		{
			mouse.scroll = 0;
			mouse.position.x = 0;
			mouse.position.y = 0;


			keys = {
				{GLFW_KEY_A, KeyInput()},
				{GLFW_KEY_B, KeyInput()},
				{GLFW_KEY_C, KeyInput()},
				{GLFW_KEY_D, KeyInput()},
				{GLFW_KEY_E, KeyInput()},
				{GLFW_KEY_F, KeyInput()},
				{GLFW_KEY_G, KeyInput()},
				{GLFW_KEY_H, KeyInput()},
				{GLFW_KEY_I, KeyInput()},
				{GLFW_KEY_J, KeyInput()},
				{GLFW_KEY_K, KeyInput()},
				{GLFW_KEY_L, KeyInput()},
				{GLFW_KEY_M, KeyInput()},
				{GLFW_KEY_N, KeyInput()},
				{GLFW_KEY_O, KeyInput()},
				{GLFW_KEY_P, KeyInput()},
				{GLFW_KEY_Q, KeyInput()},
				{GLFW_KEY_R, KeyInput()},
				{GLFW_KEY_S, KeyInput()},
				{GLFW_KEY_T, KeyInput()},
				{GLFW_KEY_U, KeyInput()},
				{GLFW_KEY_V, KeyInput()},
				{GLFW_KEY_W, KeyInput()},
				{GLFW_KEY_X, KeyInput()},
				{GLFW_KEY_Y, KeyInput()},
				{GLFW_KEY_Z, KeyInput()},
				{GLFW_KEY_0, KeyInput()},
				{GLFW_KEY_1, KeyInput()},
				{GLFW_KEY_2, KeyInput()},
				{GLFW_KEY_3, KeyInput()},
				{GLFW_KEY_4, KeyInput()},
				{GLFW_KEY_5, KeyInput()},
				{GLFW_KEY_6, KeyInput()},
				{GLFW_KEY_7, KeyInput()},
				{GLFW_KEY_8, KeyInput()},
				{GLFW_KEY_9, KeyInput()}
			};
		}

	};

} // namespace Lemur

#endif // !LEMUR_INPUT_H