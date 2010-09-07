#include "VMVupManager.h"
#include "VMVup.h"
#include "tinyxml.h"
#include <time.h>
#include <sys/timeb.h>
#include <algorithm>


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

	VMVup* newVup = new VMVup(iPassport, strIPAddr, uiPort);
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
	RDVPointParameter* pParam = pMan->AddRDVParam(_paramList[0].ToInt(), _paramList[1].ToInt());
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
	RDVPointParameter* pParam = pMan->FindRDVParam(_paramList[0].ToInt(), _paramList[1].ToInt());
	if(!pParam)
		return 1;

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
		*vup = *_newVUP;
		return false;
	}
	else
	{
#ifndef USE_UDT_LIB
		m_poVupMap.insert(std::pair<s32, VMVup*>(_newVUP->GetUniqueID(), _newVUP));
#else
		m_poVupMapByPassport.RetrieveContrainer().insert(std::pair<s32, VMVup*>(_newVUP->GetUniqueID(), _newVUP));
		m_poVupMapByPassport.ReleaseContrainer();
#endif
	}
	return true;
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
	delete (*it).second;
#ifndef USE_UDT_LIB
	m_poVupMap.erase(it);
#else
	vupMap.erase(it);
	m_poVupMapByPassport.ReleaseContrainer();
#endif
	return true;
}

void VMVupManager::StartTesting(s32 _iDelayOfStartTime, s32 _iIntervalOfEachGroup, s32 _id)
{
	VMVup* pVup = FindVup(_id);
	if(!pVup)
		return;
	
	if(pVup->GetGroup() >= 0)
	{
		struct __timeb64 timebuffer;
		_ftime64(&timebuffer);
		__int64 nowms = timebuffer.time * 1000 + timebuffer.millitm + _iDelayOfStartTime + pVup->GetGroup() * _iIntervalOfEachGroup;

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
			D_Output("sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
			return;
		}
#endif
	}
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
				D_Output("sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
				continue;
			}
#endif
		}
#ifndef USE_UDT_LIB
		m_poVupMap.clear();
#else
		m_poVupMapByPassport.ReleaseContrainer();
#endif
		m_poRDVList.clear();
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
			D_Output("sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
			return;
		}
#endif
	}
}
VMVupManager::RDVPointParameter* VMVupManager::FindRDVParam(s32 _iMajor, s32 _iMiner)
{
	s32 rdvID = _iMajor * 1000 + _iMiner;
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

	s32 rdvID = _iMajor * 1000 + _iMiner;
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
}

void VMVupManager::SetParameter(StringPtr _pOption, const VMCommandParamHolder& _param)
{
	if(!_stricmp(_pOption, "-hidesummaryifzero") || !_stricmp(_pOption, "-hs"))
	{
		m_Parameters.m_iHideSummaryIfZero = _param.ToInt();
	}
}

void VMVupManager::GetParameter(StringPtr _pOption) const
{
	if(!_pOption || !_stricmp(_pOption, "-hidesummaryifzero") || !_stricmp(_pOption, "-hs"))
	{
		D_Output("$hidesummaryifzero = %d\n", m_Parameters.m_iHideSummaryIfZero);
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

	TiXmlElement* serverPort = root->FirstChildElement();
	m_uiServerPort = atoi(serverPort->GetText());

	TiXmlElement* clientStartPort = serverPort->NextSiblingElement();
	m_uiClientStartPort = atoi(clientStartPort->GetText());

	TiXmlElement* clientEndPort = clientStartPort->NextSiblingElement();
	m_uiClientEndPort = atoi(clientEndPort->GetText());

	TiXmlElement* broadCastAddress = clientEndPort->NextSiblingElement();
	m_strBroadCastAddress = broadCastAddress->GetText();

	TiXmlElement* initCommandGroups = broadCastAddress->NextSiblingElement();
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
		D_Output("read configuration file failed\n");
		return;
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
#endif
}
void VMVupManager::Tick(f32 _fDeltaTime)
{
	//Handle udp pack
	_HandleUdpPack();

	//Check rdv point
	_UpdateRDVPoint();

	//Update summary info
	_UpdateSummary();

	//Update List
	Event evtList((EventType_t)E_ET_UIUpdateList);
	evtList.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtList);

	//Update Summary
	Event evtSummary((EventType_t)E_ET_UIUpdateSummay);
	evtSummary.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evtSummary);
}

