#ifndef __TRENDER_WIN_RENDERWINDOW__
#define __TRENDER_WIN_RENDERWINDOW__

#include "TCore_Types.h"
#include "TRender_RenderWindow.h"
#include "TWin32_Private.h"

namespace TsiU
{
	class WinRenderWindow : public RenderWindow
	{
	public:
		WinRenderWindow()
		{
			m_poInst = ::GetModuleHandle(NULL);
		}

		inline HINSTANCE GetHINST() { return m_poInst;	};
		inline HWND GetHWND()		{ return m_poHwnd;	};

	protected:
		HINSTANCE		m_poInst;
		HWND			m_poHwnd;
	};
}

#endif