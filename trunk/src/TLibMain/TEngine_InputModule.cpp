#include "TEngine_InputModule.h"
#include "TCore_LibSettings.h"
#include "TInput_InputManager.h"

namespace TsiU
{
	InputModule::InputModule(u32 _uiNumOfInput)
		:m_uiInputCount(_uiNumOfInput)
	{
		D_CHECK(m_uiInputCount <= kMaxInputPlayer);
	}
	void InputModule::Init()
	{
		for(u32 i = 0; i < m_uiInputCount; ++i)
		{
			InputManager* _poInput = CallCreator<InputManager>(E_CreatorType_Input);
			m_InputManager.PushBack(_poInput);
		}
	}
	void InputModule::RunOneFrame(float _fDeltaTime)
	{
		for(u32 i = 0; i < m_InputManager.Size(); ++i)
		{
			m_InputManager[i]->Tick(_fDeltaTime);
		}
	}
}