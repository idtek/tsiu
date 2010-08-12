#ifndef __TENGINE_INPUTMODULE__
#define __TENGINE_INPUTMODULE__

#include "TCore_Types.h"
#include "TEngine_Module.h"
#include "TUtility_Array.h"

namespace TsiU
{
	class InputManager;

	class InputModule : public IModule
	{
		static const int kMaxInputPlayer = 4; 

	public:
		InputModule(u32 _uiNumOfInput);
		~InputModule(){};

		virtual void Init();
		virtual void RunOneFrame(float p_fDeltaTime);
		virtual void UnInit(){};

	private:
		u32	m_uiInputCount;
		Array<InputManager*> m_InputManager;
	};
}

#endif