#ifndef LEMUR_UI_LABEL_CPP
#define LEMUR_UI_LABEL_CPP

/**************************************************************************************
* OpenDraft:    GUI Label Class                                                       *
*-------------------------------------------------------------------------------------*
* Filename:     label.cpp                                                           *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI label class derived by component                                          *
***************************************************************************************/



#include <iostream>
#include <sstream>
#include "label.h"


namespace Lemur
{
	/**
		* \brief Constructs a Label object with the specified attributes.
		* \param location (Point) The position of the Label.
		* \param size (Point) The size of the Label.
		* \param text (std::string) The text displayed on the Label.
		*/
	Label::Label(int aX, int aY, int aWidth, int aHeight, std::string aText)
	{
		setLocation(aX, aY);
		setSize(aWidth, aHeight);

		backColour = Colour::CLEAR;
		foreColour = Colour(255, 255, 255, 200);

		text = aText;
	}


	// Getters and Setters

	// Getters and Setters
	bool Label::isSingleLine() const
	{
		return singleLine;
	}

	void Label::setSingleLine(bool aSingleLine)
	{
		singleLine = aSingleLine;
	}

	bool Label::isTextWrap() const
	{
		return wrapText;
	}

	void Label::setTextWrap(bool aWrapText)
	{
		wrapText = aWrapText;
	}

	Align Label::getAlign() const
	{
		return align;
	}
			
	void Label::setAlign(Align aAlign)
	{
		align = aAlign;
	}
			
	float Label::getLineSpacingFactor() const
	{
		return lineSpacingFactor;
	}

	void Label::setLineSpacingFactor(float aLineSpacingFactor)
	{
		lineSpacingFactor = aLineSpacingFactor;
	}

	// Text Functions
	std::vector<std::string> Label::getTextByLines() const
	{
		std::vector<std::string> lines;
		std::string line;
		std::istringstream iss(text);
		while (std::getline(iss, line))
			lines.push_back(line);
		return lines;
	}


	/**
		* \brief Renders a default OD-GUI Label to a given NanoVG context (NVGContext) with the specified attributes.
		* \param context (NVGcontext*) The nanovg pointer for rendering.
		*/
	void Label::onFrame(NVGcontext* aContext)
	{
		if (aContext == nullptr)
			return;

		//
		// Begin drawing Label
		//
		float x = getLocation().x;
		float y = getLocation().y;
		float w = size.x;
		float h = size.y;

		// Has size changed?
		if (w != lastSize.x || h != lastSize.y)
		{
			linesDirty = true;
			lastSize = size;
		}

		// Recalculate lines if dirty (text changes, or resize)
		if (linesDirty)
			lines = getTextByLines();
				
		Draw::TextStyle labelTextStyle =
		{
			14,
			"sans",
			foreColour,
			Align(Align::LEFT | Align::MIDDLE)
		};


		if (singleLine || !wrapText)
		{
			// Get bounding box
			float x = location.x;
			float y = location.y;
			float w = size.x;
			float h = size.y;

			nvgSave(aContext);
			nvgScissor(aContext, x, y, w, h);

			Draw::Rect(aContext, x, y, w, h, backColour);
			Draw::Text(aContext, x, y, w, h, &labelTextStyle, lines[0].c_str());

			nvgRestore(aContext);

		}
		else if (wrapText)
		{
			// Get bounding box
			Draw::Rect(aContext, x, y, w, h, backColour);

			// Calculate the total height of all lines with appropriate spacing
			float lineSpace = labelTextStyle.size * lineSpacingFactor;
			float startY = y;

			if (align.getAlign() & Align::MIDDLE)
			{
				startY = y + (h - (lines.size() * lineSpace)) / 2;
			}
					
			if (align.getAlign() & Align::BOTTOM)
			{
				startY = y + h - (lines.size() * lineSpace);
			}


			// Print lines from startY
			for (int i = 0; i < lines.size(); i++)
			{
				// Update the y position for each line
				float lineY = startY + i * lineSpace;
				Draw::Text(aContext, x, lineY, w, labelTextStyle.size, &labelTextStyle, lines[i].c_str());
			}

					

		}

	}

	void Label::actionEvents(InputMap* aInput)
	{

	}

} // namespace Lemur

#endif // !LEMUR_UI_LABEL_CPP