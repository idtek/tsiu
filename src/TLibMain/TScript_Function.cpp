#include "TScript_Function.h"
#include "TScript_Machine.h"

namespace TsiU
{
	ScriptFunction::ScriptFunction(ScriptMachine *_poSM, ScriptFuncInfo* _poSFI)
		:m_poSM(_poSM)
	{
		m_FuncInfo = *_poSFI;
	}

	Bool ScriptFunction::RegisterFunction()
	{
		if(m_poSM->HasInit())
		{
			lua_pushstring(m_poSM->GetLua(), m_FuncInfo.strFuncName.c_str());
			lua_pushcfunction(m_poSM->GetLua(), m_FuncInfo.poFunc);
			lua_settable(m_poSM->GetLua(), LUA_GLOBALSINDEX);

			return true;
		}
		return false;
	}
}