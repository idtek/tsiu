#include "TNetwork_WinSocket.h"

namespace TsiU
{
	WinSocket::WinSocket()
	{
		m_Sock = INVALID_SOCKET;
	}
	s32 WinSocket::Create(u32 _ulProtoType, Bool _bIsAsync)
	{
		if(_ulProtoType == E_NETWORK_PROTO_TCP)
			m_Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		else if(_ulProtoType == E_NETWORK_PROTO_UDP)
			m_Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		m_ulProtoType		= _ulProtoType;
		m_bIsAsync			= _bIsAsync;
		m_TimeOut.tv_sec	= 0;
		m_TimeOut.tv_usec	= 0;

		if(m_Sock == INVALID_SOCKET)
		{
			D_Output("%d", GetLastError());
			return -1;
		}

		if(m_bIsAsync)
		{
			u32 ulValue = 1;
			s32 sRet = IOCtl(E_NETWORK_IOCTRL_NBIO, &ulValue);
			if(sRet == -1)
				return -1;
		}
		return 0;
	}
	s32 WinSocket::SetTimeOut(u32 _ulTimeOut)
	{
		if(m_ulProtoType)
		{
			m_TimeOut.tv_sec	= _ulTimeOut / 1000;
			m_TimeOut.tv_usec	= (_ulTimeOut - m_TimeOut.tv_sec) * 1000;
			return 0;
		}
		return -1;
	}
	s32 WinSocket::Destroy()
	{
		closesocket(m_Sock);
		return 0;
	}
	s32 WinSocket::Bind()
	{
		s32 sRet = bind(m_Sock, (sockaddr*)&m_Addr, sizeof(SOCKADDR_IN));
		if(sRet == SOCKET_ERROR)
		{
			return -1;
		}
		return 0;
	}
	s32 WinSocket::SetAddress(const Char* _poAddress, u16 _ulPort)
	{
		m_Addr.sin_family = AF_INET;
		if(!_poAddress)
			m_Addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		else
			m_Addr.sin_addr.S_un.S_addr = inet_addr(_poAddress);
		m_Addr.sin_port = htons(_ulPort);

		return 0;
	}
	const Char* WinSocket::GetIPAddress()
	{
		return inet_ntoa(m_Addr.sin_addr);
	}
	u16 WinSocket::GetPort()
	{
		return ntohs(m_Addr.sin_port);
	}
	s32 WinSocket::Connect(const Char* _poAddress, u16 _ulPort)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_TCP)
			return -1;

		SOCKADDR_IN HostAddr;
		HostAddr.sin_family = AF_INET;
		HostAddr.sin_addr.S_un.S_addr = inet_addr(_poAddress);
		HostAddr.sin_port = htons(_ulPort);

		s32 sRet = connect(m_Sock, (const sockaddr*)&HostAddr, sizeof(SOCKADDR_IN));
		if(sRet == SOCKET_ERROR)
		{
			//printf("socket failed with error %d", WSAGetLastError());
			if(m_bIsAsync)
			{
				FD_ZERO(&m_fdWrite);
				FD_SET(m_Sock, &m_fdWrite);
				s32 sRet = select(0, NULL, &m_fdWrite, NULL, &m_TimeOut);
				if(sRet == SOCKET_ERROR)
				{
					return -1;
				}
				else if(sRet == 0)
				{
					return -2;	//timeout
				}
				else if(sRet > 0)
				{
					if(FD_ISSET(m_Sock, &m_fdWrite))
						return 0;
					else
						return -1;
				}
			}
			else
				return -1;
		}
		
		return 0;
	}
	s32 WinSocket::Listen()
	{
		s32 sRet = listen(m_Sock, 5);
		if(sRet == SOCKET_ERROR)
		{
			return -1;
		}
		return 0;
	}
	s32 WinSocket::Accept(Socket* _poAcceptSock)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_TCP)
			return -1;

		if(m_bIsAsync)
		{
			FD_ZERO(&m_fdRead);
			FD_SET(m_Sock, &m_fdRead);
			s32 sRet = select(0, &m_fdRead, NULL, NULL, &m_TimeOut);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || FD_ISSET(m_Sock, &m_fdRead))
		{
			SOCKADDR_IN AcceptAddr;
			s32 lAddrLen = sizeof(SOCKADDR_IN);
			SOCKET AcceptSock = accept(m_Sock, (sockaddr*)&AcceptAddr, &lAddrLen);
			if(AcceptSock == INVALID_SOCKET)
			{
				return -1;
			}

			WinSocket* poSock = (WinSocket*)_poAcceptSock;
			if(!poSock)
				return -1;
			poSock->_AcceptInitFrom(AcceptSock, &AcceptAddr, m_bIsAsync);
			if(m_bIsAsync)
			{
				u32 ulValue = 1;
				s32 sRet = poSock->IOCtl(E_NETWORK_IOCTRL_NBIO, &ulValue);
				if(sRet == -1)
					return -1;
			}
		}
		return 0;
	}
	s32 WinSocket::Send(const Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_TCP)
			return -1;

		if(m_bIsAsync)
		{
			FD_ZERO(&m_fdWrite);
			FD_SET(m_Sock, &m_fdWrite);
			s32 sRet = select(0, NULL, &m_fdWrite, NULL, &m_TimeOut);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || FD_ISSET(m_Sock, &m_fdWrite))
		{
			s32 sRet = send(m_Sock, _poBuffer, _ulBufferLen, 0);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 WinSocket::Recv(Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_TCP)
			return -1;

		if(m_bIsAsync)
		{
			FD_ZERO(&m_fdRead);
			FD_SET(m_Sock, &m_fdRead);
			s32 sRet = select(0, &m_fdRead, NULL, NULL, &m_TimeOut);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || FD_ISSET(m_Sock, &m_fdRead))
		{
			s32 sRet = recv(m_Sock, _poBuffer, _ulBufferLen, 0);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 WinSocket::SendTo(const Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_UDP)
			return -1;

		if(m_bIsAsync)
		{
			FD_ZERO(&m_fdWrite);
			FD_SET(m_Sock, &m_fdWrite);
			s32 sRet = select(0, NULL, &m_fdWrite, NULL, &m_TimeOut);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || FD_ISSET(m_Sock, &m_fdWrite))
		{
			s32 sRet = sendto(m_Sock, _poBuffer, _ulBufferLen, 0, (const sockaddr*)&m_Addr, sizeof(SOCKADDR_IN));
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 WinSocket::RecvFrom(Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_UDP)
			return -1;

		if(m_bIsAsync)
		{
			FD_ZERO(&m_fdRead);
			FD_SET(m_Sock, &m_fdRead);
			s32 sRet = select(0, &m_fdRead, NULL, NULL, &m_TimeOut);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || FD_ISSET(m_Sock, &m_fdRead))
		{
			s32 lAddrLen = sizeof(SOCKADDR_IN);
			s32 sRet = recvfrom(m_Sock, _poBuffer, _ulBufferLen, 0, (sockaddr*)&m_Addr, &lAddrLen);
			if(sRet == SOCKET_ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 WinSocket::IOCtl(u32 _ulCmd, u32* _ulValue)
	{
		s32 ulConvertedCmd = 0;
		if(_ulCmd == E_NETWORK_IOCTRL_NBIO)
			ulConvertedCmd = FIONBIO;
		else if(_ulCmd == E_NETWORK_IOCTRL_NREAD)
			ulConvertedCmd = FIONREAD;
		s32 sRet = ioctlsocket(m_Sock, ulConvertedCmd, (u_long*)_ulValue);
		if(sRet == SOCKET_ERROR)
		{
			return -1;
		}
		return 0;
	}
	Bool WinSocket::bIsValid()
	{
		return m_Sock != INVALID_SOCKET;
	}
	s32 WinSocket::_AcceptInitFrom(SOCKET _sock, const SOCKADDR_IN* _Addr, Bool _bIsAsync)
	{
		m_Sock = _sock;
		memcpy(&m_Addr, _Addr, sizeof(SOCKADDR_IN));
		m_ulProtoType = E_NETWORK_PROTO_TCP;
		m_bIsAsync = _bIsAsync;
		m_TimeOut.tv_sec	= 0;
		m_TimeOut.tv_usec	= 0;

		return 0;
	}
}