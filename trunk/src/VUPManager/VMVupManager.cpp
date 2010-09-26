#include "VMVupManager.h"
#include "VMVup.h"
#include "tinyxml.h"
#include <time.h>
#include <sys/timeb.h>
#include <algorithm>
#include "VMSummary.h"
#include <sstream>
#include <fstream>
#include <iostream>

//--------------------------------------------------------------------------
#ifndef USE_UDT_LIB
RecvUDPRunner::RecvUDPRunner(Socket* _pRecvSock, MemPool<UDP_PACKWrapper>* _pMempool)
	:m_pRecvSocket(_pRecvSock)
	,m_pUDPPackBuffer(_pMempool)
	,m_bRequestStop(false)
{
}

u32 RecvUDPRunner::Run()
{
	while(1)
	{
		if(m_bRequestStop)
			return 0;

		if(!m_pRecvSocket || !m_pRecvSocket->bIsValid())
			return 1;

		UDP_PACKWrapper pack;
		s32 iRet = m_pRecvSocket->RecvFrom((Char*)&pack.m_InnerData, sizeof(UDP_PACK));
		if(!iRet)
		{
			pack.m_SrcIPAddress = m_pRecvSocket->GetIPAddress();
			pack.m_SrcPort = m_pRecvSocket->GetPort();
			m_pUDPPackBuffer->InsertUDPData(pack);
		}
	}
}

void RecvUDPRunner::NotifyQuit()
{
	m_bRequestStop = true;
}
#endif

//--------------------------------------------------------------------------------------------
s32 VMVupManager::AddVup(const VMCommand::ParamList& _paramList)
{
	s32 iPassport = _paramList[0].ToInt();
	const Char* strIPAddr = _paramList[1].ToString();
	u16 uiPort = static_cast<u16>(_paramList[2].ToInt());

	VMVup* newVup = new VMVup(strIPAddr, uiPort);
	newVup->SetUniqueID(iPassport);
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	Bool bRet = pMan->AddVup(newVup);
	if(!bRet)
	{
		return 1;
	}
	return 0;
}

s32 VMVupManager::UpdateVup(const VMCommand::ParamList& _paramList)
{
	s32	iPassport = _paramList[0].ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	VMVup* newVup = pMan->FindVup(iPassport);
	if(!newVup)
		return 1;

	s32 optionCount = (_paramList.Size() - 1) / 2;
	for(s32 i = 0;  i < optionCount; ++i)
	{
		const Char* pOption = _paramList[1 + i * 2].ToString();
		if(!strcmp(pOption , "-rs"))
		{
			u8	uiStatus = static_cast<u8>(_paramList[2 + i * 2].ToInt());
			newVup->SetStatus(uiStatus);
		}
		else if(!strcmp(pOption , "-tp"))
		{
			u8	uiPhase = static_cast<u8>(_paramList[2 + i * 2].ToInt());
			newVup->SetTestPhase(uiPhase);
		}
	}
	return 0;
}

s32 VMVupManager::RemoveVup(const VMCommand::ParamList& _paramList)
{
	s32	iPassport = _paramList[0].ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->RemoveVup(iPassport);
	return 0;
}

s32 VMVupManager::Refresh(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->Refresh();
	return 0;
}

s32 VMVupManager::KillClient(const VMCommand::ParamList& _paramList)
{
	s32	iPassport = _paramList[0].ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->KillClient(iPassport);

	return 0;
}
s32 VMVupManager::SetParameter(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	s32 optionCount = _paramList.Size() / 2;
	for(s32 i = 0;  i < optionCount; ++i)
	{
		pMan->SetParameter(_paramList[i * 2 + 0].ToString(), _paramList[i * 2 + 1]);
	}
	return 0;
}

s32 VMVupManager::GetParameter(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	s32 optionCount = _paramList.Size();
	if(optionCount == 0)
	{
		pMan->GetParameter(NULL);
	}
	else
	{
		for(s32 i = 0;  i < optionCount; ++i)
		{
			pMan->GetParameter(_paramList[i].ToString());
		}
	}
	return 0;
}

s32 VMVupManager::SetRDVParameter(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	RDVPointParameter* pParam = pMan->AddRDVParam(_paramList[0].ToInt(), 0);//_paramList[1].ToInt());
	if(!pParam)
		return 1;

	s32 optionCount = (_paramList.Size() - 2) / 2;
	for(s32 i = 0;  i < optionCount; ++i)
	{
		pMan->SetRDVParameter(*pParam, _paramList[i * 2 + 2].ToString(), _paramList[i * 2 + 3]);
	}
	return 0;
}

s32 VMVupManager::GetRDVParameter(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	RDVPointParameter* pParam = pMan->FindRDVParam(_paramList[0].ToInt(), 0);//_paramList[1].ToInt());
	if(!pParam)
		return 1;

	LOG_INFO("RDV Point(%d)\n", _paramList[0].ToInt());//, _paramList[1].ToInt());
	s32 optionCount = _paramList.Size() - 2;
	if(optionCount == 0)
	{
		pMan->GetRDVParameter(*pParam, NULL);
	}
	else
	{
		for(s32 i = 0;  i < optionCount; ++i)
		{
			pMan->GetRDVParameter(*pParam, _paramList[i + 2].ToString());
		}
	}
	return 0;
}

s32 VMVupManager::FindVUP(const VMCommand::ParamList& _paramList)
{
	s32 iPassport = _paramList[0].ToInt();

	Event evtGroup((EventType_t)E_ET_FindVUP);
	evtGroup.AddParam(iPassport);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtGroup);

	return 0;
}

s32 VMVupManager::Sort(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->SetSortOrder(0, NULL);
	pMan->SetSortUpOrDwon(_paramList[0].ToString());
	s32 optionCount = _paramList.Size() - 1;
	for(s32 i = 0;  i < optionCount; ++i)
	{
		pMan->SetSortOrder(i, _paramList[i + 1].ToString());
	}
	pMan->RefreshViewMap();
	return 0;
}
s32 VMVupManager::Filter(const VMCommand::ParamList& _paramList)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->SetFilter(NULL, NULL);
	s32 optionCount = _paramList.Size() / 2;
	for(s32 i = 0;  i < optionCount; ++i)
	{
		pMan->SetFilter(_paramList[i * 2].ToString(), _paramList[i * 2 + 1].ToString());
	}
	return 0;
}

s32 VMVupManager::LogRequest(const VMCommand::ParamList& _paramList)
{
	s32	iPassport = _paramList[0].ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->RequestLog(iPassport);

	return 0;
}

//--------------------------------------------------------------------------------------------
VMVupManager::VMVupManager()
#ifndef USE_UDT_LIB
	: m_pRecvThread(NULL)
	, m_pRecvSocket(NULL)
	, m_pSendSocket(NULL)
	, m_pUDPPackBuffer(NULL)
#else 
	: m_pUDPPackBuffer(NULL)  
