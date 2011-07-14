#ifndef __TSCRIPT_SCRIPTMACHINE__
#define __TSCRIPT_SCRIPTMACHINE__


#include "TScript_Private.h"

namespace TsiU
{
	class ScriptDebugger;

	class ScriptMachine
	{
	public:
		ScriptMachine();
		~ScriptMachine();

		Bool Init();
		Bool Uninit();

		Bool DoBuffer(const Char* _poBuffer, s32 _iSize, const Char* _poName = NULL);
		Bool DoFile(const Char* _File);

		inline Bool HasInit()	{ return m_bHasInit;	}
		lua_State* GetLua()		{ return m_poLuaState;	}

	public:
		static void Panic(lua_State *lua);

	private:
		lua_State*		m_poLuaState;
		Bool			m_bHasInit;
		ScriptDebugger*	m_poDbg;
	};
}

#endif