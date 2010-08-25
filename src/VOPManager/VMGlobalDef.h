#ifndef __VM_GLOBALDEF_H__
#define __VM_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

extern Engine*						g_poEngine;

class MyEngine : public Engine
{
public:
	MyEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow)
		:Engine(_uiWidth, _uiHeight, _strTitle, _bIsWindow, -1)
	{}

	virtual void DoInit();
	virtual void DoUnInit();
};

#endif