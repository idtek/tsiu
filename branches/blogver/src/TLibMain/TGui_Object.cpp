#include "TGui_Object.h"
#include "TGui_EventHandler.h"

namespace TsiU
{
	void GuiObject::RegisterEventHandler(u32 p_ulEventType, GuiEventHandler* p_poHandler)
	{
		m_poHandler.insert(std::pair<u32, GuiEventHandler*>(p_ulEventType, p_poHandler));
	}
	void GuiObject::ExecuteEventHandler(u32 p_ulEventType, GuiEvent* p_poEvent)
	{
		HandlerIterator it = m_poHandler.find(p_ulEventType);
		if(it != m_poHandler.end())
		{
			GuiEventHandler* l_poHandler = (*it).second;
			l_poHandler->Execute(p_poEvent);
		}
	}
}