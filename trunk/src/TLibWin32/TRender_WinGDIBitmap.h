#ifndef __TRENDER_WINGDI_BITMAP__
#define __TRENDER_WINGDI_BITMAP__

#include "TCore_Types.h"

#include "TRender_Color.h"

namespace TsiU
{
	class WinGDIRenderer;

	class WinGDIBitmap
	{
	public:
		WinGDIBitmap();
		WinGDIBitmap(WinGDIRenderer* _poRenderer, StringPtr szFileName);
		WinGDIBitmap(WinGDIRenderer* _poRenderer, u32 uiResID, HINSTANCE hInstance);
		WinGDIBitmap(WinGDIRenderer* _poRenderer, s32 iWidth, s32 iHeight, D_Color crColor = D_Color(0, 0, 0));
		virtual ~WinGDIBitmap();

		Bool Create(WinGDIRenderer* _poRenderer, StringPtr szFileName);
		Bool Create(WinGDIRenderer* _poRenderer, u32 uiResID, HINSTANCE hInstance);
		Bool Create(WinGDIRenderer* _poRenderer, s32 iWidth, s32 iHeight, D_Color crColor);

		void Draw(WinGDIRenderer* _poRenderer, s32 x, s32 y, 
			Bool bTrans = false,
			D_Color crTransColor = D_Color(255, 0, 255)
			);
		void DrawPart(WinGDIRenderer* _poRenderer,
			s32 x,s32 y,
			s32 xPart,s32 yPart,
			s32 wPart,s32 hPart,
			Bool bTrans,
			D_Color crTransColor = D_Color(255, 0, 255));
		void Draw(WinGDIRenderer* _poRenderer, s32 x, s32 y, s32 width, s32 height,
			Bool bTrans = false,
			D_Color crTransColor = D_Color(255, 0, 255));

		D_Inline s32 GetWidth(){ return m_iWidth;	};
		D_Inline s32 GetHeight(){ return m_iHeight; };

	private:
		HBITMAP	m_hBitmap;		
		s32		m_iWidth;		
		s32		m_iHeight;		

		void Free();
	};
}

#endif