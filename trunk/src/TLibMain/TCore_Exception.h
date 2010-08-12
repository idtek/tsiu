#ifndef __TCORE_EXCEPTION__ 
#define __TCORE_EXCEPTION__

#include "TCore_Panic.h"

namespace TsiU
{
	#define D_FatalError	g_poPanic->FatalError
	#define D_Error			g_poPanic->Error
	#define D_Warning		g_poPanic->Warning
}

#endif
