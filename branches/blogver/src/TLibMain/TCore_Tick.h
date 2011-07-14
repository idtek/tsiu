#ifndef __TCORE_TICK__
#define __TCORE_TICK__



namespace TsiU
{
	//---------------------------------------------------------------
	//
	//----------------------------------------------------------------
	class Tick
	{
	public:
		virtual s64 GetTick()		= 0;
		virtual s64 GetTickPerSec()	= 0;
	};
}

#endif
