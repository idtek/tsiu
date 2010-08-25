#ifndef __TCORE_WINTICK__
#define __TCORE_WINTICK__

#include "TCore_Types.h"
#include "TCore_Tick.h"
#include "TCore_Creator.h"

#include "TWin32_Private.h"

namespace TsiU
{
	class WinTick : public Tick
	{
	public:
		WinTick()
		{
			::timeBeginPeriod(1);
			::QueryPerformanceFrequency(&m_Freq);
		}
		~WinTick()
		{
			::timeEndPeriod(1);
		}
		virtual s64 GetTick()
		{
			LARGE_INTEGER tick;
			::QueryPerformanceCounter(&tick);
			return (s64)tick.QuadPart;
		}
		virtual s64 GetTickPerSec()
		{
			return m_Freq.QuadPart;
		}

	private:
		LARGE_INTEGER m_Freq;
	};
}

#endif