#include "..\\VUPClient\\VCExportHeader.h"

//#pragma comment(lib, "VUPClient.lib")

#include <Windows.h>

int g_RDVPoint = 1001;

void StartTestingHandler(const UDP_PACK& _pInPack)
{
	g_RDVPoint += 1;
}

int main(int argc, char* argv[])
{
	VUPClientAdapter* vcAdapter = new VUPClientAdapter;
	vcAdapter->Init(::GetCurrentProcessId());
	vcAdapter->RegisterMe();
	vcAdapter->RegisterUDPPackHandler(EPT_M2C_StartTesting, StartTestingHandler);
	while(1)
	{
		vcAdapter->ReachRDVPoint(g_RDVPoint, 0, 0);
		vcAdapter->Tick();
		::Sleep(5000);
	}
	delete vcAdapter;

	return 1;
}

