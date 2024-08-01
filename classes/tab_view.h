#ifndef LEMUR_UI_TABVIEW_H
#define LEMUR_UI_TABVIEW_H

/**************************************************************************************
* OpenDraft:    GUI TabView Class                                                     *
*-------------------------------------------------------------------------------------*
* Filename:     tab_view.h                                                            *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   A GUI tab view class derived from component                                       *
***************************************************************************************/



#include <string>
#include "core.h"
#include "component.h"
#include "draw.h"
#include "button.h"
#include "Panel.h"
#include "Input.h"


namespace Lemur
{
	class Tab : public Component
	{
	protected:
		bool resizeFlag = true; // Flag to recalculate size on text change

	public:
		Button* button;
		Panel* panel;

		Tab(std::string aText);
		~Tab();

		void setText(std::string aText);
		void recalculateSize(NVGcontext* aContext);
		void addPanelChildControl(Component* aControl);
		void onFrame(NVGcontext* aContext);
		void actionEvents(InputMap* aInput);
		void setParent(Component* aParent);

	};



	class TabView : public Component
	{
	protected:
		// Style constants
		const int HEADER_HEIGHT = 26;
		const int HEADER_FONT_SIZE = 12;
		const int PADDING = 4;

		int activeTab = 0;

	public:

		Colour headerColour;

		TabView(int aX = 0, int aY = 0, int aWidth = 400, int aHeight = 600);
		~TabView();

		void onFrame(NVGcontext* aContext) override;
		void actionEvents(InputMap* aInput) override;

		// Tab management
		Tab* getActiveTab();
		Tab* getTab(int aIndex);
		int getIndexOfTab(Tab* aTab);
		void addTab(std::string aText);
		void removeTab(int aIndex);
		void setActiveTab(int aIndex);

		// Set Tab class as a friend so it can access protected members
		friend class Tab;
	};

} // namespace Lemur

#endif // !LEMUR_UI_TABVIEW_H
