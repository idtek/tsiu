#include "VCExportHeader.h"
#include "VCGlobalDef.h"
#include "tinyxml.h"
#include "udt.h"
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

namespace{

	class OutRedirector;
		
#ifndef USE_UDT_LIB
	Socket*				g_pRecvSocket	 = NULL;
	Socket*				g_pSendSocket	 = NULL;
#endif
	Thread*				g_pRecvThread	 = NULL;
	MemPool<UDP_PACK>*	g_pUDPPackBuffer = NULL;

	std::string			g_strServerIP	= "127.0.0.1";//10.192.84.23";//"10.192.84.24";
	u16					g_uiServerPort	= 51001;
	u16					g_uiStartPort	= 50000;
	u16					g_uiEndPort		= 50050;

	std::string			g_strLogDir;

	Char				g_strLogName[MAX_PATH];
	Char				g_strLocalLogName[MAX_PATH];
	Bool				g_IsLogRedirected = false;
	OutRedirector*		g_fp = NULL;

#ifdef USE_UDT_LIB
	u16					g_uiServerTransferLogPort	= 51002;
	UDTSOCKET			g_pRecvSocket = UDT::INVALID_SOCK;
#endif

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
		TiXmlElement* serverIP = root->FirstChildElement("serverip");
		g_strServerIP = serverIP->GetText();
		
		TiXmlElement* serverPort = root->FirstChildElement("serverport");
		g_uiServerPort = atoi(serverPort->GetText());

#ifdef USE_UDT_LIB
		TiXmlElement* serverLogPort = root->FirstChildElement("serverlogport");
		g_uiServerTransferLogPort = atoi(serverLogPort->GetText());
#endif

		TiXmlElement* clientStartPort = root->FirstChildElement("clientstartport");
		g_uiStartPort = atoi(clientStartPort->GetText());

		TiXmlElement* clientEndPort = root->FirstChildElement("clientendport");
		g_uiEndPort = atoi(clientEndPort->GetText());

		TiXmlElement* logDir = root->FirstChildElement("logdir");
		g_strLogDir = logDir->GetText();

		TiXmlElement* logRedirect = root->FirstChildElement("logredirect");
		g_IsLogRedirected = (atoi(logRedirect->GetText()) != 0);

		D_Output("Client Parameter:\nServer IP: %s\nServer Port: %d\nClient Start Port = %d\nClient End Port = %d\nLog Dir = %s\n", 
			g_strServerIP.c_str(), g_uiServerPort, g_uiStartPort, g_uiEndPort, g_strLogDir.c_str());

		delete pConfigFile;

		return true;
	}

	class OutRedirector {
	public:
		OutRedirector(std::string filename){
			while(1)
			{
				_redirectedStream.open(filename.c_str(), std::ios::app);
				if(_redirectedStream.is_open())
					break;
				else
				{
					D_Output("[INFO] waiting for opening log file...\n");
					Sleep(200);
				}
			}
		}

		~OutRedirector() {
			_redirectedStream.close();
		}
		std::ofstream& Get(){
			D_CHECK(_redirectedStream.is_open());
			return _redirectedStream;
		}
	private:
		std::ofstream _redirectedStream;
	};
};

#ifdef DISABLE_LOG_ALL
	#define DISABLE_LOG_1
	#define DISABLE_LOG_2
#endif

#define DISABLE_LOG_2

#define LOG(expr) while(1)\
	{\
		OutRedirector dir(g_strLogName);\
		dir.Get() << expr << std::endl;\
		if(!dir.Get().fail())\
		{\
			break;\
		}\
		else\
		{\
			Sleep(200);\
		}\
	}\

#ifndef DISABLE_LOG_1
#define LOG_1(expr)		LOG(expr)
#else
#define LOG_1(expr)
#endif

#ifndef DISABLE_LOG_2
#define LOG_2(expr)		LOG(expr)
#else
#define LOG_2(expr)
#endif

//--------------------------------------------------------------------------------
class RecvUDPRunner : public IThreadRunner
{
public:
#ifndef USE_UDT_LIB
	RecvUDPRunner(Socket* _pRecvSock, MemPool<UDP_PACK>* _pMempool);
#else
	RecvUDPRunner(UDTSOCKET _pRecvSock, MemPool<UDP_PACK>* _pMempool);
#endif
	virtual u32		Run();
	virtual void	NotifyQuit();

private:
	MemPool<UDP_PACK>*	m_pUDPPackBuffer;
#ifndef USE_UDT_LIB
	Socket*				m_pRecvSocket;
#else
	UDTSOCKET			m_pRecvSocket;
#endif
	Bool				m_bRequestStop;
};
#ifndef USE_UDT_LIB
RecvUDPRunner::RecvUDPRunner(Socket* _pRecvSock, MemPool<UDP_PACK>* _pMempool)
	:m_pRecvSocket(_pRecvSock)
