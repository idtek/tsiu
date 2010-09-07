#ifndef __VM_VUP_H__
#define __VM_VUP_H__

#include "VMGlobalDef.h"
#include "VMProtocal.h"

class VMVup
{
public:
	class VupStatus
	{
	public:
		VupStatus()
			: m_uiStatus(0)
			, m_strStatusName()
		{}
		VupStatus(u8 _status, StringPtr _name)
			: m_uiStatus(_status)
			, m_strStatusName(_name)
		{}
		StringPtr	GetName()	const {	return m_strStatusName.c_str();	}
		u8			GetStatus() const {	return m_uiStatus;				}
	
		friend class VMVup;

	private:
		u8			m_uiStatus;
		std::string m_strStatusName;
	};

public:
	static s32 GenerateUniqueID(){
		static s32 i = 0;
		return i++;
	}
	static const VupStatus kStatus[EVupStatus_Num];
	static const VupStatus kTestPhase[ETestPhase_Num];
	static s32 kStatusSummary[EVupStatus_Num];
	static s32 kTestPhaseSummary[ETestPhase_Num];
	static std::map<std::string, s32> kIpSummary;

public:
	VMVup(s32 _id, const Char* _ipAddr, u16 _port);
	~VMVup();

	D_Inline s32		GetUniqueID()			const	{	return m_iUniqueID;				}
	D_Inline u8			GetCurrentStatus()		const	{	return m_uiCurrentStatus;		}
	D_Inline u16		GetPort()				const	{	return m_uiPort;				}
	D_Inline StringPtr	GetIPAddress()			const	{	return m_strIPAddress.c_str();	}
	D_Inline u8			GetCurrentTestPhase()	const	{	return m_uiCurrentTestPhase;	}
	D_Inline s32		GetGroup()				const	{	return m_iGroup;				}
	D_Inline s32		GetRDVPointID()			const	{	return m_iRDVPointID;			}
	D_Inline u8			GetLastStaus()			const	{	return m_uiLastStatus;			}
	D_Inline u8			GetLastTestPhase()		const	{	return m_uiLastTestPhase;		}

	D_Inline void		SetUniqueID(s32 _uid)
	{
		m_iUniqueID = _uid;
	}
	D_Inline void		SetGroup(s32 _group)
	{	
		m_iGroup = _group;
	}
	D_Inline void		SetRDVPointID(s32 _rdvid)
	{	
		m_iRDVPointID = _rdvid;
	}

	void SetStatus(u8 _status);
	void SetTestPhase(u8 _phase);
	
#ifdef USE_UDT_LIB
	D_Inline void SetClientSocket(UDTSOCKET _pSocket)
	{
		m_ClientSocket = _pSocket;
	}
	D_Inline UDTSOCKET GetClientSocket() const
	{
		return m_ClientSocket;
	}
#endif

private:
	s32				m_iUniqueID;
	u8				m_uiCurrentStatus;
	u8				m_uiCurrentTestPhase;
	u8				m_uiLastStatus;
	u8				m_uiLastTestPhase;
	u16				m_uiPort;
	std::string		m_strIPAddress;
	s32				m_iGroup;
	s32				m_iRDVPointID;

#ifdef USE_UDT_LIB
	UDTSOCKET		m_ClientSocket;
#endif
};

#endif
