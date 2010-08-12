#include "TRender_WinDxSimpleRenderObject.h"
#include "TRender_WinDxMapping.h"
#include "TRender_WinDxRenderer.h"
#include "TEngine_Private.h"
#include "TEngine_RenderModule.h"
#include "TRender_WinDxTexture.h"

namespace TsiU
{
	const u32 WinDxSimpleRenderObjectVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
	const u32 WinDxSimpleRenderObjectVertexWithTex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	WinDxSimpleRenderObjectUtility::WinDxSimpleRenderObjectUtility(WinDxRenderer* p_poRenderer)
		:m_poRenderer(p_poRenderer)
	{}

	SimpleRenderObject* WinDxSimpleRenderObjectUtility::CreateSphere(f32 p_fRadius, D_Color p_Color)
	{
		LPDIRECT3DDEVICE9 l_Device = m_poRenderer->GetActiveDevice();	
		WinDxSimpleRenderObject2* l_poShape = new WinDxSimpleRenderObject2;

		D3DXCreateSphere(l_Device, p_fRadius, 30, 30, &l_poShape->m_poMesh, NULL);

		l_poShape->m_Mtrl.Ambient  = WinDxMapping::MappingColor(p_Color);
		l_poShape->m_Mtrl.Diffuse  = WinDxMapping::MappingColor(p_Color);
		l_poShape->m_Mtrl.Specular = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
		l_poShape->m_Mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
		l_poShape->m_Mtrl.Power    = 5.0f;

		return l_poShape;
	}

