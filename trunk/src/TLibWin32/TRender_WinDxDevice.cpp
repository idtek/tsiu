#include "TRender_WinDxDevice.h"

#ifndef TLIB_NOWINDX
namespace TsiU
{
	WinDxDevice::WinDxDevice()
	{
		m_poD3DDevice = NULL;
		m_poD3D = NULL;
	}
	WinDxDevice::~WinDxDevice()
	{
		D_SafeRelease(m_poD3DDevice);
		D_SafeRelease(m_poD3D);
	}
}
#endif