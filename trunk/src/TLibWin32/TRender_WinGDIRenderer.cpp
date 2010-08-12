#include "TRender_WinGDIRenderer.h"
#include "TRender_WinGDIRenderWindow.h"

namespace TsiU
{
	void WinGDIRenderer::InitRender(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter)
	{
		WinGDIRenderWindow *l_poMainWindow = new WinGDIRenderWindow;
		D_CHECK(l_poMainWindow);

		l_poMainWindow->InitWindow(p_uiWidth, p_uiHeight, p_strTitle, p_bIsWindowed, p_poOuter);
		RenderWindowMsg::RegisterMsgListener(this);

		m_poMainWindow = l_poMainWindow;
		m_poMainHwnd = l_poMainWindow->GetHWND();

		m_poHdc = GetDC(m_poMainHwnd);
		m_poOffscreenDC = CreateCompatibleDC(m_poHdc);
		m_poOffscreenBitmap = CreateCompatibleBitmap(m_poHdc, 
													 m_poMainWindow->GetWindowWidth(),
													 m_poMainWindow->GetWindowHeight());
		SelectObject(m_poOffscreenDC, m_poOffscreenBitmap);
		ReleaseDC(m_poMainHwnd, m_poHdc);
	}
	void WinGDIRenderer::Clear(const D_Color& p_Color, u32 p_uiClearFlag)
	{
		RECT l_Rect = { 0, 0, m_poMainWindow->GetWindowWidth(), m_poMainWindow->GetWindowHeight()};
		HBRUSH l_poBrush = CreateSolidBrush(RGB(p_Color.r, p_Color.g, p_Color.b));
		FillRect(m_poOffscreenDC, &l_Rect, l_poBrush);
		DeleteObject(l_poBrush);
	}
	Bool WinGDIRenderer::BeginFrame()
	{
		m_poHdc = GetDC(m_poMainHwnd);
		return true;
	}
	void WinGDIRenderer::EndFrame()
	{
	}
	void WinGDIRenderer::Flip()
	{
		BitBlt(m_poHdc, 0, 0, m_poMainWindow->GetWindowWidth(), m_poMainWindow->GetWindowHeight(), 
			   m_poOffscreenDC, 0, 0, SRCCOPY);
		ReleaseDC(m_poMainHwnd, m_poHdc);
	}
	void WinGDIRenderer::ShowFPS(f32 fps)
	{
		Char l_strBuff[64]={0};
		sprintf(l_strBuff, "FPS = %.2f", fps);
		::SetWindowText(m_poMainHwnd, l_strBuff);
	}
	void WinGDIRenderer::OnWindowResize(s32 width, s32 height)
	{
		DeleteDC(m_poOffscreenDC);
		DeleteObject(m_poOffscreenBitmap);
		
		m_poMainWindow->SetWindowWidth(width);
		m_poMainWindow->SetWindowHeight(height);

		m_poHdc = GetDC(m_poMainHwnd);
		m_poOffscreenDC = CreateCompatibleDC(m_poHdc);
		m_poOffscreenBitmap = CreateCompatibleBitmap(m_poHdc, 
													 m_poMainWindow->GetWindowWidth(),
													 m_poMainWindow->GetWindowHeight());
		SelectObject(m_poOffscreenDC, m_poOffscreenBitmap);
		ReleaseDC(m_poMainHwnd, m_poHdc);
	}
}