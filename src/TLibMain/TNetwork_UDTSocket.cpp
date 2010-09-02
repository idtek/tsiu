#include "TNetwork_UDTSocket.h"

namespace TsiU
{
	UDTSocket::UDTSocket()
		: m_UDTSock(UDT::INVALID_SOCK)
	{
	}
	s32 UDTSocket::Create(u32 _ulProtoType, Bool _bIsAsync)
	{
		if(_ulProtoType == E_NETWORK_PROTO_TCP)
			m_UDTSock = UDT::socket(AF_INET, SOCK_STREAM, 0);
		else if(_ulProtoType == E_NETWORK_PROTO_UDP)
			m_UDTSock = UDT::socket(AF_INET, SOCK_DGRAM, 0);

		if(m_UDTSock == UDT::INVALID_SOCK)
		{
			D_Output("UDTSocket::Create failed: %s\n", UDT::getlasterror().getErrorMessage());
			return -1;
		}

		m_ulProtoType		= _ulProtoType;
		m_bIsAsync			= _bIsAsync;
		m_TimeOut.tv_sec	= 0;
		m_TimeOut.tv_usec	= 0;

		if(_bIsAsync)
		{
			Bool async = !_bIsAsync;
			s32 iRet = UDT::setsockopt(m_UDTSock, 0, UDT_SNDSYN, &async, sizeof(async));
			if(iRet == UDT::ERROR)
			{
				D_Output("UDTSocket::Create failed: %s\n", UDT::getlasterror().getErrorMessage());
				return -1;
			}
			iRet = UDT::setsockopt(m_UDTSock, 0, UDT_RCVSYN, &async, sizeof(async));
			if(iRet == UDT::ERROR)
			{
				D_Output("UDTSocket::Create failed: %s\n", UDT::getlasterror().getErrorMessage());
				return -1;
			}
		}
		return 0;
	}
	s32 UDTSocket::SetAddress(const Char* _poAddress, u16 _ulPort, Bool _bIsBroadCast)
	{
		m_Addr.sin_family = AF_INET;
		if(!_poAddress)
			m_Addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		else
			m_Addr.sin_addr.S_un.S_addr = inet_addr(_poAddress);

		if(_bIsBroadCast)
		{
			//broadcast
			//s32 iBroadcast = 1;
			//s32 iRet = setsockopt(m_Sock, SOL_SOCKET, SO_BROADCAST, (char*)&iBroadcast, sizeof(iBroadcast));
			//if(iRet == SOCKET_ERROR)
			//{
			//	return -1;
			//}
		}
		m_Addr.sin_port = htons(_ulPort);
		return 0;
	}
	s32 UDTSocket::Destroy()
	{
		s32 iRet = UDT::close(m_UDTSock);			
		if(iRet == UDT::ERROR)
		{
			D_Output("UDTSocket::Destroy failed: %s\n", UDT::getlasterror().getErrorMessage());
			return -1;
		}
		return 0;
	}
	s32 UDTSocket::Bind()
	{
		s32 iRet = UDT::bind(m_UDTSock, (sockaddr*)&m_Addr, sizeof(SOCKADDR_IN));
		if(iRet == UDT::ERROR)
		{
			D_Output("UDTSocket::Bind failed: %s\n", UDT::getlasterror().getErrorMessage());
			return -1;
		}
		return 0;
	}
	s32 UDTSocket::Listen(s32 _iBackLog)
	{
		s32 iRet = UDT::listen(m_UDTSock, _iBackLog);
		if(iRet == UDT::ERROR)
		{
			D_Output("UDTSocket::Bind failed: %s\n", UDT::getlasterror().getErrorMessage());
			return -1;
		}
		return 0;
	}
	s32 UDTSocket::Connect(const Char* _poAddress, u16 _ulPort)
	{
		//if(m_ulProtoType != E_NETWORK_PROTO_TCP)
		//	return -1;

		SOCKADDR_IN HostAddr;
		HostAddr.sin_family = AF_INET;
		HostAddr.sin_addr.S_un.S_addr = inet_addr(_poAddress);
		HostAddr.sin_port = htons(_ulPort);

		s32 iRetConnect = UDT::connect(m_UDTSock, (const sockaddr*)&HostAddr, sizeof(SOCKADDR_IN));
		if(iRetConnect == UDT::ERROR)
		{
			if(m_bIsAsync)
			{
				UD_ZERO(&m_fdWrite);
				UD_SET(m_UDTSock, &m_fdWrite);
				s32 iRet = UDT::select(0, NULL, &m_fdWrite, NULL, &m_TimeOut);
				if(iRet == UDT::ERROR)
				{
					return -1;
				}
				else if(iRet == 0)
				{
					return -2;	//timeout
				}
				else if(iRet > 0)
				{
					if(UD_ISSET(m_UDTSock, &m_fdWrite))
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
	s32 UDTSocket::IOCtl(u32 _ulCmd, u32* _ulValue)
	{
		D_CHECK(0);
		return 0;
	}
	s32 UDTSocket::Accept(Socket* _poAcceptSock)
	{
		//if(m_ulProtoType != E_NETWORK_PROTO_TCP)
		//	return -1;

		if(m_bIsAsync)
		{
			UD_ZERO(&m_fdRead);
			UD_SET(m_UDTSock, &m_fdRead);
			s32 iRet = UDT::select(0, &m_fdRead, NULL, NULL, &m_TimeOut);
			if(iRet == UDT::ERROR)
			{
				return -1;
			}
			else if(iRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || UD_ISSET(m_UDTSock, &m_fdRead))
		{
			SOCKADDR_IN acceptAddr;
			s32 lAddrLen = sizeof(SOCKADDR_IN);
			UDTSOCKET acceptSock = UDT::accept(m_UDTSock, (sockaddr*)&acceptAddr, &lAddrLen);
			if(acceptSock == UDT::INVALID_SOCK)
			{
				return -1;
			}
			UDTSocket* poSock = (UDTSocket*)_poAcceptSock;
			if(!poSock)
				return -1;
			return poSock->_AcceptInitFrom(acceptSock, &acceptAddr, m_bIsAsync);
		}
		return 0;
	}
	s32 UDTSocket::Send(const Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_TCP)
			return -1;

		if(m_bIsAsync)
		{
			UD_ZERO(&m_fdWrite);
			UD_SET(m_UDTSock, &m_fdWrite);
			s32 iRet = UDT::select(0, NULL, &m_fdWrite, NULL, &m_TimeOut);
			if(iRet == UDT::ERROR)
			{
				return -1;
			}
			else if(iRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || UD_ISSET(m_UDTSock, &m_fdWrite))
		{
			s32 iRet = UDT::send(m_UDTSock, _poBuffer, _ulBufferLen, 0);
			if(iRet == UDT::ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 UDTSocket::Recv(Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_TCP)
			return -1;

		if(m_bIsAsync)
		{
			UD_ZERO(&m_fdRead);
			UD_SET(m_UDTSock, &m_fdRead);
			s32 iRet = UDT::select(0, &m_fdRead, NULL, NULL, &m_TimeOut);
			if(iRet == UDT::ERROR)
			{
				return -1;
			}
			else if(iRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || UD_ISSET(m_UDTSock, &m_fdRead))
		{
			s32 sRet = UDT::recv(m_UDTSock, _poBuffer, _ulBufferLen, 0);
			if(sRet == UDT::ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 UDTSocket::SendTo(const Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_UDP)
			return -1;

		if(m_bIsAsync)
		{
			UD_ZERO(&m_fdWrite);
			UD_SET(m_UDTSock, &m_fdWrite);
			s32 sRet =UDT::select(0, NULL, &m_fdWrite, NULL, &m_TimeOut);
			if(sRet == UDT::ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || UD_ISSET(m_UDTSock, &m_fdWrite))
		{
			s32 sRet = UDT::sendmsg(m_UDTSock, _poBuffer, _ulBufferLen);
			if(sRet == UDT::ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 UDTSocket::RecvFrom(Char* _poBuffer, s32 _ulBufferLen)
	{
		if(m_ulProtoType != E_NETWORK_PROTO_UDP)
			return -1;

		if(m_bIsAsync)
		{
			UD_ZERO(&m_fdRead);
			UD_SET(m_UDTSock, &m_fdRead);
			s32 sRet = UDT::select(0, &m_fdRead, NULL, NULL, &m_TimeOut);
			if(sRet == UDT::ERROR)
			{
				return -1;
			}
			else if(sRet == 0)
			{
				return -2;	//timeout
			}
		}
		if(!m_bIsAsync || UD_ISSET(m_UDTSock, &m_fdRead))
		{
			s32 lAddrLen = sizeof(SOCKADDR_IN);
			s32 sRet = UDT::recvmsg(m_UDTSock, _poBuffer, _ulBufferLen);
			if(sRet == UDT::ERROR)
			{
				return -1;
			}
		}
		return 0;
	}
	s32 UDTSocket::SetTimeOut(u32 _ulTimeOut)
	{
		if(m_ulProtoType)
		{
			m_TimeOut.tv_sec	= _ulTimeOut / 1000;
			m_TimeOut.tv_usec	= (_ulTimeOut - m_TimeOut.tv_sec) * 1000;
			return 0;
		}
		return -1;
	}
	const Char* UDTSocket::GetIPAddress()
	{
		return inet_ntoa(m_Addr.sin_addr);
	}
	u16 UDTSocket::GetPort()
	{
		return ntohs(m_Addr.sin_port);
	}
	Bool UDTSocket::bIsValid()
	{
		return m_UDTSock != UDT::INVALID_SOCK;
	}

	s32 UDTSocket::_AcceptInitFrom(UDTSOCKET _sock, const SOCKADDR_IN* _Addr, Bool _bIsAsync)
	{
		m_UDTSock = _sock;
		memcpy(&m_Addr, _Addr, sizeof(SOCKADDR_IN));
		
		m_ulProtoType = E_NETWORK_PROTO_TCP;
		m_bIsAsync = _bIsAsync;
		m_TimeOut.tv_sec	= 0;
		m_TimeOut.tv_usec	= 0;

		if(_bIsAsync)
		{
			Bool async = !_bIsAsync;
			s32 iRet = UDT::setsockopt(m_UDTSock, 0, UDT_SNDSYN, &async, sizeof(async));
			if(iRet == UDT::ERROR)
			{
				D_Output("UDTSocket::Create failed: %s\n", UDT::getlasterror().getErrorMessage());
				return -1;
			}
			iRet = UDT::setsockopt(m_UDTSock, 0, UDT_RCVSYN, &async, sizeof(async));
			if(iRet == UDT::ERROR)
			{
				D_Output("UDTSocket::Create failed: %s\n", UDT::getlasterror().getErrorMessage());
				return -1;
			}
		}
		return 0;
	}
}