#ifndef __TWIN32_PRIVATE__
#define __TWIN32_PRIVATE__

#include <winsock2.h>
#pragma comment(lib,"ws2_32")

#include <windows.h>

#include "TCore_Types.h"

#ifndef TLIB_NOWINDX
#include <d3d9.h>
#include <d3dx9.h>

//force using 0x0800
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma comment(lib, "d3dxof")
#pragma comment(lib, "dxguid") 
#pragma comment(lib, "d3dx9d") 
#pragma comment(lib, "d3d9") 
#pragma comment(lib, "dinput8") 

#define D_SafeRelease(p)	if(p)	{ p->Release(); p = NULL;	}
#endif

#pragma comment(lib, "winmm") 
#pragma comment(lib,"msimg32") //for TransparentBlt

#endif