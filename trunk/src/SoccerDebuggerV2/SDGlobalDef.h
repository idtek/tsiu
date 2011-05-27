#ifndef __SD_GLOBALDEF_H__
#define __SD_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include "TWin32_Header.h"
#endif

#include "SDMacro.h"

//class MemPool;
class ODynamicObj;
struct UDP_PACK;

extern Engine*						g_poEngine;
extern SimpleRenderObjectUtility*	g_poSROU;
extern MemPool<UDP_PACK>*			g_poMem;
extern Bool							g_bStop;
extern HANDLE						g_hRecv;
extern ODynamicObj*					g_WatchPlayer;
extern u32							g_WindowWidth;
extern u32							g_WindowHeight;
extern Bool							g_bIsShowDetailed;
extern Bool							g_bIsSlowMotion;
extern Bool							g_bIsAlwaysOnTop;
extern Bool							g_bIsWatchOwner;
extern char							g_zLastOpenFile[256];

//DrawValue;
namespace Util
{
	extern void Clear(ETabIndex tab);
	extern int GetItemSize(ETabIndex tab);
	extern void DrawValue(ETabIndex tab, UDP_PACK* pack, bool shouldHighLight = false);
}

enum{
	EAppMode_None,
	EAppMode_WatchMode,
	EAppMode_FormationEditor
};

class MyEngine : public Engine
{
public:
	MyEngine(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindow)
		:Engine(p_uiWidth, p_uiHeight, p_strTitle, p_bIsWindow)
		,m_AppMode(EAppMode_None)
	{}

	virtual void DoInit();
	virtual void DoUnInit();

	void	ChangeAppMode(u32 mode);
	void	UpdateCanvas();
	u32		GetAppMode()			const{ return m_AppMode;	}

private:
	u32 m_AppMode;
};

#endif