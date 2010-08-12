#ifndef __TENGINE_NETWORKMODULE__
#define __TENGINE_NETWORKMODULE__

#include "TCore_Types.h"
#include "TEngine_Module.h"
#include "TUtility_List.h"

namespace TsiU
{	
	enum
	{
		E_PeerType_Offline = 0,
		E_PeerType_Server,
		E_PeerType_Client
	};

	class Socket;

	class NetworkModule : public IModule
	{
	public:
		NetworkModule(u32 _ulMaxClient);
		~NetworkModule(){};

		virtual void Init();
		virtual void RunOneFrame(float _fDeltaTime);
		virtual void UnInit();

		inline Bool bIsOffline(){ return m_ulPeerType == E_PeerType_Offline;	};
		inline Bool bIsServer()	{ return m_ulPeerType == E_PeerType_Server;		};
		inline Bool bIsClient()	{ return m_ulPeerType == E_PeerType_Client;		};

		void StartServer(u32 _ulPort);
		void ShutDownServer();
		void ConnectServer(const Char* _strAddr, u32 _ulPort);

	private:
		void _SendPart();
		void _RecvPart();

	private:
		u32				m_ulRS;

		u32				m_ulPeerType;
		Socket*			m_poMine;

		List<Socket*>	m_poClientArray;
		u32				m_ulMaxClientNumber;
	};
}

#endif