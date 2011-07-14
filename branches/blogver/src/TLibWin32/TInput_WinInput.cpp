#include "TInput_WinInput.h"

namespace TLib
{
	u32 WinInput::_VK_Translator(u32 p_uiVK)
	{
		switch(p_uiVK)
		{
		case E_KB_1:			return '1';
		case E_KB_2:			return '2';
		case E_KB_A:			return 'A';
		case E_KB_D:			return 'D';
		case E_KB_E:			return 'E';
		case E_KB_F:			return 'F';
		case E_KB_G:			return 'G';
		case E_KB_H:			return 'H';
		case E_KB_I:			return 'I';
		case E_KB_J:			return 'J';
		case E_KB_K:			return 'K';
		case E_KB_L:			return 'L';
		case E_KB_O:			return 'O';
		case E_KB_P:			return 'P';
		case E_KB_Q:			return 'Q';
		case E_KB_R:			return 'R';
		case E_KB_S:			return 'S';
		case E_KB_T:			return 'T';
		case E_KB_U:			return 'U';
		case E_KB_W:			return 'W';
		case E_KB_X:			return 'X';
		case E_KB_Y:			return 'Y';

		case E_KB_ESCAPE:		return VK_ESCAPE;
		case E_KB_SPACE:		return VK_SPACE;
		case E_KB_UP:			return VK_UP;
		case E_KB_DOWN:			return VK_DOWN;
		case E_KB_LEFT:			return VK_LEFT;
		case E_KB_RIGHT:		return VK_RIGHT;

		default:			return -1;
		}
	}
	void WinInput::UpdateKeyBoard(f32 p_fDeltaTime)
	{
		m_TriggerList.Clear();

		for(u32 i = 0; i < m_KeyInfo.Size(); ++i)
		{
			if(m_KeyInfo[i]->m_bIsAllowed == false)
				continue;

			s32 ret = GetAsyncKeyState(_VK_Translator(m_KeyInfo[i]->m_uiKeyID));
			m_KeyInfo[i]->m_uiLastValue = m_KeyInfo[i]->m_uiValue;
			m_KeyInfo[i]->m_uiValue = (ret & 0x8000) ? 1 : 0;

			if( ((m_KeyInfo[i]->m_uiTriggerType == E_TriggerType_Press) && (m_KeyInfo[i]->m_uiValue == 1) && (m_KeyInfo[i]->m_uiLastValue == 0)) ||
				((m_KeyInfo[i]->m_uiTriggerType == E_TriggerType_Down) && (m_KeyInfo[i]->m_uiValue == 1)) ||	
				((m_KeyInfo[i]->m_uiTriggerType == E_TriggerType_Release) && (m_KeyInfo[i]->m_uiValue == 0))
				)
			{
				m_TriggerList.PushBack(i);
			}
		}
	}
}