#endif
{

#ifdef USE_UDT_LIB
	UDT::startup();
#endif

	VMCommandCenter::GetPtr()->RegisterCommand("addvup",		VMVupManager::AddVup,		3);
	VMCommandCenter::GetPtr()->RegisterCommand("updatevup",		VMVupManager::UpdateVup,	3);
	VMCommandCenter::GetPtr()->RegisterCommand("removevup",		VMVupManager::RemoveVup,	1);
	VMCommandCenter::GetPtr()->RegisterCommand("refresh",		VMVupManager::Refresh,		0);
	VMCommandCenter::GetPtr()->RegisterCommand("kill",			VMVupManager::KillClient,	1);

	VMCommand::ParamList defaultParam;
	defaultParam.PushBack(VMCommandParamHolder("-1"));
	VMCommandCenter::GetPtr()->RegisterCommand("killall",		VMVupManager::KillClient,	1, &defaultParam);

	VMCommandCenter::GetPtr()->RegisterCommand("setp",			VMVupManager::SetParameter,		2);
	VMCommandCenter::GetPtr()->RegisterCommand("getp",			VMVupManager::GetParameter,		0);
	VMCommandCenter::GetPtr()->RegisterCommand("setrdvp",		VMVupManager::SetRDVParameter,	4);
	VMCommandCenter::GetPtr()->RegisterCommand("getrdvp",		VMVupManager::GetRDVParameter,	2);
	VMCommandCenter::GetPtr()->RegisterCommand("find",			VMVupManager::FindVUP,			1);
	VMCommandCenter::GetPtr()->RegisterCommand("sort",			VMVupManager::Sort,				1);
	VMCommandCenter::GetPtr()->RegisterCommand("filter",		VMVupManager::Filter,			0);
	VMCommandCenter::GetPtr()->RegisterCommand("getlog",		VMVupManager::LogRequest,		1);
}

VMVupManager::~VMVupManager()
{
#ifndef USE_UDT_LIB
	m_pRecvThread->Stop(1000);
	D_SafeDelete(m_pRecvThread);

	m_pRecvSocket->Destroy();
	D_SafeDelete(m_pRecvSocket);

	m_pSendSocket->Destroy();
	D_SafeDelete(m_pSendSocket);
#else
	m_ListeningThread->Stop(1000);
	D_SafeDelete(m_ListeningThread);

	m_TransferLoggerThread->Stop(1000);
	D_SafeDelete(m_TransferLoggerThread);

	m_WorkingThread->Stop(-1);
	D_SafeDelete(m_WorkingThread);

	UDT::cleanup();
#endif
	D_SafeDelete(m_pUDPPackBuffer);
}

Bool VMVupManager::AddVup(VMVup* _newVUP)
{
	VMVup* vup = FindVup(_newVUP->GetUniqueID());
	if(vup)
	{
		return false;
	}
	else
	{
#ifndef USE_UDT_LIB
		m_poVupMap.insert(std::pair<s32, VMVup*>(_newVUP->GetUniqueID(), _newVUP));
#else
		m_poVupMapByPassport.RetrieveContrainer().insert(std::pair<s32, VMVup*>(_newVUP->GetUniqueID(), _newVUP));
		m_poVupMapByPassport.ReleaseContrainer();

		std::string viewKey = GetViewKey(_newVUP);
		_newVUP->SetViewKey(viewKey.c_str());
		m_poVupViewMap.RetrieveContrainer().insert(std::pair<std::string, VMVup*>(viewKey, _newVUP));
		m_poVupViewMap.ReleaseContrainer();
#endif
	}
	return true;
}

std::string VMVupManager::GetViewKey(const VMVup* _curVUP) const
{
	std::stringstream ss;
	for(s32 i = 0; i < ESort_Num; ++i)
	{
		if(m_Parameters.m_iSortOrder[i] != ESort_Invalid)
		{
			switch(m_Parameters.m_iSortOrder[i])
			{
			case ESort_Passport:
				ss << _curVUP->GetUniqueID() << "$$";
				break;
			case ESort_IP:
				ss << _curVUP->GetIPAddress() << "$$";
				break;
			case ESort_CurPhase:
				ss << VMVup::kTestPhase[_curVUP->GetCurrentTestPhase()].GetName() << "$$";
				break;
			case ESort_LastPhase:
				ss << VMVup::kTestPhase[_curVUP->GetLastTestPhase()].GetName() << "$$";
				break;
			case ESort_LastStatus:
				ss << VMVup::kStatus[_curVUP->GetLastStaus()].GetName() << "$$";
				break;
			case ESort_Port:
				ss << _curVUP->GetPort() << "$$";
				break;
			case ESort_RDVPoint:
				ss << _curVUP->GetRDVPointID() << "_" << _curVUP->GetGroup() << "$$";
				break;
			}
		}
	}
	ss << _curVUP->GetUniqueID();
	return ss.str();
}

void VMVupManager::RefreshViewMap()
{
	VUPViewMap& vupViewMap =  m_poVupViewMap.RetrieveContrainer();
	vupViewMap.clear();

	const VUPMap& vupMap = m_poVupMapByPassport.GetContrainer();
	VUPMapConstIterator it = vupMap.begin();
	for(;it != vupMap.end(); ++it)
	{
		VMVup* vup = (*it).second;
		std::string viewKey = GetViewKey(vup);
		vup->SetViewKey(viewKey.c_str());
		vupViewMap.insert(std::pair<std::string, VMVup*>(viewKey, vup));
	}
	m_poVupViewMap.ReleaseContrainer();
}

VMVup* VMVupManager::FindVup(s32 _id)
{
#ifndef USE_UDT_LIB
	VUPMapIterator it = m_poVupMap.find(_id);
	if(it == m_poVupMap.end())
#else
	VUPMap& vupMap = m_poVupMapByPassport.RetrieveContrainer();
	VUPMapIterator it = vupMap.find(_id);
	if(it == vupMap.end())
#endif
	{
#ifdef USE_UDT_LIB
		m_poVupMapByPassport.ReleaseContrainer();
#endif
		return NULL;
	}
#ifdef USE_UDT_LIB
	m_poVupMapByPassport.ReleaseContrainer();
#endif
	return (*it).second;
}
const VMVup* VMVupManager::FindVup(s32 _id) const
{
#ifndef USE_UDT_LIB
	VUPMapConstIterator it = m_poVupMap.find(_id);
	if(it == m_poVupMap.end())
#else
	const VUPMap& vupMap = m_poVupMapByPassport.GetContrainer();
	VUPMapConstIterator it = vupMap.find(_id);
	if(it == vupMap.end())
#endif
	{
		return NULL;
	}
	return (*it).second;
}

