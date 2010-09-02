#include "VMVupManager.h"
#include "VMVup.h"
#include "tinyxml.h"
#include <time.h>

//--------------------------------------------------------------------------
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

s32 VMVupManager::StartTesting(const VMCommand::ParamList& _paramList)
{
	s32	iPassport = _paramList[0].ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->StartTesting(iPassport);
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

//--------------------------------------------------------------------------------------------
VMVupManager::VMVupManager()
	: m_pRecvThread(NULL)
	, m_pRecvSocket(NULL)
	, m_pSendSocket(NULL)
	, m_pUDPPackBuffer(NULL)
{
	VMCommandCenter::GetPtr()->RegisterCommand("addvup",		VMVupManager::AddVup,		3);
	VMCommandCenter::GetPtr()->RegisterCommand("updatevup",		VMVupManager::UpdateVup,	3);
	VMCommandCenter::GetPtr()->RegisterCommand("removevup",		VMVupManager::RemoveVup,	1);
	VMCommandCenter::GetPtr()->RegisterCommand("starttesting",	VMVupManager::StartTesting, 1);
	VMCommandCenter::GetPtr()->RegisterCommand("refresh",		VMVupManager::Refresh,		0);
	VMCommandCenter::GetPtr()->RegisterCommand("kill",			VMVupManager::KillClient,	1);

	VMCommand::ParamList defaultParam;
	defaultParam.PushBack(VMCommandParamHolder("-1"));
	VMCommandCenter::GetPtr()->RegisterCommand("killall",		VMVupManager::KillClient,	1, &defaultParam);

	VMCommandCenter::GetPtr()->RegisterCommand("setp",			VMVupManager::SetParameter,	2);
}

VMVupManager::~VMVupManager()
{
	m_pRecvThread->Stop(1000);
	D_SafeDelete(m_pRecvThread);

	m_pRecvSocket->Destroy();
	D_SafeDelete(m_pRecvSocket);

	m_pSendSocket->Destroy();
	D_SafeDelete(m_pSendSocket);

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

Bool VMVupManager::RemoveVup(s32 _id)
{
	VUPMapIterator it = m_poVupMap.find(_id);
	if(it == m_poVupMap.end())
	{
		return false;
	}
	delete (*it).second;
	m_poVupMap.erase(it);
	return true;
}

void VMVupManager::StartTesting(s32 _id)
{
	VMVup* pVup = FindVup(_id);
	if(!pVup)
		return;

	struct tm * nowtime;
	__time64_t  thetime;
	_time64(&thetime);
	nowtime = _localtime64(&thetime);
	int nowsecs = nowtime->tm_hour * 3600 + nowtime->tm_min * 60 + nowtime->tm_sec + m_Parameters.m_iDelayOfStartTime + pVup->GetGroup() * m_Parameters.m_iIntervalOfEachGroup;

	UDP_PACK pack;
	pack.m_uiType = EPT_M2C_StartTesting;
	pack.m_unValue.m_StartTestingParam.m_uiBurstTime = nowsecs;
	m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
	m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
}

void VMVupManager::Refresh()
{
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
}

void VMVupManager::KillClient(s32 _id)
{
	s32 killCount = 1;
	if(_id == -1)
	{
		VUPMapConstIterator it = m_poVupMap.begin();
		for(;it != m_poVupMap.end(); ++it)
		{
			const VMVup* pVup = (*it).second;

			UDP_PACK pack;
			pack.m_uiType = EPT_M2C_KillClient;
			m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
			m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));

			delete pVup;
		}
		m_poVupMap.clear();

		m_poRDVList.clear();
		m_RDVRunningInfo.Reset();
	}
	else
	{
		const VMVup* pVup = FindVup(_id);
		if(!pVup)
			return;
		UDP_PACK pack;
		pack.m_uiType = EPT_M2C_KillClient;
		m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
		m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));

		RemoveVup(_id);
	}
}
void VMVupManager::SetParameter(StringPtr _pOption, const VMCommandParamHolder& _param)
{
	if(!strcmp(_pOption, "-groupnumber") || !strcmp(_pOption, "-gn"))
	{
		m_Parameters.m_iGroupNum = _param.ToInt();
	}
	else if(!strcmp(_pOption, "-interval") || !strcmp(_pOption, "-i"))
	{
		m_Parameters.m_iIntervalOfEachGroup = _param.ToInt();
	}
	else if(!strcmp(_pOption, "-delay") || !strcmp(_pOption, "-d"))
	{
		m_Parameters.m_iDelayOfStartTime = _param.ToInt();
	}
	else if(!strcmp(_pOption, "-vupsnumberingroup") || !strcmp(_pOption, "-vn"))
	{
		m_Parameters.m_iVUPNumInEachGroup = _param.ToInt();
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
		return;

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
#else
	m_pListeningSocket = UDT::socket(AF_INET, SOCK_DGRAM, 0);

#endif
	//iRet = m_pSendSocket->SetAddress(NULL, 52346);
	//D_CHECK(!iRet);

	//Init mem pool
	m_pUDPPackBuffer = new MemPool<UDP_PACKWrapper>();
	m_pUDPPackBuffer->SetMaxSize(1000);

	//Init recv thread
	m_pRecvThread = new Thread(new RecvUDPRunner(m_pRecvSocket, m_pUDPPackBuffer));
	Bool bRet = m_pRecvThread->Start();
	D_CHECK(bRet);

	Refresh();
}
void VMVupManager::Tick(f32 _fDeltaTime)
{
	//Handle udp pack
	_HandleUdpPack();

	//Check rdv point
	_UpdateRDVPoint();

	//Update List
	Event evt((EventType_t)E_ET_UIUpdateList);
	evt.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
}

void VMVupManager::_UpdateRDVPoint()
{
	if(m_RDVRunningInfo.IsValid())
	{
		f32 fNow = GameEngine::GetGameEngine()->GetClockMod()->GetTotalElapsedSeconds();
		RDVPointListIterator it = m_poRDVList.find(m_RDVRunningInfo.m_uiCurrentRunningID);
		D_CHECK(it == m_poRDVList.end());
		const RDVPointInfo& info = (*it).second;
		if(m_RDVRunningInfo.m_ClientList.Size() >= info.m_uiExpectedNum ||
		   fNow - m_RDVRunningInfo.m_fStartTime >= (f32)info.m_uiTimeOut)
		{
			for(s32 i = 0; i < m_RDVRunningInfo.m_ClientList.Size(); ++i)
			{
				StartTesting(m_RDVRunningInfo.m_ClientList[i]);
			}
			m_RDVRunningInfo.Reset();
			m_poRDVList.erase(it);
		}
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
					RDVPointListIterator it = m_poRDVList.find(uiRDVPoint);
					if(it == m_poRDVList.end())
					{
						D_Output("add new rdv point: %d\n", uiRDVPoint);
						RDVPointInfo info;
						info.m_uiID = uiRDVPoint;
						info.m_uiExpectedNum = poPack->m_unValue.m_ReachRDVPointParam.m_uiExpected;
						info.m_uiTimeOut = poPack->m_unValue.m_ReachRDVPointParam.m_uiTimeout;

						m_poRDVList.insert(std::pair<RDVPointID, RDVPointInfo>(uiRDVPoint, info));

						m_RDVRunningInfo.Reset();
						m_RDVRunningInfo.m_bHasValidValue = true;
						m_RDVRunningInfo.m_uiCurrentRunningID = uiRDVPoint;
						m_RDVRunningInfo.m_fStartTime = GameEngine::GetGameEngine()->GetClockMod()->GetTotalElapsedSeconds();
						m_RDVRunningInfo.m_ClientList.PushBack(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
					}
					else
					{
						D_CHECK(m_RDVRunningInfo.m_bHasValidValue);
						D_CHECK(m_RDVRunningInfo.m_uiCurrentRunningID == uiRDVPoint);
						m_RDVRunningInfo.m_ClientList.PushBack(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);

						D_Output("reach: %d\n", poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
					}
					if(m_RDVRunningInfo.m_CurrentNumberOfVUPInGroup + 1 > m_Parameters.m_iVUPNumInEachGroup)
					{	
						m_RDVRunningInfo.m_CurrentGoup++;
						m_RDVRunningInfo.m_CurrentNumberOfVUPInGroup = 1;
					}
					else
					{
						m_RDVRunningInfo.m_CurrentNumberOfVUPInGroup++;
					}
					VMVup* vup = FindVup(poPack->m_unValue.m_ReachRDVPointParam.m_uiPassPort);
					if(vup)
					{
						if(m_RDVRunningInfo.m_CurrentGoup < m_Parameters.m_iGroupNum)
						{
							vup->SetGroup(m_RDVRunningInfo.m_CurrentGoup);
						}
						else
						{
							vup->SetGroup(m_Parameters.m_iGroupNum);
						}
					}
				}
				break;
			}
		}
		delete[] poPacArray;
	}
}
