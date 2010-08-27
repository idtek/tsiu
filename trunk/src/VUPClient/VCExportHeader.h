#ifndef __VC_EXPORT_HEADER__
#define __VC_EXPORT_HEADER__

#ifdef VUPCLIENT_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

extern "C"
{
	void DLL_EXPORT VC_Print(const char* _str);
}

#endif