#ifndef LEMUR_UI_TABVIEW_CPP
#define LEMUR_UI_TABVIEW_CPP

/**************************************************************************************
* OpenDraft:    GUI TabView Class                                                     *
*-------------------------------------------------------------------------------------*
* Filename:     tab_view.cpp                                                          *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI tab view class derived from component                                       *
***************************************************************************************/



#include "tab_view.h"


namespace Lemur
{
	//
	// Tab Component for Tab View
	//
	Tab::Tab(std::string aText)
	{
		// Create associated button
		button = new Button(0, 0, 10, 30, aText);
		addChildControl(button);

		// Create associated panel
		panel = new Panel(0, 0, 10, 10);
		addChildControl(panel);

		setText(aText);
	}

	Tab::~Tab()
	{

	}

	void Tab::setText(std::string aText)
	{
		text = aText;
		button->setText(aText);
		resizeFlag = true; // Size needs to be recalculated
	}

	void Tab::recalculateSize(NVGcontext* aContext)
	{
		// Use nanovg to calculate text width
		float bounds[4];

		// Static cast properties
		float buttonX = button->getWidth();
		float buttonY = button->getHeight();

		nvgSave(aContext);

		nvgReset(aContext);
		nvgTextBounds(aContext, buttonX, buttonY, button->text.c_str(), nullptr, bounds);

		int padding = ((TabView*)parent)->PADDING;
		int newSize = (bounds[2] - bounds[0]) + padding; // Where bounds[2]-[0] is the width of the text
		button->setWidth(newSize);

		nvgRestore(aContext);

		// Reset flag
		resizeFlag = false;
	}


	void Tab::addPanelChildControl(Component* aChild)
	{	
		// Add component to list
		panel->addChildControl(aChild);
	}


	void Tab::onFrame(NVGcontext* aContext)
	{
		// If button's size needs to be recalculated, do so
		if (resizeFlag)
			recalculateSize(aContext);

		// Draw tab button
		button->onFrame(aContext);

		panel->setWidth(parent->getWidth());

		// Draw tab panel is tab is active (enabled)
		if (enabled)
			panel->onFrame(aContext);
	}


	void Tab::actionEvents(InputMap* aInput)
	{

	}


	void Tab::setParent(Component* aParent)
	{
		parent = aParent;

		// Inherit properties from parent TabView
		button->setHeight( ((TabView*)parent)->HEADER_HEIGHT);
		button->setFontSize( ((TabView*)parent)->HEADER_FONT_SIZE );

		// Set panel location
		panel->setLocation(0, ((TabView*)parent)->HEADER_HEIGHT);
		panel->setSize(parent->getWidth(), parent->getHeight() - ((TabView*)parent)->HEADER_HEIGHT);
		panel->backColour = Colour::BACKGROUND2;
	}



	TabView::TabView(int aX, int aY, int aWidth, int aHeight)
	{
		location.x = aX;
		location.y = aY;
		size.x = aWidth;
		size.y = aHeight;

		backColour = Colour::BACKGROUND1;
		headerColour = Colour::BACKGROUND2;
		foreColour = Colour::WHITE;

		stroke = Colour(0, 0, 0, 0);

		text = "TabView";
	}



	//
	// Tab View Component
	//
	TabView::~TabView()
	{
		// Delete tabs
		for (int i = 0; i < childComponents.size(); i++)
		{
			removeTab(i);
		}
	}


	void TabView::onFrame(NVGcontext* aContext)
	{			
		// Update states
		for (std::shared_ptr<Component> component : childComponents)
		{
			if (Tab* tab = static_cast<Tab*>(component.get()))
			{
				if (tab->button->wasMousePressed())
					setActiveTab(getIndexOfTab(tab));
			}
		}


		// Static cast properties
		float x = getLocation().x;
		float y = getLocation().y;
		float w = getWidth();
		float h = getHeight();



		// Drawing window
		//Draw::Rect(aContext, x, y, w, h, Colour::PRIMARY);
			
		Draw::RectStroke(aContext, x, y, w, h, 0.5, stroke);

		// Draw header
		Draw::Rect(aContext, x, y, w, HEADER_HEIGHT, Colour::BACKGROUND1);
		Draw::RectStroke(aContext, x, y, h, HEADER_HEIGHT, 0.5, stroke);

		// Create running offset for tab buttons
		int offset = 0;


		//
		// Draw Tabs
		//
		for (int i = 0; i < childComponents.size(); i++)
		{
			//
			// Draw Buttons
			//
				 
			// Update offset
			Tab* tab = (Tab*)childComponents[i].get();
			tab->button->setLocation(offset + 4, 0);
				
			// Translate by location
			Draw::Translate(aContext, x, y);
				
			// Draw tab
			tab->onFrame(aContext);

			// Translate back
			Draw::Translate(aContext, -x, -y);
				
			// Update offset
			offset += static_cast<int>(tab->button->getWidth());

		}
	}


	void TabView::actionEvents(InputMap* aInput)
	{

	}


	Tab* TabView::getActiveTab()
	{
		// Find and return active tab
		for (int i = 0; i < childComponents.size(); i++)
		{
			if (childComponents[i]->enabled)
				return (Tab*)childComponents[i].get();
		}
		return nullptr;
	}
		
	Tab* TabView::getTab(int aIndex)
	{
		// Check index is valid
		if (aIndex < 0 || aIndex >= childComponents.size())
			return nullptr;

		// Get tab by index
		return (Tab*)childComponents[aIndex].get();
	}

	int TabView::getIndexOfTab(Tab* aTab)
	{
		// Find index of tab
		for (int i = 0; i < childComponents.size(); i++)
		{
			if (childComponents[i].get() == aTab)
				return i;
		}
		return -1;
	}

	void TabView::addTab(std::string aText)
	{
		// Add tab and associated tab button
		Tab* tab = new Tab(aText);

		// Set tab's parent
		tab->setParent(this);

		// Insert into vector
		childComponents.push_back(std::shared_ptr<Component>(tab));
	}

		
	void TabView::removeTab(int aIndex)
	{
		// Remove from vector
		childComponents.erase(childComponents.begin() + aIndex);
	}

	void TabView::setActiveTab(int aIndex)
	{
		// Check index is valid
		if (aIndex < 0 || aIndex >= childComponents.size())
		{
			// By default, set to first tab
			activeTab = 0;

			// Check not empty
			if (childComponents.size() != 0)
				childComponents[0]->enabled = true;
				
			return;
		}

		activeTab = aIndex;

		// Set tab to active
		for (int i = 0; i < childComponents.size(); i++)
		{
			// Get child as Tab
			Tab* tab = static_cast<Tab*>(childComponents[i].get());

			if (i == aIndex)
			{
				tab->enabled = true;
				tab->button->backColour = Colour::BACKGROUND2;
				tab->button->foreColour = Colour::PRIMARY;
			}
			else
			{
				tab->enabled = false;
				tab->button->backColour = Colour::BACKGROUND1;
				tab->button->foreColour = Colour::WHITE;
			}
		}
	}
		
} // namespace OpenDraft

#endif // !LEMUR_UI_BUTTON_CPP