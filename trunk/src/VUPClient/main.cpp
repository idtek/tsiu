#include "VCExportHeader.h"
#include "VCGlobalDef.h"
#include "tinyxml.h"

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

	std::string			g_strServerIP	= "127.0.0.1";//10.192.84.23";//"10.192.84.24";
	u16					g_uiServerPort	= 51001;
	u16					g_uiStartPort	= 50000;
	u16					g_uiEndPort		= 50050;

	u16 _GeneratePort(u16 _uiCurrentPort){
		if(_uiCurrentPort >= g_uiEndPort)
			return 0; //failed
		if(_uiCurrentPort >= g_uiStartPort)
			return (_uiCurrentPort + 1);
		return g_uiStartPort;// + (::GetTickCount() % (suiEndPort - suiStartPort));
	}

	Bool _InitParameter(){
		//Init config file
		TiXmlDocument* pConfigFile = new TiXmlDocument();
		bool isOK = pConfigFile->LoadFile("VUPClientAdapterConfig.xml");
		if(!isOK)
		{
			delete pConfigFile;
			return false;
		}
		
		TiXmlElement* root = pConfigFile->RootElement();

		//ServerIP;
		TiXmlElement* serverIP = root->FirstChildElement();
		g_strServerIP = serverIP->GetText();
		
		TiXmlElement* serverPort = serverIP->NextSiblingElement();
		g_uiServerPort = atoi(serverPort->GetText());

		TiXmlElement* clientStartPort = serverPort->NextSiblingElement();
		g_uiStartPort = atoi(clientStartPort->GetText());

		TiXmlElement* clientEndPort = clientStartPort->NextSiblingElement();
		g_uiEndPort = atoi(clientEndPort->GetText());

		D_Output("Client Parameter:\nServer IP: %s\nServer Port: %d\nClient Start Port = %d\nClient End Port = %d\n", g_strServerIP.c_str(), g_uiServerPort, g_uiStartPort, g_uiEndPort);

		delete pConfigFile;

		return true;
	}
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
	: m_HasConnectedToManager(false) 
{
	WSADATA wsaData;
	s32 sRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(sRet)
	{
		D_FatalError("Network init failed\n");
	}
	for(int i = 0; i < EPT_Num; ++i)
	{
		m_PackFunctions[i] = NULL;
		m_UDPPackHandler[i] = NULL;
	}
}
VUPClientAdapter::~VUPClientAdapter()
{
	WSACleanup();
}
bool VUPClientAdapter::Init(unsigned int _uiPassport)
{
	Bool isOK = _InitParameter();
	if(!isOK)
	{
		D_Output("read configuration file failed\n");
		return false;
	}

	//Init status
	m_uiStatus = EVupStatus_Invalid;
	m_uiPassport = _uiPassport;
	m_uiTestPhase = ETestPhase_INVALID;

	//Init pack func
	m_PackFunctions[EPT_C2M_ReportClientRunningStatus]	= &VUPClientAdapter::_PACK_ReportClientRunningStatus;
	m_PackFunctions[EPT_C2M_ReportClientTesingPhase]	= &VUPClientAdapter::_PACK_ReportClientTesingPhase;

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
			{
				D_Output("generate port failed\n");
				return false;
			}
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
	iRet = g_pSendSocket->SetAddress(g_strServerIP.c_str(), g_uiServerPort);
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
		pack.m_unValue.m_ClientRegisterParam.m_uiStatus		= m_uiPassport;
		g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
		return true;
	}
	return false;
}

void VUPClientAdapter::Watch(unsigned char _uiKey, const char* _zName, const int* _watchedvalue)
{
	WatchedValue<int>* wV = new WatchedValue<int>(_uiKey);
	wV->Set(_watchedvalue);
	WatchedNameValueMap* pNameValueMap = NULL;

	WatchedValueMapIterator it = m_WatchValues.find(_uiKey);
	if(it == m_WatchValues.end())
	{
		pNameValueMap = new WatchedNameValueMap;
		m_WatchValues.insert(std::pair<unsigned char, WatchedNameValueMap*>(_uiKey, pNameValueMap));
	}
	else
	{
		pNameValueMap = (*it).second;
	}
	pNameValueMap->insert(std::pair<std::string, WatchedValueBase*>(_zName, wV));
}

