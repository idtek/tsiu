#include "TScript_Debugger.h"
#include "TScript_Machine.h"

namespace TsiU
{
	void ScriptDebugger::ReportError(s32 _iErr)
	{
		Char errorMsg[256];
		if(_iErr != 0 )
		{	
			sprintf(errorMsg, "%s", lua_tostring(m_poSM->GetLua(), -1));
			lua_pop(m_poSM->GetLua(), 1);
			
			switch( _iErr ) 
			{
			case LUA_YIELD:
				D_Output("<Lua> RUN ERROR::: Yield %s\n", errorMsg);
				break;
			case LUA_ERRRUN:
				D_Output("<Lua> RUN ERROR::: Runtime %s\n", errorMsg);
				break;	
			case LUA_ERRMEM:
				D_Output("<Lua> RUN ERROR::: Memory %s\n", errorMsg);	
				break;
			case LUA_ERRSYNTAX:
				D_Output("<Lua> RUN ERROR::: Syntax %s\n", errorMsg);	
				break;
			case LUA_ERRERR:
				D_Output("<Lua> RUN ERROR::: Error %s\n", errorMsg);
				break;
			}
		}
	}
}