#else
RecvUDPRunner::RecvUDPRunner(UDTSOCKET _pRecvSock, MemPool<UDP_PACK>* _pMempool)
	:m_pRecvSocket(_pRecvSock)
#endif
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

#ifndef USE_UDT_LIB
		if(!m_pRecvSocket || !m_pRecvSocket->bIsValid())
			return 1;

		UDP_PACK pack;
		s32 iRet = m_pRecvSocket->RecvFrom((Char*)&pack, sizeof(UDP_PACK));
		if(!iRet)
#else
		if(m_pRecvSocket == UDT::INVALID_SOCK)
			return 1;

		UDP_PACK pack;
		s32 iRet = UDT::recvmsg(m_pRecvSocket, (char*)&pack, sizeof(UDP_PACK));
		if(iRet == UDT::ERROR)
		{
			LOG_1("recvmsg failed(" << m_pRecvSocket << "): " << UDT::getlasterror().getErrorMessage());
			return 1;
		}
		else
#endif
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
#ifndef USE_UDT_LIB
	WSADATA wsaData;
	s32 sRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(sRet)
	{
		D_FatalError("Network init failed\n");
	}
#else
	UDT::startup();
#endif
	for(int i = 0; i < EPT_Num; ++i)
	{
		m_PackFunctions[i] = NULL;
		m_UDPPackHandler[i] = NULL;
	}
}
VUPClientAdapter::~VUPClientAdapter()
{
	g_pRecvThread->Stop(1000);
	D_SafeDelete(g_pRecvThread);
	D_SafeDelete(g_pUDPPackBuffer);
	D_SafeDelete(g_fp);

#ifndef USE_UDT_LIB
	WSACleanup();
#else
	UDT::close(g_pRecvSocket);
	UDT::cleanup();
#endif
}
bool VUPClientAdapter::Init(unsigned int _uiPassport)
{
	Bool isOK = _InitParameter();
	if(!isOK)
	{
		D_Output("[ERROR] read configuration file failed\n");
		return false;
	}

	if(g_IsLogRedirected)
	{
		struct tm * timeNow;
		__time64_t  thetime;
		_time64(&thetime);
		timeNow = _localtime64(&thetime);

		Char strHostName[255];
		gethostname(strHostName, sizeof(strHostName));
		sprintf(g_strLogName, "%s\\%s_%d_%d_%d_%d_%d_%d_%d.log", g_strLogDir.c_str(), strHostName, 
			timeNow->tm_year + 1900, timeNow->tm_mon + 1, timeNow->tm_mday, timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec, ::GetCurrentProcessId());
		D_Output("Out put log name: %s\n", g_strLogName);
	}

	//Init status
	m_uiStatus = EVupStatus_Invalid;
	m_uiPassport = _uiPassport;
	m_uiTestPhase = ETestPhase_INVALID;

	//Init pack func
	m_PackFunctions[EPT_C2M_ReportClientStatus]	= &VUPClientAdapter::_PACK_ReportClientStatus;

	//Init mem pool
	g_pUDPPackBuffer = new MemPool<UDP_PACK>();
	g_pUDPPackBuffer->SetMaxSize(1000);

#ifndef USE_UDT_LIB
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
#else
	g_pRecvSocket = UDT::socket(AF_INET, SOCK_DGRAM, 0);
	if(g_pRecvSocket == UDT::INVALID_SOCK)
	{
		LOG_1("create socket failed: " << UDT::getlasterror().getErrorMessage());
		return false;
	}
	//Decrease
	//s32 delayConnect = ::GetCurrentProcessId();
	//s32 delayInterval = delayConnect % 10 * 500;
	//s32 lastTickCount = ::GetTickCount();
	//while(1){
	//	s32 tickCount = ::GetTickCount();
	//	if(tickCount - lastTickCount > delayInterval)
	//		break;
	//	else
	//		Sleep(10);
	//}
	sockaddr_in addrinfo;
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.S_un.S_addr = inet_addr(g_strServerIP.c_str());
	addrinfo.sin_port = htons(g_uiServerPort);
	while(1)
	{
		if(UDT::ERROR == UDT::connect(g_pRecvSocket, (struct sockaddr*)&addrinfo, sizeof(addrinfo)))
		{
			LOG_1("[ERROR] connected failed: " << UDT::getlasterror().getErrorMessage() << ", waiting for reconnecting");
		}
		else
		{
			LOG_1("[INFO] connected successfully: " << g_strServerIP.c_str() << ":" << g_uiServerPort);
			sockaddr_in addrLocal;
			s32 addrLen = sizeof(sockaddr_in);
			if(UDT::ERROR == UDT::getsockname(g_pRecvSocket, (struct sockaddr*)&addrLocal, &addrLen))
			{
				LOG_1("[ERROR] getsockname failed: " << UDT::getlasterror().getErrorMessage());
			}
			else
			{
				LOG_1("[INFO] client port = " << htons(addrLocal.sin_port));
			}
			break;
		}
		Sleep(3000);
	}
#endif
	//Init recv thread
	g_pRecvThread = new Thread(new RecvUDPRunner(g_pRecvSocket, g_pUDPPackBuffer));
	Bool bRet = g_pRecvThread->Start();
	D_CHECK(bRet);

	return true;
}
void VUPClientAdapter::OutputLog(const char* _log)
{
	LOG_2(_log);
}

