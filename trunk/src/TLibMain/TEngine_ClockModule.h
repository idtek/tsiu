#ifndef __TENGINE_CLOCKMODULE__
#define __TENGINE_CLOCKMODULE__

#include "TCore_Types.h"
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
		D_Inline u32 GetFrameElapsedTicks() const;
		D_Inline u32 GetTotalElapsedTicks() const;

	private:
		Tick* m_poTick;
		
		f32	m_fFrameElapsedSeconds;
		f32	m_fTotalElapsedSeconds;

		u32	m_uiFrameElapsedTicks;
		u32	m_uiTotalElapsedTicks;

		u32	m_uiCurrentTicks;
		u32	m_uiLastTicks;
	};

	D_Inline f32 ClockModule::GetFrameElapsedSeconds() const
	{	
		return m_fFrameElapsedSeconds;	
	};
	D_Inline f32 ClockModule::GetTotalElapsedSeconds() const
	{	
		return m_fTotalElapsedSeconds;	
	};
	D_Inline u32 ClockModule::GetFrameElapsedTicks() const
	{
		return m_uiFrameElapsedTicks;
	}
	D_Inline u32 ClockModule::GetTotalElapsedTicks() const
	{
		return m_uiTotalElapsedTicks;
	}
}

#endif