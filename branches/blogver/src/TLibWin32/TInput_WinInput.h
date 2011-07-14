#ifndef __TINPUT_WININPUT__
#define __TINPUT_WININPUT__

#include "TCore_Types.h"
#include "TInput_Input.h"
#include "TInput_Enum.h"
#include "TWin32_Private.h"

namespace TLib
{
	class WinInput : public Input
	{
	protected:
		virtual void UpdateKeyBoard(f32 p_fDeltaTime);

	private:
		u32 _VK_Translator(u32 p_uiVK);
	};
}

#endif