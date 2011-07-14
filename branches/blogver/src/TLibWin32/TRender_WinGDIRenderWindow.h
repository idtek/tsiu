#ifndef __TRENDER_WINGDI_RENDERWINDOW__
#define __TRENDER_WINGDI_RENDERWINDOW__

#include "TCore_Types.h"
#include "TRender_WinRenderWindow.h"


namespace TsiU
{
	class WinGDIRenderWindow : public WinRenderWindow
	{
	public:
		WinGDIRenderWindow();

		virtual void InitWindow(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter);
		virtual void SetFullScreen(Bool p_bFull);
	};
}

#endif
