#ifndef __VM_VUP_H__
#define __VM_VUP_H__

#include "VMGlobalDef.h"

class VMVup
{
public:
	class VupStatus
	{
	public:
		enum EVupStatus
		{
			EVupStatus_Invalid,
			EVupStatus_Ready,
			EVupStatus_Running,
			EVupStatus_Success,
			EVupStatus_Failed,

			EVupStatus_Num
		};

		VupStatus()
			: m_eStatus(EVupStatus_Invalid)
			, m_strStatusName()
		{}
		VupStatus(EVupStatus _status, StringPtr _name)
			: m_eStatus(_status)
			, m_strStatusName(_name)
		{}
		StringPtr	GetName()	const {	return m_strStatusName.c_str();	}
		EVupStatus	GetStatus() const {	return m_eStatus;				}
	
		friend class VMVup;

	private:
		EVupStatus	m_eStatus;
		std::string m_strStatusName;
	};

public:
	static s32 GenerateUniqueID(){
		static s32 i = 0;
		return i++;
	}
	static const VupStatus kStatus[VupStatus::EVupStatus_Num];

public:
	VMVup(s32 _id);
	VMVup(s32 _id, VupStatus::EVupStatus _status);

	VMVup& operator = (const VMVup& _rhs);
	s32 GetUniqueID() const{
		return m_iUniqueID;
	}
	VupStatus::EVupStatus GetCurrentStatus() const{
		return m_eCurrentStatus;
	}

private:
	s32						m_iUniqueID;
	VupStatus::EVupStatus	m_eCurrentStatus;
};

#endif
