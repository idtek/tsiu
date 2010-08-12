#include "TCore_Panic.h"

namespace TsiU
{
	void DefaultPanic::FatalError(StringPtr _strInfo)
	{
		D_Output("[FatalError]:");
		ShowInfo(_strInfo);
		//exit program
		exit(0);
	}
	void DefaultPanic::Error(StringPtr _strInfo)
	{
		D_Output("[Error]:");
		ShowInfo(_strInfo);
	}
	void DefaultPanic::Warning(StringPtr _strInfo)
	{
		D_Output("[Warning]:");
		ShowInfo(_strInfo);
	}
	void DefaultPanic::ShowInfo(StringPtr _strInfo)
	{
		D_Output("%s\n", _strInfo);
	}

	Panic* g_poPanic = NULL;
}