void VUPClientAdapter::ReachRDVPoint(unsigned short _uiRDVPointID, unsigned short _uiExpected, unsigned short _uiTimeout)
{
	UDP_PACK pack;
	pack.m_uiType = EPT_C2M_ReachRDVPoint;
	pack.m_unValue.m_ReachRDVPointParam.m_uiPassPort	= m_uiPassport;
	pack.m_unValue.m_ReachRDVPointParam.m_uiRDVPointID	= _uiRDVPointID;
	pack.m_unValue.m_ReachRDVPointParam.m_uiTimeout		= _uiTimeout;
	pack.m_unValue.m_ReachRDVPointParam.m_uiExpected	= _uiExpected;
	g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
}

void VUPClientAdapter::RegisterUDPPackHandler(unsigned char _uiType, UdpPackHandler _pPackHandler)
{
	if(_uiType >= 0 && _uiType < EPT_Num)
	{
		m_UDPPackHandler[_uiType] = _pPackHandler;
	}
}

bool VUPClientAdapter::Tick()
{
	_HandleWatchedValue();
	_HandleRecvPack();
	return true;
}
void VUPClientAdapter::_HandleRecvPack()
{
	s32 iSize = g_pUDPPackBuffer->GetSize();
	if(iSize != 0)
	{
		UDP_PACK *poPacArray = new UDP_PACK[iSize];
		g_pUDPPackBuffer->GetUDPData(poPacArray, iSize);
		for(s32 i = 0; i < iSize; ++i)
		{
			UDP_PACK *poPack = poPacArray + i;
			u8 packType = poPack->m_uiType;

			UdpPackHandler userFunc = m_UDPPackHandler[packType];
			if(userFunc)
			{
				(*userFunc)(*poPack);
			}
			switch(packType)
			{
			case EPT_M2C_ClientRegisterACK:
				{
					m_HasConnectedToManager = true;
				}
				break;
			case EPT_M2C_Refresh:
				{
					m_HasConnectedToManager = false;

					UDP_PACK pack;
					pack.m_uiType = EPT_C2M_ClientRegister;
					pack.m_unValue.m_ClientRegisterParam.m_uiPassPort	= m_uiPassport;
					pack.m_unValue.m_ClientRegisterParam.m_uiPort		= m_uiPort;
					pack.m_unValue.m_ClientRegisterParam.m_uiStatus		= m_uiStatus;
					pack.m_unValue.m_ClientRegisterParam.m_uiTestPhase	= m_uiTestPhase;
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
}

void VUPClientAdapter::_HandleWatchedValue()
{
	WatchedValueMapIterator itWV;
	for(itWV = m_WatchValues.begin(); itWV != m_WatchValues.end(); ++itWV)
	{
		WatchedNameValueMap* pNameValueMap = (*itWV).second;
		WatchedNameValueMapIterator innerIt;
	
		Bool shouldSendMsg = false;
		for(innerIt = pNameValueMap->begin(); innerIt != pNameValueMap->end(); ++innerIt)
		{
			WatchedValueBase* wV = (*innerIt).second;
			if(wV->HasChanged())
			{
				shouldSendMsg = true;
				wV->Flush();
			}
		}
		if(shouldSendMsg)
		{
			UDP_PACK pack;
			pack.m_uiType = (*itWV).first;
			Pack_Func func = m_PackFunctions[pack.m_uiType];
			if(func)
				(this->*func)(&pack, *pNameValueMap);
			else
			{
				D_CHECK(0);
			}
			g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
		}
	}
}

void VUPClientAdapter::_PACK_ReportClientTesingPhase(UDP_PACK* outPack, WatchedNameValueMap& nameValue)
{
	m_uiTestPhase = (u8)*static_cast<const int*>(nameValue[kNAME_ReportClientTesingPhase_Phase]->GetValue());

	outPack->m_unValue.m_ReportClientTesingPhaseParam.m_uiPassPort = m_uiPassport;
	outPack->m_unValue.m_ReportClientTesingPhaseParam.m_uiPhase = m_uiTestPhase;
}

void VUPClientAdapter::_PACK_ReportClientRunningStatus(UDP_PACK* outPack, WatchedNameValueMap& nameValue)
{
	m_uiStatus = (u8)*static_cast<const int*>(nameValue[kNAME_ReportClientRunningStatus_Phase]->GetValue());

	outPack->m_unValue.m_ReportClientRunningStatusParam.m_uiPassPort = m_uiPassport;
	outPack->m_unValue.m_ReportClientRunningStatusParam.m_uiStatus = m_uiStatus;
}