#ifndef __TENGINE_CLOCKMODULE__
#define __TENGINE_CLOCKMODULE__


#include "TEngine_Module.h"

namespace TsiU
{
	class Tick;

	class ClockModule : public IModule
	{
	public:
		ClockModule();
		~ClockModule();

		virtual void Init(){};
		virtual void RunOneFrame(float _fDeltaTime);
		virtual void UnInit(){};

		D_Inline f32 GetFrameElapsedSeconds() const;
		D_Inline f32 GetTotalElapsedSeconds() const;
		D_Inline s64 GetFrameElapsedTicks() const;
		D_Inline s64 GetTotalElapsedTicks() const;

		D_Inline void SetRateLock(s32 _iRateLock);

		void StartFrame();
		void EndFrame();

	private:
		Tick* m_poTick;
		
		f32	m_fFrameElapsedSeconds;
		f32	m_fTotalElapsedSeconds;

		s64	m_uiFrameElapsedTicks;
		s64	m_uiTotalElapsedTicks;

		s64	m_uiCurrentTicks;
		s64	m_uiLastTicks;

		s32	m_iRateLock;
		s64 m_iStartFrameTicks;
	};

	D_Inline f32 ClockModule::GetFrameElapsedSeconds() const
	{	
		return m_fFrameElapsedSeconds;	
	};
	D_Inline f32 ClockModule::GetTotalElapsedSeconds() const
	{	
		return m_fTotalElapsedSeconds;	
	};
	D_Inline s64 ClockModule::GetFrameElapsedTicks() const
	{
		return m_uiFrameElapsedTicks;
	}
	D_Inline s64 ClockModule::GetTotalElapsedTicks() const
	{
		return m_uiTotalElapsedTicks;
	}
	D_Inline void ClockModule::SetRateLock(s32 _iRateLock)
	{
		m_iRateLock = _iRateLock;

	}
}

#endif