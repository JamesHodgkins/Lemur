#ifndef LEMUR_DRAW_CPP
#define LEMUR_DRAW_CPP

/**************************************************************************************
* Lemur:        Graphic Draw Class                                                    *
*-------------------------------------------------------------------------------------*
* Filename:     Draw.cpp                                                              *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Draw routine wrappers for NanoVG intended for GUI rendering                       *
***************************************************************************************/



#include "Draw.h"

namespace Lemur
{
	void Draw::Line(NVGcontext* aContext, float aX1, float aY1, float aX2, float aY2, float thickness, Colour aColour)
	{
		nvgBeginPath(aContext);
		nvgMoveTo(aContext, aX1, aY1);
		nvgLineTo(aContext, aX2, aY2);
		nvgStrokeWidth(aContext, thickness);
		nvgStrokeColor(aContext, aColour.asNvgColour());
		nvgStroke(aContext);
		nvgClosePath(aContext);
	}

	void Draw::Rect(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, const Colour aColour)
	{
		nvgBeginPath(aContext);
		nvgMoveTo(aContext, aX, aY);
		nvgRect(aContext, aX, aY, aWidth, aHeight);
		nvgFillColor(aContext, aColour.asNvgColour());
		nvgFill(aContext);
		nvgClosePath(aContext);
	}

	void Draw::RectStroke(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, float aWeight, const Colour aColour)
	{
		nvgBeginPath(aContext);
		nvgRect(aContext, aX, aY, aWidth, aHeight);
		nvgStrokeColor(aContext, aColour.asNvgColour());
		nvgStrokeWidth(aContext, aWeight);
		nvgStroke(aContext);
		nvgClosePath(aContext);
	}

	void Draw::RoundedRect(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, float aRadius, const Colour aColour)
	{
		nvgBeginPath(aContext);
		nvgRoundedRect(aContext, aX, aY, aWidth, aHeight, aRadius);
		nvgFillColor(aContext, aColour.asNvgColour());
		nvgFill(aContext);
		nvgClosePath(aContext);
	}

	void Draw::RoundedRectStroke(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, float aRadius, float aWeight, const Colour aColour)
	{
		nvgBeginPath(aContext);
		nvgRoundedRect(aContext, aX, aY, aWidth, aHeight, aRadius);
		nvgStrokeColor(aContext, aColour.asNvgColour());
		nvgStrokeWidth(aContext, aWeight);
		nvgStroke(aContext);
		nvgClosePath(aContext);
	}

	void Draw::Text(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, const TextStyle* aStyle, const char* text)
	{
		nvgBeginPath(aContext);
		nvgFillColor(aContext, aStyle->colour.asNvgColour());
		nvgFill(aContext);
		nvgFontSize(aContext, aStyle->size);
		nvgFontFace(aContext, aStyle->font);

		// Set text alignment - TODO: Add support for vertical alignment
		if (aStyle->align.getAlign() & Align::LEFT)
		{
			nvgTextAlign(aContext, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
			nvgText(aContext, aX, aY + aHeight / 2, text, nullptr);
		}
		else if (aStyle->align.getAlign() & Align::RIGHT)
		{
			nvgTextAlign(aContext, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
			nvgText(aContext, aX + aWidth / 2, aY + aHeight / 2, text, nullptr);
		}
		else
		{
			nvgTextAlign(aContext, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			nvgText(aContext, aX + aWidth / 2, aY + aHeight / 2, text, nullptr);
		}
		
		nvgClosePath(aContext);
	}

	void Draw::ResourceImage(NVGcontext* aContext, float aX, float aY, float aWidth, float aHeight, Image* aImage)
	{
		// Check if image is valid
		if (aImage == nullptr)
			return;

		if (aImage->getId() != 0)
		{
			nvgBeginPath(aContext);
			nvgRect(aContext, aX, aY, aWidth, aHeight);
			nvgFillPaint(aContext, nvgImagePattern(aContext, aX, aY, aWidth, aHeight, 0, aImage->getId(), 1.0f));
			nvgFill(aContext);
			nvgClosePath(aContext);
		}
	}

	void Draw::ResourceImage(NVGcontext* aContext, float aX, float aY, Image* aImage)
	{
		// Check if image is valid
		if (aImage == nullptr)
			return;
		
		Draw::ResourceImage(aContext, aX, aY, aImage->getWidth(), aImage->getHeight(), aImage);
	}

	void Draw::Translate(NVGcontext* aContext, float aX, float aY)
	{
		nvgTranslate(aContext, aX, aY);
	}

	void Draw::Scale(NVGcontext* aContext, float aX, float aY)
	{
		nvgScale(aContext, aX, aY);
	}

	void Draw::Rotate(NVGcontext* aContext, float aAngle)
	{
		nvgRotate(aContext, aAngle);
	}

	void Draw::ResetTransform(NVGcontext* aContext)
	{
		nvgResetTransform(aContext);
	}

} // namespace Lemur

#endif // !LEMUR_DRAW_CPP