bool VUPClientAdapter::RegisterMe()
{
#ifndef USE_UDT_LIB
	D_CHECK(g_pSendSocket && g_pSendSocket->bIsValid());
#else
	D_CHECK(g_pRecvSocket != UDT::INVALID_SOCK);
#endif
	{
		UDP_PACK pack;
		pack.m_uiType = EPT_C2M_ClientRegister;
		pack.m_unValue.m_ClientRegisterParam.m_uiPassPort	= m_uiPassport;
		pack.m_unValue.m_ClientRegisterParam.m_uiPort		= m_uiPort;
		pack.m_unValue.m_ClientRegisterParam.m_uiStatus		= m_uiStatus;
		pack.m_unValue.m_ClientRegisterParam.m_uiTestPhase	= m_uiTestPhase;

#ifndef USE_UDT_LIB
		g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
		return true;
#else
		s32 iRet = UDT::sendmsg(g_pRecvSocket, (const Char*)&pack, sizeof(UDP_PACK));
		if(iRet == UDT::ERROR)
		{
			LOG_1("[ERROR] sendmsg failed: " << UDT::getlasterror().getErrorMessage());
			return false;
		}
		return true;
#endif
	}
}

void VUPClientAdapter::Watch(unsigned char _uiKey, const char* _zName, const unsigned short* _watchedvalue)
{
	WatchedValue<unsigned short>* wV = new WatchedValue<unsigned short>(_uiKey);
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
	D_Unused(_uiExpected);
	D_Unused(_uiTimeout);

	UDP_PACK pack;
	pack.m_uiType = EPT_C2M_ReachRDVPoint;
	pack.m_unValue.m_ReachRDVPointParam.m_uiPassPort	= m_uiPassport;
	pack.m_unValue.m_ReachRDVPointParam.m_uiRDVPointID	= _uiRDVPointID;
	//pack.m_unValue.m_ReachRDVPointParam.m_uiTimeout		= _uiTimeout;
	//pack.m_unValue.m_ReachRDVPointParam.m_uiExpected	= _uiExpected;

#ifndef USE_UDT_LIB
	g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
#else
	s32 iRet = UDT::sendmsg(g_pRecvSocket, (const Char*)&pack, sizeof(UDP_PACK));
	if(iRet == UDT::ERROR)
	{
		LOG_1("[ERROR] sendmsg failed: " << UDT::getlasterror().getErrorMessage());
	}
#endif
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
#define CHECK_RET(b) {if(!(b)) return false;}
	bool bRet;
	if(HasConnectToManager())
	{
		bRet = _HandleWatchedValue();
		CHECK_RET(bRet);
		bRet = _HandleRecvPack();
		CHECK_RET(bRet);
	}
	else
	{
		bRet = _WaitForRegisterACK();
		CHECK_RET(bRet);
	}
	return true;
}
bool VUPClientAdapter::_WaitForRegisterACK()
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
			switch(packType)
			{
			case EPT_M2C_ClientRegisterACK:
				{
					if(poPack->m_unValue.m_RegisterAckParam.m_uiHasSuccessed)
					{
						LOG_1("[INFO] get register ack successfully");
						m_HasConnectedToManager = true;
					}
					else
					{
						LOG_1("[INFO] get register ack failed, for using duplicate passport");
						m_HasConnectedToManager = false;

						return false;
					}
				}
				break;
			default:
				{
					LOG_1("[ERROR] get msg when not connected to manager yet: " << packType);
					g_pUDPPackBuffer->InsertUDPData(*poPack);
					break;
				}
			}
		}
		delete[] poPacArray;
	}
	return true;
}
bool VUPClientAdapter::_HandleRecvPack()
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
			case EPT_M2C_Refresh:
				{
					m_HasConnectedToManager = false;

					UDP_PACK pack;
					pack.m_uiType = EPT_C2M_ClientRegister;
					pack.m_unValue.m_ClientRegisterParam.m_uiPassPort	= m_uiPassport;
					pack.m_unValue.m_ClientRegisterParam.m_uiPort		= m_uiPort;
					pack.m_unValue.m_ClientRegisterParam.m_uiStatus		= m_uiStatus;
					pack.m_unValue.m_ClientRegisterParam.m_uiTestPhase	= m_uiTestPhase;
#ifndef USE_UDT_LIB
					g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
#else
					s32 iRet = UDT::sendmsg(g_pRecvSocket, (const Char*)&pack, sizeof(UDP_PACK));
					if(iRet == UDT::ERROR)
					{
						LOG_1("[ERROR] sendmsg failed: " << UDT::getlasterror().getErrorMessage());
					}
#endif
					break;
				}
			case EPT_M2C_KillClient:
				{
					LOG_1("[INFO] get \"kill\" command from manager, wait for quit...");

					exit(0);

					break;
				}
			case EPT_M2C_StartTesting:
				{
					//__int64 nowms = poPack->m_unValue.m_StartTestingParam.m_uiBurstTime;
					//__int64 nows = nowms / 1000;
					//char timeline[26];
					//ctime_s(timeline, 26, &(nows));
					//char timeStr[64] = {};
					//sprintf(timeStr, "%.19s.%03hu", timeline, nowms % 1000);

					//OutRedirector dir(g_strLogName);
					//dir.Get() << "[INFO] get start testing: " << timeStr << std::endl;
					break;
				}
			case EPT_M2C_LogRequest:
				{
					_SendingLog();
					break;
				}
			}
		}
		delete[] poPacArray;
	}
	return true;
}

