#include "TEngine_ClockModule.h"
#include "TCore_LibSettings.h"
#include "TCore_Tick.h"

#if PLATFORM_TYPE == PLATFORM_WIN32
#include <windows.h>
#endif

namespace TsiU
{
	ClockModule::ClockModule()
		:m_uiFrameElapsedTicks(0)
		,m_uiTotalElapsedTicks(0)
		,m_fFrameElapsedSeconds(0.f)
		,m_fTotalElapsedSeconds(0.f)
		,m_uiCurrentTicks(0)
		,m_iStartFrameTicks(0)
		,m_iRateLock(-1)
	{
		m_poTick = CallCreator<Tick>(E_CreatorType_Tick);
		D_CHECK(m_poTick);

		m_uiLastTicks = m_poTick->GetTick();
	}

	ClockModule::~ClockModule()
	{
		D_SafeDelete(m_poTick);
	}

	void ClockModule::StartFrame()
	{
		m_iStartFrameTicks = m_poTick->GetTick();
	}

	void ClockModule::EndFrame()
	{
		if(m_iRateLock > 0)
		{
			do
			{
				s64 endFrameTicks = m_poTick->GetTick();
				f32 eltime = (static_cast<float>(endFrameTicks - m_iStartFrameTicks)/static_cast<float>(m_poTick->GetTickPerSec()));
				if(eltime > 1.f / m_iRateLock)
					break;

				s32 time = s32(1000.f * (1.f / m_iRateLock - GetFrameElapsedSeconds()));
				if(time >= 0)
				{
#if PLATFORM_TYPE == PLATFORM_WIN32
					::Sleep(time);
#endif
				}
			}
			while(1);
		}
	}

	void ClockModule::RunOneFrame(float _fDeltaTime)
	{
		D_Unused(_fDeltaTime);

		m_uiCurrentTicks = m_poTick->GetTick();

		m_uiFrameElapsedTicks = m_uiCurrentTicks - m_uiLastTicks;
		m_uiTotalElapsedTicks += m_uiFrameElapsedTicks;

		m_fFrameElapsedSeconds = (f32)m_uiFrameElapsedTicks * 1.f / m_poTick->GetTickPerSec();
		m_fTotalElapsedSeconds += m_fFrameElapsedSeconds;

		m_uiLastTicks = m_uiCurrentTicks;
	}
}