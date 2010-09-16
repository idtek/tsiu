#ifndef __VM_PROTOCAL_H__
#define __VM_PROTOCAL_H__

//Packet Type
enum
{
	EPT_C2M_ClientRegister = 0,
	EPT_C2M_ReportClientRunningStatus,
	EPT_C2M_ReportClientTesingPhase,
	EPT_C2M_ReachRDVPoint,
	EPT_C2M_LostConnection,

	EPT_M2C_ClientRegisterACK,
	EPT_M2C_StartTesting,
	EPT_M2C_KillClient,
	EPT_M2C_Refresh,

	EPT_Num
};

enum
{
	EVupStatus_Invalid,
	EVupStatus_Ready,
	EVupStatus_Running,
	EVupStatus_Success,
	EVupStatus_Failed,

	EVupStatus_Num
};

enum
{
	ETestPhase_INVALID							= 0,
	ETestPhase_PASSPORT_LOGIN					= 1,
	ETestPhase_PASSPORT_LOGOUT					= 2,
	ETestPhase_CHARACTER_LOGIN					= 3,
	ETestPhase_CHARACTER_LOGOUT					= 4,
	ETestPhase_REFRESH_CHARACTER				= 5,
	ETestPhase_CREATE_CHARACTER					= 6,
	ETestPhase_DELETE_CHARACTER					= 7,
	ETestPhase_GET_LINE_PLAYERLIST				= 8,
	ETestPhase_QUERY_CHARACTER_DATASET 			= 9,
	ETestPhase_SEND_INSTANT            			= 10,
	ETestPhase_GET_LOBBY_INFO          			= 11,
	ETestPhase_CREATE_ROOM             			= 12,
	ETestPhase_JOIN_ROOM               			= 13,
	ETestPhase_QUIT_ROOM               			= 14,
	ETestPhase_SET_READY               			= 15,
	ETestPhase_LISTEN_TO_LOADING       			= 16,
	ETestPhase_GET_ROOM_INFO           			= 17,
	ETestPhase_LISTEN_TO_BACK_LOBBY    			= 18,
	ETestPhase_START_GAME              			= 19,
	ETestPhase_SEND_GAME_RESULT        			= 20,
	ETestPhase_LISTEN_TO_GAME_END      			= 21,
	ETestPhase_SLEEP_OP                			= 22,
	ETestPhase_IN_GAME_OP              			= 23,
	ETestPhase_ON_TIMEOUT_OP           			= 24,
	ETestPhase_CONTROL_OP              			= 25,
	ETestPhase_RDV_POINT               			= 26,
	ETestPhase_GET_GAME_RESULT         			= 27,

	ETestPhase_Num
};

//Macro
#define kNAME_ReportClientRunningStatus_Phase	"ClientRunningStatus"
#define kNAME_ReportClientTesingPhase_Phase		"ClientTestingPhase"

//Packet
struct UDP_PACK
{
	unsigned char m_uiType;
	union
	{
		struct{
			int				m_uiPassPort;
			unsigned short	m_uiPort;
			unsigned char	m_uiStatus;
			unsigned char	m_uiTestPhase;
		}m_ClientRegisterParam;
		struct{
			int				m_uiPassPort;
			unsigned char	m_uiStatus;
		}m_ReportClientRunningStatusParam;
		struct{
			int				m_uiPassPort;
			unsigned char	m_uiPhase;
		}m_ReportClientTesingPhaseParam;
		struct{
			int				m_uiPassPort;
			unsigned short	m_uiRDVPointID;
			unsigned short	m_uiExpected;
			unsigned short	m_uiTimeout;
		}m_ReachRDVPointParam;
		struct{
			__int64			m_uiBurstTime;
		}m_StartTestingParam;
		struct{
			bool			m_uiHasSuccessed;
		}m_RegisterAckParam;
	}m_unValue;
};

namespace Protocal
{
	static const unsigned short kInvalidRDVPoint = 0xffff;

	inline unsigned short GetRDVPointMajor(unsigned short _rdvPoint)
	{
		return _rdvPoint / 1000;
	}
	inline unsigned short GetRDVPointMinor(unsigned short _rdvPoint)
	{
		return _rdvPoint % 1000;
	}
	inline unsigned short GetRDVPointID(unsigned short _majorid, unsigned short _minorid )
	{
		return _majorid * 1000 + _minorid;
	}
}

#endif