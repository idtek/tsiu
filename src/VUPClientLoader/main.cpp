#include "..\\VUPClient\\VCExportHeader.h"

//#pragma comment(lib, "VUPClient.lib")

#include <Windows.h>

int main(int argc, char* argv[])
{
	VUPClientAdapter* vcAdapter = new VUPClientAdapter;
	vcAdapter->Init(::GetCurrentProcessId());
	vcAdapter->RegisterMe();
	//delete vcAdapter;

	while(1)
	{
		vcAdapter->Tick();
		::Sleep(1000);
	}
	return 1;
}

