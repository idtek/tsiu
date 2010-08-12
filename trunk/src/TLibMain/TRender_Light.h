#ifndef __TRENDER_LIGHT__
#define __TRENDER_LIGHT__

#include "TCore_Types.h"
#include "TRender_Color.h"
#include "TMath_Vector3.h"
#include "TEngine_Object.h"

namespace TsiU
{
	enum
	{
		E_LightType_None,
		E_LightType_Spot,
		E_LightType_Point
	};

	class Light : public Object
	{
	public:
		Light();
		virtual ~Light();

		virtual void Create(){};
		virtual void Tick(f32 _fDeltaTime){};

		void ResetLight();
		//Helper function
		void CreateSpotLight(const Vec3& p_vPos, const Vec3& p_vDirection, D_Color& p_Color);
		void CreatePointLight(const Vec3& p_vPos, D_Color& p_Color);

	public:
		u32		m_uiType;
		D_Color m_Ambient;
		D_Color m_Diffuse;
		D_Color m_Specular;
		Vec3	m_vPosition;
		Vec3	m_vDirection;
		f32		m_fRange;
		f32		m_fFalloff;
		f32		m_fAttenuation[3];
		f32		m_fTheta;
		f32		m_fPhi;
	};
}

#endif