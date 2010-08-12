#ifndef __TINPUT_INPUT__
#define __TINPUT_INPUT__

#include "TCore_Types.h"
#include "TUtility_Array.h"

namespace TLib
{
	class Input
	{
	public:
		void Tick(f32 p_fDeltaTime);
		Bool RegisterKey(u32 p_uiKey, u32 p_uiTriggerType, Bool p_bIsAllowed);
		Bool IsKeyTriggered(u32 p_uiKey, u32 p_uiTriggerType);

	protected:
		virtual void UpdateKeyBoard(f32 p_fDeltaTime) = 0;

	protected:
		struct RegKeyInfo
		{
			u32		m_uiKeyID;
			Bool	m_bIsAllowed;
			u32		m_uiValue;
			u32		m_uiLastValue;
			u32		m_uiTriggerType;
		};
		Array<RegKeyInfo*>	m_KeyInfo;
		Array<u32>			m_TriggerList;
	};
}

#endif