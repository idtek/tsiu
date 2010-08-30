#include "d:\\Tsiu\\src\\VUPClient\\VCExportHeader.h"

//#pragma comment(lib, "VUPClient.lib")

#include <Windows.h>

int main(int argc, char* argv[])
{
	VUPClientAdapter* vcAdapter = new VUPClientAdapter;
	vcAdapter->Init(::GetCurrentProcessId());
	vcAdapter->RegisterMe();

	while(1)
	{
		vcAdapter->Tick();
		::Sleep(1000);
	}
	return 1;
}

