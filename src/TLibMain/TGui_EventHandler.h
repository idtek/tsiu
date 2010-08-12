#ifndef __TGUI_EVENTHANDLER__
#define __TGUI_EVENTHANDLER__

#include "TCore_Types.h"

namespace TLib
{
	enum
	{
		//Button
		E_GUI_EVENT_BUTTON_CLICK = 0,

		//Scroll
		E_GUI_EVENT_SCROLL_MOVE = 10,

		//Sliding BAR
		E_GUI_EVENT_SLIDINGBAR_VALUECHANGE = 20,

		//Sliding BAR
		E_GUI_EVENT_CHECKBOX_VALUECHANGE = 30,
	};

	class GuiEvent;

	class GuiEventHandler
	{
	public:
		virtual void Execute(GuiEvent* p_poEvent) = 0;
	};

	template<typename T>
	class GuiMEvnetHandler : public GuiEventHandler
	{
		typedef void (T::*FunPtr)(GuiEvent* p_poEvent);

	public:
		GuiMEvnetHandler(T* _obj = 0, FunPtr _fPtr = 0):obj(_obj), fPtr(_fPtr){};
		~GuiMEvnetHandler(){};

		virtual void Execute(GuiEvent* p_poEvent){ 
			if( obj && fPtr ) 
				(obj->*fPtr)(p_poEvent);
		};

	private:
		T* obj;
		FunPtr fPtr;
	};

	class GuiGEvnetHandler : public GuiEventHandler
	{
		typedef void (*FunPtr)(GuiEvent* p_poEvent);

	public:
		GuiGEvnetHandler(FunPtr _fPtr = 0):fPtr(_fPtr){};
		~GuiGEvnetHandler(){};

		virtual void Execute(GuiEvent* p_poEvent){ 
			if( fPtr ) 
				(*fPtr)(p_poEvent);
		};

	private:
		FunPtr fPtr;
	};
}

#endif