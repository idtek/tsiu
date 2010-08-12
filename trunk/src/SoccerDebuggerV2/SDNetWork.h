#ifndef __SD_NETWORK_H__
#define __SD_NETWORK_H__

extern void NetWorkInit();
extern void NetWorkDeinit();

extern DWORD WINAPI RecvUDPPack(PVOID);

extern SOCKET		g_hSock;
extern SOCKET		g_hSSock;
extern SOCKADDR_IN	g_stDestAddr;
extern SOCKADDR_IN	g_stSrcAddr;

#endif