#ifndef __TUTILITY_DEBUGCONSOLE__
#define __TUTILITY_DEBUGCONSOLE__

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
			fclose(stdout);
#endif
		}
	};
}
	
#endif