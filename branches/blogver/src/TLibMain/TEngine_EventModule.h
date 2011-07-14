#ifndef __TENGINE_EVENTMODULE__
#define __TENGINE_EVENTMODULE__


#include "TEngine_Module.h"
#include <map>
#include "TEvent_EventObject.h"
#include "TEvent_EventID.h"

namespace TsiU
{
	class Event;
	class EventHandler;

	class EventModule : public IModule
	{
		static const u8	k_MaxEventCount	= 128;

	public:
		EventModule();
		~EventModule(){};

		virtual void Init(){};
		virtual void RunOneFrame(f32 _fDeltaTime);
		virtual void UnInit(){};

		void RegisterHandler(EventType_t _ucEventID, EventHandler* _poEH);

		void PostEvent(const Event* _evt);
		void SendEvent(const Event* _evt);

	private:
		//Event Handler
		typedef std::multimap<EventType_t, EventHandler*>::iterator HandlerIterator;
		typedef std::multimap<EventType_t, EventHandler*>::size_type HandlerSizeType;
		std::multimap<EventType_t, EventHandler*> m_poHandler;

		u8		m_ucEventPoolHead;
		u8		m_ucEventPoolTail;
		Event	m_EventPool[k_MaxEventCount];
	};
}

#endif