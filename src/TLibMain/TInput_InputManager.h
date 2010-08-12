#ifndef __TINPUT_INPUTMANAGER__
#define __TINPUT_INPUTMANAGER__

#include "TCore_Types.h"
#include "TUtility_Array.h"
#include "TInput_Enum.h"

namespace TsiU
{
	class InputDevice;

	class InputManager
	{
	public:
		InputManager();
		virtual ~InputManager();

		void Tick(f32 _fDeltaTime);

	protected:
		void _ResetKeyState();

	protected:
		KeyState_t			m_KeyState[E_INPUT_ID_NUM];
		Array<InputDevice*>	m_DeviceList;
	};
}

#endif