#ifndef __TEVENT_EVENTHANDLER__
#define __TEVENT_EVENTHANDLER__

#include "TCore_Types.h"

namespace TsiU
{
	class Event;

	class EventHandler
	{
	public:
		virtual void Execute(const Event* p_poEvent) = 0;
	};

	template<typename T>
	class MEventHandler : public EventHandler
	{
		typedef void (T::*FunPtr)(const Event* p_poEvent);

	public:
		MEventHandler(T* _obj = 0, FunPtr _fPtr = 0):obj(_obj), fPtr(_fPtr){};
		~MEventHandler(){};

		virtual void Execute(const Event* p_poEvent){ 
			if( obj && fPtr ) 
				(obj->*fPtr)(p_poEvent);
		};

	private:
		T* obj;
		FunPtr fPtr;
	};

	class GEventHandler : public EventHandler
	{
		typedef void (*FunPtr)(const Event* p_poEvent);

	public:
		GEventHandler(FunPtr _fPtr = 0):fPtr(_fPtr){};
		~GEventHandler(){};

		virtual void Execute(const Event* p_poEvent){ 
			if( fPtr ) 
				(*fPtr)(p_poEvent);
		};

	private:
		FunPtr fPtr;
	};
}

#endif