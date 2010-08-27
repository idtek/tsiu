#include "VMVupManager.h"
#include "VMVup.h"

//--------------------------------------------------------------------------------------------
s32 VMVupManager::AddVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2)
{
	VMVup* newVup = new VMVup(VMVup::GenerateUniqueID());
	Bool bRet = VMVupManager::GetPtr()->AddVup(newVup);
	if(!bRet)
	{
		delete newVup;
	}
	return 0;
}

s32 VMVupManager::UpdateVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2)
{
	s32								vupID		= _p1.ToInt();
	VMVup::VupStatus::EVupStatus	vupStatus	= static_cast<VMVup::VupStatus::EVupStatus>(_p2.ToInt());
	VMVup* newVup = new VMVup(vupID);
	Bool bRet = VMVupManager::GetPtr()->AddVup(newVup);
	if(!bRet)
	{
		delete newVup;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
void VMVupManager::Create()
{
	VMCommandCenter::GetPtr()->RegisterCommand("addvup",	VMVupManager::AddVup);
	VMCommandCenter::GetPtr()->RegisterCommand("updatevup", VMVupManager::UpdateVup, VMCommand::EParamType_Int, VMCommand::EParamType_Int);
}

Bool VMVupManager::AddVup(VMVup* _newVUP)
{
	VMVup* vup = FindVup(_newVUP->GetUniqueID());
	if(vup)
	{
		*vup = *_newVUP;
		return false;
	}
	else
	{
		m_poVupMap.insert(std::pair<s32, VMVup*>(_newVUP->GetUniqueID(), _newVUP));
	}
	return true;
}

VMVup* VMVupManager::FindVup(s32 _id)
{
	VUPMapIterator it = m_poVupMap.find(_id);
	if(it == m_poVupMap.end())
	{
		return NULL;
	}
	return (*it).second;
}
const VMVup* VMVupManager::FindVup(s32 _id) const
{
	VUPMapConstIterator it = m_poVupMap.find(_id);
	if(it == m_poVupMap.end())
	{
		return NULL;
	}
	return (*it).second;
}