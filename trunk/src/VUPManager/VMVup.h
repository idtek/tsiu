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
			: m_uiStatus(EVupStatus_Invalid)
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

public:
	VMVup(s32 _id, const Char* _ipAddr, u16 _port);

	VMVup& operator = (const VMVup& _rhs);
	s32 GetUniqueID() const{
		return m_iUniqueID;
	}
	u8 GetCurrentStatus() const{
		return m_uiCurrentStatus;
	}
	u16 GetPort() const{
		return m_uiPort;
	}
	const Char* GetIPAddress() const{
		return m_strIPAddress.c_str();
	}
	void SetStatus(u8 _status){
		m_uiCurrentStatus = _status;
	}

private:
	s32				m_iUniqueID;
	u8				m_uiCurrentStatus;
	u16				m_uiPort;
	std::string		m_strIPAddress;
};

#endif
