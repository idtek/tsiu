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
			timeBeginPeriod(1);
		}
		~WinTick()
		{
			timeEndPeriod(1);
		}
		virtual u32 GetTick()
		{
			return (u32)::timeGetTime();
		}
		virtual u32 GetTickPerSec()
		{
			return 1000;
		}
	};
}

#endif