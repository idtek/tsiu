#ifndef __TEVENT_EVENTID__
#define __TEVENT_EVENTID__

namespace TsiU
{
	enum EventType_t
	{
		E_ET_Invalid = -1,
		E_ET_Input,

		E_ET_User,
	};

	enum EventSubType_t
	{
		E_EST_Invalid = -1,

		E_EST_Input_Mouse,
		E_EST_Input_KeyBoard,
		E_EST_Input_Pad,
		E_EST_Input_ListInfo
	};
}

#endif