void VMVupManager::_UpdateRDVPoint()
{
	RDVPointListIterator it = m_poRDVList.begin();
	while(it != m_poRDVList.end())
	{
		RDVPointInfo& info = (*it).second;
		D_CHECK(info.m_RunningInfo.IsValid());
		f32 fNow = GameEngine::GetGameEngine()->GetClockMod()->GetTotalElapsedSeconds();
		if(info.m_RunningInfo.m_ClientList.Size() >= info.m_uiExpectedNum ||
			fNow - info.m_RunningInfo.m_fStartTime >= (f32)info.m_uiTimeOut)
		{
			//Default value
			s32 iDelayOfStartTime = 5000;
			s32 iIntervalOfEachGroup = 0;
			RDVPointParameterListIterator itRDVPointParam = m_poRDVParam.find(info.m_RunningInfo.m_uiCurrentRunningID);
			if(itRDVPointParam != m_poRDVParam.end())
			{
				RDVPointParameter& param = (*itRDVPointParam).second;
				iDelayOfStartTime = param.m_iDelayOfStartTime;
				iIntervalOfEachGroup = param.m_iIntervalOfEachGroup;
			}
			for(s32 i = 0; i < info.m_RunningInfo.m_ClientList.Size(); ++i)
			{
				StartTesting(iDelayOfStartTime, iIntervalOfEachGroup, info.m_RunningInfo.m_ClientList[i]);
				D_Output("[RDV Point] release test of %d\n", info.m_RunningInfo.m_uiCurrentRunningID);
			}
			it = m_poRDVList.erase(it);
		}
		else
		{
			++it;
		}
	}
}	

void VMVupManager::_UpdateSummary()
{
	Bool hideSummary = (m_Parameters.m_iHideSummaryIfZero > 0);
	for(s32 i = 0; i < EVupStatus_Num; ++i)
	{
		m_WatchedInfo.UpdateValue(VMVup::kStatus[i].GetName(), VMVup::kStatusSummary[i], hideSummary);
	}
	for(s32 i = 0; i < ETestPhase_Num; ++i)
	{
		m_WatchedInfo.UpdateValue(VMVup::kTestPhase[i].GetName(), VMVup::kTestPhaseSummary[i], hideSummary);
	}
#ifndef USE_UDT_LIB
	m_WatchedInfo.UpdateValue("0.Total VUPs", (s32)m_poVupMap.size(), hideSummary);
#else
	m_WatchedInfo.UpdateValue("0.Total VUPs", (s32)m_poVupMapByPassport.GetContrainer().size(), hideSummary);
#endif
	std::map<std::string, s32>::iterator it;
	for(it = VMVup::kIpSummary.begin(); it != VMVup::kIpSummary.end(); ++it)
	{
		m_WatchedInfo.UpdateValue((*it).first.c_str(), (s32)(*it).second, hideSummary);
	}
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

						AddVup(pVup);

						UDP_PACK pack;
						pack.m_uiType = EPT_M2C_ClientRegisterACK;
						s32 iRet = UDT::sendmsg(pVup->GetClientSocket(), (const Char*)&pack, sizeof(UDP_PACK));
						if(iRet == UDT::ERROR)
						{
							D_Output("sendmsg failed: %s\n", UDT::getlasterror().getErrorMessage());
							break;
						}
					}
					else
					{
						m_pUDPPackBuffer->InsertUDPData(*poPackWrapper);
					}
