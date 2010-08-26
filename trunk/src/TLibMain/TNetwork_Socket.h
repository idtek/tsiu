#ifndef __TNETWORK_SOCKET__
#define __TNETWORK_SOCKET__


#include "TUtility_List.h"

namespace TsiU
{
	enum
	{
		E_NETWORK_PROTO_UDP = 0,
		E_NETWORK_PROTO_TCP
	};

	enum
	{
		E_NETWORK_IOCTRL_NBIO = 0,
		E_NETWORK_IOCTRL_NREAD
	};

	class Socket
	{
	public:
		Socket(){};
		virtual s32 Create(u32 _ulProtoType, Bool _bIsAsync) = 0;
		virtual s32 Destroy() = 0;
		virtual s32 Bind(const Char* _poAddress, u16 _ulPort) = 0;
		virtual s32 Listen() = 0;
		virtual s32 Connect(const Char* _poAddress, u16 _ulPort) = 0;
		virtual s32 IOCtl(u32 _ulCmd, u32* _ulValue) = 0;
		virtual s32 Accept(Socket* _poAcceptSock) = 0;
		virtual s32 Send(const Char* _poBuffer, s32 _ulBufferLen) = 0;
		virtual s32 Recv(Char* _poBuffer, s32 _ulBufferLen) = 0;
		virtual s32 SendTo(const Char* _poBuffer, s32 _ulBufferLen) = 0;
		virtual s32 RecvFrom(Char* _poBuffer, s32 _ulBufferLen) = 0;
		virtual s32 SetTimeOut(u32 _ulTimeOut) = 0;

		virtual Bool bIsValid() = 0;
	};
}

#endif