#include "VCGlobalDef.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

void VC_Print(StringPtr _str)
{
	D_Output("%s\n", _str);
}
