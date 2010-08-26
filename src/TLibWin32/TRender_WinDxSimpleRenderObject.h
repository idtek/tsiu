#ifndef __TRENDER_WINDX_SIMPLERENDEROBJECT__
#define __TRENDER_WINDX_SIMPLERENDEROBJECT__

#include "TCore_Types.h"

#include "TRender_SimpleRenderObject.h"

#ifndef TLIB_NOWINDX

namespace TsiU
{
	class WinDxRenderer;

	struct WinDxSimpleRenderObjectVertex
	{
		WinDxSimpleRenderObjectVertex(f32 _x, f32 _y, f32 _z, f32 _nx, f32 _ny, f32 _nz)
		{
			x = _x; y = _y; z = _z; 
			nx = _nx; ny = _ny; nz = _nz;
		}
		f32		x, y, z;
		f32		nx, ny, nz;
		static const u32 FVF;
	};

	struct WinDxSimpleRenderObjectVertexWithTex
	{
		WinDxSimpleRenderObjectVertexWithTex(f32 _x, f32 _y, f32 _z, f32 _nx, f32 _ny, f32 _nz, f32 _tu, f32 _tv)
		{
			x = _x; y = _y; z = _z; 
			nx = _nx; ny = _ny; nz = _nz;
			tu = _tu; tv = _tv;
		}
		f32		x, y, z;
		f32		nx, ny, nz;
		f32		tu, tv;
		static const u32 FVF;
	};

	class WinDxSimpleRenderObject : public SimpleRenderObject
	{
	public:
		WinDxSimpleRenderObject()
		{
			m_ulType = e_SimpleRenderObject_Type_1;
		}
		virtual ~WinDxSimpleRenderObject()
		{
			D_SafeRelease(m_poVertexBuffer);
			D_SafeRelease(m_poIndexBuffer);
		}
		virtual void Draw();

		IDirect3DVertexBuffer9*	m_poVertexBuffer;
		IDirect3DIndexBuffer9*  m_poIndexBuffer;
		D3DMATERIAL9			m_Mtrl;
		u32						m_ulNumberVertex;
		u32						m_ulNumberTri;
		u32						m_ulDrawType;
	};

	class WinDxSimpleRenderObjectWithTex : public WinDxSimpleRenderObject
	{
	public:
		WinDxSimpleRenderObjectWithTex()
		{
			m_ulType = e_SimpleRenderObject_Type_1_WithTex;
		}
		virtual void Draw();

		LPDIRECT3DTEXTURE9 m_poTex;
	};

	class WinDxSimpleRenderObject2 : public SimpleRenderObject
	{
	public:
		WinDxSimpleRenderObject2()
		{
			m_ulType = e_SimpleRenderObject_Type_2;
		}
		virtual ~WinDxSimpleRenderObject2()
		{
			D_SafeRelease(m_poMesh);
		}
		virtual void Draw();

		D3DMATERIAL9	m_Mtrl;
		ID3DXMesh*		m_poMesh;
	};

	class WinDxSimpleRenderObjectUtility : public SimpleRenderObjectUtility
	{
	public:
		WinDxSimpleRenderObjectUtility(WinDxRenderer* p_poRenderer);

		virtual void DrawObject(SimpleRenderObject* p_poObj);
		virtual SimpleRenderObject* CreateCube(f32 p_fWidth, f32 p_fHeight, f32 p_fDepth, D_Color p_Color);
		virtual SimpleRenderObject* CreateCubeWithTex(f32 p_fWidth, f32 p_fHeight, f32 p_fDepth, D_Color p_Color, Texture* _poTex);
		virtual SimpleRenderObject* CreateSphere(f32 p_fRadius, D_Color p_Color);

	private:
		WinDxRenderer* m_poRenderer;
	};
}

#endif

#endif