#ifndef __TNETWORK_WINSOCKET__
#define __TNETWORK_WINSOCKET__

#include "TWin32_Private.h"
#include "TNetwork_Socket.h"

namespace TsiU
{
	class WinSocket : public Socket
	{
	public:
		WinSocket();

		virtual s32 Create(u32 _ulProtoType, Bool _bIsAsync);
		virtual s32 Destroy();
		virtual s32 Bind(const Char* _poAddress, u16 _ulPort);
		virtual s32 Listen();
		virtual s32 Connect(const Char* _poAddress, u16 _ulPort);
		virtual s32 IOCtl(u32 _ulCmd, u32* _ulValue);
		virtual s32 Accept(Socket* _poAcceptSock);
		virtual s32 Send(const Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 Recv(Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 SendTo(const Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 RecvFrom(Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 SetTimeOut(u32 _ulTimeOut);
		
		virtual Bool bIsValid();

	private:
		s32 _AcceptInitFrom(SOCKET _sock, const SOCKADDR_IN* _Addr, Bool _bIsAsync);

	private:
		u32			m_ulProtoType;
		Bool		m_bIsAsync;
		TIMEVAL		m_TimeOut;
	
		SOCKET		m_Sock;
		SOCKADDR_IN m_Addr;

		FD_SET		m_fdRead;
		FD_SET		m_fdWrite;
	};
}


#endif