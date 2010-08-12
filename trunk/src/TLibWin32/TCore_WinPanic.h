#ifndef __TCORE_WINPANIC__ 
#define __TCORE_WINPANIC__

#include "TCore_Panic.h"
#include "TWin32_Private.h"

namespace TsiU
{
	class WinPanic : public Panic
	{
	public:
		virtual void FatalError(const Char* p_strInfo);
		virtual void Error(const Char* p_strInfo);
		virtual void Warning(const Char* p_strInfo);
	};
}

#endif