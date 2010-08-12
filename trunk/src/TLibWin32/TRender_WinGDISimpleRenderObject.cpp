#include "TRender_WinGDISimpleRenderObject.h"
#include "TRender_WinGDIRenderer.h"

namespace TsiU
{
	WinGDISimpleRenderObjectUtility::WinGDISimpleRenderObjectUtility(WinGDIRenderer* p_poRenderer)
		:m_poRenderer(p_poRenderer)
	{}

	void WinGDISimpleRenderObjectUtility::DrawString(f32 x, f32 y, const Char* str, D_Color clr)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();
		SetTextColor(hdc, RGB(clr.r, clr.g, clr.b));
		SetBkMode(hdc,TRANSPARENT);
		TextOut(hdc, x, y, str, strlen(str));
	}
	void WinGDISimpleRenderObjectUtility::DrawStringEx(f32 x, f32 y, f32 width, f32 height, f32 size, const Char* str, const Char* fontName, D_Color clr)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC(); 
		RECT rect={x, y, x + width,y + height};
		SetTextColor(hdc, RGB(clr.r, clr.g, clr.b));
		SetBkMode(hdc,TRANSPARENT);
		HFONT font = CreateFont(size,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,fontName);
		HFONT oldfont = (HFONT)SelectObject(hdc, font);
		DrawText(hdc,str,-1,&rect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		DeleteObject(SelectObject(hdc, oldfont));
	}
	void WinGDISimpleRenderObjectUtility::DrawPixel(f32 x, f32 y, D_Color clr)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();  
		SetPixel(hdc, x, y, RGB(clr.r, clr.g, clr.b));
	}
	void WinGDISimpleRenderObjectUtility::DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, D_Color clr, f32 lineWidth)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();  
		HPEN hpen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, lineWidth, RGB(clr.r, clr.g, clr.b))) ;

		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);

		DeleteObject(SelectObject(hdc, hpen));
	}
	void WinGDISimpleRenderObjectUtility::DrawCircle(f32 x, f32 y, f32 radius, D_Color clr, f32 lineWidth)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();  
		HPEN hpen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, lineWidth, RGB(clr.r, clr.g, clr.b))) ;

		MoveToEx(hdc, x + radius, y, NULL);
		AngleArc(hdc, x, y, radius, 0.f, 360.0f);

		DeleteObject(SelectObject(hdc, hpen));
	}
	void WinGDISimpleRenderObjectUtility::DrawRectangle(f32 x, f32 y, f32 width, f32 height, D_Color clr, f32 lineWidth)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();  
		HPEN hpen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, lineWidth, RGB(clr.r, clr.g, clr.b)));
		HBRUSH hbrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

		Rectangle(hdc, x, y, x + width, y + height);

		DeleteObject(SelectObject(hdc, hpen));
		DeleteObject(SelectObject(hdc, hbrush));
	}
	void WinGDISimpleRenderObjectUtility::DrawFillRectangle(f32 x, f32 y, f32 width, f32 height, D_Color clr, D_Color outclr)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();  
		RECT rect = { x, y, x + width, y + height};
		HBRUSH hbrush = CreateSolidBrush(RGB(clr.r, clr.g, clr.b));
		HPEN hpen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(outclr.r, outclr.g, outclr.b)));
		HBRUSH holdbrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

		FillRect(hdc, &rect, hbrush);
		Rectangle(hdc, x, y, x + width, y + height);

		DeleteObject(SelectObject(hdc, hpen));
		DeleteObject(hbrush);
		DeleteObject(holdbrush);
	}
	void WinGDISimpleRenderObjectUtility::DrawFillCircle(f32 x, f32 y, f32 radius, D_Color clr, D_Color outclr)
	{
		HDC hdc = m_poRenderer->GetOffscreenDC();
		HPEN hpen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(outclr.r, outclr.g, outclr.b)));
		HBRUSH hbrush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(clr.r, clr.g, clr.b)));

		Ellipse(hdc, x-radius, y-radius, x+radius, y+radius);

		DeleteObject(SelectObject(hdc, hpen));
		DeleteObject(SelectObject(hdc, hbrush));
	}
}