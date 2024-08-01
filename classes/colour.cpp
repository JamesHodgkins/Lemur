#ifndef LEMUR_COLOUR_CPP
#define LEMUR_COLOUR_CPP

/**************************************************************************************
* Lemur:        System Colour Class                                                   *
*-------------------------------------------------------------------------------------*
* Filename:     Colour.cpp                                                            *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A aColour class for the OpenDraft framework.                                      *
***************************************************************************************/



#include "colour.h"

namespace Lemur
{
	// Conversions
	const float Colour::getHue(const Colour& aColour)
	{
		float r = aColour.getRed();
		float g = aColour.getGreen();
		float b = aColour.getBlue();
			
		float max_val = Math::max3(r, g, b);
		float min_val = Math::min3(r, g, b);
		float chroma = max_val - min_val;
		float hue = 0.0;

		if (chroma != 0.0)
		{
			if (max_val == r)
			{
				hue = fmod((60 * ((g - b) / chroma) + 360), 360);
			}
			else if (max_val == g)
			{
				hue = fmod((60 * ((b - r) / chroma) + 120), 360);
			}
			else // max_val == b
			{
				hue = fmod((60 * ((r - g) / chroma) + 240), 360);
			}
		}

		return hue;

	}
		
	const float Colour::getSaturation(const Colour& aColour)
	{
		float r = aColour.getRed();
		float g = aColour.getGreen();
		float b = aColour.getBlue();

		float max_val = Math::max3(r, g, b);
		float min_val = Math::min3(r, g, b);
		float chroma = max_val - min_val;
		float lightness = (max_val + min_val) / 2.0;
		float saturation = 0.0;

		if (chroma != 0.0)
		{
			saturation = chroma / (1 - Math::abs(2 * lightness - 1));
		}

		return saturation;
	}
		
	const float Colour::getLightness(const Colour& aColour)
	{
		float r = aColour.getRed();
		float g = aColour.getGreen();
		float b = aColour.getBlue();

		float max_val = std::max(r, std::max(g, b));
		float min_val = std::min(r, std::min(g, b));
		float lightness = (max_val + min_val) / 2.0;

		return lightness;
	}
		
	const Colour Colour::asHex(const Colour& aColour)
	{
		return Colour();
	}
		
	const Colour Colour::fromHSL(float aH, float aS, float aL)
	{
		double c = (1.0 - std::abs(2.0 * aH - 1.0)) * aS;
		double h = aH / 60.0;
		double x = c * (1.0 - std::abs(fmod(h, 2.0) - 1.0));
		double m = aL - c / 2.0;

		double r, g, b;
		if (h >= 0 && h < 1) {
			r = c;
			g = x;
			b = 0;
		}
		else if (h >= 1 && h < 2) {
			r = x;
			g = c;
			b = 0;
		}
		else if (h >= 2 && h < 3) {
			r = 0;
			g = c;
			b = x;
		}
		else if (h >= 3 && h < 4) {
			r = 0;
			g = x;
			b = c;
		}
		else if (h >= 4 && h < 5) {
			r = x;
			g = 0;
			b = c;
		}
		else {
			r = c;
			g = 0;
			b = x;
		}

		Colour result((r + m) * 255, (g + m) * 255, (b + m) * 255, 255);
		return result;
	}
		
	const Colour Colour::fromHex(int hex)
	{
		return Colour();
	}

	// Colour utilities
	const Colour Colour::lighten(const Colour& aColour, float aAmount)
	{
		// Lighten the aColour by the specified aAmount
		float newR = Math::clamp(aColour.r + (aColour.r * aAmount), 0, 255);
		float newG = Math::clamp(aColour.g + (aColour.g * aAmount), 0, 255);
		float newB = Math::clamp(aColour.b + (aColour.b * aAmount), 0, 255);
		return Colour(newR, newG, newB, aColour.a);
	}

	const Colour Colour::darken(const Colour& aColour, float aAmount)
	{
		// Darken the aColour by the specified aAmount
		float newR = Math::clamp(aColour.r *(1 -aAmount), 0, 255);
		float newG = Math::clamp(aColour.g *(1 -aAmount), 0, 255);
		float newB = Math::clamp(aColour.b *(1 -aAmount), 0, 255);
		return Colour(newR, newG, newB, aColour.a);
	}

	const Colour Colour::saturate(const Colour& aColour, float aAmount)
	{
		// Check aAmount is between 0 and 1
		Math::clamp(aAmount, 0, 1);

		// Saturate the aColour by the specified aAmount
		float H = getHue(aColour);
		float S = getSaturation(aColour);
		float L = getLightness(aColour);

		float newS = Math::clamp(S + aAmount, 0, 1);
		return fromHSL(H, newS, L);
	}

