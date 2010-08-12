#include "TScript_API_Dbg.h"
#include "TCore_Types.h"

//using namespace TsiU;
/****************************************************************/
/*	Name:	TPrint												*/
/*  Help:	Output string in DebugConsole						*/
/*  Param:	string												*/
/*  Ret:	void												*/
/****************************************************************/
int API_Dbg_TPrint(lua_State* L)
{
	if(lua_isstring(L, 1))
	{
		D_Output(lua_tostring(L,1));
	}
	return 1;
}