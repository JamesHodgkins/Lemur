#ifndef LEMUR_COMPONENT_CPP
#define LEMUR_COMPONENT_CPP

/**************************************************************************************
* Lemur:        GUI Component Base Class                                              *
*-------------------------------------------------------------------------------------*
* Filename:     Component.cpp                                                         *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Base class for GUI Components.                                                    *
***************************************************************************************/



#include "Component.h"


namespace Lemur
{
	//
	// Invoke onFrame with a header
	//
	void Component::invokeOnFrame(NVGcontext* aContext)
	{
		// Check if Component is enabled, and return if not
		if (!enabled)
			return;

		// Check if NVG context is null, and return if so
		if (aContext == nullptr)
			return;

		// Update size based on any anchors that are enabled
		updateSizeForAnchors();

		// Invoke onFrame
		onFrame(aContext);
	}


	//
	//	Getters and Setters
	//

	void Component::setLocation(int aX, int aY)
	{
		location.x = static_cast<double>(aX);
		location.y = static_cast<double>(aY);
	}

	void Component::setLocation(double aX, double aY)
	{
		location.x = aX;
		location.y = aY;
	}

	void Component::setLocation(Vector2 aPoint)
	{
		location.x = aPoint.x;
		location.y = aPoint.y;
	}

	void Component::setText(std::string aText)
	{
		text = aText;
	}

	void Component::setSize(int aWidth, int aHeight)
	{
		size.x = static_cast<float>(aWidth);
		size.y = static_cast<float>(aHeight);
	}

	void Component::setSize(double aWidth, double aHeight)
	{
		size.x = static_cast<float>(aWidth);
		size.y = static_cast<float>(aHeight);
	}
		
	void Component::setSize(Vector2 aSize)
	{
		size.x = aSize.x;
		size.y = aSize.y;
	}

	void Component::setWidth(int aWidth)
	{
		size.x = static_cast<float>(aWidth);
	}

	void Component::setHeight(int aHeight)
	{
		size.y = static_cast<float>(aHeight);
	}

	void Component::setName(std::string aName)
	{
		name = aName;
	}

	Vector2 Component::Component::getLocation() const
	{
		return location;
	}

	int Component::getLocationX() const
	{
		return static_cast<int>(location.x);
	}

	int Component::getLocationY() const
	{
		return static_cast<int>(location.y);
	}

	std::string Component::getText()
	{
		return text;
	}

	Vector2 Component::getSize() const
	{
		return size;
	}

	int Component::getWidth() const
	{
		return static_cast<int>(size.x);
	}

	int Component::getHeight() const
	{
		return static_cast<int>(size.y);
	}

	bool Component::isActive() const
	{
		return active;
	}

	Component* Component::getParent() const
	{
		return parent;
	}

	Vector2 Component::getOffset() const
	{
		Vector2 result;

		if (parent != nullptr)
		{
			Vector2 parentOffset = parent->getOffset();
			result.x = parentOffset.x + location.x;
			result.y = parentOffset.y + location.y;
		}

		return result;
	}



	//
	// Mouse Events
	//

	bool Component::isMouseOver() const
	{
		if (this == nullptr)
			return false;

		return mouseOver;
	}

	bool Component::isMouseEnter() const
	{
		if (this == nullptr)
			return false;

		return mouseEnter;
	}

	bool Component::isMouseLeave() const
	{
		if (this == nullptr)
			return false;

		return mouseLeave;
	}

	bool Component::isMouseDown() const
	{
		if (this == nullptr)
			return false;

		return mousePressDown;
	}

	bool Component::wasMousePressed() const
	{
		if (this == nullptr)
			return false;

		return mousePressUp;
	}

	//
	// Anchor Management
	//
	bool Component::isAnchorSet(Anchor::Direction direction) const
	{
		int index = static_cast<int>(direction);
		return anchor[index].enabled;
	}

	void Component::setAnchor(Anchor::Direction direction, bool aAnchor)
	{
		int index = static_cast<int>(direction);
		anchor[index].enabled = aAnchor;

		// If turning AnchorBottom on, get the current space between the bottom of the Component and the bottom of the parent
		if (aAnchor)
			updateAnchor(direction);
		
	}

	void Component::updateAnchor(Anchor::Direction direction)
	{
		if (parent == nullptr)
			return;

		int index = static_cast<int>(direction);

		// Update anchor offsets
		// Top:
		if (index == 0)
			anchor[0].offset = location.y;

		// Right:
		else if (index == 1)
			anchor[1].offset = parent->getWidth() - (location.x + size.x);
		
		// Bottom:
		else if (index == 2)
			anchor[2].offset = parent->getHeight() - (location.y + size.y);
		
		// Left:
		else if (index == 3)
			anchor[3].offset = location.x;
	}

	void Component::updateSizeForAnchors()
	{
		// Update horizontal size based on anchor points
		// Anchor top=0, right=1, bottom=2, left=3

		// If left and right anchors are enabled
		if (anchor[3].enabled && anchor[1].enabled)
		{
			// Calculate new width
			size.x = parent->getSize().x - anchor[3].offset - anchor[1].offset - location.x;
		}

		// If right anchor is enabled
		else if (!anchor[3].enabled && anchor[1].enabled)
		{
			// Calculate new location, pinning the right side
			location.x = parent->getSize().x - anchor[1].offset - size.x;
		}

		// If top and bottom anchors are enabled
		if (anchor[0].enabled && anchor[2].enabled)
		{
			// Calculate new height
			size.y = parent->getSize().y - anchor[0].offset - anchor[2].offset - location.y;
		}

		// If bottom anchor is enabled
		else if (!anchor[0].enabled && anchor[2].enabled)
		{
			// Calculate new location, pinning the bottom side
			location.y = parent->getSize().y - anchor[2].offset - size.y;
		}
	}

