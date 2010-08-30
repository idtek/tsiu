#include "VMVupManager.h"
#include "VMVup.h"

 Mutex g_pMutex;
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
s32 VMVupManager::AddVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3)
{
	s32 iPassport = _p1.ToInt();
	const Char* strIPAddr = _p2.ToString();
	u16 uiPort = static_cast<u16>(_p3.ToInt());

	VMVup* newVup = new VMVup(iPassport, strIPAddr, uiPort);
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	Bool bRet = pMan->AddVup(newVup);
	if(!bRet)
	{
		return 1;
	}
	return 0;
}

s32 VMVupManager::UpdateVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3)
{
	s32	iPassport = _p1.ToInt();
	u8	uiStatus = static_cast<u8>(_p2.ToInt());
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	VMVup* newVup = pMan->FindVup(iPassport);
	if(newVup)
	{
		newVup->SetStatus(uiStatus);
		return 0;
	}
	return 1;
}

s32 VMVupManager::RemoveVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3)
{
	s32	iPassport = _p1.ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->RemoveVup(iPassport);
	return 0;
}

s32 VMVupManager::StartTesting(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3)
{
	s32	iPassport = _p1.ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->StartTesting(iPassport);
	return 0;
}

s32 VMVupManager::Refresh(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2, const VMCommandParamHolder& _p3)
{
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->Refresh();
	return 0;
}

//--------------------------------------------------------------------------------------------
VMVupManager::VMVupManager()
	: m_pRecvThread(NULL)
	, m_pRecvSocket(NULL)
	, m_pSendSocket(NULL)
	, m_pUDPPackBuffer(NULL)
{
	VMCommandCenter::GetPtr()->RegisterCommand("addvup",	VMVupManager::AddVup, VMCommand::EParamType_Int, VMCommand::EParamType_String, VMCommand::EParamType_Int);
	VMCommandCenter::GetPtr()->RegisterCommand("updatevup", VMVupManager::UpdateVup, VMCommand::EParamType_Int, VMCommand::EParamType_Int);
	VMCommandCenter::GetPtr()->RegisterCommand("removevup", VMVupManager::RemoveVup, VMCommand::EParamType_Int);
	VMCommandCenter::GetPtr()->RegisterCommand("starttesting", VMVupManager::StartTesting, VMCommand::EParamType_Int);
	VMCommandCenter::GetPtr()->RegisterCommand("refresh", VMVupManager::Refresh);
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
	const VMVup* pVup = FindVup(_id);
	if(!pVup)
		return;

	UDP_PACK pack;
	pack.m_uiType = EPT_M2C_StartTesting;
	m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
	m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
}

void VMVupManager::Refresh()
{
	VUPMapConstIterator it = m_poVupMap.begin();
	for(;it != m_poVupMap.end(); ++it)
	{
		const VMVup* pVup = (*it).second;

		UDP_PACK pack;
		pack.m_uiType = EPT_M2C_Refresh;
		m_pSendSocket->SetAddress(pVup->GetIPAddress(), pVup->GetPort());
		m_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));

		delete pVup;
	}
	m_poVupMap.clear();
}

void VMVupManager::Create()
{
	//Init network
	m_pRecvSocket = new WinSocket;
	s32 iRet = m_pRecvSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	iRet = m_pRecvSocket->SetAddress(NULL, 51001);
	D_CHECK(!iRet);
	iRet = m_pRecvSocket->Bind();
	D_CHECK(!iRet);

	m_pSendSocket = new WinSocket;
	iRet = m_pSendSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	//iRet = m_pSendSocket->SetAddress(NULL, 52346);
	//D_CHECK(!iRet);

	//Init mem pool
	m_pUDPPackBuffer = new MemPool<UDP_PACKWrapper>();
	m_pUDPPackBuffer->SetMaxSize(1000);

	//Init recv thread
	m_pRecvThread = new Thread(new RecvUDPRunner(m_pRecvSocket, m_pUDPPackBuffer));
	Bool bRet = m_pRecvThread->Start();
	D_CHECK(bRet);
}
void VMVupManager::Tick(f32 _fDeltaTime)
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
					Char cmd[512];
					sprintf(cmd, "addvup %d %s %d",	poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort,
													poPackWrapper->m_SrcIPAddress.c_str(),
													poPack->m_unValue.m_ClientRegisterParam.m_uiPort);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);

					sprintf(cmd, "updatevup %d %d",	poPack->m_unValue.m_ClientRegisterParam.m_uiPassPort,
													poPack->m_unValue.m_ClientRegisterParam.m_uiStatus);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);
				}
				break;
			case EPT_C2M_ReportClientStatus:
				{
					Char cmd[512];
					sprintf(cmd, "updatevup %d %d",	poPack->m_unValue.m_ReportClientStatusParam.m_uiPassPort,
													poPack->m_unValue.m_ReportClientStatusParam.m_uiStatus);
					VMCommandCenter::GetPtr()->ExecuteFromString(cmd);
				}
				break;
			}
		}
		delete[] poPacArray;
	}

	//Update List
	Event evt((EventType_t)E_ET_UIUpdateList);
	evt.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
}
