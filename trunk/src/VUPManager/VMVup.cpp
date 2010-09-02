#include "VMVup.h"

const VMVup::VupStatus VMVup::kStatus[EVupStatus_Num]	= {
	VMVup::VupStatus(EVupStatus_Invalid,	"Invalid"),
	VMVup::VupStatus(EVupStatus_Ready,		"Ready"),
	VMVup::VupStatus(EVupStatus_Running,	"Running"),
	VMVup::VupStatus(EVupStatus_Success,	"Success"),
	VMVup::VupStatus(EVupStatus_Failed,		"Failed")
};

const VMVup::VupStatus VMVup::kTestPhase[ETestPhase_Num]	= {
	VMVup::VupStatus(ETestPhase_INVALID,					"Invalid"					),
	VMVup::VupStatus(ETestPhase_PASSPORT_LOGIN,				"Passport Login"			),
	VMVup::VupStatus(ETestPhase_PASSPORT_LOGOUT,			"Passport Logout"			),
	VMVup::VupStatus(ETestPhase_CHARACTER_LOGIN,			"Character Login"			),
	VMVup::VupStatus(ETestPhase_CHARACTER_LOGOUT,			"Character Logout"			),
	VMVup::VupStatus(ETestPhase_REFRESH_CHARACTER,			"Refresh Character"			),
	VMVup::VupStatus(ETestPhase_CREATE_CHARACTER,			"Create Character"			),
	VMVup::VupStatus(ETestPhase_DELETE_CHARACTER,			"Delete Character"			),
	VMVup::VupStatus(ETestPhase_GET_LINE_PLAYERLIST,		"Get Line Playerlist"		),
	VMVup::VupStatus(ETestPhase_QUERY_CHARACTER_DATASET,	"Query Character Dataset"	),
	VMVup::VupStatus(ETestPhase_SEND_INSTANT,           	"Send Instance"				),
	VMVup::VupStatus(ETestPhase_GET_LOBBY_INFO,         	"Get Lobby Info"			),
	VMVup::VupStatus(ETestPhase_CREATE_ROOM,            	"Create Room"				),
	VMVup::VupStatus(ETestPhase_JOIN_ROOM,             		"Join Room"					),
	VMVup::VupStatus(ETestPhase_QUIT_ROOM,              	"Quit Room"					),
	VMVup::VupStatus(ETestPhase_SET_READY,              	"Set Ready"					),
	VMVup::VupStatus(ETestPhase_LISTEN_TO_LOADING,      	"Listen to Loading"			),
	VMVup::VupStatus(ETestPhase_GET_ROOM_INFO,          	"Get Room Info"				),
	VMVup::VupStatus(ETestPhase_LISTEN_TO_BACK_LOBBY,   	"Listen to Back Lobby"		),
	VMVup::VupStatus(ETestPhase_START_GAME,             	"Start Game"				),
	VMVup::VupStatus(ETestPhase_SEND_GAME_RESULT,       	"Send Game Result"			),
	VMVup::VupStatus(ETestPhase_LISTEN_TO_GAME_END,    		"Listen to Game End"		),
	VMVup::VupStatus(ETestPhase_SLEEP_OP,               	"Operation(Sleep)"			),
	VMVup::VupStatus(ETestPhase_IN_GAME_OP,             	"Operation(In Game)"		),
	VMVup::VupStatus(ETestPhase_ON_TIMEOUT_OP,          	"Operation(On Timeout)"		),
	VMVup::VupStatus(ETestPhase_CONTROL_OP,             	"Operation(Control)"		),
	VMVup::VupStatus(ETestPhase_RDV_POINT,              	"RDV Point"					),
	VMVup::VupStatus(ETestPhase_GET_GAME_RESULT,        	"Get Game Result"			)
};

VMVup::VMVup(s32 _id, const Char* _ipAddr, u16 _port)
	: m_iUniqueID(_id)
	, m_strIPAddress(_ipAddr)
	, m_uiPort(_port)
	, m_uiCurrentStatus(EVupStatus_Invalid)
	, m_uiCurrentTestPhase(ETestPhase_INVALID)
	, m_iGroup(-1)
{
}

VMVup& VMVup::operator = (const VMVup& _rhs)
{
	if(this != &_rhs &&
	   m_iUniqueID == _rhs.m_iUniqueID)
	{
		m_uiCurrentStatus = _rhs.m_uiCurrentStatus;
	}
	return *this;
}