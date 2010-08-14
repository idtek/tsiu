#include "TRender_WinDxRenderer.h"
#include "TRender_WinDxRenderWindow.h"
#include "TRender_WinDxMapping.h"
#include "TMath_Matrix4.h"
#include "TRender_Light.h"
#include "TRender_WinDxTexture.h"
#include "TRender_WinDxDevice.h"

#ifndef TLIB_NOWINDX
namespace TsiU
{
	WinDxRenderer::WinDxRenderer()
	{
	}
	WinDxRenderer::~WinDxRenderer()
	{
		D_SafeDelete(m_poMainWindow);
	}

	void WinDxRenderer::InitRender(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter)
	{
		WinDxRenderWindow *l_poMainWindow = new WinDxRenderWindow;
		D_CHECK(l_poMainWindow);

		l_poMainWindow->InitWindow(p_uiWidth, p_uiHeight, p_strTitle, p_bIsWindowed, p_poOuter);

		m_poMainWindow = l_poMainWindow;
		m_poMainD3DDevice = l_poMainWindow->GetDevice()->GetD3DDevice();
		m_poMainHwnd = l_poMainWindow->GetHWND();

		m_qRotationFromGUI = Quat::IDENTITY;
		m_vTranslationFromGUI = Vec3::ZERO;
		m_fZoomFromGUI = 1.f;
	}
	void WinDxRenderer::Clear(const D_Color& p_Color, u32 p_uiClearFlag)
	{
		D_CHECK(m_poMainD3DDevice);
		if(FAILED(m_poMainD3DDevice->Clear(0, NULL,
			WinDxMapping::MappingClearFlag(p_uiClearFlag),
			WinDxMapping::MappingColor(p_Color),
			1.f, 0)))
				D_FatalError("Clear Failed");
	}
	Bool WinDxRenderer::BeginFrame()
	{
		D_CHECK(m_poMainD3DDevice);
		//Clear(D_Color(200, 200, 200), E_ClearFlagTarget | E_ClearFlagZBuffer);

		//m_poMainD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		return SUCCEEDED(m_poMainD3DDevice->BeginScene());
	}
	void WinDxRenderer::EndFrame()
	{
		D_CHECK(m_poMainD3DDevice);
		m_poMainD3DDevice->EndScene();
	}
	void WinDxRenderer::Flip()
	{
		D_CHECK(m_poMainD3DDevice);
		m_poMainD3DDevice->Present(NULL, NULL, NULL, NULL);
	}
	void WinDxRenderer::ShowFPS(f32 fps)
	{
		Char l_strBuff[64]={0};
		sprintf(l_strBuff, "FPS = %.2f", fps);
		::SetWindowText(m_poMainHwnd, l_strBuff);
	}
	void WinDxRenderer::SetWorldMatrix(const Mat4 &p_WorldMat)
	{
		D_CHECK(m_poMainD3DDevice);
		
		/*Mat4 mRotFromGUI;
		mRotFromGUI.Rotation(m_qRotationFromGUI);
		Mat4 mTrans = p_WorldMat;

		Vec3 v = m_vTranslationFromGUI;
		mTrans.Translation(v.x, v.y, v.z);
		
		mTrans = mTrans * mRotFromGUI;*/

		D3DXMATRIX l_WorldMat = WinDxMapping::MappingMat4(p_WorldMat);
		HRESULT hr;
		if(FAILED( hr = m_poMainD3DDevice->SetTransform(D3DTS_WORLD, &l_WorldMat) ))
			D_FatalError("Cannot set World Matrix");
	}
	void WinDxRenderer::SetViewMatrix(const Mat4 &p_ViewMat)
	{
		D_CHECK(m_poMainD3DDevice);

		D3DXMATRIX l_ViewMat = WinDxMapping::MappingMat4(p_ViewMat);
		HRESULT hr;
		if(FAILED( hr = m_poMainD3DDevice->SetTransform(D3DTS_VIEW, &l_ViewMat) ))
			D_FatalError("Cannot set View Matrix");
	}
	void WinDxRenderer::SetProjectionMatrix(const Mat4 &p_ProjectionMatrix)
	{
		D_CHECK(m_poMainD3DDevice);

		D3DXMATRIX l_ProjectionMatrix = WinDxMapping::MappingMat4(p_ProjectionMatrix);
		HRESULT hr;
		if(FAILED( hr = m_poMainD3DDevice->SetTransform(D3DTS_PROJECTION, &l_ProjectionMatrix) ))
			D_FatalError("Cannot set Projection Matrix");
	}

