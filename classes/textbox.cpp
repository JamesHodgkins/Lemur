#ifndef LEMUR_UI_TEXTBOX_CPP
#define LEMUR_UI_TEXTBOX_CPP

/**************************************************************************************
* OpenDraft:    GUI Textbox Class                                                     *
*-------------------------------------------------------------------------------------*
* Filename:     textbox.cpp                                                           *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI textbox input class derived from component                                  *
***************************************************************************************/



#include "textbox.h"


namespace Lemur
{
	
	Textbox::Textbox()
	{
		text = "";
		cursorIndex = 0;
		cursorBlinkTimer = 0;
		foreColour = Colour::BLACK;
		backColour = Colour::WHITE;

		// Set default text style
		textStyle.font = "sans";
		textStyle.size = 12.f;
		textStyle.align = Align(Align::LEFT | Align::MIDDLE);
		textStyle.colour = Colour::BLACK;
	}

	Textbox::Textbox(int aX, int aY, int aWidth, int aHeight, std::string aText)
	{
		location.x = aX;
		location.y = aY;
		size.x = aWidth;
		size.y = aHeight;
		foreColour = Colour::BLACK;
		backColour = Colour::WHITE;
		text = aText;
		cursorIndex = 0;
		cursorBlinkTimer = 0;

		// Set default text style
		textStyle.font = "sans";
		textStyle.size = 12.f;
		textStyle.align = Align(Align::LEFT | Align::MIDDLE);
		textStyle.colour = Colour::BLACK;
	}

	Textbox::Textbox(Vector2 aLocation, std::string aText)
	{
		location.x = aLocation.x;
		location.y = aLocation.y;
		foreColour = Colour::BLACK;
		backColour = Colour::WHITE;
		text = aText;
		cursorIndex = 0;
		cursorBlinkTimer = 0;

		// Set default text style
		textStyle.font = "sans";
		textStyle.size = 12.f;
		textStyle.align = Align(Align::LEFT | Align::MIDDLE);
		textStyle.colour = Colour::BLACK;
	}


	//
	// Getters and Setters
	//
	
	// Active
	bool Textbox::getActive()
	{
		return isActive;
	}
	void Textbox::setActive(bool aActive)
	{
		isActive = aActive;
	}

	// Font Size
	float Textbox::getFontSize()
	{
		return textStyle.size;
	}
	void Textbox::setFontSize(float aSize)
	{
		textStyle.size = aSize;
	}
	
	// Text Style
	void Textbox::setTextStyle(Draw::TextStyle* aStyle)
	{
		textStyle.font = aStyle->font;
		textStyle.size = aStyle->size;
		textStyle.align = aStyle->align;
		textStyle.colour = aStyle->colour;
	}

	// Font
	const char* Textbox::getFont()
	{
		return textStyle.font;
	}
	void Textbox::setFont(const char* aFont)
	{
		textStyle.font = aFont;
	}
	
	// Text Horizontal Alignment
	Align Textbox::getAlign()
	{
		return textStyle.align;
	}
	void Textbox::setAlign(Align aAlign)
	{
		textStyle.align = aAlign;
	}
	
	// Text Colour
	Colour Textbox::getColour()
	{
		return textStyle.colour;
	}
	void Textbox::setColour(Colour aColour)
	{
		textStyle.colour = aColour;
	}



