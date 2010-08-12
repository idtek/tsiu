#include "TEngine_ClockModule.h"
#include "TCore_LibSettings.h"
#include "TCore_Tick.h"

namespace TsiU
{
	ClockModule::ClockModule()
		:m_uiFrameElapsedTicks(0)
		,m_uiTotalElapsedTicks(0)
		,m_fFrameElapsedSeconds(0.f)
		,m_fTotalElapsedSeconds(0.f)
		,m_uiCurrentTicks(0)
	{
		m_poTick = CallCreator<Tick>(E_CreatorType_Tick);
		D_CHECK(m_poTick);

		m_uiLastTicks = m_poTick->GetTick();
	}

	ClockModule::~ClockModule()
	{
		D_SafeDelete(m_poTick);
	}

	void ClockModule::RunOneFrame(float _fDeltaTime)
	{
		m_uiCurrentTicks = m_poTick->GetTick();

		m_uiFrameElapsedTicks = m_uiCurrentTicks - m_uiLastTicks;
		m_uiTotalElapsedTicks += m_uiFrameElapsedTicks;

		m_fFrameElapsedSeconds = (f32)m_uiFrameElapsedTicks * 1.f / m_poTick->GetTickPerSec();
		m_fTotalElapsedSeconds += m_fFrameElapsedSeconds;

		m_uiLastTicks = m_uiCurrentTicks;
	}
}