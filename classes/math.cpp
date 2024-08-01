#ifndef LEMUR_MATH_CPP
#define LEMUR_MATH_CPP

/**************************************************************************************
* Lemur:        System Math Class                                                     *
*-------------------------------------------------------------------------------------*
* Filename:     Math.cpp                                                              *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A math class for the OpenDraft framework.                                         *
***************************************************************************************/



#include "math.h"


namespace Lemur
{
	// Trigonometry
	float Math::sin(float aAngle)
	{
		return sinf(aAngle);
	}

	float Math::cos(float aAngle)
	{
		return cosf(aAngle);
	}

	float Math::tan(float aAngle)
	{
		return tanf(aAngle);
	}

	float Math::asin(float aAngle)
	{
		return asinf(aAngle);
	}

	float Math::acos(float aAngle)
	{
		return acosf(aAngle);
	}
	float Math::atan(float aAngle)
	{
		return atanf(aAngle);
	}
	float Math::atan2(float aY, float aX)
	{
		return atan2f(aY, aX);
	}

	// Rounding
	float Math::ceil(float aValue)
	{
		return ceilf(aValue);
	}

	float Math::floor(float aValue)
	{
		return floorf(aValue);
	}

	float Math::round(float aValue)
	{
		return roundf(aValue);
	}

	float Math::round(float aValue, int aPrecision)
	{
		return roundf(aValue * pow(10, aPrecision)) / pow(10, aPrecision);
	}

	float Math::round(float aValue, int aPrecision, bool aTruncate)
	{
		if (aTruncate)
		{
			return floorf(aValue * pow(10, aPrecision)) / pow(10, aPrecision);
		}
		else
		{
			return roundf(aValue * pow(10, aPrecision)) / pow(10, aPrecision);
		}
	}


	// Min/max and clamp
	float Math::min2(float aValue1, float aValue2)
	{
		// Calculate the minimum value
		return (aValue1 < aValue2)? aValue1 : aValue2;
	}
	
	float Math::max2(float aValue1, float aValue2)
	{
		// Calculate the maximum value
		return (aValue1 > aValue2)? aValue1 : aValue2;
	}
	
	float Math::min3(float aValue1, float aValue2, float aValue3)
	{
		return (aValue1 < aValue2)? ((aValue1 < aValue3)? aValue1 : aValue3) : ((aValue2 < aValue3)? aValue2 : aValue3);
	}
	
	float Math::max3(float aValue1, float aValue2, float aValue3)
	{
		return (aValue1 > aValue2)? ((aValue1 > aValue3)? aValue1 : aValue3) : ((aValue2 > aValue3)? aValue2 : aValue3);
	}

	float Math::clamp(float aValue, float aMin, float aMax)
	{
		if (aValue < aMin)
			return aMin;
		else if (aValue > aMax)
			return aMax;
		else
			return aValue;
	}




	// Exponents
	float Math::pow(float aBase, float aExponent)
	{
		return powf(aBase, aExponent);
	}

	float Math::sqrt(float aValue)
	{
		return sqrtf(aValue);
	}

	float Math::cbrt(float aValue)
	{
		return cbrtf(aValue);
	}

	float root(float aValue, float aRoot)
	{
		return powf(aValue, 1 / aRoot);
	}

	// Logarithms
	float Math::log(float aValue)
	{
		return logf(aValue);
	}

	float Math::log(float aValue, float aBase)
	{
		return logf(aValue) / logf(aBase);
	}

	float Math::log2(float aValue)
	{
		return logf(aValue) / logf(2);
	}

	float Math::log10(float aValue)
	{
		return log10f(aValue);
	}

	// Absolute value
	float Math::abs(float aValue)
	{
		return fabsf(aValue);
	}


}


#endif // !LEMUR_MATH_CPP