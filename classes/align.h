#ifndef LEMUR_ALIGN
#define LEMUR_ALIGN

/**************************************************************************************
* Lemur:        Graphic Draw Class                                                    *
* Lemur:        Graphic Draw Class                                                    *
*-------------------------------------------------------------------------------------*
* Filename:     Align.h                                                               *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A class for storing alignment states.                                             *
***************************************************************************************/

namespace Lemur
{
	class Align {
	public:
		static const int LEFT = 1 << 0;
		static const int RIGHT = 1 << 1;
		static const int TOP = 1 << 2;
		static const int BOTTOM = 1 << 3;
		static const int CENTRE = 1 << 4;
		static const int MIDDLE = 1 << 5;

		Align() : align_(LEFT | MIDDLE) {}
		explicit Align(int align) : align_(align) {}

		void setAlign(int align) {
			align_ = align;
		}

		int getAlign() const {
			return align_;
		}

		Align operator|(const Align& other) const {
			return Align(align_ | other.align_);
		}

		Align operator|(const int rhs) {
			return Align(getAlign() | rhs);
		}

	private:
		int align_;
	};
}

#endif // !LEMUR_ALIGN