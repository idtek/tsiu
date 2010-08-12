#ifndef __TGUI_OBJECT__
#define __TGUI_OBJECT__

#include "TCore_Types.h"
#include "TEngine_Object.h"
#include "TCore_Exception.h"
#include <map>

namespace TsiU
{
	class GuiEvent;
	class GuiEventHandler;

	class GuiObject : public Object
	{
	public:
		void RegisterEventHandler(u32 p_ulEventType, GuiEventHandler* p_poHandler);
		void ExecuteEventHandler(u32 p_ulEventType, GuiEvent* p_poEvent);

	protected:
		//Event Handler
		typedef std::map<u32, GuiEventHandler*>::iterator HandlerIterator;
		std::map<u32, GuiEventHandler*> m_poHandler;
	};
}

#endif