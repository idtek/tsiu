#ifndef __TRENDER_WINGDI_RENDERER__
#define __TRENDER_WINGDI_RENDERER__

#include "TCore_Types.h"
#include "TRender_Renderer.h"
#include "TRender_RenderWindowMsg.h"

#include "TWin32_Private.h"

namespace TsiU
{
	class WinGDIRenderer : public Renderer,public RenderWindowMsgListener
	{
	public:		
		virtual void InitRender(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter);
		virtual void Clear(const D_Color& p_Color, u32 p_uiClearFlag);
		virtual Bool BeginFrame();
		virtual void EndFrame();
		virtual void Flip();
		virtual void ShowFPS(f32 fps);

		virtual void SetWorldMatrix(const Mat4 &m){};
		virtual void SetViewMatrix(const Mat4 &m){};
		virtual void SetProjectionMatrix(const Mat4 &m){};

		//virtual Mat4 GetViewMatrix(){};
		//virtual Mat4 GetProjectionMatrix(){};

		virtual void SetLight(u32 p_uiIdx, const Light& p_Light){};
		virtual Texture* CreateTexture(const Char * filename)
		{
			return NULL;
		};

		//virtual void MakeProjectionMatrix(f32 p_fFOV, f32 p_fAspect, f32 p_fNearPlane, 
		//	f32 p_fFarPlane, Mat4& p_Result){};

		inline HDC GetOffscreenDC(){ return m_poOffscreenDC;	};

		virtual void OnWindowResize(s32 width, s32 height);

		inline HWND GetHWND() { return m_poMainHwnd; }

	private:
		HWND	m_poMainHwnd;
		HDC		m_poHdc;

		HDC		m_poOffscreenDC;
		HBITMAP	m_poOffscreenBitmap;
	};
}

#endif