Bool VMVupManager::RemoveVup(s32 _id)
{
#ifndef USE_UDT_LIB
	VUPMapIterator it = m_poVupMap.find(_id);
	if(it == m_poVupMap.end())
#else
	VUPMap& vupMap = m_poVupMapByPassport.RetrieveContrainer();
	VUPMapIterator it = vupMap.find(_id);
	if(it == vupMap.end())
#endif
	{
#ifdef USE_UDT_LIB
		m_poVupMapByPassport.ReleaseContrainer();
#endif
		return false;
	}
#ifndef USE_UDT_LIB
	m_poVupMap.erase(it);
#else
	VMVup* delVup = (*it).second;

	std::set<VMVup*>& dirtyMap = m_poDirtyVUP.RetrieveContrainer();
	std::set<VMVup*>::iterator itSet = dirtyMap.begin();
	for(;itSet != dirtyMap.end(); ++itSet)
	{
		VMVup* curVUP = (*itSet);
		if(curVUP == delVup)
		{
			dirtyMap.erase(itSet);
			break;
		}
	}
	m_poDirtyVUP.ReleaseContrainer();

	u16 vupRDVPoint = delVup->GetRDVPointID();
	if(vupRDVPoint != Protocal::kInvalidRDVPoint)
	{
		RDVPointListIterator itRDVPoint = m_poRDVList.find(delVup->GetRDVPointID());
		if(itRDVPoint != m_poRDVList.end())
		{
			RDVPointInfo& info = (*itRDVPoint).second;
			if(delVup->GetGroup() >= 0)
			{
				std::vector<s32>::iterator itClientPassport = find(info.m_RunningInfo.m_ClientList.begin(), info.m_RunningInfo.m_ClientList.end(), delVup->GetUniqueID());
				if(itClientPassport != info.m_RunningInfo.m_ClientList.end())
				{
					info.m_RunningInfo.m_ClientList.erase(itClientPassport);
					info.m_RunningInfo.m_GroupList[delVup->GetGroup()]--;
					info.m_RunningInfo.m_bIsGroupFull = false;
				}
			}
		}
	}

	VUPViewMap& vupViewMap = m_poVupViewMap.RetrieveContrainer();
	VUPViewMapIterator itView = vupViewMap.find(delVup->GetViewKey());
	if(itView != vupViewMap.end())
	{
		vupViewMap.erase(itView);
	}
	vupMap.erase(it);
	m_poVupViewMap.ReleaseContrainer();
	m_poVupMapByPassport.ReleaseContrainer();
#endif
	delete delVup;

	return true;
}

