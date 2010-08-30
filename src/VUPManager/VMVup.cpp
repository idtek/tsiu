#include "VMVup.h"

const VMVup::VupStatus VMVup::kStatus[EVupStatus_Num]	= {
	VMVup::VupStatus(EVupStatus_Invalid,	"Invalid"),
	VMVup::VupStatus(EVupStatus_Ready,		"Ready"),
	VMVup::VupStatus(EVupStatus_Running,	"Running"),
	VMVup::VupStatus(EVupStatus_Success,	"Success"),
	VMVup::VupStatus(EVupStatus_Failed,		"Failed")
};

VMVup::VMVup(s32 _id, const Char* _ipAddr, u16 _port)
	: m_iUniqueID(_id)
	, m_strIPAddress(_ipAddr)
	, m_uiPort(_port)
	, m_uiCurrentStatus(EVupStatus_Invalid)
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