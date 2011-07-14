#include "TRender_WinGDIRenderWindow.h"
#include "TCore_Exception.h"'
#include "TRender_WinMsg.h"

namespace TsiU
{
	WinGDIRenderWindow::WinGDIRenderWindow()
		:WinRenderWindow()
	{
	}

	void WinGDIRenderWindow::InitWindow(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter)
	{
		m_uiWidth		= p_uiWidth;
		m_uiHeight		= p_uiHeight;
		m_bIsWindowed	= p_bIsWindowed;

		if(!p_poOuter)
		{
			WNDCLASSEX l_Wincl;

			/* The Window structure */
			l_Wincl.hInstance = m_poInst;
			l_Wincl.lpszClassName = p_strTitle;
			l_Wincl.lpfnWndProc = WinRenderWindowMsg::WndProc;
			l_Wincl.style = CS_DBLCLKS;
			l_Wincl.cbSize = sizeof(WNDCLASSEX);

			l_Wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
			l_Wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
			l_Wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
			l_Wincl.lpszMenuName = NULL;
			l_Wincl.cbClsExtra = 0;
			l_Wincl.cbWndExtra = 0; 
			l_Wincl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

			if( !::RegisterClassEx(&l_Wincl) )
			{			
				D_FatalError("Register Window Error");
			}

			m_bIsWindowed = p_bIsWindowed;
			m_uiWidth = p_uiWidth;
			m_uiHeight = p_uiHeight;

			int iWindowWidth = m_uiWidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
				iWindowHeight = m_uiHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
				GetSystemMetrics(SM_CYCAPTION);
			if(l_Wincl.lpszMenuName != NULL)
				iWindowHeight += GetSystemMetrics(SM_CYMENU);
			int iXWindowPos = (GetSystemMetrics(SM_CXSCREEN) - iWindowWidth) / 2,
				iYWindowPos = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;

			m_poHwnd = ::CreateWindowEx(0, p_strTitle, p_strTitle, WS_OVERLAPPEDWINDOW
				/*WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX*/, iXWindowPos, iYWindowPos,
				iWindowWidth, iWindowHeight, NULL, NULL, m_poInst, NULL );

			::ShowWindow(m_poHwnd, SW_SHOW);
			::UpdateWindow(m_poHwnd);

			//Always on top
			if(m_bIsWindowed)
				SetWindowPos(m_poHwnd,HWND_TOPMOST,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
		}
		else
		{
			m_poHwnd = (HWND)p_poOuter;
		}

	}
	void WinGDIRenderWindow::SetFullScreen(Bool p_bFull)
	{
		m_bIsWindowed = !p_bFull;
		if(m_bIsWindowed)
		{
			SetWindowPos(m_poHwnd,HWND_TOPMOST,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
		}
		else
		{
			SetWindowPos(m_poHwnd,HWND_NOTOPMOST,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
		}
	}
}