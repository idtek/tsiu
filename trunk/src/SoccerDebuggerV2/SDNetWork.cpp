#include "SDGlobalDef.h"
#include "SDMempool.h"

#define MAXADDRSTR	16
#define BUFSIZE		1024

#define CHECKRET(x)		if(x) goto Failed;

int _CheckWinsockVersion(VOID)
{
    WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err==0)
	{
		if ((LOBYTE(wsaData.wVersion)==2) && (HIBYTE(wsaData.wVersion)==2)) 
			return 0;
		WSACleanup();
		err = WSAVERNOTSUPPORTED;
	}
    printf("WinSock DLL does not support requested API version.\n");
    return err;
}

SOCKET		g_hSock;
SOCKET		g_hSSock;
SOCKADDR_IN	g_stDestAddr;
SOCKADDR_IN	g_stSrcAddr;

void NetWorkInit(void)
{
	s32		nRet;
	s32		iLen = MAXADDRSTR;
	//Char	strDestMulti[]	= "127.0.0.1";
	u16		nDestPort		= 51122;
	
	nRet = _CheckWinsockVersion();
	CHECKRET(nRet);

	/*SOCKADDR_IN stDestAddr;
	nRet = WSAStringToAddress(strDestMulti, AF_INET, NULL, (LPSOCKADDR)&stDestAddr, &iLen);
	CHECKRET(nRet)*/

	nRet = WSAStringToAddress ( "127.0.0.1", AF_INET, NULL, (LPSOCKADDR)&g_stDestAddr, &iLen);
	CHECKRET(nRet)

	g_stDestAddr.sin_port = htons(52211);

	//Create socket
	g_hSock = WSASocket(AF_INET, SOCK_DGRAM,  IPPROTO_UDP, (LPWSAPROTOCOL_INFO)NULL, 0, 
						WSA_FLAG_OVERLAPPED/* | WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF*/);
	CHECKRET(g_hSock == INVALID_SOCKET)  

	g_hSSock = WSASocket(AF_INET, SOCK_DGRAM,  IPPROTO_UDP, (LPWSAPROTOCOL_INFO)NULL, 0, 
						WSA_FLAG_OVERLAPPED/* | WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF*/);
	CHECKRET(g_hSSock == INVALID_SOCKET) 

	g_stSrcAddr.sin_family = AF_INET;//PF_INET;
	g_stSrcAddr.sin_port = htons(nDestPort);
	g_stSrcAddr.sin_addr.s_addr = INADDR_ANY;
	nRet = bind(g_hSock, (struct sockaddr FAR *)&g_stSrcAddr, sizeof(struct sockaddr));
	CHECKRET(nRet == SOCKET_ERROR)

	/*s32		nIP_TTL = 1;
	DWORD	cbRet;
	nRet = WSAIoctl(g_hSock,SIO_MULTICAST_SCOPE,&nIP_TTL,sizeof(nIP_TTL),NULL,0,&cbRet,NULL,NULL);
	CHECKRET(nRet)

	bFlag = FALSE;
	nRet = WSAIoctl(g_hSock,SIO_MULTIPOINT_LOOPBACK,&bFlag,sizeof (bFlag),NULL,0,&cbRet,NULL,NULL);
	CHECKRET(nRet)*/
 
	/*stDestAddr.sin_family = PF_INET;
	nRet = WSAHtons(g_hSock, nDestPort, &(stDestAddr.sin_port));
	CHECKRET(nRet == SOCKET_ERROR)

	g_hNewSock = WSAJoinLeaf(g_hSock,(PSOCKADDR)&stDestAddr,sizeof(stDestAddr),NULL,NULL,NULL,NULL,JL_RECEIVER_ONLY);
	CHECKRET(g_hNewSock == INVALID_SOCKET)*/

	return;

Failed:
	WSACleanup();
	return;
}

void NetWorkDeinit(void)
{
	closesocket(g_hSock);
	closesocket(g_hSSock);
	WSACleanup();
}

DWORD WINAPI RecvUDPPack(PVOID)
{
	DWORD dFlag, cbRet;
	int iLen;
	int nRet;
	char achInBuf [BUFSIZE]="";
	WSABUF stWSABuf;
	while(!g_bStop)
	{
		stWSABuf.buf = achInBuf;
		stWSABuf.len = BUFSIZE;
		cbRet = 0;
		iLen = sizeof(g_stSrcAddr);
		dFlag = 0;

		nRet = WSARecvFrom(g_hSock,&stWSABuf,1,&cbRet,&dFlag,(struct sockaddr *)&g_stSrcAddr,&iLen,NULL,NULL);
		if(nRet == SOCKET_ERROR)
		{
			printf("WSARecvFrom() failed, Err:%d\n", WSAGetLastError());
		}
		else 
		{
			g_poMem->InsertUDPData(*(UDP_PACK *)stWSABuf.buf);
        }
	}
	return 0;
}
