#include "TInput_Input.h"

namespace TLib
{
	void Input::Tick(f32 p_fDeltaTime)
	{
		UpdateKeyBoard(p_fDeltaTime);
	}

	Bool Input::RegisterKey(u32 p_uiKey, u32 p_uiTriggerType, Bool p_bIsAllowed)
	{
		RegKeyInfo *l_RKI = new RegKeyInfo;
		D_CHECK(l_RKI);

		l_RKI->m_uiKeyID		= p_uiKey;
		l_RKI->m_bIsAllowed		= p_bIsAllowed;
		l_RKI->m_uiTriggerType	= p_uiTriggerType;
		l_RKI->m_uiLastValue	= 0;
		l_RKI->m_uiValue		= 0;

		m_KeyInfo.PushBack(l_RKI);

		return true;
	}
	Bool Input::IsKeyTriggered(u32 p_uiKey, u32 p_uiTriggerType)
	{
		for(u32 i = 0; i < m_TriggerList.Size(); ++i)
		{
			if(m_KeyInfo[m_TriggerList[i]]->m_uiKeyID == p_uiKey && 
				m_KeyInfo[m_TriggerList[i]]->m_uiTriggerType == p_uiTriggerType )
				return true;
		}
		return false;
	}
}