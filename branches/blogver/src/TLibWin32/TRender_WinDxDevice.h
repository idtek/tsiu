#ifndef __TRENDER_WINDX_DEVICE__
#define __TRENDER_WINDX_DEVICE__

#include "TCore_Types.h"
#include "TCore_Memory.h"


#ifndef TLIB_NOWINDX
namespace TsiU
{
	class WinDxDevice
	{
	public:
		WinDxDevice();
		~WinDxDevice();

		LPDIRECT3D9 GetD3D(){ return m_poD3D;	}
		LPDIRECT3DDEVICE9 GetD3DDevice(){ return m_poD3DDevice;	}
		void SetD3DDevice(const LPDIRECT3DDEVICE9& p_poDevice){ m_poD3DDevice = p_poDevice; }

	private:
		LPDIRECT3D9 m_poD3D;
		LPDIRECT3DDEVICE9 m_poD3DDevice;
	};

}
#endif

#endif