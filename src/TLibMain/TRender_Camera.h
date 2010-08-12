#ifndef __TRENDER_CAMERA__
#define __TRENDER_CAMERA__

#include "TCore_Types.h"
#include "TMath_Vector3.h"
#include "TEngine_Object.h"
#include "TMath_Matrix4.h"

namespace TsiU
{
	class Camera : public Object
	{
	public:
		Camera();
		~Camera();

		virtual void Create(){};
		virtual void Tick(f32 _fDeltaTime){};

		void MoveLeftRight(f32 _fUnits);
		void MoveUpDown(f32 _fUnits);
		void MoveForwardBack(f32 _fUnits);

		void Pitch(f32 _fAngle);
		void Yaw(f32 _fAngle);
		void Roll(f32 _fAngle);

		const Mat4& MakeViewMatrix();
		const Mat4& MakeProjectionMatrix();

		inline const Mat4& GetViewMatrix()		 const	{ return m_mViewMatrix;			}
		inline const Mat4& GetProjectionMatrix() const	{ return m_mProjectionMatrix;	}

		void SetLookAt(const Vec3& _vTargetPoint);
		void SetLook(const Vec3& _vDirection);

	private:	
		Mat4 m_mViewMatrix;
		Mat4 m_mProjectionMatrix;

		f32	 m_fFov;
		f32  m_fNearPlane;
		f32  m_fFarPlane;
		f32  m_fAspect;

		Vec3 m_vLook;
		Vec3 m_vUp;
		Vec3 m_vRight;
	};
}

#endif