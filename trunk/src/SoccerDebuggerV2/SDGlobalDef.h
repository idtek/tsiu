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
extern char							g_zRootDir[MAX_PATH];


enum{
	E_ET_SIM_SelectPlayerInSimulating = E_ET_User,
	E_ET_SIM_SelectPlayerInRealGame,
	E_ET_SIM_SelectRefCanvasInSimulating,
	E_ET_SIM_SelectRefCanvasInRealGame,
	E_ET_AIParamUpdate,
};

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

struct PlayerOtherAttributes
{
	enum{
		EPlayRole_Keeper,
		EPlayRole_Defend,
		EPlayRole_Midfield,
		EPlayRole_Forward
	};

	enum{
		ESideAttack_Any,
		ESideAttack_Left,
		ESideAttack_Middle,
		ESideAttack_Right,

		ESideAttack_Num
	};
	enum{
		EDefensiveLine_Front,
		EDefensiveLine_Middle,
		EDefensiveLine_Back,

		EDefensiveLine_Num
	};
	enum{
		EFocusPassing_Any,
		EFocusPassing_Left,
		EFocusPassing_Middle,
		EFocusPassing_Right,

		EFocusPassing_Num,
	};

public:
	PlayerOtherAttributes()
		: m_Height(0.f)
		, m_Role(-1)
		, m_PositionInTacticsBoard(-1)
	{
		ResetAIValue();
	}

	void ResetAIValue()
	{
		m_FormationDensity = 0.5f;
		m_SideAttack = ESideAttack_Any;
		m_DefensiveLine = EDefensiveLine_Middle;
		m_Width = 0.5f;
		m_Mentality = 0.5f;
		m_Tempo = 0.5f;
		m_TimeWasting = 0.5f;
		m_FocusPassing = EFocusPassing_Any;
		m_ClosingDown = 0.5f;
		m_TargetMan = false;
		m_Playmaker = false;
		m_CounterAttack = false;
	}

	float	m_Height;
	int		m_Role;
	int		m_PositionInTacticsBoard;

	float			m_FormationDensity;
	unsigned int	m_SideAttack;
	unsigned int	m_DefensiveLine;
	float			m_Width;
	float			m_Mentality;
	float			m_Tempo;
	float			m_TimeWasting;
	unsigned int	m_FocusPassing;
	float			m_ClosingDown;
	bool			m_TargetMan;
	bool			m_Playmaker;
	bool			m_CounterAttack;
};
struct PlayerIndividualAIParam
{
	PlayerOtherAttributes m_OtherAttributes[10];
};

struct RealGameInfo   
{
	RealGameInfo()
		: m_IsLargePitch(false)
		, m_IsHomeAttacking(true)
	{
		for(int i = 0; i < 10; ++i)
			m_Player[i] = PlayerInfo();
	}
	bool m_IsLargePitch;
	bool m_IsHomeAttacking;
	struct TeamInfo
	{
		TeamInfo()
			:m_HasGK(false)
		{}
		bool m_HasGK;
	};
	TeamInfo m_Team[2];
	struct PlayerInfo
	{
		PlayerInfo()
			: m_Pos(-1)
			, m_HasValidData(false)
			, m_Team(-1)
			, m_IsGK(false)
		{}
		bool	m_HasValidData;
		bool    m_IsGK;
		int		m_Pos;
		int		m_Team;
	};
	PlayerInfo m_Player[10];
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
	void	UpdateRefCanvas();
	u32		GetAppMode()			const{ return m_AppMode;	}

private:
	u32 m_AppMode;
};

#endif