void VUPClientAdapter::_SendingLog()
{
	UDTSOCKET pSendLogSocket = UDT::socket(AF_INET, SOCK_STREAM, 0);
	if(pSendLogSocket == UDT::INVALID_SOCK)
	{
		LOG_1("create socket failed: " << UDT::getlasterror().getErrorMessage());
		return;
	}
	sockaddr_in addrinfo;
	addrinfo.sin_family = AF_INET;
	addrinfo.sin_addr.S_un.S_addr = inet_addr(g_strServerIP.c_str());
	addrinfo.sin_port = htons(g_uiServerTransferLogPort);
	if(UDT::ERROR == UDT::connect(pSendLogSocket, (struct sockaddr*)&addrinfo, sizeof(addrinfo)))
	{
		LOG_1("[ERROR] connected failed: " << UDT::getlasterror().getErrorMessage());
		return;
	}
	std::fstream ifs(g_strLogName, std::ios::in | std::ios::binary);
	int64_t size;
	if(ifs.is_open())
	{
		ifs.seekg(0, std::ios::end);
		size = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
	}
	else
	{
		size = -1; //failed to open file
	}
	if (UDT::ERROR == UDT::send(pSendLogSocket, (char*)&size, sizeof(int64_t), 0))
	{
		UDT::close(pSendLogSocket);
		ifs.close();
		LOG_1("[ERROR] send failed" << UDT::getlasterror().getErrorMessage());
		return;
	}
	if(size >= 0)
	{
		if (UDT::ERROR == UDT::sendfile(pSendLogSocket, ifs, 0, size))
		{
			LOG_1("[ERROR] sendfile failed" << UDT::getlasterror().getErrorMessage());
		}
		else
		{
			LOG_1("[INFO] send log file successfully");
		}
		ifs.close();
	}
	UDT::close(pSendLogSocket);
}

bool VUPClientAdapter::_HandleWatchedValue()
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

#ifndef USE_UDT_LIB
			g_pSendSocket->SendTo((const Char*)&pack, sizeof(UDP_PACK));
#else
			s32 iRet = UDT::sendmsg(g_pRecvSocket, (const Char*)&pack, sizeof(UDP_PACK));
			if(iRet == UDT::ERROR)
			{
				LOG_1("[ERROR] sendmsg failed: " << UDT::getlasterror().getErrorMessage());
			}
#endif
		}
	}
	return true;
}

void VUPClientAdapter::_PACK_ReportClientStatus(UDP_PACK* outPack, WatchedNameValueMap& nameValue)
{
	u16 uiStatus =  *static_cast<const u16*>(nameValue[kNAME_ReportClientStatus]->GetValue());

	m_uiStatus = (u8)(uiStatus & 0x00FF);
	m_uiTestPhase = (u8)((uiStatus >> 8) & 0x00FF);

	outPack->m_unValue.m_ReportClientStatusParam.m_uiPassPort = m_uiPassport;
	outPack->m_unValue.m_ReportClientStatusParam.m_uiRunningStatus = m_uiStatus;
	outPack->m_unValue.m_ReportClientStatusParam.m_uiTestPhase = m_uiTestPhase;
}