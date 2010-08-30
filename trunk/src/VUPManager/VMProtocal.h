#ifndef __VM_PROTOCAL_H__
#define __VM_PROTOCAL_H__

//Packet Type
enum
{
	EPT_C2M_ClientRegister = 0,
	EPT_C2M_ReportClientStatus,
	
	EPT_M2C_StartTesting,
	EPT_M2C_KillClient,
	EPT_M2C_Refresh,
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

//Packet
struct UDP_PACK
{
	u8	m_uiType;
	union
	{
		struct{
			u32 m_uiPassPort;
			u16 m_uiPort;
			u8  m_uiStatus;
		}m_ClientRegisterParam;
		struct{
			u32 m_uiPassPort;
			u8  m_uiStatus;
		}m_ReportClientStatusParam;
	}m_unValue;
};

#endif