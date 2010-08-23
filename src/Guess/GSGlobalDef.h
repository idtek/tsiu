#ifndef __GS_GLOBALDEF_H__
#define __GS_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

extern Engine*						g_poEngine;

class GameEngine : public Engine
{
public:
	static GameEngine* GetGameEngine() { return (GameEngine*)g_poEngine;	}

public:
	GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed);

	virtual void DoInit();
	virtual void DoUnInit();
	virtual void DoPreFrame(){};

public:
	enum{
		E_ET_EffectFinished = E_ET_User,
	};
};

class GSWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnWindowResize(s32 width, s32 height);
	virtual void OnMouseLDown(s32 x, s32 y);
};

#endif