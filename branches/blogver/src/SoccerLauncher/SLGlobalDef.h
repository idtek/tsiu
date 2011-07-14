#ifndef __SD_GLOBALDEF_H__
#define __SD_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

extern Engine*						g_poEngine;

class MyEngine : public Engine
{
public:
	MyEngine(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindow)
		:Engine(p_uiWidth, p_uiHeight, p_strTitle, p_bIsWindow)
	{}

	virtual void DoInit();
	virtual void DoUnInit();
};

#endif