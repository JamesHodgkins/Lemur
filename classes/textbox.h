#ifndef LEMUR_UI_TEXTBOX_H
#define LEMUR_UI_TEXTBOX_H

/**************************************************************************************
* OpenDraft:    GUI Textbox Class                                                     *
*-------------------------------------------------------------------------------------*
* Filename:     textbox.h                                                             *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI textbox input class derived from component                                  *
***************************************************************************************/



#include <unordered_map>
#include "core.h"
#include "component.h"
#include "draw.h"


namespace Lemur
{
	class Textbox : public Component
	{
	protected:
		// Constants
		const int CURSOR_BLINK_FRAME_COUNT = 25;

		// Properties
		bool isActive;
		int cursorIndex;
		int cursorBlinkTimer;

		Draw::TextStyle textStyle;


		// Protected Methods
		int calculateCursorPosition(NVGcontext* aContext, int aIndex, const Draw::TextStyle* aStyle);


	public:
		Textbox();
		Textbox(int aX, int aY, int aWidth, int aHeight, std::string aText);
		Textbox(Vector2 aLocation, std::string aText);


		// Getters and Setters
		bool getActive();
		void setActive(bool aActive);
		float getFontSize();
		void setFontSize(float aSize);
		void setTextStyle(Draw::TextStyle* aStyle);
		const char* getFont();
		void setFont(const char* aFont);
		Align getAlign();
		void setAlign(Align aAlign);
		Colour getColour();
		void setColour(Colour aColour);




		// Virtual method overrides
		virtual void onFrame(NVGcontext* aContext) override;
		virtual void actionEvents(InputMap* aInput) override;

	};

}// namespace Lemur

#endif // !LEMUR_UI_TEXTBOX_H