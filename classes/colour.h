#ifndef LEMUR_COLOUR_H
#define LEMUR_COLOUR_H

/**************************************************************************************
* Lemur:        System Colour Class                                                   *
*-------------------------------------------------------------------------------------*
* Filename:     Colour.h                                                              *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A aColour class for the OpenDraft framework.                                      *
***************************************************************************************/



#include <nanovg.h>
#include "math.h"

namespace Lemur
{

	// Colour class
	class Colour
	{
	private:
		int r, g, b, a;

	public:

		//
		// Static methods
		//

		// Conversions
		const static float getHue(const Colour& aColour);
		const static float getSaturation(const Colour& aColour);
		const static float getLightness(const Colour& aColour);
		const static Colour asHex(const Colour& aColour);
		const static Colour fromHSL(float h, float s, float l);
		const static Colour fromHex(int hex);

		// Colour utilities
		const static Colour lighten(const Colour& aColour, float aAmount);
		const static Colour darken(const Colour& aColour, float aAmount);
		const static Colour saturate(const Colour& aColour, float aAmount);
		const static Colour mix(const Colour& colour1, const Colour& colour2, float aAmount);
		const static Colour invert(const Colour& aColour);
		const static Colour lerp(const Colour& colour1, const Colour& colour2, float aAmount);

		// Theme colours
		const static Colour BACKGROUND1;
		const static Colour BACKGROUND2;
		const static Colour PRIMARY;
		const static Colour CLEAR;
		const static Colour WHITE;

		// Standard colours
		const static Colour BLACK;
		const static Colour BLUE;
		const static Colour BROWN;
		const static Colour CYAN;
		const static Colour DARK_GREY;
		const static Colour GREEN;
		const static Colour GREY;
		const static Colour LIGHT_GREY;
		const static Colour LIME;
		const static Colour MAGENTA;
		const static Colour MAROON;
		const static Colour NAVY;
		const static Colour OLIVE;
		const static Colour ORANGE;
		const static Colour PINK;
		const static Colour PURPLE;
		const static Colour RED;
		const static Colour TEAL;
		const static Colour YELLOW;
	

		//
		// Constructors
		//
		Colour();
		Colour(int aRed, int aGreen, int aBlue, int aAlpha = 255);


		//
		// Getters and setters
		//
			
		// Getters and setters for red
		int getRed() const;
		float getRedNorm() const;
		void setRed(int aRed);

		// Getters and setters for green
		int getGreen() const;
		float getGreenNorm() const;
		void setGreen(int aGreen);

		// Getters and setters for blue
		int getBlue() const;
		float getBlueNorm() const;
		void setBlue(int aBlue);

		// Getters and setters for alpha
		int getAlpha() const;
		float getAlphaNorm() const;
		void setAlpha(int aAlpha);

		// Set aColour by RGB or RGBA
		void setRGB(int aRed, int aGreen, int aBlue, int aAlpha=255);

		// Returns the aColour as a NanoVG aColour
		NVGcolor asNvgColour() const;


		// Operator overloads
		bool operator==(const Colour& aColour) const;

		bool operator!=(const Colour& aColour) const;

		Colour operator=(const Colour& aColour);

	};

} // namespace Lemur

#endif // !LEMUR_COLOUR_H