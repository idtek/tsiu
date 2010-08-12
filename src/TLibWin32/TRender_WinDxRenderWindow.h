#ifndef __TRENDER_WINDX_RENDERWINDOW__
#define __TRENDER_WINDX_RENDERWINDOW__

#include "TCore_Types.h"
#include "TRender_WinRenderWindow.h"
#include "TWin32_Private.h"

namespace TsiU
{
	class WinDxDevice;

	class WinDxRenderWindow : public WinRenderWindow
	{
	public:
		WinDxRenderWindow();
		virtual ~WinDxRenderWindow();

		virtual void InitWindow(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter);
		virtual void SetFullScreen(Bool p_bFull){};		
		
		inline WinDxDevice* GetDevice(){ return m_poDevice;	};


	private:
		void InitD3D();

	private:
		WinDxDevice*		m_poDevice; 
	};
}

#endif