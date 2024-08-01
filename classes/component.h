#ifndef LEMUR_COMPONENT_H
#define LEMUR_COMPONENT_H

/**************************************************************************************
* Lemur:        GUI Component Base Class                                              *
*-------------------------------------------------------------------------------------*
* Filename:     Component.h                                                           *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Base class for GUI Components.                                                    *
***************************************************************************************/



#include <string>
#include <vector>
#include "draw.h"
#include "core.h"
#include "input.h"
#include <nanovg.h>

namespace Lemur
{

	struct Anchor
	{
		// Create enum for anchor direction
		enum class Direction
		{
			Top = 0,
			Right = 1,
			Bottom = 2,
			Left = 3
		};

		bool enabled = false;
		int offset = 0;
	};


	class Component
	{
	protected:
		// Mouse state
		bool mouseOver = false;
		bool mouseEnter = false;
		bool mouseLeave = false;
		bool mousePressDown = false;
		bool mousePressUp = false;

		// Anchor array
		Anchor anchor[4] = { Anchor(), Anchor(), Anchor(), Anchor() };

		// Parent-child relationship
		Component* parent;											// Parent container control for the control.
		std::vector<std::shared_ptr<Component>> childComponents;	// Child Components

		// Component properties
		Vector2 location = { 0,0 };			// Location of Component
		Vector2 size = { 50,50 };			// Size of Component

		// Event Handling
		bool active;						// Control is active or inactive.

		// Rendering properties
		float drawBounds[4] = { 0,0,0,0 };	// Bounds of the control for rendering.

	public:

		// Component properties
		std::string name;					// Name of the control.

		// Appearance properties
		float strokeWidth;					// Width of stroke
		void* font;							// Font of the text displayed by the control.
		Colour backColour;					// Background colour of the control.
		Colour stroke;						// Stroke colour
		Colour foreColour;					// Foreground colour of the control.

		// Behavior properties
		bool enabled = true;				// Control is enabled or disabled.
		bool show;							// Visible or hidden.
		int tabIndex;						// Tab order of the control within its container.
		int zOrder = 0;						// Z-order of the control within its container.

		// Text properties
		std::string text;					// Text associated with the control.

		// Resource properties
		ResourceManager* resourceManager;	// Pointer to injected resource manager.

		// Destructor
		virtual ~Component() = default;		// Default destructor

		// Virtual Functions
		virtual void onFrame(NVGcontext* aContext) = 0;
		void invokeOnFrame(NVGcontext* aContext);

		// Setters
		void setLocation(int aX, int aY);
		void setLocation(double aX, double aY);
		void setLocation(Vector2 aPoint);
		virtual void setText(std::string aText);
		void setSize(int aWidth, int aHeight);
		void setSize(double aWidth, double aHeight);
		void setSize(Vector2 aSize);
		void setWidth(int aWidth);
		void setHeight(int aHeight);
		void setName(std::string aName);
		Vector2 getOffset() const;

		// Getters
		std::string getText();
		Vector2 getLocation() const;
		int getLocationX() const;
		int getLocationY() const;
		Vector2 getSize() const;
		int getWidth() const;
		int getHeight() const;
		bool isActive() const;
		Component* getParent() const;

		// Mouse Events
		bool isMouseOver() const;
		bool isMouseEnter() const;
		bool isMouseLeave() const;
		bool isMouseDown() const;
		bool wasMousePressed() const;

		// Anchor Events
		bool isAnchorSet(Anchor::Direction direction) const;
		void setAnchor(Anchor::Direction direction, bool aAnchor);
		void updateAnchor(Anchor::Direction direction);
		void updateSizeForAnchors();

		// Child Component Management
		virtual void addChildControl(Component* aChild);
		Component* getChildByName(std::string name);

		// Event Handling
		virtual void processEvents(InputMap* aInput) final;
		virtual void actionEvents(InputMap* aInput) = 0;

		// Drawing
		void drawChildComponents(NVGcontext* aContext);
	};

} // namespace Lemur

#endif // !LEMUR_COMPONENT_H
