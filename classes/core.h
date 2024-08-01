#ifndef LEMUR_CORE_H
#define LEMUR_CORE_H

/**************************************************************************************
* Lemur:        Core System Class                                                     *
*-------------------------------------------------------------------------------------*
* Filename:     Core.h                                                                *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Used to load all primitive system classes. System classes should be free from     *
*	dependencies, intended to be included as a base for all future classes needing    *
*	primitives.                                                                       *
***************************************************************************************/


#define GLEW_STATIC					// Use static glew library (glew32s.lib)
#include "GL/glew.h"				// Include glew
#include "glfw/glfw3.h"				// Include glfw3
#include "nanovg.h"					// Include core nanovg library

#include "align.h"
#include "math.h"
#include "colour.h"
#include "font.h"
#include "image.h"
#include "vector2.h"
#include "resource_manager.h"

#endif // !LEMUR_CORE_H