	int Textbox::calculateCursorPosition(NVGcontext* aContext, int aIndex, const Draw::TextStyle* aStyle)
	{
		// Use nanovg to calculate text width
		float bounds[4];

		// Static cast properties
		float textboxX = getWidth();
		float textboxY = getHeight();

		// Get characters of text property up to aIndex
		std::string subText = text.substr(0, aIndex);

		nvgSave(aContext);
		nvgReset(aContext);
		nvgFontSize(aContext, aStyle->size);
		nvgFontFace(aContext, aStyle->font);
		nvgFontBlur(aContext, 0);
		
		if (aStyle->align.getAlign() & Align::CENTRE)
			nvgTextAlign(aContext, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		else if (aStyle->align.getAlign() & Align::RIGHT)
			nvgTextAlign(aContext, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
		else
			nvgTextAlign(aContext, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

		nvgTextBounds(aContext, textboxX, textboxY, subText.c_str(), nullptr, bounds);

		int cursorX = bounds[2]-bounds[0];

		// If text ends with a space, add a space to the end of the cursor
		if (text.length() > 0)
			if (text[aIndex - 1] == ' ')
			{
				int charWidth = static_cast<int>(aStyle->size/3);
				cursorX += charWidth;
			}

		nvgRestore(aContext);

		return cursorX;

	}


	void Textbox::onFrame(NVGcontext* aContext)
	{
		// If context is null, return
		if (aContext == nullptr)
			return;

		// If cursor timer is 0, reset it. Multiply by 2, half time is on and half off
		if (cursorBlinkTimer < 1)
			cursorBlinkTimer = CURSOR_BLINK_FRAME_COUNT*2;

		// Static cast parameters to int
		int w = static_cast<int>(size.x);
		int h = static_cast<int>(size.y);
		int x = static_cast<int>(location.x);
		int y = static_cast<int>(location.y);

		Draw::Rect(aContext, x, y, w, h, backColour);
		Draw::Text(aContext, x - (w/2), y +2, w, h, &textStyle, text.c_str());

		// if cursorBlinkTimer is less than half the time, draw the cursor
		if (cursorBlinkTimer < CURSOR_BLINK_FRAME_COUNT)
		{
			int cursorX = calculateCursorPosition(aContext, cursorIndex, &textStyle) + x;
			Draw::Line(aContext, cursorX, y, cursorX, y + h, 2, Colour::RED);
		}

		// Decrement cursor timer
		cursorBlinkTimer--;
	}


	

	void Textbox::actionEvents(InputMap* aInput)
	{
		bool wasShifted = aInput->keys[GLFW_KEY_LEFT_SHIFT].isDown() || aInput->keys[GLFW_KEY_RIGHT_SHIFT].isDown();
		bool caps = (wasShifted || aInput->capsLock);
		

		// Helper function to insert a character at the cursor index
		auto insertCharacter = [&](char character) {
			text.insert(cursorIndex, 1, character);
			cursorIndex++;
		};

		// Numeric keys
		for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
			if (aInput->keys[key].isPressDown()) {
				char character = static_cast<char>(key);
				insertCharacter(character);
			}
		}

		// Alphabets (supports lowercase and uppercase)
		for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++) {
			if (aInput->keys[key].isPressDown()) {
				char character = wasShifted? static_cast<char>(key) : std::tolower(static_cast<char>(key));
				insertCharacter(character);
			}
		}

		// Special characters with Shift key support
		if (aInput->keys[GLFW_KEY_MINUS].isPressDown()) {
			insertCharacter(wasShifted? '_' : '-');
		}

		if (aInput->keys[GLFW_KEY_EQUAL].isPressDown()) {
			insertCharacter(wasShifted? '+' : '=');
		}

		if (aInput->keys[GLFW_KEY_LEFT_BRACKET].isPressDown()) {
			insertCharacter(wasShifted? '{' : '[');
		}

		if (aInput->keys[GLFW_KEY_RIGHT_BRACKET].isPressDown()) {
			insertCharacter(wasShifted? '}' : ']');
		}

		if (aInput->keys[GLFW_KEY_BACKSLASH].isPressDown()) {
			insertCharacter(wasShifted? '~' : '#');
		}

		if (aInput->keys[GLFW_KEY_SEMICOLON].isPressDown()) {
			insertCharacter(wasShifted? ':' : ';');
		}

		if (aInput->keys[GLFW_KEY_APOSTROPHE].isPressDown()) {
			insertCharacter(wasShifted? '@' : '\'');
		}

		if (aInput->keys[GLFW_KEY_GRAVE_ACCENT].isPressDown()) {
			insertCharacter(wasShifted? '¬' : '`');
		}

		if (aInput->keys[GLFW_KEY_COMMA].isPressDown()) {
			insertCharacter(wasShifted? '<' : ',');
		}

		if (aInput->keys[GLFW_KEY_PERIOD].isPressDown()) {
			insertCharacter(wasShifted? '>' : '.');
		}

		if (aInput->keys[GLFW_KEY_SLASH].isPressDown()) {
			insertCharacter(wasShifted? '?' : '/');
		}


		// Common special characters
		if (aInput->keys[GLFW_KEY_SPACE].isPressDown()) {
			insertCharacter(' ');
		}

		if (aInput->keys[GLFW_KEY_BACKSPACE].isPressDown()) {
			if (cursorIndex > 0) {
				text.erase(cursorIndex - 1, 1);
				cursorIndex--;
			}
		}

		if (aInput->keys[GLFW_KEY_DELETE].isPressDown()) {
			if (cursorIndex < text.length()) {
				text.erase(cursorIndex, 1);
			}
		}

		if (aInput->keys[GLFW_KEY_LEFT].isPressDown()) {
			if (cursorIndex > 0) {
				cursorIndex--;
			}
		}

		if (aInput->keys[GLFW_KEY_RIGHT].isPressDown()) {
			if (cursorIndex < text.length()) {
				cursorIndex++;
			}
		}
	}





}

#endif // !LEMUR_UI_TEXTBOX_CPP