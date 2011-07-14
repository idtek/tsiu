#ifndef __TNETWORK_UDTSOCKET__
#define __TNETWORK_UDTSOCKET__

#include "udt.h"
#include "TNetwork_Socket.h"

namespace TsiU
{
	class UDTSocket : public Socket
	{
	public:
		UDTSocket();
		virtual s32 Create(u32 _ulProtoType, Bool _bIsAsync);
		virtual s32 SetAddress(const Char* _poAddress, u16 _ulPort, Bool _bIsBroadCast = false);
		virtual s32 Destroy();
		virtual s32 Bind();
		virtual s32 Listen(s32 _iBackLog);
		virtual s32 Connect(const Char* _poAddress, u16 _ulPort);
		virtual s32 IOCtl(u32 _ulCmd, u32* _ulValue);
		virtual s32 Accept(Socket* _poAcceptSock);
		virtual s32 Send(const Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 Recv(Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 SendTo(const Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 RecvFrom(Char* _poBuffer, s32 _ulBufferLen);
		virtual s32 SetTimeOut(u32 _ulTimeOut);

		virtual const Char* GetIPAddress();
		virtual u16			GetPort();
		virtual Bool		bIsValid();

	private:
		s32 _AcceptInitFrom(UDTSOCKET _sock, const SOCKADDR_IN* _Addr, Bool _bIsAsync);

	private:
		UDTSOCKET	m_UDTSock;
		u32			m_ulProtoType;
		Bool		m_bIsAsync;
		TIMEVAL		m_TimeOut;
		SOCKADDR_IN m_Addr;
		UDT::UDSET		m_fdRead;
		UDT::UDSET		m_fdWrite;
	};
}

#endif