	const Colour Colour::mix(const Colour& aColour1, const Colour& aColour2, float aAmount)
	{
		return Colour();
	}

	const Colour Colour::invert(const Colour& aColour)
	{
		return Colour();
	}

	const Colour Colour::lerp(const Colour& aColour1, const Colour& aColour2, float aAmount)
	{
		return Colour();
	}


	// Theme colours
	const Colour Colour::BACKGROUND1 = Colour(49, 54, 58, 255);
	const Colour Colour::BACKGROUND2 = Colour(41, 51, 61, 255);
	const Colour Colour::PRIMARY = Colour(113, 195, 61, 255);
	const Colour Colour::CLEAR = Colour(0, 0, 0, 0);
	const Colour Colour::WHITE = Colour(218, 224, 230, 255);

	// Standard colours
	const Colour Colour::BLACK = Colour(0, 0, 0, 255);
	const Colour Colour::BLUE = Colour(0, 0, 255, 255);
	const Colour Colour::BROWN = Colour(165, 42, 42, 255);
	const Colour Colour::CYAN = Colour(0, 255, 255, 255);
	const Colour Colour::DARK_GREY = Colour(169, 169, 169, 255);
	const Colour Colour::GREEN = Colour(90, 190, 70, 255);
	const Colour Colour::GREY = Colour(128, 128, 128, 255);
	const Colour Colour::LIGHT_GREY = Colour(211, 211, 211, 255);
	const Colour Colour::LIME = Colour(0, 255, 0, 255);
	const Colour Colour::MAGENTA = Colour(255, 0, 255, 255);
	const Colour Colour::MAROON = Colour(128, 0, 0, 255);
	const Colour Colour::NAVY = Colour(0, 0, 128, 255);
	const Colour Colour::OLIVE = Colour(128, 128, 0, 255);
	const Colour Colour::ORANGE = Colour(255, 165, 0, 255);
	const Colour Colour::PINK = Colour(255, 192, 203, 255);
	const Colour Colour::PURPLE = Colour(128, 0, 128, 255);
	const Colour Colour::RED = Colour(255, 0, 0, 255);
	const Colour Colour::TEAL = Colour(0, 128, 128, 255);
	const Colour Colour::YELLOW = Colour(255, 255, 0, 255);


	// Default constructor
	Colour::Colour() : r(0), g(0), b(0), a(255) {}

	// Custom constructor
	Colour::Colour(int aRed, int aGreen, int aBlue, int aAlpha) : r(aRed), g(aGreen), b(aBlue), a(aAlpha) {}

	// Getters and setters
	int Colour::getRed() const { return r; }
	float Colour::getRedNorm() const { return static_cast<float>(r) / 255.f; }
	void Colour::setRed(int aRed) { r = aRed; }
	int Colour::getGreen() const { return g; }
	float Colour::getGreenNorm() const { return static_cast<float>(g) / 255.f; }
	void Colour::setGreen(int aGreen) { g = aGreen; }
	int Colour::getBlue() const { return b; }
	float Colour::getBlueNorm() const { return static_cast<float>(b) / 255.f; }
	void Colour::setBlue(int aBlue) { b = aBlue; }
	int Colour::getAlpha() const { return a; }
	float Colour::getAlphaNorm() const { return static_cast<float>(a) / 255.f; }
	void Colour::setAlpha(int aAlpha) { a = aAlpha; }
	void Colour::setRGB(int aRed, int aGreen, int aBlue, int aAlpha)
	{
		r = aRed;
		g = aGreen;
		b = aBlue;
		a = aAlpha;
	}

	// Returns the aColour as NVGcolor
	NVGcolor Colour::asNvgColour() const
	{
		return nvgRGBA(getRed(), getGreen(), getBlue(), getAlpha());
	}


	// Operator overloads
	bool Colour::operator==(const Colour& aColour) const
	{
		return (r == aColour.r && g == aColour.g && b == aColour.b && a == aColour.a);
	}

	bool Colour::operator!=(const Colour& aColour) const
	{
		return !(*this == aColour);
	}

	Colour Colour::operator=(const Colour& aColour)
	{
		r = aColour.r;
		g = aColour.g;
		b = aColour.b;
		a = aColour.a;
		return *this;
	}

} // namespace Lemur::Colour

#endif // !LEMUR_COLOUR_CPP