#include "VCExportHeader.h"
#include "VCGlobalDef.h"
#include "..\\VUPManager\\VMProtocal.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

namespace{
	Socket*				g_pRecvSocket	 = NULL;
	Socket*				g_pSendSocket	 = NULL;
	Thread*				g_pRecvThread	 = NULL;
	MemPool<UDP_PACK>*	g_pUDPPackBuffer = NULL;

	u16 _GeneratePort(u16 _uiCurrentPort){
		static const u16 suiStartPort	= 50000;
		static const u16 suiEndPort		= 51000;

		if(_uiCurrentPort >= suiEndPort)
			return 0; //failed
		if(_uiCurrentPort >= suiStartPort)
			return (_uiCurrentPort + 1);
		return suiStartPort + (::GetTickCount() % (suiEndPort - suiStartPort));
	}

	const Char*			g_strServerIP	= "127.0.0.1";//"10.192.84.23";//"10.192.84.24";
	u16					g_uiServerPort	= 51001;
};

//--------------------------------------------------------------------------------
class RecvUDPRunner : public IThreadRunner
{
public:
	RecvUDPRunner(Socket* _pRecvSock, MemPool<UDP_PACK>* _pMempool);
	virtual u32		Run();
	virtual void	NotifyQuit();

private:
	MemPool<UDP_PACK>*	m_pUDPPackBuffer;
	Socket*				m_pRecvSocket;
	Bool				m_bRequestStop;
};
RecvUDPRunner::RecvUDPRunner(Socket* _pRecvSock, MemPool<UDP_PACK>* _pMempool)
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
//----------------------------------------------------------------------------------

VUPClientAdapter::VUPClientAdapter()
{
	WSADATA wsaData;
	s32 sRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(sRet)
	{
		D_FatalError("Network init failed\n");
	}
}
VUPClientAdapter::~VUPClientAdapter()
{
	WSACleanup();
}
bool VUPClientAdapter::Init(unsigned int _uiPassport)
{
	m_uiStatus = EVupStatus_Ready;
	m_uiPassport = _uiPassport;

	g_pRecvSocket = new WinSocket;
	s32 iRet = g_pRecvSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	u16 uiPort = _GeneratePort(0);
	do{
		g_pRecvSocket->SetAddress(NULL, uiPort);
		iRet = g_pRecvSocket->Bind();
		if(iRet)
		{
			uiPort = _GeneratePort(uiPort);
			if(!uiPort)
				return false;
		}
		else
		{
			m_uiPort = uiPort;
			break;
		}
	}while(1);

	g_pSendSocket = new WinSocket;
	iRet = g_pSendSocket->Create(E_NETWORK_PROTO_UDP, false);
	D_CHECK(!iRet);
	iRet = g_pSendSocket->SetAddress(g_strServerIP, g_uiServerPort);
	D_CHECK(!iRet);

	//Init mem pool
	g_pUDPPackBuffer = new MemPool<UDP_PACK>();
	g_pUDPPackBuffer->SetMaxSize(1000);

	//Init recv thread
	g_pRecvThread = new Thread(new RecvUDPRunner(g_pRecvSocket, g_pUDPPackBuffer));
	Bool bRet = g_pRecvThread->Start();
	D_CHECK(bRet);

	return true;
}
bool VUPClientAdapter::RegisterMe()
{
	if(g_pSendSocket &&
	   g_pSendSocket->bIsValid())
	{
		UDP_PACK pack;
		pack.m_uiType = EPT_C2M_ClientRegister;
		pack.m_unValue.m_ClientRegisterParam.m_uiPassPort	= m_uiPassport;
		pack.m_unValue.m_ClientRegisterParam.m_uiPort		= m_uiPort;
		pack.m_unValue.m_ClientRegisterParam.m_uiStatus		= m_uiStatus;
		g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
		return true;
	}
	return false;
}
bool VUPClientAdapter::Tick()
{
	s32 iSize = g_pUDPPackBuffer->GetSize();
	if(iSize != 0)
	{
		UDP_PACK *poPacArray = new UDP_PACK[iSize];
		g_pUDPPackBuffer->GetUDPData(poPacArray, iSize);
		for(s32 i = 0; i < iSize; ++i)
		{
			UDP_PACK *poPack = poPacArray + i;
			switch(poPack->m_uiType)
			{
			case EPT_M2C_StartTesting:
				{
					m_uiStatus = EVupStatus_Running;

					UDP_PACK pack;
					pack.m_uiType = EPT_C2M_ReportClientStatus;
					pack.m_unValue.m_ReportClientStatusParam.m_uiPassPort	= m_uiPassport;
					pack.m_unValue.m_ReportClientStatusParam.m_uiStatus		= m_uiStatus;
					g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
					break;
				}
			case EPT_M2C_Refresh:
				{
					UDP_PACK pack;
					pack.m_uiType = EPT_C2M_ClientRegister;
					pack.m_unValue.m_ClientRegisterParam.m_uiPassPort	= m_uiPassport;
					pack.m_unValue.m_ClientRegisterParam.m_uiPort		= m_uiPort;
					pack.m_unValue.m_ClientRegisterParam.m_uiStatus		= m_uiStatus;
					g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
					break;
				}
			case EPT_M2C_KillClient:
				{
					exit(0);
					break;
				}
			}
		}
		delete[] poPacArray;
	}
	return true;
}