#include "VMVup.h"
#include "VMSummary.h"

const VMVup::VupStatus VMVup::kStatus[EVupStatus_Num]	= {
	VMVup::VupStatus(EVupStatus_Invalid,	"1.Invalid"	),
	VMVup::VupStatus(EVupStatus_Ready,		"1.Ready"	),
	VMVup::VupStatus(EVupStatus_Running,	"1.Running"	),
	VMVup::VupStatus(EVupStatus_Success,	"1.Success"	),
	VMVup::VupStatus(EVupStatus_Failed,		"1.Failed"	)
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
	VMVup::VupStatus(ETestPhase_SLEEP_OP,               	"2.Sleep"					),
	VMVup::VupStatus(ETestPhase_IN_GAME_OP,             	"2.In Game"					),
	VMVup::VupStatus(ETestPhase_ON_TIMEOUT_OP,          	"2.On Timeout"				),
	VMVup::VupStatus(ETestPhase_CONTROL_OP,             	"2.Waiting"					),
	VMVup::VupStatus(ETestPhase_RDV_POINT,              	"2.RDV Point"				),
	VMVup::VupStatus(ETestPhase_GET_GAME_RESULT,        	"2.Get Game Result"			)
};

VMVup::VMVup(const Char* _ipAddr, u16 _port)
	: m_iUniqueID(-1)
	, m_strIPAddress(_ipAddr)
	, m_uiPort(_port)
	, m_uiCurrentStatus(EVupStatus_Invalid)
	, m_uiCurrentTestPhase(ETestPhase_INVALID)
	, m_iGroup(-1)
	, m_iRDVPointID(Protocal::kInvalidRDVPoint)
	, m_uiLastStatus(EVupStatus_Invalid)
	, m_uiLastTestPhase(ETestPhase_INVALID)
	, m_strViewKey()
#ifdef USE_UDT_LIB
	, m_ClientSocket(UDT::INVALID_SOCK)
#endif
{
}

VMVup::~VMVup()
{
	if(HasRegistered())
	{
		//Update Summary
		VMSummary::SummaryUpdateParam suParam;
		suParam.m_UpdateType = VMSummary::SummaryUpdateParam::ESummaryUpdateType_VUP |
							   VMSummary::SummaryUpdateParam::ESummaryUpdateType_Status |
							   VMSummary::SummaryUpdateParam::ESummaryUpdateType_TestPhase;
		suParam.m_AddorRemove = false;
		suParam.m_LastStatus = m_uiCurrentStatus;
		suParam.m_CurStatus = -1;
		suParam.m_LastTestPhase = m_uiCurrentTestPhase;
		suParam.m_CurTestPhase = -1;
		VMSummary::GetPtr()->UpdateSummary(m_strIPAddress.c_str(), suParam);

		//Update Summary
		VMSummary::GroupUpdateParam guParam;
		guParam.m_AddorRemove = false;
		guParam.m_MaxNumberInGroup = 0;		//ignore
		guParam.m_MyGroup = m_iGroup;
		guParam.m_VUPsPassport = m_iUniqueID;
		VMSummary::GetPtr()->UpdateGroupInfo(Protocal::GetRDVPointMajor(GetRDVPointID()), Protocal::GetRDVPointMinor(GetRDVPointID()), 0, guParam);
	}
#ifdef USE_UDT_LIB
	if(m_ClientSocket != UDT::INVALID_SOCK)
		UDT::close(m_ClientSocket);
#endif
}

void VMVup::SetStatus(u8 _status)
{
	if(_status >= EVupStatus_Num || _status < 0)
	{
		D_CHECK(0);
	}
	else
	{
		D_CHECK(HasRegistered());

		if(m_uiCurrentStatus != _status)
		{
			m_uiLastStatus = m_uiCurrentStatus;
			m_uiCurrentStatus = _status;

			//Update Summary
			VMSummary::SummaryUpdateParam suParam;
			suParam.m_UpdateType = VMSummary::SummaryUpdateParam::ESummaryUpdateType_Status;
			suParam.m_LastStatus = m_uiLastStatus;
			suParam.m_CurStatus = m_uiCurrentStatus;
			VMSummary::GetPtr()->UpdateSummary(m_strIPAddress.c_str(), suParam);

			Event evtGroup((EventType_t)E_ET_VUPInfoUpdate);
			evtGroup.AddParam((void*)this);
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtGroup);
		}
	}
}
void VMVup::SetTestPhase(u8 _phase)
{
	if(_phase >= ETestPhase_Num || _phase < 0)
	{
		D_CHECK(0);
	}
	else
	{
		D_CHECK(HasRegistered());

		if(m_uiCurrentStatus != _phase)
		{
			m_uiLastTestPhase = m_uiCurrentTestPhase;
			m_uiCurrentTestPhase = _phase;

			//Update Summary
			VMSummary::SummaryUpdateParam suParam;
			suParam.m_UpdateType = VMSummary::SummaryUpdateParam::ESummaryUpdateType_TestPhase;
			suParam.m_LastTestPhase = m_uiLastTestPhase;
			suParam.m_CurTestPhase = m_uiCurrentTestPhase;
			VMSummary::GetPtr()->UpdateSummary(m_strIPAddress.c_str(), suParam);

			Event evtGroup((EventType_t)E_ET_VUPInfoUpdate);
			evtGroup.AddParam((void*)this);
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtGroup);
		}
	}
}
void VMVup::SetUniqueID(s32 _uid)
{
	m_iUniqueID = _uid;

	//Update Summary
	VMSummary::SummaryUpdateParam suParam;
	suParam.m_UpdateType = VMSummary::SummaryUpdateParam::ESummaryUpdateType_VUP;
	suParam.m_AddorRemove = true;
	VMSummary::GetPtr()->UpdateSummary(m_strIPAddress.c_str(), suParam);
}
void VMVup::SetGroup(s32 _group)
{	
	D_CHECK(HasRegistered());
	m_iGroup = _group;

	Event evtGroup((EventType_t)E_ET_VUPInfoUpdate);
	evtGroup.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtGroup);
}
void VMVup::SetRDVPointID(u16 _rdvid)
{	
	D_CHECK(HasRegistered());

	m_iRDVPointID = _rdvid;

	Event evtGroup((EventType_t)E_ET_VUPInfoUpdate);
	evtGroup.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtGroup);
}

void VMVup::SetViewKey(StringPtr _viewKey)
{
	D_CHECK(HasRegistered());

	m_strViewKey = _viewKey;
}