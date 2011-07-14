#include "TScript_Function.h"

#include "TScript_API_Dbg.h"

namespace TsiU
{
	#define EMPTY_FUNC		{0,"",NULL,0,0,""}
	#define END_FUNC		{-1,"",NULL,0,0,""}

	ScriptFuncInfo g_API_Def[] = 
	{
		{1,		"TPrint",		API_Dbg_TPrint,	1,	0,	"void TPrint(string): Output string in DebugConsole"},
		END_FUNC	//TJQ: don't delete this line
	};
}