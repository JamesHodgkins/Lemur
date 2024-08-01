#ifndef LEMUR_UI_PANEL_CPP
#define LEMUR_UI_PANEL_CPP

/**************************************************************************************
* OpenDraft:    GUI Panel Container Class                                             *
*-------------------------------------------------------------------------------------*
* Filename:     panel.cpp                                                           *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI panel container class derived from component                              *
***************************************************************************************/



#include "Panel.h"


namespace Lemur
{
	/**
	* \brief Constructs a Panel object with the specified attributes.
	* \param location (Point) The position of the panel.
	* \param size (Point) The size of the panel.
	* \param text (std::string) The text displayed on the panel.
	*/
	Panel::Panel(int aX, int aY, int aWidth, int aHeight)
	{
		location.x = aX;
		location.y = aY;
		size.x = aWidth;
		size.y = aHeight;

		backColour = Colour::BACKGROUND1;
		backColourHover = Colour::BACKGROUND2;
		backColourActive = Colour(0, 0, 255, 255);
		stroke = Colour(0, 0, 0, 0);
		foreColour = Colour(255, 255, 255, 200);

		backgroundImage = nullptr;

		text = "";
	}

	void Panel::setBackgroundImage(Image* aImage)
	{
		backgroundImage = aImage;
	}

	void Panel::clearBackgroundImage()
	{
		backgroundImage = nullptr;
	}


	/**
	* \brief Renders a default OD-GUI Panel to a given NanoVG context (NVGContext) with the specified attributes.
	* \param context (NVGcontext*) The nanovg pointer for rendering.
	*/
	void Panel::onFrame(NVGcontext* aContext)
	{
		// Static cast properties
		float x = getLocation().x;
		float y = getLocation().y;
		float w = size.x;
		float h = size.y;

		//
		// Begin drawing Panel
		//
		Draw::Rect(aContext, x, y, w, h, backColour);

		if (backgroundImage != nullptr)
			Draw::ResourceImage(aContext, x, y, w, h, backgroundImage);

		Draw::RectStroke(aContext, x, y, w, h, 0.5, stroke);

		// Draw child UI components
		drawChildComponents(aContext);

	}


	void Panel::actionEvents(InputMap* aInput)
	{

	}

		
}// namespace Lemur


#endif // !LEMUR_UI_PANEL_CPP