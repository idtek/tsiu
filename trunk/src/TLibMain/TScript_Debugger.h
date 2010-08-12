#ifndef __TSCRIPT_SCRIPTDEBUGGER__
#define __TSCRIPT_SCRIPTDEBUGGER__

#include "TCore_Types.h"
#include "TScript_Private.h"

namespace TsiU
{
	class ScriptMachine;

	class ScriptDebugger
	{
	public:
		ScriptDebugger(ScriptMachine* _poSM)
			:m_poSM(_poSM)
		{}

		void ReportError(s32 _iErr);

	private:
		ScriptMachine* m_poSM;
	};
}

#endif