#ifndef LEMUR_UI_BUTTON_H
#define LEMUR_UI_BUTTON_H

/**************************************************************************************
* OpenDraft:    GUI Button Class                                                      *
*-------------------------------------------------------------------------------------*
* Filename:     button.h                                                              *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI button class derived from component                                         *
***************************************************************************************/



#include "core.h"
#include "component.h"
#include "draw.h"

namespace Lemur
{
	class Button : public Component
	{
	protected:
		Image* backgroundImage;
		float fontSize;
		float padding;

	public:
		Colour backColourHover;
		Colour foreColourHover;
		Colour backColourActive;
		Colour foreColourActive;

		/**
			* \brief Constructs a Button object with the specified attributes.
			* \param location (Point) The position of the button.
			* \param size (Point) The size of the button.
			* \param text (std::string) The text displayed on the button.
			*/

		Button(const int aX = 0, const int aY = 0, const int aWidth = 150, const int aHeight = 30, const std::string aText = "Button");

		/**
			* \brief Sets the background image of the button. 
			* \param aImage (Image*) The image to set as the background.
			* \return void
			*/
		void setBackgroundImage(Image* aImage);

		/**
			* \brief Sets font size.
			* \param aFontSize (int) The font size to set.
			* \return void
			*/
		void setFontSize(int aFontSize) { fontSize = aFontSize; }

		/**
			* \brief Clears the background image of the button.
			* \return void
			*/
		void clearBackgroundImage();

		/**
			* \brief Draws the button to the specified NanoVG context (NVGContext) with the specified attributes.
			* \param aContext (NVGcontext*) The nanovg pointer for rendering.
			* \param x The x position of the button.
			* \param y The y position of the button.
			* \param backColour The background colour of the button.
			* \param foreColor The foreground colour of the button.
			*/
		void DrawButtonState(NVGcontext* aContext, const float x, const float y, const Colour& backColour, const Colour& foreColour);

		/**
			* \brief Renders a default OD-GUI Button to a given NanoVG context (NVGContext) with the specified attributes.
			* \param context (NVGcontext*) The nanovg pointer for rendering.
			*/
		virtual void onFrame(NVGcontext* aContext) override;


		void actionEvents(InputMap* aInput) override;

	};

} // namespace Lemur

#endif // !LEMUR_UI_BUTTON_H
