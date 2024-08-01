#ifndef LEMUR_UI_LABEL_H
#define LEMUR_UI_LABEL_H

/**************************************************************************************
* OpenDraft:    GUI Label Class                                                       *
*-------------------------------------------------------------------------------------*
* Filename:     label.h                                                               *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI label class derived by component                                            *
***************************************************************************************/



#include <string>
#include "core.h"
#include "component.h"
#include "draw.h"


namespace Lemur
{
	class Label : public Component
	{
	protected:
		bool singleLine = true;
		bool wrapText = false;
		Align align = Align(Align::LEFT | Align::MIDDLE);
		float lineSpacingFactor = 1.0f;

		// Cached calculated lines
		std::vector<std::string> lines;
		bool linesDirty = true;
		Vector2 lastSize;

	public:

		/**
			* \brief Constructs a Label object with the specified attributes.
			* \param location (Point) The position of the Label.
			* \param size (Point) The size of the Label.
			* \param text (std::string) The text displayed on the Label.
			*/
		Label(int aX = 0, int aY = 0, int aWidth = 150, int aHeight = 30, std::string aText = "Label");

		// Getters and Setters
		bool isSingleLine() const;
		void setSingleLine(bool aSingleLine);
		bool isTextWrap() const;
		void setTextWrap(bool aWrapText);
		Align getAlign() const;
		void setAlign(Align aAlign);
		float getLineSpacingFactor() const;
		void setLineSpacingFactor(float aLineSpacingFactor);

		// Text Functions
		std::vector<std::string> getTextByLines() const;

		/**
			* \brief Renders a default OD-GUI Label to a given NanoVG context (NVGContext) with the specified attributes.
			* \param context (NVGcontext*) The nanovg pointer for rendering.
			*/
		virtual void onFrame(NVGcontext* aContext) override;

		void actionEvents(InputMap* aInput);

	};

}// namespace Lemur

#endif // !LEMUR_UI_LABEL_H