	//
	// Child Component Management
	//

	void Component::addChildControl(Component* aChild)
	{
		// Check if the Component already has a parent
		if (aChild->parent != nullptr)
		{
			// Remove child from the parent
			for (int i = 0; i < aChild->parent->childComponents.size(); i++)
			{
				if (aChild->parent->childComponents[i].get() == aChild)
				{
					aChild->parent->childComponents.erase(aChild->parent->childComponents.begin() + i);
					break;
				}
			}
		}

		// Set the parent of the child to this Component
		aChild->parent = this;
		childComponents.push_back(std::shared_ptr<Component>(aChild));
	}

	Component* Component::getChildByName(std::string name)
	{
		// Search through childComponents vector and get by name
		for (std::shared_ptr<Component> control : childComponents)
		{
			if (control->name == name)
				return control.get();

			if (control->childComponents.size() > 0)
			{
				Component* result = control->getChildByName(name);
				if (result != nullptr)
					return result;
			}
		}

		return nullptr;
	}


	//
	// Event Handling
	//

	void Component::processEvents(InputMap* aInput)
	{
		// Get mouse position from input
		int mousePosX = static_cast<int>(aInput->mouse.position.x);
		int mousePosY = static_cast<int>(aInput->mouse.position.y);

		// Calculate object boundaries
		Vector2 offset = getOffset();

		// Calculate object boundaries
		int objectLeft = static_cast<int>(offset.x);
		int objectTop = static_cast<int>(offset.y);
		int objectRight = static_cast<int>(objectLeft + size.x);
		int objectBottom = static_cast<int>(objectTop + size.y);


		bool mouseOverCheck = true;

		if (parent != nullptr)
			if (parent->mouseOver == false)
				mouseOverCheck = false;

		if (mousePosX < objectLeft)
			mouseOverCheck = false;

		if (mousePosX > objectRight)
			mouseOverCheck = false;
		
		if (mousePosY < objectTop)
			mouseOverCheck = false;
			
		if (mousePosY > objectBottom)
			mouseOverCheck = false;


		if (mouseOverCheck)
		{
			// Mouse is over the object
			mouseOver = true;

			// Check if mouse entered the object
			if (!mouseEnter)
				mouseEnter = true;

		}
		else
		{
			// Mouse is not over the object
			mouseOver = false;

			// Check if mouse left the object
			if (!mouseLeave)
				mouseLeave = true;
			
		}


		if (mouseOver && aInput->mouse.leftButton.isPressDown())
		{
			mousePressDown = true;
			active = true;
		}

		// Check if mouse was pressed down
		if (!aInput->mouse.leftButton.isDown())
			mousePressDown = false;

		// Check if mouse was released
		if (mouseOver && aInput->mouse.leftButton.isPressUp())
			mousePressUp = true;
		else
			mousePressUp = false;

		if(!mouseOver && aInput->mouse.leftButton.isPressUp())
			active = false;


		// Process child Components
		for (std::shared_ptr<Component> control : childComponents)
			control->processEvents(aInput);

		actionEvents(aInput);
	}


	//
	// Drawing
	//

	// Draw child Components
	void Component::drawChildComponents(NVGcontext* aContext)
	{
		// Draw child Components in order of draw priority
		std::vector<std::shared_ptr<Component>> drawStack;
		for (std::shared_ptr<Component> control : childComponents)
			drawStack.push_back(control);


		// Sort child Components by draw 'zOrder'
		std::sort(drawStack.begin(), drawStack.end(), [](const std::shared_ptr<Component>& a, const std::shared_ptr<Component>& b) {
			return a->zOrder < b->zOrder;
		});

		// Set scissor rectangle to the size of the this component
		drawBounds[0] = location.x;
		drawBounds[1] = location.y;
		drawBounds[2] = size.x;
		drawBounds[3] = size.y;


		// Get the intersection of the parent bounds and this component bounds
		if (parent != nullptr)
		{
			if (location.x < 0)
				drawBounds[0] = 0;

			if (location.y < 0)
				drawBounds[1] = 0;

			if (location.x + size.x > parent->size.x)
				drawBounds[2] = parent->size.x - location.x;

			if (location.y + size.y > parent->size.y)
				drawBounds[3] = parent->size.y - location.y;
		}


		// Translate by location
		Draw::Translate(aContext, location.x, location.y);

		// Update child UI Components
		for (std::shared_ptr<Component> control : drawStack)
		{
			// Set draw boundary to the size of the this component
			nvgScissor(aContext, drawBounds[0]-location.x, drawBounds[1]-location.y, drawBounds[2], drawBounds[3]);
			
			// Invoke onFrame for the child component
			control->invokeOnFrame(aContext);
		}

		// Translate back
		Draw::Translate(aContext, -location.x, -location.y);

	}

} // namespace Lemur


#endif // !LEMUR_COMPONENT_CPP