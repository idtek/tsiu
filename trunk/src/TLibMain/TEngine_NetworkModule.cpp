#include "TEngine_NetworkModule.h"
#include "TCore_LibSettings.h"
#include "TNetwork_Socket.h"
#include "TCore_Exception.h"

#if PLATFORM_TYPE == PLATFORM_WIN32
#include <winsock2.h>
#endif

namespace TsiU
{
	NetworkModule::NetworkModule(u32 _ulMaxClient)
		:m_ulMaxClientNumber(_ulMaxClient)
	{
		m_poMine = NULL;
		m_ulPeerType = E_PeerType_Offline;
	}

	void NetworkModule::RunOneFrame(float _fDeltaTime)
	{
		//m_ulRS % 2 ? _RecvPart() : _SendPart();
		//m_ulRS++;
	}

	void NetworkModule::Init()
	{
#if PLATFORM_TYPE == PLATFORM_WIN32
		WSADATA wsaData;
		s32 sRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if(sRet)
		{
			D_FatalError("Network init failed\n");
		}
#endif
	}
	
	void NetworkModule::UnInit()
	{
#if PLATFORM_TYPE == PLATFORM_WIN32
		WSACleanup();
#endif
	}
	
	void NetworkModule::StartServer(u32 _ulPort)
	{
		if(bIsOffline())
		{
			m_poMine = CallCreator<Socket>(E_CreatorType_Socket);
			m_poMine->Create(E_NETWORK_PROTO_TCP, true);
			m_poMine->SetAddress(NULL, _ulPort);
			m_poMine->Bind();
			m_poMine->Listen();

			m_ulPeerType = E_PeerType_Server;
		}
	}

	void NetworkModule::ConnectServer(const Char* _strAddr, u32 _ulPort)
	{
		if(!bIsServer())
		{
			m_poMine = CallCreator<Socket>(E_CreatorType_Socket);
			m_poMine->Create(E_NETWORK_PROTO_TCP, true);
			m_poMine->Connect(_strAddr, _ulPort);

			m_ulPeerType = E_PeerType_Client;
		}
	}

	void NetworkModule::ShutDownServer()
	{
		if(bIsServer())
		{
			m_poMine->Destroy();
			delete m_poMine;
			m_poMine = NULL;

			for(s32 i = 0; i< m_poClientArray.Size(); ++i)
			{
				Socket* poCurrent = m_poClientArray.Get(i);
				poCurrent->Destroy();
				delete poCurrent;
			}
			m_poClientArray.Clear();

			m_ulPeerType = E_PeerType_Offline;
		}
	}

	void NetworkModule::_RecvPart()
	{
		if(bIsServer())
		{
			Socket* tmpSock = CallCreator<Socket>(E_CreatorType_Socket);
			s32 sRet = m_poMine->Accept(tmpSock);
			if(!sRet)
			{
				if(m_poClientArray.Size() < m_ulMaxClientNumber)
					m_poClientArray.AddTail(tmpSock);
				else
					delete tmpSock;
			}
			else
				delete tmpSock;

			for(s32 i = 0; i< m_poClientArray.Size(); ++i)
			{
				Socket* poCurrent = m_poClientArray.Get(i);
				Char strHello[10];
				s32 sRet = poCurrent->Recv(strHello, 10);
				if(!sRet)
					printf("%s From Client\n", strHello);
			}
		}
		else if(bIsClient())
		{
			Char strHello[10];
			s32 sRet = m_poMine->Recv(strHello, 10);
			if(!sRet)
				printf("%s From Server\n", strHello);
		}
	}

	void NetworkModule::_SendPart()
	{
		if(bIsServer())
		{
			for(s32 i = 0; i< m_poClientArray.Size(); ++i)
			{
				Socket* poCurrent = m_poClientArray.Get(i);
				Char strHello[] = "Hello";
				poCurrent->Send(strHello, strlen(strHello) + 1);
			}
		}
		else if(bIsClient())
		{
			Char strHello[] = "Hello";
			m_poMine->Send(strHello, strlen(strHello) + 1);
		}
	}
}