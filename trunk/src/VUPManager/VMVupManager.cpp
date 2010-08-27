#include "VMVupManager.h"
#include "VMVup.h"

//--------------------------------------------------------------------------
RecvUDPRunner::RecvUDPRunner(Socket* _pRecvSock, MemPool* _pMempool)
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

		UDP_PACK pack;
		s32 iRet = m_pRecvSocket->RecvFrom((Char*)&pack, sizeof(UDP_PACK));
		if(!iRet)
		{
			m_pUDPPackBuffer->InsertUDPData(pack);
		}
	}
}

void RecvUDPRunner::NotifyQuit()
{
	m_bRequestStop = true;
}
//--------------------------------------------------------------------------------------------
s32 VMVupManager::AddVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2)
{
	VMVup* newVup = new VMVup(VMVup::GenerateUniqueID());
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	Bool bRet = pMan->AddVup(newVup);
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
	VMVup* newVup = new VMVup(vupID, vupStatus);
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	Bool bRet = pMan->AddVup(newVup);
	if(!bRet)
	{
		delete newVup;
	}
	return 0;
}

s32 VMVupManager::RemoveVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2)
{
	s32	vupID = _p1.ToInt();
	VMVupManager* pMan = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<VMVupManager>("VUMMan");
	pMan->RemoveVup(vupID);
	return 0;
}

//--------------------------------------------------------------------------------------------
VMVupManager::VMVupManager()
	: m_pRecvThread(NULL)
	, m_pRecvSocket(NULL)
	, m_pSendSocket(NULL)
	, m_pUDPPackBuffer(NULL)
{
	VMCommandCenter::GetPtr()->RegisterCommand("addvup",	VMVupManager::AddVup);
	VMCommandCenter::GetPtr()->RegisterCommand("updatevup", VMVupManager::UpdateVup, VMCommand::EParamType_Int, VMCommand::EParamType_Int);
	VMCommandCenter::GetPtr()->RegisterCommand("removevup", VMVupManager::RemoveVup, VMCommand::EParamType_Int);
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
	m_poVupMap.erase(it);
	return true;
}

void VMVupManager::Create()
{
	//Init network
	m_pRecvSocket = new WinSocket;
	s32 iRet = m_pRecvSocket->Create(E_NETWORK_PROTO_UDP, true);
	D_CHECK(!iRet);
	iRet = m_pRecvSocket->Bind(NULL, 52345);
	D_CHECK(!iRet);

	m_pSendSocket = new WinSocket;
	iRet = m_pSendSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);

	//Init mem pool
	m_pUDPPackBuffer = new MemPool();
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
		UDP_PACK *poPacArray = new UDP_PACK[iSize];
		m_pUDPPackBuffer->GetUDPData(poPacArray, iSize);
		for(s32 i = 0; i < iSize; ++i)
		{
			UDP_PACK *poPack = poPacArray + i;

		}
		delete[] poPacArray;
	}

	//Update List
	Event evt((EventType_t)E_ET_UIUpdateList);
	evt.AddParam((void*)this);
	GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
}