#endif
				}
				break;
			case EPT_C2M_ReportClientRunningStatus:
				{
					Char cmd[VMCommand::kMaxCommandLength];
					sprintf(cmd, "updatevup %d -rs %d",	poPack->m_unValue.m_ReportClientRunningStatusParam.m_uiPassPort,
														poPack->m_unValue.m_ReportClientRunningStatusParam.m_uiStatus);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);
				}
				break;
			case EPT_C2M_ReportClientTesingPhase:
				{
					Char cmd[VMCommand::kMaxCommandLength];
					sprintf(cmd, "updatevup %d -tp %d",	poPack->m_unValue.m_ReportClientTesingPhaseParam.m_uiPassPort,
														poPack->m_unValue.m_ReportClientTesingPhaseParam.m_uiPhase);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);
				}
				break;
			case EPT_C2M_ReachRDVPoint:
				{
					u16 uiRDVPoint = poPack->m_unValue.m_ReachRDVPointParam.m_uiRDVPointID;
					RDVPointListIterator itRDVPoint = m_poRDVList.find(uiRDVPoint);
					if(itRDVPoint == m_poRDVList.end())
					{
						D_Output("[RDV Point] new rdv point: %d\n", uiRDVPoint);
						RDVPointInfo info;
						info.m_uiID								= uiRDVPoint;
						info.m_uiExpectedNum					= poPack->m_unValue.m_ReachRDVPointParam.m_uiExpected;
						info.m_uiTimeOut						= poPack->m_unValue.m_ReachRDVPointParam.m_uiTimeout;
						info.m_RunningInfo.m_bHasValidValue		= true;
						info.m_RunningInfo.m_uiCurrentRunningID = uiRDVPoint;
						info.m_RunningInfo.m_fStartTime			= GameEngine::GetGameEngine()->GetClockMod()->GetTotalElapsedSeconds();
						itRDVPoint = m_poRDVList.insert(std::pair<RDVPointID, RDVPointInfo>(uiRDVPoint, info)).first;
					}
					else
					{
						RDVPointInfo& pi = (*itRDVPoint).second;
						D_CHECK(pi.m_RunningInfo.m_bHasValidValue);
						D_CHECK(pi.m_RunningInfo.m_uiCurrentRunningID == uiRDVPoint);
						D_CHECK(pi.m_uiExpectedNum == poPack->m_unValue.m_ReachRDVPointParam.m_uiExpected);
						D_CHECK(pi.m_uiTimeOut == poPack->m_unValue.m_ReachRDVPointParam.m_uiTimeout);
					}
					RDVPointRunningInfo& runningInfo = (*itRDVPoint).second.m_RunningInfo;
					runningInfo.m_ClientList.PushBack(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);

					RDVPointParameterListIterator itRDVPointParam = m_poRDVParam.find(uiRDVPoint);
					if(itRDVPointParam == m_poRDVParam.end())
					{	
						//use default
						VMVup* vup = FindVup(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
						if(vup)
						{
							vup->SetGroup(0);
							vup->SetRDVPointID(uiRDVPoint);
							
							runningInfo.m_CurrentGoup = 0;
							runningInfo.m_CurrentNumberOfVUPInGroup++;
							
							D_Output("[RDV Point] add vup to: 0[%d/-]\n", runningInfo.m_CurrentNumberOfVUPInGroup);
						}
					}
					else
					{
						const RDVPointParameter& param = (*itRDVPointParam).second;
						if(runningInfo.m_CurrentNumberOfVUPInGroup + 1 > param.m_iVUPNumInEachGroup)
						{	
							runningInfo.m_CurrentGoup++;
							runningInfo.m_CurrentNumberOfVUPInGroup = 1;
						}
						else
						{
							runningInfo.m_CurrentNumberOfVUPInGroup++;
						}
						VMVup* vup = FindVup(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
						if(vup)
						{
							if(runningInfo.m_CurrentGoup < param.m_iGroupNum)
							{
								vup->SetGroup(runningInfo.m_CurrentGoup);
							}
							else
							{
								vup->SetGroup(-1);
							}
							vup->SetRDVPointID(uiRDVPoint);
							D_Output("[RDV Point] add vup to: %d[%d/%d]\n", runningInfo.m_CurrentGoup, runningInfo.m_CurrentNumberOfVUPInGroup, param.m_iVUPNumInEachGroup);
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
		D_Output("create m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
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
		D_Output("bind m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}
	if(UDT::ERROR == UDT::listen(m_pListeningSocket, 10))
	{
		D_Output("listen m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
		return 1;
	}

	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	UDTSOCKET pClientSocket;
	while (!m_bRequestStop)
	{
		if(UDT::INVALID_SOCK == (pClientSocket = UDT::accept(m_pListeningSocket, (sockaddr*)&clientaddr, &addrlen)))
		{
			D_Output("accept m_pListeningSocket failed: %s\n", UDT::getlasterror().getErrorMessage());
			return 1;
		}
		linger l;
		l.l_linger = 1;
		l.l_onoff = 0;
		UDT::setsockopt(pClientSocket, 0, UDT_LINGER, (int*)&l, sizeof(int));

		D_Output("get client connection from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		m_pMan->AddClientSocket(pClientSocket);

		long addr = inet_addr(inet_ntoa(clientaddr.sin_addr));
		struct hostent* pHostent = gethostbyaddr((char*)&addr, sizeof(long),  AF_INET); 

		VMVup* newVup = new VMVup(-1, pHostent->h_name, ntohs(clientaddr.sin_port));
		newVup->SetClientSocket(pClientSocket);
		m_pMan->AddVupBySocket(newVup);
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
			D_Output("selectEx failed: %s\n", UDT::getlasterror().getErrorMessage());
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
					D_Output("recvmsg failed(%d): %s\n", readSocket, UDT::getlasterror().getErrorMessage());
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
				m_pMan->LostConnection(expectSocket);
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

	D_Output("LostConnection %d\n", _pLostConnection);

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

	UDT::close(_pLostConnection);

	return true;
}	

#endif