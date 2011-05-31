#ifndef __SD_MACRO_H__
#define __SD_MACRO_H__

#include <process.h>

extern u32 g_WindowWidth;
extern u32 g_WindowHeight;

#define kWINDOW_WIDTH		1280
#define kWINDOW_HEIGHT		680

static const f32 kPithLenghNormal	= 50.f;
static const f32 kPitchWidthNormal	= 30.f;
static const f32 kPithLenghLarge	= 60.f;
static const f32 kPitchWidthLarge	= 40.f;

struct CoordinateInfo
{
	static f32	GetPixelPerMeter();
	static f32	GetMeterPerPixel();
	static Vec2 WorldToScreen(const Vec2& worldPos, Bool awayView = false);
	static Vec2 ScreenToWorld(const Vec2& screenPos, Bool awayView = false);

	static Bool sAwayView;
	static f32  sLength;
	static f32	sWidth;
};

#ifdef SSO_YETI
#define kRULE_GOALLINE			52.5f	
#define kRULE_SIDELINE			34.0f	
#define kRULE_CENTERCIRCLE		9.15f
#define kRULE_PENALTY_POS		(kRULE_SIDELINE - (3.66f + 16.5f))
#define kRULE_PENALTY_HEIGHT	(2 * (3.66f + 16.5f))
#define kRULE_PENALTY_WIDTH		16.5f
#else
#define kRULE_GOALLINE			(CoordinateInfo::sLength / 2.f)	
#define kRULE_SIDELINE			(CoordinateInfo::sWidth / 2.f)
#define kRULE_CENTERCIRCLE		6.0f
#define kRULE_PENALTY_POS		(kRULE_SIDELINE - (3.66f + 16.5f) * 0.6f)
#define kRULE_PENALTY_HEIGHT	(2 * (3.66f + 16.5f) * 0.6f)
#define kRULE_PENALTY_WIDTH		(16.5f * 0.6f)
#endif

#define kCANVAS_START_X			20
#define kCANVAS_START_Y			15
#define kCANVAS_WIDTH			g_WindowWidth
#define kCANVAS_HEIGHT			(g_WindowHeight)

#define kWATCH_WIDTH			0
#define kWATCH_HEIGHT			(g_WindowHeight - 2 * kCANVAS_START_Y)
#define kWATCH_FONTHEIGHT		15
#define kWATCH_START_Y2			kCANVAS_START_Y + kWATCH_FONTHEIGHT * 20
#define kWATCH_START_X			kCANVAS_WIDTH - (kCANVAS_START_X + kWATCH_WIDTH)
#define kWATCH_START_X2			kCANVAS_WIDTH - (kCANVAS_START_X + kWATCH_WIDTH/2)
#define kWATCH_START_Y			kCANVAS_START_Y

#define kADDITION_HEIGHT		0
#define kADDITION_START_X		kCANVAS_START_X
#define kADDITION_START_Y		(kCANVAS_HEIGHT - kADDITION_HEIGHT)


#define kPIXEL_PER_METER_X		((kCANVAS_WIDTH - (kWATCH_WIDTH + kCANVAS_START_X * 2 + 16))/(kRULE_GOALLINE*2))
#define kPIXEL_PER_METER_Y		((kCANVAS_HEIGHT - (kCANVAS_START_Y + 32))/(kRULE_SIDELINE*2))
#define kPIXEL_PER_METER		(kPIXEL_PER_METER_X>kPIXEL_PER_METER_Y?kPIXEL_PER_METER_Y:kPIXEL_PER_METER_X)

#ifdef SSO_YETI
#define kPLAYER_RADIUS			1.5f
#define kPLAYER_NUMSIZE			2.7f
#define kPLAYER_DIRLINE			1.0f
#define kBALL_RADIUS			0.6f
#else
#define kPLAYER_RADIUS			(1.5f*0.6f)
#define kPLAYER_NUMSIZE			(2.7f*0.6f)
#define kPLAYER_DIRLINE			(1.0f*0.6f)
#define kBALL_RADIUS			(0.6f*0.6f)
#endif

#define kPLAYER_COUNT			5

#define kHOME_TEAM				0
#define kAWAY_TEAM				1

#define kFILTER_WATCH					0
#define kFILTER_BALL					1
#define kFILTER_TEAM_HOME_0				10
#define kFILTER_TEAM_HOME_1				11
#define kFILTER_TEAM_HOME_2				12
#define kFILTER_TEAM_HOME_3				13
#define kFILTER_TEAM_HOME_4				14
#define kFILTER_TEAM_HOME_5				15
#define kFILTER_TEAM_HOME_6				16
#define kFILTER_TEAM_HOME_7				17
#define kFILTER_TEAM_HOME_8				18
#define kFILTER_TEAM_HOME_9				19
#define kFILTER_TEAM_HOME_10			20
#define kFILTER_TEAM_AWAY_0				30
#define kFILTER_TEAM_AWAY_1				31
#define kFILTER_TEAM_AWAY_2				32
#define kFILTER_TEAM_AWAY_3				33
#define kFILTER_TEAM_AWAY_4				34
#define kFILTER_TEAM_AWAY_5				35
#define kFILTER_TEAM_AWAY_6				36
#define kFILTER_TEAM_AWAY_7				37
#define kFILTER_TEAM_AWAY_8				38
#define kFILTER_TEAM_AWAY_9				39
#define kFILTER_TEAM_AWAY_10			40

enum
{
	E_GUI_Button_Exit = 0,

	E_GUI_CheckBox_ShowDetailed,
	E_GUI_CheckBox_AlwaysTop,
	E_GUI_CheckBox_WatchOwner,
	E_Gui_SlidingBar_SlowMotion,
	E_Gui_Label_SlowMotion1X,
	E_Gui_Label_SlowMotion1D100X,
	E_GUI_ButtonOpen,
	E_GUI_ButtonSave,
	E_GUI_ButtonOpenLast,

	E_GUI_Total
};

enum ETabIndex
{
	E_Tab_Entity,
	E_Tab_Attributes,
	E_Tab_Statistics,
	E_Tab_Watch,
	E_Tab_Num,

	E_Tab_FormationEditor = E_Tab_Num,
};


typedef unsigned (__stdcall *PTHREAD_START) (void *);
#define BEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadID) \
		((HANDLE) _beginthreadex(	\
				(void*) (psa), \
				(unsigned) (cbStack), \
				(PTHREAD_START) (pfnStartAddr), \
				(void*) (pvParam), \
				(unsigned) (fdwCreate), \
				(unsigned*) (pdwThreadID)))

#endif