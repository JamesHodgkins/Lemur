#ifndef LEMUR_VECTOR2_H
#define LEMUR_VECTOR2_H

/**************************************************************************************
* Lemur:        System Vector2 Class                                                  *
*-------------------------------------------------------------------------------------*
* Filename:     Vector2.h                                                             *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A 2D vector class for the OpenDraft framework for geometry elements               *
***************************************************************************************/


#include <string>


namespace Lemur
{
	// Vector2 class
	class Vector2 {
	private:

	public:
		double x, y;

		// Constructors
		Vector2();
		Vector2(float aX, float aY);
		Vector2(double aX, double aY);
		Vector2(int aX, int aY);

		//
		// General Methods
		//

		// ToString
		std::string ToString();


		//
		// Vector Math
		//

		// Magnitude
		double magnitude();

		// Normalized
		Vector2 normalized();

		// Dot product
		double dot(Vector2 aVector);

		// Cross product
		double cross(Vector2 aVector);

		// Angle between vectors
		double angle(Vector2 aVector);

		// Lerp
		Vector2 lerp(Vector2 aVector, double aT);

		// Rotate
		Vector2 rotate(double aAngle);

		// RotateAround
		Vector2 rotateAround(Vector2 aPoint, double aAngle);

		// Scale
		Vector2 scale(Vector2 aScale);

		// Distance to
		double distanceTo(const Vector2& other) const;

		//
		// Operators
		//

		// operator+
		Vector2 operator+(Vector2 aVector);

		// operator-
		Vector2 operator-(Vector2 aVector);

		// operator*
		Vector2 operator*(double aScalar);

		// operator/
		Vector2 operator/(double aScalar);

		// operator+=
		void operator+=(Vector2 aVector);

		// operator-=
		void operator-=(Vector2 aVector);

		// operator*=
		void operator*=(double aScalar);

		// operator/=
		void operator/=(double aScalar);

		// operator==
		bool operator==(Vector2 aVector);

		// operator!=
		bool operator!=(Vector2 aVector);
		
	};

} // namespace Lemur

#endif // !LEMUR_VECTOR2_H
