#ifndef LEMUR_DRAW_H
#define LEMUR_DRAW_H

/**************************************************************************************
* Lemur:        Graphic Draw Class                                                    *
*-------------------------------------------------------------------------------------*
* Filename:     Draw.h                                                                *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Draw routine wrappers for NanoVG intended for GUI rendering                       *
***************************************************************************************/



#include "Libraries/nanovg/src/nanovg.h"
#include "core.h"
#include <iostream>

namespace Lemur
{

	class Draw
	{
	public:

		// Structures
		struct TextStyle { int size; const char* font; Colour colour; Align align; };

		// Methods
		static void Line(NVGcontext* aContext, float aX1, float aY1, float aX2, float aY2, float thickness, Colour aColour);
		static void Rect(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, Colour aColour);
		static void RectStroke(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, float aWeight, Colour aColour);
		static void RoundedRect(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, float aRadius, Colour aColour);
		static void RoundedRectStroke(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, float aRadius, float aWeight, Colour aColour);
		static void Text(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, const TextStyle* aStyle, const char* aText);
		static void ResourceImage(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, Image* aImage);
		static void ResourceImage(NVGcontext* aContext, float aX, float aY, Image* aImage);
		static void Translate(NVGcontext* aContext, float aX, float aY);
		static void Scale(NVGcontext* aContext, float aX, float aY);
		static void Rotate(NVGcontext* aContext, float aAngle);
		static void ResetTransform(NVGcontext* aContext);
	};

} // namespace Lemur

#endif // !LEMUR_DRAW_H