	/*Mat4 WinDxRenderer::GetViewMatrix()
	{
		D3DXMATRIX mView;
		m_poMainD3DDevice->GetTransform(D3DTS_VIEW, &mView);
		return WinDxMapping::MappingDXMat4(mView);
	}
	Mat4 WinDxRenderer::GetProjectionMatrix()
	{
		D3DXMATRIX mProj;
		m_poMainD3DDevice->GetTransform(D3DTS_PROJECTION, &mProj);
		return WinDxMapping::MappingDXMat4(mProj);
	}*/
	Texture* WinDxRenderer::CreateTexture(const Char * _strFilename)
	{
		return new WinDxTexture(_strFilename);
	}

	void WinDxRenderer::SetLight(u32 p_uiIdx, const Light& p_Light)
	{
		D_CHECK(m_poMainD3DDevice);
		D3DLIGHT9 l_Light;
		memset(&l_Light, 0, sizeof(l_Light));
		switch(p_Light.m_uiType)
		{
		case E_LightType_Spot:
			l_Light.Type = D3DLIGHT_SPOT;
			break;
		case E_LightType_Point:
			l_Light.Type = D3DLIGHT_POINT;
			break;
		}
		l_Light.Ambient		= WinDxMapping::MappingColor(p_Light.m_Ambient);
		l_Light.Diffuse		= WinDxMapping::MappingColor(p_Light.m_Diffuse);
		l_Light.Specular	= WinDxMapping::MappingColor(p_Light.m_Specular);
		l_Light.Position	= D3DXVECTOR3(p_Light.m_vPosition.x, p_Light.m_vPosition.y, p_Light.m_vPosition.z);
		l_Light.Direction	= D3DXVECTOR3(p_Light.m_vDirection.x, p_Light.m_vDirection.y, p_Light.m_vDirection.z);
		l_Light.Range		= p_Light.m_fRange;
		l_Light.Falloff		= p_Light.m_fFalloff;
		l_Light.Attenuation0= p_Light.m_fAttenuation[0];
		l_Light.Attenuation1= p_Light.m_fAttenuation[1];
		l_Light.Attenuation2= p_Light.m_fAttenuation[2];
		l_Light.Theta		= p_Light.m_fTheta;
		l_Light.Phi			= p_Light.m_fPhi;

		m_poMainD3DDevice->SetLight(p_uiIdx, &l_Light);
		m_poMainD3DDevice->LightEnable(p_uiIdx, true);
	}
	/*void WinDxRenderer::MakeProjectionMatrix(f32 p_fFOV, f32 p_fAspect, f32 p_fNearPlane, 
		f32 p_fFarPlane, Mat4& p_Result)
	{
		f32 theta = p_fFOV * 0.5f;
		f32 h = 1 / Math::Tan(theta);
		f32 w = h / p_fAspect;
		f32 q, qn;
		if(p_fFarPlane == 0)
		{
			q = 1 - 0.00001f;
			qn = p_fNearPlane * q;
		}
		else
		{
			q = p_fFarPlane / ( p_fFarPlane - p_fNearPlane );
			qn = q * p_fNearPlane;
		}

		p_Result = Mat4::ZERO;
		p_Result[0][0] = w;
		p_Result[1][1] = h;
		p_Result[2][2] = q;
		p_Result[3][2] = -1.0f;
		p_Result[2][3] = qn;
	}*/

	void WinDxRenderer::SetBlendMode(EBlendMode blendMode)
	{
		m_poMainD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_poMainD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); 
        m_poMainD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);  

		m_poMainD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_poMainD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	void WinDxRenderer::SetAddressMode(EAddressMode addrMode)
	{
		m_poMainD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        m_poMainD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_poMainD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        m_poMainD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

	void WinDxRenderer::SetMaskMode(EMaskMode maskMode)
	{
		m_poMainD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		m_poMainD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 
		m_poMainD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_poMainD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
	}
}

#endif