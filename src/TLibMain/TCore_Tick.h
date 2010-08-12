#ifndef __TCORE_TICK__
#define __TCORE_TICK__

#include "TCore_Types.h"

namespace TsiU
{
	//---------------------------------------------------------------
	//
	//----------------------------------------------------------------
	class Tick
	{
	public:
		virtual u32 GetTick()		= 0;
		virtual u32 GetTickPerSec()	= 0;
	};
}

#endif
