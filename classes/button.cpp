#ifndef LEMUR_UI_BUTTON_CPP
#define LEMUR_UI_BUTTON_CPP

/**************************************************************************************
* OpenDraft:    GUI Button Class                                                      *
*-------------------------------------------------------------------------------------*
* Filename:     button.cpp                                                            *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI button class derived from component                                         *
***************************************************************************************/



#include "button.h"

namespace Lemur
{
	Button::Button(const int aX, const int aY, const int aWidth, const int aHeight, const std::string aText)
	{
		location.x = aX;
		location.y = aY;
		size.x = aWidth;
		size.y = aHeight;

		backColour = Colour::BACKGROUND1;
		backColourHover = Colour::lighten(Colour::BACKGROUND1, 0.4);
		backColourActive = Colour::lighten(Colour::BACKGROUND1, 0.6);

		foreColour = Colour::WHITE;
		foreColourHover = Colour::WHITE;
		foreColourActive = Colour::PRIMARY;

		padding = 10;

		stroke = Colour(0, 0, 0, 0);

		backgroundImage = nullptr;

		text = aText;
		fontSize = 14;
	}

	void Button::setBackgroundImage(Image* aImage)
	{
		backgroundImage = aImage;
	}

	void Button::clearBackgroundImage()
	{
		backgroundImage = nullptr;
	}

	void Button::DrawButtonState(NVGcontext* aContext, const float x, const float y, const Colour& backColour, const Colour& foreColour)
	{
		// Static cast parameters to int
		int w = static_cast<int>(size.x);
		int h = static_cast<int>(size.y);

		Draw::TextStyle buttonTextStyle =
		{
			fontSize,
			"sans",
			foreColour,
			Align(Align::CENTRE | Align::MIDDLE)
		};

		Draw::RoundedRect(aContext, x, y, w, h, 1, backColour);
		Draw::Text(aContext, x, y, w, h, &buttonTextStyle, text.c_str());
	}

	void Button::onFrame(NVGcontext* aContext)
	{
		// Calculate draw location
		float x = getLocation().x;
		float y = getLocation().y;

		// Draw button state
		if (mousePressDown)
			DrawButtonState(aContext, x, y, backColourActive, foreColourActive);

		else if (mouseOver)
			DrawButtonState(aContext, x, y, backColourHover, foreColourHover);

		else
			DrawButtonState(aContext, x, y, backColour, foreColour);


		// Draw background image if set
		if (backgroundImage != nullptr)
			Draw::ResourceImage(aContext, x + (padding/2), y + (padding/2), size.x - padding, size.y - padding, backgroundImage);


		Draw::RectStroke(aContext, x, y, size.x, size.y, 1, stroke);

		if (active)
			Draw::RoundedRectStroke(aContext, x, y, size.x, size.y, 1, 1, Colour::PRIMARY);

	}


	void Button::actionEvents(InputMap* aInput)
	{

	}


} // namespace Lemur

#endif // !LEMUR_UI_BUTTON_CPP