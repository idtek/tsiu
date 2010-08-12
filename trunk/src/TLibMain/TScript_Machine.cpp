#include "TScript_Machine.h"
#include "TScript_Debugger.h"
#include "TCore_Exception.h"

namespace TsiU
{
	ScriptMachine::ScriptMachine()
		:m_poLuaState(NULL), m_poDbg(NULL), m_bHasInit(false)
	{
	}

	ScriptMachine::~ScriptMachine()
	{
		Uninit();
	}

	Bool ScriptMachine::Init()
	{
		if(HasInit())
			return true;
		
		m_poLuaState = lua_open();

		if(m_poLuaState) 
		{
			m_bHasInit = true;

			luaopen_base(m_poLuaState);
			luaopen_table(m_poLuaState);
			luaopen_string(m_poLuaState);
			luaopen_math(m_poLuaState);
#ifdef TLIB_DEBUG
			luaopen_debug(m_poLuaState);
			m_poDbg = new ScriptDebugger(this);
#endif
			lua_atpanic(m_poLuaState, (lua_CFunction)ScriptMachine::Panic);

			return true;
		}
		return false;
	}

	Bool ScriptMachine::Uninit()
	{
		if(m_poLuaState)
		{
			lua_close(m_poLuaState);
			m_poLuaState = NULL;
			m_bHasInit = false;
		}
		if(m_poDbg)
		{
			delete m_poDbg;
			m_poDbg = NULL;
		}
		return true;
	}

	void ScriptMachine::Panic(lua_State *lua)
	{
		D_FatalError("Error in Script Machine");
	}

	Bool ScriptMachine::DoBuffer(const Char* _poBuffer, s32 _iSize, const Char* _poName)
	{
		Bool bSuccess = false;
		s32 iErr = 0;

		if(_poName == NULL)
		{
			_poName = "Temp";
		}
		if((iErr = luaL_loadbuffer(m_poLuaState, (const Char *)_poBuffer, _iSize, _poName)) == 0)
		{
			if((iErr = lua_pcall(m_poLuaState, 0, LUA_MULTRET, 0)) == 0)
			{
				bSuccess = true;
			}
		}
		if(bSuccess == false)
		{
			if(m_poDbg != NULL)
				m_poDbg->ReportError(iErr);
		}
		return bSuccess;   
	}

	Bool ScriptMachine::DoFile(const Char* _File)
	{
		Bool bSuccess = false;
		s32 iErr = 0;

		if((iErr = luaL_loadfile(m_poLuaState, _File)) == 0)
		{
			if((iErr = lua_pcall(m_poLuaState, 0, LUA_MULTRET, 0)) == 0)
			{
				bSuccess = true;
			}
		}
		if(bSuccess == false)
		{
			if(m_poDbg != NULL)
				m_poDbg->ReportError(iErr);
		}
		return bSuccess;  
	}
}