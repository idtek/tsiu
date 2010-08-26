#ifndef __TEVNET_EVENTOBJECT__
#define __TEVNET_EVENTOBJECT__


#include "TEvent_EventID.h"
#include "TUtility_Array.h"

namespace TsiU
{
	union EventParam
	{
		s32		lParam;
		u32		ulParam;
		f32		fParam;
		void*	poParam;
	};

	class EventParamObject
	{
	public:
		void SetValue(const EventParam& _Param){ m_Param = _Param; };

		virtual	operator s32()		const { return m_Param.lParam;	}
		virtual	operator u32()		const { return m_Param.ulParam;	}
		virtual	operator f32()		const { return m_Param.fParam;	}
		virtual	operator void*()	const { return m_Param.poParam;	}

		EventParamObject& operator = (const EventParamObject& _epo)
		{
			m_Param = _epo.m_Param;
			return *this;
		}

	private:
		EventParam m_Param;
	};

	template<typename T>
	class EventParamObjectFactory{
	public:
		static EventParamObject* Create(T value){
			EventParamObject* obj = new EventParamObject();
			obj->SetValue(*((EventParam*)&value));
			return obj;
		};
	};
	
	class Event
	{
	public:
		Event()
			: m_ulEventType(E_ET_Invalid), m_ulEventSubType(E_EST_Invalid)
		{};
		Event(EventType_t _ulType, EventSubType_t _ulSubType = E_EST_Invalid) 
			: m_ulEventType(_ulType), m_ulEventSubType(_ulSubType)
		{};
		~Event()
		{
			m_arParamList.Clear();
		};

		template<typename T>
		Event& AddParam(T value){ 
			EventParamObject epo;
			epo.SetValue(*((EventParam*)&value));
			m_arParamList.PushBack(epo); 
			return *this; 
		};
		
		template<typename T>
		T	GetParam(s32 idx) const { return (T)m_arParamList[idx]; };


		s32	CountParam() const { return (s32)m_arParamList.Size(); };

		void			SetEventType(EventType_t _ulType)			{ m_ulEventType = _ulType;			};
		void			SetEventSubType(EventSubType_t _ulSubType)	{ m_ulEventSubType = _ulSubType;	};
		EventType_t		GetEventType()		const					{ return m_ulEventType;				};
		EventSubType_t	GetEventSubType()	const					{ return m_ulEventSubType;			};

		Event& operator = (const Event& _evt)
		{
			m_ulEventType	= _evt.m_ulEventType;
			m_ulEventSubType= _evt.m_ulEventSubType;
			m_arParamList.ReSize(_evt.m_arParamList.Size());
			for(int i = 0; i < m_arParamList.Size(); ++i)
			{
				m_arParamList[i] = _evt.m_arParamList[i];
			}
			return *this;
		}

	private:
		EventType_t		m_ulEventType;
		EventSubType_t	m_ulEventSubType;
		Array<EventParamObject> m_arParamList;
	};
}

#endif