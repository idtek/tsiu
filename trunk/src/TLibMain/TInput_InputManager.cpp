#include "TInput_InputManager.h"
#include "TInput_InputDevice.h"

namespace TsiU
{
	InputManager::InputManager()
	{
		_ResetKeyState();
	}

	InputManager::~InputManager()
	{
		for(s32 i = 0; i < m_DeviceList.Size(); ++i)
		{
			if(m_DeviceList[i])
			{
				delete m_DeviceList[i];
			}
		}
		m_DeviceList.Clear();
	}

	void InputManager::Tick(f32 _fDeltaTime)
	{
		for(u32 i = 0; i < m_DeviceList.Size(); ++i)
		{
			if(m_DeviceList[i]->IsActive())
				m_DeviceList[i]->Tick(_fDeltaTime);
		}
	}

	void InputManager::_ResetKeyState()
	{
		for(s32 i = 0; i < E_INPUT_ID_NUM; ++i)
			m_KeyState[i] = E_KS_Up;
	}
}