	SimpleRenderObject* WinDxSimpleRenderObjectUtility::CreateCube(f32 p_fWidth, f32 p_fHeight, f32 p_fDepth, D_Color p_Color)
	{
		LPDIRECT3DDEVICE9 l_Device = m_poRenderer->GetActiveDevice();	

		WinDxSimpleRenderObject* l_poShape = new WinDxSimpleRenderObject;

		l_poShape->m_ulNumberVertex = 24;
		l_poShape->m_ulNumberTri = 12;

		l_Device->CreateVertexBuffer(l_poShape->m_ulNumberVertex * sizeof(WinDxSimpleRenderObjectVertex),
			D3DUSAGE_WRITEONLY,
			WinDxSimpleRenderObjectVertex::FVF,
			D3DPOOL_MANAGED,
			&l_poShape->m_poVertexBuffer,
			0);
		l_Device->CreateIndexBuffer(3 * l_poShape->m_ulNumberTri * sizeof(u16),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&l_poShape->m_poIndexBuffer,
			0);

		WinDxSimpleRenderObjectVertex* l_poVertices = NULL;
		l_poShape->m_poVertexBuffer->Lock(0, 0, (void**)&l_poVertices, 0);
		// l_poVertices of a unit cube
		l_poVertices[0] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f);
		l_poVertices[1] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f);
		l_poVertices[2] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f);
		l_poVertices[3] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f);
		// fill in the back face vertex data
		l_poVertices[4] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, -p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f);
		l_poVertices[5] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, -p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f);
		l_poVertices[6] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f,  p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f);
		l_poVertices[7] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f,  p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f);
		// fill in the top face vertex data
		l_poVertices[8]  = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 1.0f, 0.0f);
		l_poVertices[9]  = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, 1.0f, 0.0f);
		l_poVertices[10] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, 1.0f, 0.0f);
		l_poVertices[11] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 1.0f, 0.0f);
		// fill in the bottom face vertex data
		l_poVertices[12] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, -1.0f, 0.0f);
		l_poVertices[13] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, -1.0f, 0.0f);
		l_poVertices[14] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, -1.0f, 0.0f);
		l_poVertices[15] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, -1.0f, 0.0f);
		// fill in the left face vertex data
		l_poVertices[16] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, -1.0f, 0.0f, 0.0f);
		l_poVertices[17] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f,  p_fHeight/2.0f,  p_fDepth/2.0f, -1.0f, 0.0f, 0.0f);
		l_poVertices[18] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, -1.0f, 0.0f, 0.0f);
		l_poVertices[19] = WinDxSimpleRenderObjectVertex(-p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, -1.0f, 0.0f, 0.0f);
		// fill in the right face vertex data
		l_poVertices[20] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 1.0f, 0.0f, 0.0f);
		l_poVertices[21] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, 1.0f, 0.0f, 0.0f);
		l_poVertices[22] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f,  p_fHeight/2.0f,  p_fDepth/2.0f, 1.0f, 0.0f, 0.0f);
		l_poVertices[23] = WinDxSimpleRenderObjectVertex( p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, 1.0f, 0.0f, 0.0f);
		l_poShape->m_poVertexBuffer->Unlock();

		// define the triangles of the cube:
		u16* l_poIndices = NULL;
		l_poShape->m_poIndexBuffer->Lock(0, 0, (void**)&l_poIndices, 0);
		// fill in the front face index data
		l_poIndices[0] = 0; l_poIndices[1] = 1; l_poIndices[2] = 2;
		l_poIndices[3] = 0; l_poIndices[4] = 2; l_poIndices[5] = 3;
		// fill in the back face index data
		l_poIndices[6] = 4; l_poIndices[7]  = 5; l_poIndices[8]  = 6;
		l_poIndices[9] = 4; l_poIndices[10] = 6; l_poIndices[11] = 7;
		// fill in the top face index data
		l_poIndices[12] = 8; l_poIndices[13] =  9; l_poIndices[14] = 10;
		l_poIndices[15] = 8; l_poIndices[16] = 10; l_poIndices[17] = 11;
		// fill in the bottom face index data
		l_poIndices[18] = 12; l_poIndices[19] = 13; l_poIndices[20] = 14;
		l_poIndices[21] = 12; l_poIndices[22] = 14; l_poIndices[23] = 15;
		// fill in the left face index data
		l_poIndices[24] = 16; l_poIndices[25] = 17; l_poIndices[26] = 18;
		l_poIndices[27] = 16; l_poIndices[28] = 18; l_poIndices[29] = 19;
		// fill in the right face index data
		l_poIndices[30] = 20; l_poIndices[31] = 21; l_poIndices[32] = 22;
		l_poIndices[33] = 20; l_poIndices[34] = 22; l_poIndices[35] = 23;
		
		l_poShape->m_poIndexBuffer->Unlock();

		l_poShape->m_ulDrawType = D3DPT_TRIANGLELIST;

		l_poShape->m_Mtrl.Ambient  = WinDxMapping::MappingColor(p_Color);
		l_poShape->m_Mtrl.Diffuse  = WinDxMapping::MappingColor(p_Color);
		l_poShape->m_Mtrl.Specular = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
		l_poShape->m_Mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
		l_poShape->m_Mtrl.Power    = 5.0f;

		return l_poShape;
	}

	SimpleRenderObject* WinDxSimpleRenderObjectUtility::CreateCubeWithTex(f32 p_fWidth, f32 p_fHeight, f32 p_fDepth, D_Color p_Color, Texture* _poTex)
	{
		LPDIRECT3DDEVICE9 l_Device = m_poRenderer->GetActiveDevice();	

		WinDxSimpleRenderObjectWithTex* l_poShape = new WinDxSimpleRenderObjectWithTex;

		l_poShape->m_ulNumberVertex = 24;
		l_poShape->m_ulNumberTri = 12;

		l_Device->CreateVertexBuffer(l_poShape->m_ulNumberVertex * sizeof(WinDxSimpleRenderObjectVertexWithTex),
			D3DUSAGE_WRITEONLY,
			WinDxSimpleRenderObjectVertexWithTex::FVF,
			D3DPOOL_MANAGED,
			&l_poShape->m_poVertexBuffer,
			0);
		l_Device->CreateIndexBuffer(3 * l_poShape->m_ulNumberTri * sizeof(u16),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&l_poShape->m_poIndexBuffer,
			0);

		WinDxSimpleRenderObjectVertexWithTex* l_poVertices = NULL;
		l_poShape->m_poVertexBuffer->Lock(0, 0, (void**)&l_poVertices, 0);

		f32 fMax = Math::Max<f32>(Math::Max<f32>(p_fWidth, p_fHeight), p_fDepth);
		fMax = 1.f;
		// l_poVertices of a unit cube
		l_poVertices[0] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f, 0.f, 0.f);
		l_poVertices[1] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f, 0.f, fMax);
		l_poVertices[2] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f, fMax, fMax);
		l_poVertices[3] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 0.0f, -1.0f, fMax, 0.f);
		// fill in the back face vertex data
		l_poVertices[4] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, -p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f, 0.f, 0.f);
		l_poVertices[5] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, -p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f, fMax, 0.f);
		l_poVertices[6] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f,  p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f, fMax, fMax);
		l_poVertices[7] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f,  p_fHeight/2.0f, p_fDepth/2.0f, 0.0f, 0.0f, 1.0f, 0.f, fMax);
		// fill in the top face vertex data
		l_poVertices[8]  = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 1.0f, 0.0f, 0.f, 0.f);
		l_poVertices[9]  = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, 1.0f, 0.0f, 0.f, fMax);
		l_poVertices[10] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, 1.0f, 0.0f, fMax, fMax);
		l_poVertices[11] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, 1.0f, 0.0f, fMax, 0.f);
		// fill in the bottom face vertex data
		l_poVertices[12] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, -1.0f, 0.0f, 0.f, 0.f);
		l_poVertices[13] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 0.0f, -1.0f, 0.0f, fMax, 0.f);
		l_poVertices[14] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, -1.0f, 0.0f, fMax, fMax);
		l_poVertices[15] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, 0.0f, -1.0f, 0.0f, 0.f, fMax);
		// fill in the left face vertex data
		l_poVertices[16] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, -1.0f, 0.0f, 0.0f, 0.f, fMax);
		l_poVertices[17] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f,  p_fHeight/2.0f,  p_fDepth/2.0f, -1.0f, 0.0f, 0.0f, fMax, fMax);
		l_poVertices[18] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, -1.0f, 0.0f, 0.0f, fMax, 0.f);
		l_poVertices[19] = WinDxSimpleRenderObjectVertexWithTex(-p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, -1.0f, 0.0f, 0.0f, 0.f, 0.f);
		// fill in the right face vertex data
		l_poVertices[20] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, -p_fHeight/2.0f, -p_fDepth/2.0f, 1.0f, 0.0f, 0.0f, 0.f, 0.f);
		l_poVertices[21] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f,  p_fHeight/2.0f, -p_fDepth/2.0f, 1.0f, 0.0f, 0.0f, fMax, 0.f);
		l_poVertices[22] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f,  p_fHeight/2.0f,  p_fDepth/2.0f, 1.0f, 0.0f, 0.0f, fMax, fMax);
		l_poVertices[23] = WinDxSimpleRenderObjectVertexWithTex( p_fWidth/2.0f, -p_fHeight/2.0f,  p_fDepth/2.0f, 1.0f, 0.0f, 0.0f, 0.f, fMax);
		l_poShape->m_poVertexBuffer->Unlock();

		// define the triangles of the cube:
		u16* l_poIndices = NULL;
		l_poShape->m_poIndexBuffer->Lock(0, 0, (void**)&l_poIndices, 0);
		// fill in the front face index data
		l_poIndices[0] = 0; l_poIndices[1] = 1; l_poIndices[2] = 2;
		l_poIndices[3] = 0; l_poIndices[4] = 2; l_poIndices[5] = 3;
		// fill in the back face index data
		l_poIndices[6] = 4; l_poIndices[7]  = 5; l_poIndices[8]  = 6;
		l_poIndices[9] = 4; l_poIndices[10] = 6; l_poIndices[11] = 7;
		// fill in the top face index data
		l_poIndices[12] = 8; l_poIndices[13] =  9; l_poIndices[14] = 10;
		l_poIndices[15] = 8; l_poIndices[16] = 10; l_poIndices[17] = 11;
		// fill in the bottom face index data
		l_poIndices[18] = 12; l_poIndices[19] = 13; l_poIndices[20] = 14;
		l_poIndices[21] = 12; l_poIndices[22] = 14; l_poIndices[23] = 15;
		// fill in the left face index data
		l_poIndices[24] = 16; l_poIndices[25] = 17; l_poIndices[26] = 18;
		l_poIndices[27] = 16; l_poIndices[28] = 18; l_poIndices[29] = 19;
		// fill in the right face index data
		l_poIndices[30] = 20; l_poIndices[31] = 21; l_poIndices[32] = 22;
		l_poIndices[33] = 20; l_poIndices[34] = 22; l_poIndices[35] = 23;
		
		l_poShape->m_poIndexBuffer->Unlock();

		l_poShape->m_ulDrawType = D3DPT_TRIANGLELIST;

		l_poShape->m_Mtrl.Ambient  = WinDxMapping::MappingColor(p_Color);
		l_poShape->m_Mtrl.Diffuse  = WinDxMapping::MappingColor(p_Color);
		l_poShape->m_Mtrl.Specular = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
		l_poShape->m_Mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
		l_poShape->m_Mtrl.Power    = 5.0f;

		WinDxTexture* poDxTex = (WinDxTexture*)_poTex;
		l_poShape->m_poTex		   = poDxTex->GetHandle();

		return l_poShape;
	}

	void WinDxSimpleRenderObjectUtility::DrawObject(SimpleRenderObject* p_poObj)
	{
		LPDIRECT3DDEVICE9 l_Device = m_poRenderer->GetActiveDevice();

		l_Device->SetRenderState(D3DRS_LIGHTING, true);
		l_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

		if(p_poObj->m_ulType == e_SimpleRenderObject_Type_1)
		{
			WinDxSimpleRenderObject* l_poShape = (WinDxSimpleRenderObject*)p_poObj;
			l_Device->SetMaterial(&l_poShape->m_Mtrl);
			l_Device->SetStreamSource(0, l_poShape->m_poVertexBuffer, 0, sizeof(WinDxSimpleRenderObjectVertex));
			l_Device->SetIndices(l_poShape->m_poIndexBuffer);
			l_Device->SetFVF(WinDxSimpleRenderObjectVertex::FVF);
			l_Device->DrawIndexedPrimitive((D3DPRIMITIVETYPE)l_poShape->m_ulDrawType, 0, 0, 
											l_poShape->m_ulNumberVertex, 0, 
											l_poShape->m_ulNumberTri);
		}
		else if(p_poObj->m_ulType == e_SimpleRenderObject_Type_2)
		{
			WinDxSimpleRenderObject2* l_poShape = (WinDxSimpleRenderObject2*)p_poObj;
			l_Device->SetMaterial(&l_poShape->m_Mtrl);
			l_poShape->m_poMesh->DrawSubset(0);
		}
		else if(p_poObj->m_ulType == e_SimpleRenderObject_Type_1_WithTex)
		{
		}
	}

	void WinDxSimpleRenderObject::Draw()
	{
		WinDxRenderer* poRenderer = (WinDxRenderer*)poGetEngine()->GetRenderMod()->GetRenderer();
		LPDIRECT3DDEVICE9 poDevice = poRenderer->GetActiveDevice();

		poRenderer->SetWorldMatrix(GetMatrix());

		poDevice->SetRenderState(D3DRS_LIGHTING, true);
		poDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

		poDevice->SetTexture(0, NULL);

		poDevice->SetMaterial(&m_Mtrl);
		poDevice->SetStreamSource(0, m_poVertexBuffer, 0, sizeof(WinDxSimpleRenderObjectVertex));
		poDevice->SetIndices(m_poIndexBuffer);
		poDevice->SetFVF(WinDxSimpleRenderObjectVertex::FVF);
		poDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)m_ulDrawType, 0, 0, 
										m_ulNumberVertex, 0, 
										m_ulNumberTri);
	}

	void WinDxSimpleRenderObjectWithTex::Draw()
	{
		WinDxRenderer* poRenderer = (WinDxRenderer*)poGetEngine()->GetRenderMod()->GetRenderer();
		LPDIRECT3DDEVICE9 poDevice = poRenderer->GetActiveDevice();

		poRenderer->SetWorldMatrix(GetMatrix());

		poDevice->SetRenderState(D3DRS_LIGHTING, true);
		poDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

		poDevice->SetMaterial(&m_Mtrl);
		poDevice->SetTexture(0, m_poTex);
		
		poDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        poDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        poDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        poDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		poDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		poDevice->SetStreamSource(0, m_poVertexBuffer, 0, sizeof(WinDxSimpleRenderObjectVertexWithTex));
		poDevice->SetIndices(m_poIndexBuffer);
		poDevice->SetFVF(WinDxSimpleRenderObjectVertexWithTex::FVF);
		poDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)m_ulDrawType, 0, 0, 
										m_ulNumberVertex, 0, 
										m_ulNumberTri);
	}

	void WinDxSimpleRenderObject2::Draw()
	{
		WinDxRenderer* poRenderer = (WinDxRenderer*)poGetEngine()->GetRenderMod()->GetRenderer();
		LPDIRECT3DDEVICE9 poDevice = poRenderer->GetActiveDevice();

		poRenderer->SetWorldMatrix(GetMatrix());

		poDevice->SetRenderState(D3DRS_LIGHTING, true);
		poDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		poDevice->SetTexture(0, NULL);

		poDevice->SetMaterial(&m_Mtrl);
		m_poMesh->DrawSubset(0);
	}
}