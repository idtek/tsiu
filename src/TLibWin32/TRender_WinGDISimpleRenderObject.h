#ifndef __TRENDER_WINGDI_SIMPLERENDEROBJECT__
#define __TRENDER_WINGDI_SIMPLERENDEROBJECT__

#include "TCore_Types.h"

#include "TRender_SimpleRenderObject.h"

namespace TsiU
{
	class WinGDIRenderer;

	class WinGDISimpleRenderObjectUtility : public SimpleRenderObjectUtility
	{
	public:
		WinGDISimpleRenderObjectUtility(WinGDIRenderer* p_poRenderer);

		virtual void DrawString(f32 x, f32 y, const Char* str, D_Color clr);
		virtual void DrawStringEx(f32 x, f32 y, f32 width, f32 height, f32 size, const Char* str, const Char* fontName, D_Color clr);
		virtual void DrawPixel(f32 x, f32 y, D_Color clr);
		virtual void DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, D_Color clr, f32 lineWidth);
		virtual void DrawCircle(f32 x, f32 y, f32 radius, D_Color clr, f32 lineWidth);
		virtual void DrawRectangle(f32 x, f32 y, f32 width, f32 height, D_Color clr, f32 lineWidth);
		virtual void DrawFillRectangle(f32 x, f32 y, f32 width, f32 height, D_Color clr, D_Color outclr);
		virtual void DrawFillCircle(f32 x, f32 y, f32 radius, D_Color clr, D_Color outclr);

	private:
		WinGDIRenderer* m_poRenderer;
	};
}

#endif