void VMVupManager::StartTesting(VMVup* _curVUP, __int64 _startTime)
{
	VMVup* pVup = _curVUP;
	D_CHECK(pVup);
	__int64 nowms = _startTime;

	UDP_PACK pack;
	pack.m_uiType = EPT_M2C_StartTesting;
	pack.m_unValue.m_StartTestingParam.m_uiBurstTime = nowms;

#ifndef USE_UDT_LIB
	m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
	m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
#else
	s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
	if(iRet == UDT::ERROR)
	{
		LOG_INFO("[ERROR] sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
		return;
	}
	else
	{
		if(iRet != sizeof(UDP_PACK))
		{
			LOG_INFO("[ERROR] send start testing to %d(%s:%d) failed\n", _curVUP->GetUniqueID(), _curVUP->GetIPAddress(), _curVUP->GetPort());
		}
		else
		{
			LOG_INFO("send start testing to %d(%s:%d) successfully\n", _curVUP->GetUniqueID(), _curVUP->GetIPAddress(), _curVUP->GetPort());
		}
		return;
	}
#endif
}

void VMVupManager::Refresh()
{
#ifndef USE_UDT_LIB
	for(int i = m_uiClientStartPort; i <= m_uiClientEndPort; i++)
	{
		UDP_PACK pack;
		pack.m_uiType = EPT_M2C_Refresh;
		m_pSendSocket->SetAddress(m_strBroadCastAddress.c_str(), i, true);
		m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
	}
	VUPMapConstIterator it = m_poVupMap.begin();
	for(;it != m_poVupMap.end(); ++it)
	{
		const VMVup* pVup = (*it).second;
		delete pVup;
	}
	m_poVupMap.clear();
#endif
}

void VMVupManager::RequestLog(s32 _id)
{
#ifdef USE_UDT_LIB
	const VMVup* pVup = FindVup(_id);
	if(!pVup)
		return;

	UDP_PACK pack;
	pack.m_uiType = EPT_M2C_LogRequest;
	s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
	if(iRet == UDT::ERROR)
	{
		LOG_INFO("[ERROR] sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
		return;
	}
#endif
}

void VMVupManager::KillClient(s32 _id)
{
	s32 killCount = 1;
	if(_id == -1)
	{
#ifndef USE_UDT_LIB
		VUPMapConstIterator it = m_poVupMap.begin();
		for(;it != m_poVupMap.end(); ++it)
#else
		VUPMap& vupMap = m_poVupMapByPassport.RetrieveContrainer();
		VUPMapConstIterator it = vupMap.begin();
		for(;it != vupMap.end(); ++it)
#endif
		{
			const VMVup* pVup = (*it).second;

			UDP_PACK pack;
			pack.m_uiType = EPT_M2C_KillClient;

#ifndef USE_UDT_LIB
			m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
			m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
			
			delete pVup;
#else
			s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
			if(iRet == UDT::ERROR)
			{
				LOG_INFO("[ERROR] sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
				continue;
			}
#endif
		}
#ifndef USE_UDT_LIB
		m_poVupMap.clear();
#else
		m_poVupMapByPassport.ReleaseContrainer();
#endif
		//m_poRDVList.clear();
	}
	else
	{
		const VMVup* pVup = FindVup(_id);
		if(!pVup)
			return;

		UDP_PACK pack;
		pack.m_uiType = EPT_M2C_KillClient;

#ifndef USE_UDT_LIB
		m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
		m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
		RemoveVup(_id);
#else
		s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
		if(iRet == UDT::ERROR)
		{
			LOG_INFO("[ERROR] sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
			return;
		}
#endif
	}
}
VMVupManager::RDVPointParameter* VMVupManager::FindRDVParam(s32 _iMajor, s32 _iMiner)
{
	s32 rdvID = Protocal::GetRDVPointID(_iMajor, _iMiner);
	RDVPointParameterListIterator itRDVPointParam = m_poRDVParam.find(rdvID);
	if(itRDVPointParam != m_poRDVParam.end())
	{
		return &(*itRDVPointParam).second;
	}
	return NULL;
}

VMVupManager::RDVPointParameter* VMVupManager::AddRDVParam(s32 _iMajor, s32 _iMiner)
{
	RDVPointParameter* pParam = FindRDVParam(_iMajor, _iMiner);
	if(pParam)
		return pParam;

	s32 rdvID = Protocal::GetRDVPointID(_iMajor, _iMiner);
	RDVPointParameterListIterator it = m_poRDVParam.insert(std::pair<RDVPointID, RDVPointParameter>(rdvID, RDVPointParameter())).first;
	return &(*it).second;
}
void VMVupManager::SetRDVParameter(RDVPointParameter& _pRDVParam, StringPtr _pOption, const VMCommandParamHolder& _param)
{
	if(!_stricmp(_pOption, "-groupnumber") || !_stricmp(_pOption, "-gn"))
	{
		_pRDVParam.m_iGroupNum = _param.ToInt();
	}
	else if(!_stricmp(_pOption, "-interval") || !_stricmp(_pOption, "-i"))
	{
		_pRDVParam.m_iIntervalOfEachGroup = _param.ToInt();
	}
	else if(!_stricmp(_pOption, "-delay") || !_stricmp(_pOption, "-d"))
	{
		_pRDVParam.m_iDelayOfStartTime = _param.ToInt();
	}
	else if(!_stricmp(_pOption, "-vupsnumberingroup") || !_stricmp(_pOption, "-vn"))
	{
		_pRDVParam.m_iVUPNumInEachGroup = _param.ToInt();
	}
	else if(!_stricmp(_pOption, "-expectednumber") || !_stricmp(_pOption, "-en"))
	{
		_pRDVParam.m_uiExpectedNum = _param.ToInt();
	}
	else if(!_stricmp(_pOption, "-timeout") || !_stricmp(_pOption, "-to"))
	{
		_pRDVParam.m_uiTimeOut = _param.ToInt();
	}
}
void VMVupManager::GetRDVParameter(const RDVPointParameter& _pRDVParam, StringPtr _pOption) const
{
	if(!_pOption || !_stricmp(_pOption, "-groupnumber") || !_stricmp(_pOption, "-gn"))
	{
		D_Output("$groupnumber = %d\n", _pRDVParam.m_iGroupNum);
	}
	if(!_pOption || !_stricmp(_pOption, "-interval") || !_stricmp(_pOption, "-i"))
	{
		D_Output("$interval = %d\n", _pRDVParam.m_iIntervalOfEachGroup);
	}
	if(!_pOption || !_stricmp(_pOption, "-delay") || !_stricmp(_pOption, "-d"))
	{
		D_Output("$delay = %d\n", _pRDVParam.m_iDelayOfStartTime);
	}
	if(!_pOption || !_stricmp(_pOption, "-vupsnumberingroup") || !_stricmp(_pOption, "-vn"))
	{
		D_Output("$vupsnumberingroup = %d\n", _pRDVParam.m_iVUPNumInEachGroup);
	}
	if(!_pOption || !_stricmp(_pOption, "-expectednumber") || !_stricmp(_pOption, "-en"))
	{
		D_Output("$expectednumber = %d\n", _pRDVParam.m_uiExpectedNum);
	}
	if(!_pOption || !_stricmp(_pOption, "-timeout") || !_stricmp(_pOption, "-to"))
	{
		D_Output("$timeout = %d\n", _pRDVParam.m_uiTimeOut);
	}
}

void VMVupManager::SetParameter(StringPtr _pOption, const VMCommandParamHolder& _param)
{
	if(!_stricmp(_pOption, "-hidesummaryifzero") || !_stricmp(_pOption, "-hs"))
	{
		m_Parameters.m_iHideSummaryIfZero = _param.ToInt();
	}
	else if(!_stricmp(_pOption, "-freezelist") || !_stricmp(_pOption, "-fl"))
	{
		m_Parameters.m_iFreezeList = _param.ToInt();
	}
}

void VMVupManager::GetParameter(StringPtr _pOption) const
{
	if(!_pOption || !_stricmp(_pOption, "-hidesummaryifzero") || !_stricmp(_pOption, "-hs"))
	{
		D_Output("$hidesummaryifzero = %d\n", m_Parameters.m_iHideSummaryIfZero);
	}
	if(!_pOption || !_stricmp(_pOption, "-freezelist") || !_stricmp(_pOption, "-fl"))
	{
		D_Output("$freezelist = %d\n", m_Parameters.m_iFreezeList);
	}
}

void VMVupManager::SetSortOrder(s32 _iOrder, StringPtr _pOrder)
{
	if(!_pOrder)
	{
		for(s32 i = 0; i < ESort_Num; ++i)
		{
			m_Parameters.m_iSortOrder[i] = ESort_Invalid;
		}
	}
	else
	{
		if(!_stricmp(_pOrder, "-passport"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_Passport;
		}
		else if(!_stricmp(_pOrder, "-ip"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_IP;
		}
		else if(!_stricmp(_pOrder, "-port"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_Port;
		}
		else if(!_stricmp(_pOrder, "-rdvpoint"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_RDVPoint;
		}
		else if(!_stricmp(_pOrder, "-curphase"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_CurPhase;
		}
		else if(!_stricmp(_pOrder, "-lastphase"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_LastPhase;
		}
		else if(!_stricmp(_pOrder, "-laststatus"))
		{
			m_Parameters.m_iSortOrder[_iOrder] = ESort_LastStatus;
		}
	}
}
void VMVupManager::SetSortUpOrDwon(StringPtr _pOrder)
{
	if(!_stricmp(_pOrder, "up") || !_stricmp(_pOrder, "u"))
	{
		m_Parameters.m_iSortUpOrDown = ESort_Up;
	}
	else if(!_stricmp(_pOrder, "down") || !_stricmp(_pOrder, "d"))
	{
		m_Parameters.m_iSortUpOrDown = ESort_Down;
	}
}

void VMVupManager::SetFilter(StringPtr _pOrder, StringPtr _pName)
{
	if(!_pOrder)
	{
		for(s32 i = 0; i < ESort_Num; ++i)
		{
			m_Parameters.m_Filters[i].m_bActiveFilter = false;
		}
	}
	else
	{
		s32 setType = ESort_Invalid;

		if(!_stricmp(_pOrder, "-passport"))
		{
			setType = ESort_Passport;
		}
		else if(!_stricmp(_pOrder, "-ip"))
		{
			setType = ESort_IP;
		}
		else if(!_stricmp(_pOrder, "-port"))
		{
			setType = ESort_Port;
		}
		else if(!_stricmp(_pOrder, "-rdvpoint"))
		{
			setType = ESort_RDVPoint;
		}
		else if(!_stricmp(_pOrder, "-curphase"))
		{
			setType = ESort_CurPhase;
		}
		else if(!_stricmp(_pOrder, "-lastphase"))
		{
			setType = ESort_LastPhase;
		}
		else if(!_stricmp(_pOrder, "-laststatus"))
		{
			setType = ESort_LastStatus;
		}
		if(setType != ESort_Invalid)
		{
			m_Parameters.m_Filters[setType].m_bActiveFilter = true;
			m_Parameters.m_Filters[setType].m_strFilterName = _pName;
		}
	}
}

Bool VMVupManager::_InitParameter()
{
	//Init config file
	TiXmlDocument* pConfigFile = new TiXmlDocument();
	bool isOK = pConfigFile->LoadFile("VUPManagerConfig.xml");
	if(!isOK)
	{
		delete pConfigFile;
		return false;
	}

	TiXmlElement* root = pConfigFile->RootElement();

	TiXmlElement* serverPort = root->FirstChildElement("serverport");
	m_uiServerPort = atoi(serverPort->GetText());

	TiXmlElement* serverLogPort = root->FirstChildElement("serverlogport");
	m_uiServerTransferLogPort = atoi(serverLogPort->GetText());

	TiXmlElement* clientStartPort = root->FirstChildElement("clientstartport");
	m_uiClientStartPort = atoi(clientStartPort->GetText());

	TiXmlElement* clientEndPort = root->FirstChildElement("clientendport");
	m_uiClientEndPort = atoi(clientEndPort->GetText());

	TiXmlElement* broadCastAddress = root->FirstChildElement("broadcastip");
	m_strBroadCastAddress = broadCastAddress->GetText();

	TiXmlElement* initCommandGroups = root->FirstChildElement("commands");
	TiXmlElement* command = initCommandGroups->FirstChildElement();
	while(command)
	{
		VMCommandCenter::GetPtr()->ExecuteFromString(command->GetText());
		command = command->NextSiblingElement();
	}

	delete pConfigFile;

	return true;
}

void VMVupManager::Create()
{
	if(!_InitParameter())
	{
		LOG_INFO("read configuration file failed\n");
		return;
	}
	else
	{
		LOG_INFO("read configuration successfully\n");
	}

	//Init recving packet pool
	m_pUDPPackBuffer = new MemPool<UDP_PACKWrapper>();
	m_pUDPPackBuffer->SetMaxSize(1000);

	//Init watched value
	//m_WatchedInfo.Init();

#ifndef USE_UDT_LIB
	//Init network
	m_pRecvSocket = new WinSocket;
	s32 iRet = m_pRecvSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	iRet = m_pRecvSocket->SetAddress(NULL, m_uiServerPort);
	D_CHECK(!iRet);
	iRet = m_pRecvSocket->Bind();
	D_CHECK(!iRet);

	m_pSendSocket = new WinSocket;
	iRet = m_pSendSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	//iRet = m_pSendSocket->SetAddress(NULL, 52346);
	//D_CHECK(!iRet);

	//Init recv thread
	m_pRecvThread = new Thread(new RecvUDPRunner(m_pRecvSocket, m_pUDPPackBuffer));
	Bool bRet = m_pRecvThread->Start();
	D_CHECK(bRet);
	Refresh();
#else
	m_ListeningThread = new Thread(new ListeningRunner(this, m_uiServerPort));
	Bool bRet = m_ListeningThread->Start();
	D_CHECK(bRet);

	m_WorkingThread = new Thread(new WorkingRunner(this, m_pUDPPackBuffer));
	bRet = m_WorkingThread->Start();
	D_CHECK(bRet);

	m_TransferLoggerThread = new Thread(new TransferLogger(m_uiServerTransferLogPort));
	bRet = m_TransferLoggerThread->Start();
	D_CHECK(bRet);
#endif

	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_AgentLeave), 
		new MEventHandler<VMVupManager>(this, &VMVupManager::onAgentLeave));

	GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_VUPInfoUpdate), 
		new MEventHandler<VMVupManager>(this, &VMVupManager::onVUPInfoChange));
}
void VMVupManager::Tick(f32 _fDeltaTime)
{
	//Handle udp pack
	_HandleUdpPack();

	//Check rdv point
	_UpdateRDVPoint();

	//Update summary info
	_UpdateSummary();

	//Update view
	_UpdateViewMap();

	//Update List
	Event evtList((EventType_t)E_ET_UIUpdateList);
	evtList.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtList);

	//Update Summary
	Event evtSummary((EventType_t)E_ET_UIUpdateSummary);
	evtSummary.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtSummary);
}

void VMVupManager::onAgentLeave(const Event* _poEvent)
{
	StringPtr pName = static_cast<StringPtr>(_poEvent->GetParam<void*>(0));
	WatchedInfos::WatchedValueMapInterator it = m_WatchedInfo.m_Values.find(pName);
	if(it != m_WatchedInfo.m_Values.end())
	{
		m_WatchedInfo.m_Values.erase(it);
	}
}

void VMVupManager::onVUPInfoChange(const Event* _poEvent)
{
	VMVup* pVup = static_cast<VMVup*>(_poEvent->GetParam<void*>(0));
	m_poDirtyVUP.RetrieveContrainer().insert(pVup);
	m_poDirtyVUP.ReleaseContrainer();
}

//#pragma optimize("", off)
void VMVupManager::_UpdateRDVPoint()
{
	RDVPointListIterator it = m_poRDVList.begin();
	while(it != m_poRDVList.end())
	{
		RDVPointInfo& info = (*it).second;
		D_CHECK(info.m_RunningInfo.IsValid());
		f32 fNow = GameEngine::GetGameEngine()->GetClockMod()->GetTotalElapsedSeconds();
		if(info.m_RunningInfo.m_ClientList.size() >= info.m_uiExpectedNum ||
			fNow - info.m_RunningInfo.m_fStartTime >= (f32)info.m_uiTimeOut)
		{
			//Default value
			s32 iDelayOfStartTime = 5000;
			s32 iIntervalOfEachGroup = 0;
			s32 iMaxGroupNum = 1;
			s32 iRunningID = info.m_RunningInfo.m_uiCurrentRunningID;
			RDVPointParameterListIterator itRDVPointParam = m_poRDVParam.find(Protocal::GetRDVPointID(Protocal::GetRDVPointMajor(iRunningID), 0));
			if(itRDVPointParam != m_poRDVParam.end())
			{
				RDVPointParameter& param = (*itRDVPointParam).second;
				iDelayOfStartTime = param.m_iDelayOfStartTime;
				iIntervalOfEachGroup = param.m_iIntervalOfEachGroup;
				iMaxGroupNum = param.m_iGroupNum;
			}
			struct __timeb64 timebuffer;
			_ftime64(&timebuffer);
			__int64 curTime = timebuffer.time * 1000 + timebuffer.millitm;

			for(s32 i = 0; i < info.m_RunningInfo.m_ClientList.size(); ++i)
			{
				VMVup* pVup = FindVup(info.m_RunningInfo.m_ClientList[i]);
				if(!pVup)
					continue;
				if(pVup->GetGroup() >= 0)
				{
					StartTesting(pVup, curTime + iDelayOfStartTime + pVup->GetGroup() * iIntervalOfEachGroup);
				}
			}
			for(s32 i = 0; i < iMaxGroupNum; ++i)
			{
				__int64 nowms = curTime + iDelayOfStartTime + i * iIntervalOfEachGroup;
				__int64 nows = nowms / 1000;
				char timeline[26];
				ctime_s(timeline, 26, &(nows));
				LOG_INFO("RDVPoint(%d,%d)..Group(%d)..Time of starting test: %.19s.%03hu\n", 
					Protocal::GetRDVPointMajor(iRunningID), Protocal::GetRDVPointMinor(iRunningID), i, timeline, nowms % 1000);
			}
			VMSummary::GetPtr()->RemoveRDVPoint(Protocal::GetRDVPointMajor(iRunningID), Protocal::GetRDVPointMinor(iRunningID));
			it = m_poRDVList.erase(it);
		}
		else
		{
			++it;
		}
	}
}	
void VMVupManager::_UpdateViewMap()
{
	std::set<VMVup*>& dirtyMap = m_poDirtyVUP.RetrieveContrainer();
	if(!m_Parameters.m_iFreezeList)
	{
		VUPViewMap& vupViewMap =  m_poVupViewMap.RetrieveContrainer();
		std::set<VMVup*>::iterator itSet = dirtyMap.begin();
		for(;itSet != dirtyMap.end(); ++itSet)
		{
			VMVup* curVUP = (*itSet);
			std::string oldViewKey = curVUP->GetViewKey();
			VUPViewMapIterator itView = vupViewMap.find(oldViewKey);
			if(itView != vupViewMap.end())
			{
				vupViewMap.erase(itView);
			}
			std::string viewKey = GetViewKey(curVUP);
			curVUP->SetViewKey(viewKey.c_str());
			vupViewMap.insert(std::pair<std::string, VMVup*>(viewKey, curVUP));
		}
		m_poVupViewMap.ReleaseContrainer();
	}
	dirtyMap.clear();
	m_poDirtyVUP.ReleaseContrainer();
}

void VMVupManager::_UpdateSummary()
{
	VMSummary::GetPtr()->UpdateToWatch(m_WatchedInfo, (m_Parameters.m_iHideSummaryIfZero > 0));
}
//#pragma optimize("", off)
void VMVupManager::_HandleUdpPack()
{
	s32 iSize = m_pUDPPackBuffer->GetSize();
	if(iSize != 0)
	{
		UDP_PACKWrapper *poPacArray = new UDP_PACKWrapper[iSize];
		m_pUDPPackBuffer->GetUDPData(poPacArray, iSize);
		for(s32 i = 0; i < iSize; ++i)
		{
			UDP_PACKWrapper *poPackWrapper = poPacArray + i;
			UDP_PACK* poPack = &(poPackWrapper->m_InnerData);
			switch(poPack->m_uiType)
			{
			case EPT_C2M_LostConnection:
				{
					LostConnection(poPackWrapper->m_ClientSocket);
				}
				break;
			case EPT_C2M_ClientRegister:
				{
#ifndef USE_UDT_LIB
					Char cmd[VMCommand::kMaxCommandLength];
					sprintf(cmd, "addvup %d %s %d",	poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort,
													poPackWrapper->m_SrcIPAddress.c_str(),
													poPack->m_unValue.m_ClientRegisterParam.m_uiPort);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);

					sprintf(cmd, "updatevup %d -rs %d -tp %d",	poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort,
															    poPack->m_unValue.m_ClientRegisterParam.m_uiStatus,
																poPack->m_unValue.m_ClientRegisterParam.m_uiTestPhase);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);

					UDP_PACK pack;
					pack.m_uiType = EPT_M2C_ClientRegisterACK;
					m_pSendSocket->SetAddress(poPackWrapper->m_SrcIPAddress.c_str(), poPack->m_unValue.m_ClientRegisterParam.m_uiPort);
					m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
#else
					UDTSOCKET curSocket = poPackWrapper->m_ClientSocket;
					VMVup* pVup = FindVupBySocket(curSocket);
					if(pVup)
					{
						pVup->SetUniqueID(poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort);
						pVup->SetStatus(poPack->m_unValue.m_ClientRegisterParam.m_uiStatus);
						pVup->SetTestPhase(poPack->m_unValue.m_ClientRegisterParam.m_uiTestPhase);

						LOG_INFO("get registered info from: %d(%s:%d)\n", pVup->GetUniqueID(), pVup->GetIPAddress(), pVup->GetPort());

						Bool isOk = AddVup(pVup);

						UDP_PACK pack;
						pack.m_uiType = EPT_M2C_ClientRegisterACK;
						if(isOk)
						{
							pack.m_unValue.m_RegisterAckParam.m_uiHasSuccessed = true;
							s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
							if(iRet == UDT::ERROR)
							{
								LOG_INFO("[ERROR] sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
							}
						}
						else
						{
							pack.m_unValue.m_RegisterAckParam.m_uiHasSuccessed = false;
							s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
							if(iRet == UDT::ERROR)
							{
								LOG_INFO("[ERROR] sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
							}
							LOG_INFO("[ERROR] duplicate passport: %d\n", poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort);
						}
					}
					else
					{
						LOG_INFO("[ERROR] cannot find vup in client list and retry: %d\n", poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort);
						m_pUDPPackBuffer->InsertUDPData(*poPackWrapper);
					}
#endif
				}
				break;
			case EPT_C2M_ReportClientStatus:
				{
					Char cmd[VMCommand::kMaxCommandLength];
					sprintf(cmd, "updatevup %d -rs %d",	poPack->m_unValue.m_ReportClientStatusParam.m_uiPassPort,
														poPack->m_unValue.m_ReportClientStatusParam.m_uiRunningStatus);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);

					sprintf(cmd, "updatevup %d -tp %d",	poPack->m_unValue.m_ReportClientStatusParam.m_uiPassPort,
														poPack->m_unValue.m_ReportClientStatusParam.m_uiTestPhase);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);

					//LOG_INFO("get status %s, %s\n", VMVup::kStatus[poPack->m_unValue.m_ReportClientStatusParam.m_uiRunningStatus].GetName(),
					//								VMVup::kTestPhase[poPack->m_unValue.m_ReportClientStatusParam.m_uiTestPhase].GetName());
				}
				break;
			case EPT_C2M_ReachRDVPoint:
				{
					u16 uiRDVPoint = poPack->m_unValue.m_ReachRDVPointParam.m_uiRDVPointID;
					RDVPointListIterator itRDVPoint = m_poRDVList.find(uiRDVPoint);
					if(itRDVPoint == m_poRDVList.end())
					{
						LOG_INFO("[RDV Point] new rdv point: %d\n", uiRDVPoint);
						RDVPointInfo info;
						info.m_uiID								= uiRDVPoint;
						info.m_RunningInfo.m_bHasValidValue		= true;
						info.m_RunningInfo.m_uiCurrentRunningID = uiRDVPoint;
						info.m_RunningInfo.m_fStartTime			= GameEngine::GetGameEngine()->GetClockMod()->GetTotalElapsedSeconds();
						info.m_RunningInfo.m_bIsGroupFull		= false;

						itRDVPoint = m_poRDVList.insert(std::pair<RDVPointID, RDVPointInfo>(uiRDVPoint, info)).first;

						RDVPointInfo& pi = (*itRDVPoint).second;
						RDVPointRunningInfo& runningInfo = (*itRDVPoint).second.m_RunningInfo;
						RDVPointParameterListIterator itRDVPointParam = m_poRDVParam.find(Protocal::GetRDVPointID(Protocal::GetRDVPointMajor(uiRDVPoint), 0));
						if(itRDVPointParam != m_poRDVParam.end())
						{
							const RDVPointParameter& param = (*itRDVPointParam).second;
							runningInfo.m_GroupList.ReSize(param.m_iGroupNum);
							for(s32 i = 0; i < runningInfo.m_GroupList.Size(); ++i)
							{
								runningInfo.m_GroupList[i] = 0;
							}
							pi.m_uiExpectedNum = param.m_uiExpectedNum;
							pi.m_uiTimeOut = param.m_uiTimeOut;
						}
						else
						{
							runningInfo.m_GroupList.ReSize(1);
							runningInfo.m_GroupList[0] = 0;

							//TJQ: use default value
							pi.m_uiExpectedNum = 9999;
							pi.m_uiTimeOut = 10 * 60;
						}
					}
					else
					{
						RDVPointInfo& pi = (*itRDVPoint).second;
						D_CHECK(pi.m_RunningInfo.m_bHasValidValue);
						D_CHECK(pi.m_RunningInfo.m_uiCurrentRunningID == uiRDVPoint);
						//D_CHECK(pi.m_uiExpectedNum == poPack->m_unValue.m_ReachRDVPointParam.m_uiExpected);
						//D_CHECK(pi.m_uiTimeOut == poPack->m_unValue.m_ReachRDVPointParam.m_uiTimeout);
					}
					RDVPointRunningInfo& runningInfo = (*itRDVPoint).second.m_RunningInfo;

					RDVPointParameterListIterator itRDVPointParam = m_poRDVParam.find(Protocal::GetRDVPointID(Protocal::GetRDVPointMajor(uiRDVPoint), 0));
					if(itRDVPointParam == m_poRDVParam.end())
					{	
						//use default
						VMVup* vup = FindVup(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
						if(vup)
						{
							vup->SetGroup(0);
							vup->SetRDVPointID(uiRDVPoint);
							
							runningInfo.m_GroupList[0]++;
							
							LOG_INFO("[RDV Point] add vup %d to: R(%d, %d)G(0)[%d/-1]\n", 
								poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort, 
								Protocal::GetRDVPointMajor(uiRDVPoint),
								Protocal::GetRDVPointMinor(uiRDVPoint),
								runningInfo.m_GroupList[0]);

							runningInfo.m_ClientList.push_back(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);

							VMSummary::GroupUpdateParam guParam;
							guParam.m_AddorRemove = true;
							guParam.m_MaxNumberInGroup = -1;
							guParam.m_MyGroup = 0;
							guParam.m_VUPsPassport = poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort;
							VMSummary::GetPtr()->UpdateGroupInfo(Protocal::GetRDVPointMajor(uiRDVPoint), Protocal::GetRDVPointMinor(uiRDVPoint), 1, guParam);
						}
					}
					else
					{
						const RDVPointParameter& param = (*itRDVPointParam).second;
						VMVup* vup = FindVup(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
						if(vup)
						{
							//Set RDV Point
							vup->SetRDVPointID(uiRDVPoint);

							//Set Group
							Bool bShouldFindAEmptyGroup = true;
							s32 iOriginalGroup = vup->GetGroup();
							vup->SetGroup(-1);

							if(!runningInfo.m_bIsGroupFull)
							{
								if(iOriginalGroup >= 0 && iOriginalGroup < runningInfo.m_GroupList.Size())
								{
									if(runningInfo.m_GroupList[iOriginalGroup] < param.m_iVUPNumInEachGroup)
									{
										runningInfo.m_GroupList[iOriginalGroup]++;
										vup->SetGroup(iOriginalGroup);
										bShouldFindAEmptyGroup = false;
									}
								}
								if(bShouldFindAEmptyGroup)
								{
									s32 idx = 0;
									for(idx = 0; idx < runningInfo.m_GroupList.Size(); ++idx)
									{
										if(runningInfo.m_GroupList[idx] < param.m_iVUPNumInEachGroup)
										{
											runningInfo.m_GroupList[idx]++;
											vup->SetGroup(idx);
											break;
										}
									}
									if(idx == runningInfo.m_GroupList.Size())
									{
										runningInfo.m_bIsGroupFull = true;
									}
								}
							}
							if(vup->GetGroup() >= 0)
							{
								runningInfo.m_ClientList.push_back(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);

								VMSummary::GroupUpdateParam guParam;
								guParam.m_AddorRemove = true;
								guParam.m_MaxNumberInGroup = param.m_iVUPNumInEachGroup;
								guParam.m_MyGroup = vup->GetGroup();
								guParam.m_VUPsPassport = poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort;
								VMSummary::GetPtr()->UpdateGroupInfo(Protocal::GetRDVPointMajor(uiRDVPoint), Protocal::GetRDVPointMinor(uiRDVPoint), param.m_iGroupNum, guParam);

								LOG_INFO("[RDV Point] add vup %d to: R(%d, %d)G(%d)[%d/%d]\n", 
									poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort, 
									Protocal::GetRDVPointMajor(uiRDVPoint),
									Protocal::GetRDVPointMinor(uiRDVPoint),
									vup->GetGroup(), 
									runningInfo.m_GroupList[vup->GetGroup()], 
									param.m_iVUPNumInEachGroup);
							}
						}
					}
				}
				break;
			}
		}
		delete[] poPacArray;
	}
}

#ifdef USE_UDT_LIB
ListeningRunner::ListeningRunner(VMVupManager* _pVUPMan, u16 _uiPort)
	: m_pMan(_pVUPMan)
	, m_uiPort(_uiPort)
	, m_bRequestStop(false)
{
}

//#pragma optimize("", off)
u32 ListeningRunner::Run()
{
	m_pListeningSocket = UDT::socket(AF_INET, SOCK_DGRAM, 0);
	if(m_pListeningSocket == UDT::INVALID_SOCK)
	{
		LOG_INFO("create m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}
	bool bParam = true;
	UDT::setsockopt(m_pListeningSocket, 0, UDT_REUSEADDR,	&bParam, sizeof(bool));
	UDT::setsockopt(m_pListeningSocket, 0, UDT_SNDSYN,		&bParam, sizeof(bool));
	UDT::setsockopt(m_pListeningSocket, 0, UDT_RCVSYN,		&bParam, sizeof(bool));
	linger l;
	l.l_linger = 1;
	l.l_onoff = 0;
	UDT::setsockopt(m_pListeningSocket, 0, UDT_LINGER, (int*)&l, sizeof(int));

	sockaddr_in addrinfo;
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrinfo.sin_port = htons(m_uiPort);
	if(UDT::ERROR == UDT::bind(m_pListeningSocket, (struct sockaddr*)&addrinfo, sizeof(addrinfo)))
	{
		LOG_INFO("bind m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}
	if(UDT::ERROR == UDT::listen(m_pListeningSocket, 50))
	{
		LOG_INFO("listen m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}

	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	UDTSOCKET pClientSocket;
	while (!m_bRequestStop)
	{
		if(UDT::INVALID_SOCK == (pClientSocket = UDT::accept(m_pListeningSocket, (sockaddr*)&clientaddr, &addrlen)))
		{
			LOG_INFO("accept m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
			return 1;
		}
		//linger l;
		//l.l_linger = 1;
		//l.l_onoff = 0;
		//UDT::setsockopt(pClientSocket, 0, UDT_LINGER, (int*)&l, sizeof(int));
		long addr = inet_addr(inet_ntoa(clientaddr.sin_addr));
		struct hostent* pHostent = gethostbyaddr((char*)&addr, sizeof(long),  AF_INET); 
		VMVup* newVup = new VMVup(pHostent->h_name, ntohs(clientaddr.sin_port));

		LOG_INFO("get client connection from %s:%d\n", newVup->GetIPAddress(), newVup->GetPort());

		newVup->SetClientSocket(pClientSocket);

		if(m_pMan->AddVupBySocket(newVup))
		{
			m_pMan->AddClientSocket(pClientSocket);
		}
		else
		{
			delete newVup;
			LOG_INFO("[ERROR] add to socket list failed %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		}
	}
	UDT::close(m_pListeningSocket);
	return 0;
}
void ListeningRunner::NotifyQuit()
{
	m_bRequestStop = true;
}
//---------------------------------------------------------------------------------------------------------
WorkingRunner::WorkingRunner(VMVupManager* _pVUPMan, MemPool<UDP_PACKWrapper>* _pMempool)
	:m_pMan(_pVUPMan)
	,m_bRequestStop(false)
	,m_pUDPPackBuffer(_pMempool)
{
}
u32 WorkingRunner::Run()
{
	VMThreadSafeContainer<std::vector<UDTSOCKET>>& clientSocketContainer = m_pMan->GetClientSocket(); 
	std::vector<UDTSOCKET>* rfds = NULL, *efds = NULL;
	rfds = new std::vector<UDTSOCKET>;
	efds = new std::vector<UDTSOCKET>;
	while(!m_bRequestStop)
	{
		std::vector<UDTSOCKET>& clientSocket = clientSocketContainer.RetrieveContrainer();
		s32 iRet = UDT::selectEx(clientSocket, rfds, NULL, efds, 0);
		clientSocketContainer.ReleaseContrainer();
		
		if(iRet == UDT::ERROR)
		{
			LOG_INFO("selectEx failed: %s\n", UDT::getlasterror().getErrorMessage());
			return 1;
		}
		else if(iRet == 0)
		{
			::Sleep(10);
		}
		else if(iRet > 0)
		{
			//Handle rfds
			UDP_PACKWrapper recvPacket;
			std::vector<UDTSOCKET>::const_iterator itRead = rfds->begin();
			for(itRead = rfds->begin(); itRead != rfds->end(); ++itRead)
			{
				UDTSOCKET readSocket = *itRead;
				D_CHECK(readSocket != UDT::INVALID_SOCK);
				s32 iRecv = UDT::recvmsg(readSocket, (char*)&recvPacket.m_InnerData, sizeof(UDP_PACK));
				if(iRecv == UDT::ERROR)
				{
					LOG_INFO("recvmsg failed(%d): %s\n", readSocket, UDT::getlasterror().getErrorMessage());
					continue;
				}
				recvPacket.m_ClientSocket = readSocket;
				m_pUDPPackBuffer->InsertUDPData(recvPacket);
			}
			//Handle efds
			std::vector<UDTSOCKET>::const_iterator itExpect = efds->begin();
			for(itExpect = efds->begin(); itExpect != efds->end(); ++itExpect)
			{
				UDTSOCKET expectSocket = *itExpect;
				recvPacket.m_ClientSocket = expectSocket;
				recvPacket.m_InnerData.m_uiType = EPT_C2M_LostConnection;
				m_pUDPPackBuffer->InsertUDPData(recvPacket);
				//m_pMan->LostConnection(expectSocket);
			}
		}
	}
	//TJQ: crash if clean up these vector, don't know why
	//delete rfds;
	//delete efds;
	return 0;
}
void WorkingRunner::NotifyQuit()
{
	m_bRequestStop = true;
}
//---------------------------------------------------------------------------------------------------------
TransferLogger::TransferLogger(u16 _port)
	: m_uiServerTransferLogPort(_port)
	, m_bRequestStop(false)
{
}

u32 TransferLogger::Run()
{
	UDTSOCKET pListeningSocket = UDT::socket(AF_INET, SOCK_STREAM, 0);
	if(pListeningSocket == UDT::INVALID_SOCK)
	{
		D_Output("[ERROR] create pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}
	sockaddr_in addrinfo;
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrinfo.sin_port = htons(m_uiServerTransferLogPort);
	if(UDT::ERROR == UDT::bind(pListeningSocket, (struct sockaddr*)&addrinfo, sizeof(addrinfo)))
	{
		D_Output("[ERROR] bind pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}
	if(UDT::ERROR == UDT::listen(pListeningSocket, 1))
	{
		D_Output("[ERROR] listen pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}
	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	UDTSOCKET pClientSocket;
	while (!m_bRequestStop)
	{
		if(UDT::INVALID_SOCK == (pClientSocket = UDT::accept(pListeningSocket, (sockaddr*)&clientaddr, &addrlen)))
		{
			D_Output("[ERROR] accept m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
			return 1;
		}
		long addr = inet_addr(inet_ntoa(clientaddr.sin_addr));
		struct hostent* pHostent = gethostbyaddr((char*)&addr, sizeof(long),  AF_INET); 
		D_Output("[INFO] get file transferring request from %s:%d\n", pHostent->h_name, ntohs(clientaddr.sin_port));

		int64_t size = -1;
		if(UDT::ERROR == UDT::recv(pClientSocket, (char*)&size, sizeof(int64_t), 0))
		{
			D_Output("[ERROR] recv file size failed: %s\n", UDT::getlasterror().getErrorMessage());
			continue;
		}
		if(size >= 0)
		{
			Char strClientLogName[MAX_PATH];
			sprintf(strClientLogName, "%s_%d.log", pHostent->h_name, ntohs(clientaddr.sin_port));
			std::fstream ofs(strClientLogName, std::ios::out | std::ios::binary | std::ios::trunc);
			if(ofs.is_open())
			{
				int64_t recvsize; 
				if (UDT::ERROR == (recvsize = UDT::recvfile(pClientSocket, ofs, 0, size)))
				{
					ofs.close();
					D_Output("[ERROR] recvfile failed: %s\n", UDT::getlasterror().getErrorMessage());
					continue;
				}
				D_Output("[INFO] recvfile log file successfully: %s\n", strClientLogName);
				ofs.close();

				HWND mainHwnd = (HWND)GameEngine::GetGameEngine()->GetRenderMod()->GetGUIMainWindow()->id();
				HINSTANCE hInst = ::ShellExecute(mainHwnd,"open", strClientLogName, "", "", SW_SHOW);
				if((s32)hInst <= 32)
				{
					if((s32)hInst == ERROR_ACCESS_DENIED)
					{
						hInst = ::ShellExecute(mainHwnd, "open", "notepad.exe", strClientLogName, "", SW_SHOW);
						if((int)hInst > 32)
						{
							continue;
						}
					}
					D_Output("[ERROR] failed to open file: %s\n", strClientLogName);
				}
			}
			else
			{
				D_Output("[ERROR] failed to create %s\n", strClientLogName);
			}
		}
		else
		{
			D_Output("[ERROR] recvfile log file failed: open remote log file failed\n");
		}

	}
	UDT::close(pListeningSocket);
	return 0;
}

void TransferLogger::NotifyQuit()
{
	m_bRequestStop = true;
}
//----------------------------------------------------------------------------------------------------------------
void VMVupManager::AddClientSocket(UDTSOCKET _pNewSocket)
{
	D_CHECK(_pNewSocket != UDT::ERROR);
	std::vector<UDTSOCKET>& pClientArray = m_pClientSockets.RetrieveContrainer();
	pClientArray.push_back(_pNewSocket);
	m_pClientSockets.ReleaseContrainer();
}
Bool VMVupManager::AddVupBySocket(VMVup* _newVUP)
{
	VMVup* vup = FindVupBySocket(_newVUP->GetClientSocket());
	if(vup)
	{
		return false;
	}
	else
	{
		VUPMap& vupMap = m_poVupMapBySocket.RetrieveContrainer();
		vupMap.insert(std::pair<s32, VMVup*>(_newVUP->GetClientSocket(), _newVUP));
		m_poVupMapBySocket.ReleaseContrainer();
	}
	return true;
}
VMVup* VMVupManager::FindVupBySocket(s32 _id)
{
	VUPMap& vupMap = m_poVupMapBySocket.RetrieveContrainer();
	VUPMapIterator it = vupMap.find(_id);
	if(it == vupMap.end())
	{
		m_poVupMapBySocket.ReleaseContrainer();
		return NULL;
	}
	m_poVupMapBySocket.ReleaseContrainer();
	return (*it).second;
}
Bool VMVupManager::LostConnection(UDTSOCKET _pLostConnection)
{
	VMVup* delVup = FindVupBySocket(_pLostConnection);
	if(!delVup)
		return false;

	LOG_INFO("LostConnection->id(%d),ip(%s),rdvpoint(%d, %d)%d\n", 
		_pLostConnection, delVup->GetIPAddress(), Protocal::GetRDVPointMajor(delVup->GetRDVPointID()), Protocal::GetRDVPointMinor(delVup->GetRDVPointID()), delVup->GetGroup());

	VUPMap& vupMapBySocket = m_poVupMapBySocket.RetrieveContrainer();
	VUPMapIterator it = vupMapBySocket.find(_pLostConnection);
	if(it != vupMapBySocket.end())
	{
		vupMapBySocket.erase(it);
	}
	m_poVupMapBySocket.ReleaseContrainer();

	RemoveVup(delVup->GetUniqueID());
	
	std::vector<UDTSOCKET>& clientSockets = m_pClientSockets.RetrieveContrainer();
	std::vector<UDTSOCKET>::iterator itClientSockets = find(clientSockets.begin(), clientSockets.end(), _pLostConnection);
	if(itClientSockets != clientSockets.end())
	{
		clientSockets.erase(itClientSockets);
	}
	m_pClientSockets.ReleaseContrainer();
	return true;
}
#endif