#ifndef __TRENDER_WIN_RENDERWINDOW_MSG__
#define __TRENDER_WIN_RENDERWINDOW_MSG__

#include "TCore_Types.h"
#include "TRender_RenderWindowMsg.h"


namespace TsiU
{
	class WinRenderWindowMsg : public RenderWindowMsg
	{
	public:
		virtual Bool MessageBump();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static s32	LastMousePosX;
		static s32	LastMousePosY;
	};
}

#endif
