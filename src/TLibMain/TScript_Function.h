#ifndef __TSCRIPT_FUNCTION__
#define __TSCRIPT_FUNCTION__


#include <string>
#include "TScript_Private.h"

namespace TsiU
{
	class ScriptMachine;

	struct ScriptFuncInfo
	{
		s32				iFuncIdx;
		std::string		strFuncName;
		lua_CFunction	poFunc;
		s32				iParamCount;
		s32				iRetCount;
		std::string		strDesc;
	};	

	class ScriptFunction
	{
	public:
		ScriptFunction(ScriptMachine* _poSM, ScriptFuncInfo* _poSFI);
		Bool RegisterFunction();

	private:
		ScriptMachine*	m_poSM;
		ScriptFuncInfo	m_FuncInfo;
	};
}

#endif