#include "TEngine_EventModule.h"
#include "TEvent_EventHandler.h"

namespace TsiU
{
	EventModule::EventModule()
	{
		m_ucEventPoolHead = m_ucEventPoolTail = 0; 
	}
	
	void EventModule::RegisterHandler(EventType_t _ucEventID, EventHandler* _poEH)
	{
		m_poHandler.insert(std::make_pair(_ucEventID, _poEH));
	}

	void EventModule::RunOneFrame(f32 _fDeltaTime)
	{
		u8	   evtCount = 0;
		Event* evtInput[k_MaxEventCount];

		for(; m_ucEventPoolTail != m_ucEventPoolHead; 
			  m_ucEventPoolTail = (m_ucEventPoolTail + 1) & ( k_MaxEventCount - 1 ))
		{
			Event *evt = m_EventPool + m_ucEventPoolTail;
			if(evt->GetEventType() == E_ET_Input)
			{
				evtInput[evtCount++] = evt;
			}
			else
			{
				SendEvent(evt);
			}
		}
		//TJQ: Send Input Event List to handler
		if(evtCount)
		{
			/* TJQ: We use event list for input, so handler for input can go through all input
					at one time.

					Param Type: E_ET_Input
					Param Sub Type: E_EST_Input_ListInfo
					Param 0: total count of input
					Param 1: pointer of input list

			Sample code is showed below:

			Event *pEvtInfo = const_cast<Event*>(_poEvent);
			if(pEvtInfo->GetEventSubType() != E_EST_Input_ListInfo)
				return;

			u8 ulCount = pEvtInfo->GetParam<u32>(0);
			Event **pEvtList = (Event**)pEvtInfo->GetParam<void*>(1);
			Event *pEvt = pEvtList[ulCount];

			for(s32 i = 0; i < ulCount; ++i)
			{
				Event *pEvt = pEvtList[i];
				//handle input here;
			}

			*/
			Event evtInfo(E_ET_Input, E_EST_Input_ListInfo);
			evtInfo.AddParam((u32)evtCount).AddParam((void*)&evtInput);

			SendEvent(&evtInfo);
		}
	}
	void EventModule::PostEvent(const Event* _evt)
	{
		m_EventPool[m_ucEventPoolHead] = *_evt;
		m_ucEventPoolHead = (m_ucEventPoolHead + 1) & (k_MaxEventCount - 1);
	}
	void EventModule::SendEvent(const Event* _evt)
	{
		HandlerSizeType st = m_poHandler.count(_evt->GetEventType());
		HandlerIterator it = m_poHandler.find(_evt->GetEventType());

		for(HandlerSizeType i = 0; i != st; i++, it++ )
		{
            EventHandler* poHandler = (*it).second;
			poHandler->Execute(_evt);   
        }
	}
}