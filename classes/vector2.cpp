#ifndef LEMUR_VECTOR2_CPP
#define LEMUR_VECTOR2_CPP

/**************************************************************************************
* Lemur:        System Vector2 Class                                                  *
*-------------------------------------------------------------------------------------*
* Filename:     Vector2.cpp                                                           *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A 2D vector class for the OpenDraft framework for geometry elements               *
***************************************************************************************/


#include "vector2.h"
#include "math.h"

namespace Lemur
{

	// Constructors
	Vector2::Vector2() : x(0), y(0) {}
	Vector2::Vector2(float aX, float aY) : x(aX), y(aY) {}
	Vector2::Vector2(double aX, double aY) : x(aX), y(aY) {}
	Vector2::Vector2(int aX, int aY) : x(aX), y(aY) {}


	//
	// General Methods
	//

	// ToString
	std::string Vector2::ToString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}



	//
	// Vector Math
	//

	// Magnitude
	double Vector2::magnitude()
	{
		return sqrt(x * x + y * y);
	}

	// Normalized
	Vector2 Vector2::normalized()
	{
		return Vector2(x / magnitude(), y / magnitude());
	}

	// Dot product
	double Vector2::dot(Vector2 aVector)
	{
		return x * aVector.x + y * aVector.y;
	}

	// Cross product
	double Vector2::cross(Vector2 aVector)
	{
		return x * aVector.y - y * aVector.x;
	}

	// Angle between vectors
	double Vector2::angle(Vector2 aVector)
	{
		return acos(dot(aVector) / (magnitude() * aVector.magnitude()));
	}

	// Lerp
	Vector2 Vector2::lerp(Vector2 aVector, double aT)
	{
		return Vector2(x + (aVector.x - x) * aT, y + (aVector.y - y) * aT);
	}

	// Rotate
	Vector2 Vector2::rotate(double aAngle)
	{
		return Vector2(x * cos(aAngle) - y * sin(aAngle), x * sin(aAngle) + y * cos(aAngle));
	}

	// RotateAround
	Vector2 Vector2::rotateAround(Vector2 aPoint, double aAngle)
	{
		Vector2 offset = *this - aPoint;
		double cosAngle = cos(aAngle);
		double sinAngle = sin(aAngle);
		double newX = offset.x * cosAngle - offset.y * sinAngle;
		double newY = offset.x * sinAngle + offset.y * cosAngle;
		Vector2 rotatedVector = aPoint + Vector2(newX, newY);
		return rotatedVector;
	}

	// Scale
	Vector2 Vector2::scale(Vector2 aScale)
	{
		return Vector2(x * aScale.x, y * aScale.y);
	}

	// Distance to
	double Vector2::distanceTo(const Vector2& other) const {
		double dx = other.x - x;
		double dy = other.y - y;
		return Math::sqrt(dx * dx + dy * dy);
	}



	//
	// Operators
	//

	// operator+
	Vector2 Vector2::operator+(Vector2 aVector)
	{
		return Vector2(x + aVector.x, y + aVector.y);
	}

	// operator-
	Vector2 Vector2::operator-(Vector2 aVector)
	{
		return Vector2(x - aVector.x, y - aVector.y);
	}

	// operator*
	Vector2 Vector2::operator*(double aScalar)
	{
		return Vector2(x * aScalar, y * aScalar);
	}

	// operator/
	Vector2 Vector2::operator/(double aScalar)
	{
		return Vector2(x / aScalar, y / aScalar);
	}

	// operator+=
	void Vector2::operator+=(Vector2 aVector)
	{
		x += aVector.x;
		y += aVector.y;
	}

	// operator-=
	void Vector2::operator-=(Vector2 aVector)
	{
		x -= aVector.x;
		y -= aVector.y;
	}

	// operator*=
	void Vector2::operator*=(double aScalar)
	{
		x *= aScalar;
		y *= aScalar;
	}

	// operator/=
	void Vector2::operator/=(double aScalar)
	{
		x /= aScalar;
		y /= aScalar;
	}

	// operator==
	bool Vector2::operator==(Vector2 aVector)
	{
		return (x == aVector.x && y == aVector.y);
	}

	// operator!=
	bool Vector2::operator!=(Vector2 aVector)
	{
		return !(x == aVector.x && y == aVector.y);
	}


} // namespace Lemur

#endif // !LEMUR_VECTOR2_cpp