#ifndef __TINPUT_WININPUTMANAGER__
#define __TINPUT_WININPUTMANAGER__

#include "TCore_Types.h"
#include "TInput_Enum.h"

#include "TInput_InputManager.h"

namespace TsiU
{
	class WinInputManager : public InputManager
	{
	public:
		WinInputManager();

		void PostMouseEvent(s16 _x, s16 _y);
		void PostButtonEvent(Key_t _key, KeyState_t _ks);
	};
}

#endif