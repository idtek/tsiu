#include "VMVup.h"

const VMVup::VupStatus VMVup::kStatus[EVupStatus_Num]	= {
	VMVup::VupStatus(EVupStatus_Invalid,	"1.Invalid"),
	VMVup::VupStatus(EVupStatus_Ready,		"1.Ready"),
	VMVup::VupStatus(EVupStatus_Running,	"1.Running"),
	VMVup::VupStatus(EVupStatus_Success,	"1.Success"),
	VMVup::VupStatus(EVupStatus_Failed,		"1.Failed")
};

const VMVup::VupStatus VMVup::kTestPhase[ETestPhase_Num]	= {
	VMVup::VupStatus(ETestPhase_INVALID,					"2.Invalid"					),
	VMVup::VupStatus(ETestPhase_PASSPORT_LOGIN,				"2.Passport Login"			),
	VMVup::VupStatus(ETestPhase_PASSPORT_LOGOUT,			"2.Passport Logout"			),
	VMVup::VupStatus(ETestPhase_CHARACTER_LOGIN,			"2.Character Login"			),
	VMVup::VupStatus(ETestPhase_CHARACTER_LOGOUT,			"2.Character Logout"		),
	VMVup::VupStatus(ETestPhase_REFRESH_CHARACTER,			"2.Refresh Character"		),
	VMVup::VupStatus(ETestPhase_CREATE_CHARACTER,			"2.Create Character"		),
	VMVup::VupStatus(ETestPhase_DELETE_CHARACTER,			"2.Delete Character"		),
	VMVup::VupStatus(ETestPhase_GET_LINE_PLAYERLIST,		"2.Get Line Playerlist"		),
	VMVup::VupStatus(ETestPhase_QUERY_CHARACTER_DATASET,	"2.Query Character Dataset"	),
	VMVup::VupStatus(ETestPhase_SEND_INSTANT,           	"2.Send Instance"			),
	VMVup::VupStatus(ETestPhase_GET_LOBBY_INFO,         	"2.Get Lobby Info"			),
	VMVup::VupStatus(ETestPhase_CREATE_ROOM,            	"2.Create Room"				),
	VMVup::VupStatus(ETestPhase_JOIN_ROOM,             		"2.Join Room"				),
	VMVup::VupStatus(ETestPhase_QUIT_ROOM,              	"2.Quit Room"				),
	VMVup::VupStatus(ETestPhase_SET_READY,              	"2.Set Ready"				),
	VMVup::VupStatus(ETestPhase_LISTEN_TO_LOADING,      	"2.Listen to Loading"		),
	VMVup::VupStatus(ETestPhase_GET_ROOM_INFO,          	"2.Get Room Info"			),	
	VMVup::VupStatus(ETestPhase_LISTEN_TO_BACK_LOBBY,   	"2.Listen to Back Lobby"	),
	VMVup::VupStatus(ETestPhase_START_GAME,             	"2.Start Game"				),
	VMVup::VupStatus(ETestPhase_SEND_GAME_RESULT,       	"2.Send Game Result"		),
	VMVup::VupStatus(ETestPhase_LISTEN_TO_GAME_END,    		"2.Listen to Game End"		),
	VMVup::VupStatus(ETestPhase_SLEEP_OP,               	"2.Op(Sleep)"				),
	VMVup::VupStatus(ETestPhase_IN_GAME_OP,             	"2.Op(In Game)"				),
	VMVup::VupStatus(ETestPhase_ON_TIMEOUT_OP,          	"2.Op(On Timeout)"			),
	VMVup::VupStatus(ETestPhase_CONTROL_OP,             	"2.Op(Control)"				),
	VMVup::VupStatus(ETestPhase_RDV_POINT,              	"2.RDV Point"				),
	VMVup::VupStatus(ETestPhase_GET_GAME_RESULT,        	"2.Get Game Result"			)
};

s32 VMVup::kStatusSummary[EVupStatus_Num] = {0};
s32 VMVup::kTestPhaseSummary[ETestPhase_Num] = {0};
std::map<std::string, s32> VMVup::kIpSummary;

VMVup::VMVup(s32 _id, const Char* _ipAddr, u16 _port)
	: m_iUniqueID(_id)
	, m_strIPAddress(_ipAddr)
	, m_uiPort(_port)
	, m_uiCurrentStatus(EVupStatus_Invalid)
	, m_uiCurrentTestPhase(ETestPhase_INVALID)
	, m_iGroup(-1)
{
	kStatusSummary[m_uiCurrentStatus]++;
	kTestPhaseSummary[m_uiCurrentTestPhase]++;

	std::map<std::string, s32>::iterator it = kIpSummary.find(_ipAddr);
	if(it == kIpSummary.end())
	{
		kIpSummary.insert(std::pair<std::string, s32>(_ipAddr, 1));
	}
	else
	{
		(*it).second++;
	}
}

VMVup::~VMVup()
{
	kStatusSummary[m_uiCurrentStatus]--;
	kTestPhaseSummary[m_uiCurrentTestPhase]--;

	std::map<std::string, s32>::iterator it = kIpSummary.find(m_strIPAddress);
	if(it != kIpSummary.end())
	{
		(*it).second--;
	}
}

void VMVup::SetStatus(u8 _status)
{
	if(_status >= EVupStatus_Num)
	{
		D_CHECK(0);
	}
	else
	{
		m_uiLastStatus = m_uiCurrentStatus;
		kStatusSummary[m_uiLastStatus]--;

		m_uiCurrentStatus = _status;
		kStatusSummary[m_uiCurrentStatus]++;
	}
}
void VMVup::SetTestPhase(u8 _phase)
{
	if(_phase >= ETestPhase_Num)
	{
		D_CHECK(0);
	}
	else
	{
		m_uiLastTestPhase = m_uiCurrentTestPhase;
		kTestPhaseSummary[m_uiLastTestPhase]--;

		m_uiCurrentTestPhase = _phase;
		kTestPhaseSummary[m_uiCurrentTestPhase]++;
	}
}