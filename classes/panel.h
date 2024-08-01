#ifndef LEMUR_UI_PANEL_H
#define LEMUR_UI_PANEL_H

/**************************************************************************************
* OpenDraft:    GUI Panel Container Class                                             *
*-------------------------------------------------------------------------------------*
* Filename:     panel.h                                                               *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI panel container class derived from component                                *
***************************************************************************************/



#include "core.h"
#include "component.h"
#include "draw.h"


namespace Lemur
{
	class Panel : public Component
	{
	protected:
		Colour backColourHover;
		Colour backColourActive;
		Image* backgroundImage;

	public:

		/**
			* \brief Constructs a Panel object with the specified attributes.
			* \param location (Point) The position of the panel.
			* \param size (Point) The size of the panel.
			* \param text (std::string) The text displayed on the panel.
			*/
		Panel(int aX = 0, int aY = 0, int aWidth = 150, int aHeight = 30);

		void setBackgroundImage(Image* aImage);
		void clearBackgroundImage();
			

		/**
		* \brief Renders a default OD-GUI Panel to a given NanoVG context (NVGContext) with the specified attributes.
		* \param context (NVGcontext*) The nanovg pointer for rendering.
		*/
		virtual void onFrame(NVGcontext* aContext) override;

			
		void actionEvents(InputMap* aInput);
			
	};

}// namespace Lemur


#endif // !LEMUR_UI_PANEL_H