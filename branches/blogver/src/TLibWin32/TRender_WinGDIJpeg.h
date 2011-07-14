#ifndef __TRENDER_WINGDI_JPEG__
#define __TRENDER_WINGDI_JPEG__

#include "TCore_Types.h"

#include "TRender_Color.h"

namespace TsiU
{
	class WinGDIRenderer;

	class WinGDIJpeg
	{
	public:
		WinGDIJpeg();
		WinGDIJpeg(WinGDIRenderer* _poRenderer, StringPtr szFileName);
		virtual ~WinGDIJpeg();

		Bool Create(WinGDIRenderer* _poRenderer, StringPtr szFileName);
		void Draw(WinGDIRenderer* _poRenderer, s32 x, s32 y, s32 width, s32 height);

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