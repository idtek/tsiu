#include "TRender_Light.h"

namespace TsiU
{
	Light::Light()
	{
		ResetLight();
	}
	Light::~Light()
	{
	}
	void Light::ResetLight()
	{
		m_uiType			= E_LightType_None;
		m_Ambient			= D_Color(0,0,0);
		m_Diffuse			= D_Color(0,0,0);
		m_Specular			= D_Color(0,0,0);
		m_vPosition			= Vec3::ZERO;
		m_vDirection		= Vec3::ZUNIT;
		m_fRange			= 0.f;
		m_fFalloff			= 0.f;
		m_fAttenuation[0]	= 0.f;
		m_fAttenuation[1]	= 0.f;
		m_fAttenuation[2]	= 0.f;
		m_fTheta			= 0.f;
		m_fPhi				= 0.f;
	}
	void Light::CreateSpotLight(const Vec3& p_vPos, const Vec3& p_vDirection, D_Color& p_Color)
	{
		ResetLight();
		m_uiType			= E_LightType_Spot;
		m_Ambient			= p_Color * 0.0f;
		m_Diffuse			= p_Color;
		m_Specular			= p_Color * 0.6f;
		m_vPosition			= p_vPos;
		m_vDirection		= p_vDirection;
		m_fRange			= 1000.0f;
		m_fFalloff			= 1.0f;
		m_fAttenuation[0]	= 1.0f;
		m_fTheta			= 0.4f;
		m_fPhi				= 0.9f;
	}
	void Light::CreatePointLight(const Vec3& p_vPos, D_Color& p_Color)
	{
		ResetLight();
		m_uiType			= E_LightType_Point;
		m_Ambient			= p_Color * 0.6f;
		m_Diffuse			= p_Color;
		m_Specular			= p_Color * 0.6f;
		m_vPosition			= p_vPos;
		m_fRange			= 1000.0f;
		m_fFalloff			= 1.0f;
		m_fAttenuation[0]	= 1.0f;
	}
}