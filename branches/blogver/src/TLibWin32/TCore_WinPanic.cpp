#include "TCore_WinPanic.h"

namespace TsiU
{
	WinPanic g_WinPanic;

	void WinPanic::FatalError(const Char* p_strInfo)
	{
		::MessageBox(NULL, p_strInfo, "Fatal Error", MB_OK | MB_ICONERROR );
		//quit
		exit(1);
	}
	void WinPanic::Error(const Char* p_strInfo)
	{
		::MessageBox(NULL, p_strInfo, "Error", MB_OK | MB_ICONERROR );
	}
	void WinPanic::Warning(const Char* p_strInfo)
	{
		::MessageBox(NULL, p_strInfo, "Warning", MB_OK | MB_ICONINFORMATION );
	}
}