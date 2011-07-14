#include "TRender_Camera.h"
#include "TMath_Matrix4.h"
#include "TMath_Quaternion.h"
#include "TRender_Renderer.h"
#include "TEngine_RenderModule.h"
#include "TCore_LibSettings.h"
#include "TEngine_Private.h"

namespace TsiU
{
	Camera::Camera()
		:Object(),
		 m_mViewMatrix(Mat4::ZERO),
		 m_mProjectionMatrix(Mat4::ZERO),
		 m_fFov(Math::kTPI * 0.25f),
		 m_fNearPlane(1.f),
		 m_fFarPlane(1000.f),
		 m_fAspect(1.33333333333f),
		 m_vLook(Vec3::ZUNIT),
		 m_vUp(Vec3::YUNIT),
		 m_vRight(Vec3::XUNIT)
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::MoveLeftRight(f32 _vUnits)
	{
		m_vPos += m_vRight * _vUnits;
	}

	void Camera::MoveUpDown(f32 _vUnits)
	{
		m_vPos += m_vUp * _vUnits;
	}

	void Camera::MoveForwardBack(f32 _vUnits)
	{
		m_vPos += m_vLook * _vUnits;
	}

	void Camera::Pitch(f32 _vAngle)
	{
		Quat q;
		q.AngleAxisToQuat(_vAngle, m_vRight);
		m_vUp = m_vUp.TransformCoord(q);
		m_vLook = m_vLook.TransformCoord(q);
	}

	void Camera::Yaw(f32 _vAngle)
	{
		Quat l_Q;
		l_Q.AngleAxisToQuat(_vAngle, m_vUp);
		m_vRight = m_vRight.TransformCoord(l_Q);
		m_vLook = m_vLook.TransformCoord(l_Q);
	}

	void Camera::Roll(f32 _vAngle)
	{
		Quat l_Q;
		l_Q.AngleAxisToQuat(_vAngle, m_vLook);
		m_vUp = m_vUp.TransformCoord(l_Q);
		m_vRight = m_vRight.TransformCoord(l_Q);
	}

	void Camera::SetLookAt(const Vec3& _vTargetPoint)
	{
		Vec3 vDir = _vTargetPoint - m_vPos;
		SetLook(vDir);
	}

	void Camera::SetLook(const Vec3& _vDirection)
	{
		if(_vDirection == Vec3::ZERO)
			return;

		m_vLook = _vDirection;
		m_vLook.Normalize();

		m_vUp = Vec3::YUNIT;
		m_vRight = m_vUp.CrossProduct(m_vLook);
		m_vRight.Normalize();
		m_vUp = m_vLook.CrossProduct(m_vRight);
		m_vUp.Normalize();
	}

	const Mat4& Camera::MakeProjectionMatrix()
	{
		f32 fZoom = 1.f;
		if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
		{
			Renderer* poRenderer = poGetEngine()->GetRenderMod()->GetRenderer();
			if(poRenderer)
				fZoom = poGetEngine()->GetRenderMod()->GetRenderer()->GetZoomFromGUI();
		}
		f32 theta = m_fFov * 0.5f;
		f32 h = 1 / Math::Tan(theta);
		f32 w = h / m_fAspect;
		f32 q, qn;
		if(m_fFarPlane == 0)
		{
			q = 1 - 0.00001f;
			qn = m_fNearPlane * q;
		}
		else
		{
			q = m_fFarPlane / ( m_fFarPlane - m_fNearPlane );
			qn = q * m_fNearPlane;
		}

		m_mProjectionMatrix = Mat4::ZERO;
		m_mProjectionMatrix[0][0] = w;
		m_mProjectionMatrix[1][1] = h;
		m_mProjectionMatrix[2][2] = q;
		m_mProjectionMatrix[3][2] = -1.0f;
		m_mProjectionMatrix[2][3] = qn;

		return m_mProjectionMatrix;
	}

	const Mat4& Camera::MakeViewMatrix()
	{
		SetLook(m_vLook);

		f32 x = -m_vRight.DotProduct(m_vPos);
		f32 y = -m_vUp.DotProduct(m_vPos);
		f32 z = -m_vLook.DotProduct(m_vPos);

		m_mViewMatrix.mat[0][0] = m_vRight.x;	m_mViewMatrix.mat[0][1] = m_vUp.x;	m_mViewMatrix.mat[0][2] = m_vLook.x;	m_mViewMatrix.mat[0][3] = 0.0f;
		m_mViewMatrix.mat[1][0] = m_vRight.y;	m_mViewMatrix.mat[1][1] = m_vUp.y;	m_mViewMatrix.mat[1][2] = m_vLook.y;	m_mViewMatrix.mat[1][3] = 0.0f;
		m_mViewMatrix.mat[2][0] = m_vRight.z;	m_mViewMatrix.mat[2][1] = m_vUp.z;	m_mViewMatrix.mat[2][2] = m_vLook.z;	m_mViewMatrix.mat[2][3] = 0.0f;
		m_mViewMatrix.mat[3][0] = x;			m_mViewMatrix.mat[3][1] = y;		m_mViewMatrix.mat[3][2] = z;			m_mViewMatrix.mat[3][3] = 1.0f;

		return m_mViewMatrix;
	}
}