#ifndef __AT_GLOBALDEF_H__
#define __AT_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

//-----------------------------------------------------------------------------------
extern Engine*		g_poEngine;
//------------------------------------------------------------------------------------
class GameEngine : public Engine
{
public:
	static GameEngine* GetGameEngine() { return (GameEngine*)g_poEngine;	}

	GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow);

	virtual void DoInit();
	virtual void DoUnInit();
};

#endif