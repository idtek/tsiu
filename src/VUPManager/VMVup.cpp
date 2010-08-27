#include "VMVup.h"

const VMVup::VupStatus VMVup::kStatus[VMVup::VupStatus::EVupStatus_Num]	= {
	VMVup::VupStatus(VMVup::VupStatus::EVupStatus_Invalid,	"Invalid"),
	VMVup::VupStatus(VMVup::VupStatus::EVupStatus_Ready,	"Ready"),
	VMVup::VupStatus(VMVup::VupStatus::EVupStatus_Running,	"Running"),
	VMVup::VupStatus(VMVup::VupStatus::EVupStatus_Success,	"Success"),
	VMVup::VupStatus(VMVup::VupStatus::EVupStatus_Failed,	"Failed")
};

VMVup::VMVup(s32 _id)
	: m_iUniqueID(_id)
	, m_eCurrentStatus(VupStatus::EVupStatus_Invalid)
{
}

VMVup::VMVup(s32 _id, VupStatus::EVupStatus _status)
	: m_iUniqueID(_id)
	, m_eCurrentStatus(_status)
{

}

VMVup& VMVup::operator = (const VMVup& _rhs)
{
	if(this != &_rhs &&
	   m_iUniqueID == _rhs.m_iUniqueID)
	{
		m_eCurrentStatus = _rhs.m_eCurrentStatus;
	}
	return *this;
}