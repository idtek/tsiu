#ifndef __TRENDER_WINDX_RENDERER__
#define __TRENDER_WINDX_RENDERER__

#include "TCore_Types.h"
#include "TRender_Renderer.h"

#include "TWin32_Private.h"

namespace TsiU
{
#define VERTICES_FOR_QUAD 4

	struct VertexDX
	{
		D3DXVECTOR3 m_pos;
		DWORD m_color;
		FLOAT m_u, m_v;
		FLOAT m_u1, m_v1;
	};

	const DWORD WINDX_FVF = { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 };

	class WinDxRenderer : public Renderer
	{
	public:		
		WinDxRenderer();
		virtual ~WinDxRenderer();

		virtual void InitRender(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter);
		virtual void Clear(const D_Color& p_Color, u32 p_uiClearFlag);
		virtual Bool BeginFrame();
		virtual void EndFrame();
		virtual void Flip();
		virtual void ShowFPS(f32 fps);

		virtual void SetWorldMatrix(const Mat4 &m);
		virtual void SetViewMatrix(const Mat4 &m);
		virtual void SetProjectionMatrix(const Mat4 &m);

		//virtual Mat4 GetViewMatrix();
		//virtual Mat4 GetProjectionMatrix();
		//virtual Viewport 

		virtual void SetLight(u32 p_uiIdx, const Light& p_Light);

		/*virtual void MakeProjectionMatrix(f32 p_fFOV, f32 p_fAspect, f32 p_fNearPlane, 
			f32 p_fFarPlane, Mat4& p_Result);*/
		virtual Texture* CreateTexture(const Char * _strFilename);

		LPDIRECT3DDEVICE9& GetActiveDevice(){
			return m_poMainD3DDevice;
		}

		void SetBlendMode(EBlendMode blendMode);
		void SetAddressMode(EAddressMode addrMode);
		void SetMaskMode(EMaskMode maskMode);
private:
		LPDIRECT3DDEVICE9	m_poMainD3DDevice;
		HWND				m_poMainHwnd;
	};
}

#endif