#ifndef __TUTILITY_DEBUGCONSOLE__
#define __TUTILITY_DEBUGCONSOLE__



#if PLATFORM_TYPE == PLATFORM_WIN32
#include <windows.h>
#endif

namespace TsiU
{
	class DebugConsole
	{
	public:
		DebugConsole(const Char* file = NULL)
		{
#if PLATFORM_TYPE == PLATFORM_WIN32
			::AllocConsole();
			if(!file)
				freopen("CONOUT$" ,"w+t",stdout);
			else
				freopen(file ,"w+t",stdout);
#endif
		}
		~DebugConsole()
		{
#if PLATFORM_TYPE == PLATFORM_WIN32
			::FreeConsole();
#endif
		}
	};
}
	
#endif