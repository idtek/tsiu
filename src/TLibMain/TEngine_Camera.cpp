#include "TEngine_Camera.h"
#include "TMath_Matrix4.h"
#include "TMath_Quaternion.h"

namespace TsiU
{
	Camera::Camera()
	{
		ResetCamera();	
	}

	Camera::~Camera()
	{
	}

	void Camera::MoveLeftRight( float p_vUnits )
	{
		m_vPos += m_vRight * p_vUnits;
	}

	void Camera::MoveUpDown( float p_vUnits )
	{
		m_vPos += m_vUp * p_vUnits;
	}

	void Camera::MoveForwardBack( float p_vUnits )
	{
		m_vPos += m_vLook * p_vUnits;
	}

	void Camera::Pitch( float p_vAngle )
	{
		Quat l_Q;
		l_Q.AngleAxisToQuat(p_vAngle, m_vRight);
		m_vUp = m_vUp.TransformCoord(l_Q);
		m_vLook = m_vLook.TransformCoord(l_Q);
	}

	void Camera::Yaw( float p_vAngle )
	{
		Quat l_Q;
		l_Q.AngleAxisToQuat(p_vAngle, m_vUp);
		m_vRight = m_vRight.TransformCoord(l_Q);
		m_vLook = m_vLook.TransformCoord(l_Q);
	}

	void Camera::Roll( float p_vAngle )
	{
		Quat l_Q;
		l_Q.AngleAxisToQuat(p_vAngle, m_vLook);
		m_vUp = m_vUp.TransformCoord(l_Q);
		m_vRight = m_vRight.TransformCoord(l_Q);
	}

	void Camera::CalViewMatrix(Mat4 *vm)
	{
		m_vLook.Nomalize();
		m_vUp = m_vLook.CrossProduct(m_vRight);
		m_vUp.Nomalize();
		m_vRight = m_vUp.CrossProduct(m_vLook);
		m_vRight.Nomalize();

		float x = -m_vRight.DotProduct(m_vPos);
		float y = -m_vUp.DotProduct(m_vPos);
		float z = -m_vLook.DotProduct(m_vPos);

		vm->mat[0][0] = m_vRight.x;	vm->mat[0][1] = m_vUp.x;	vm->mat[0][2] = m_vLook.x;	vm->mat[0][3] = 0.0f;
		vm->mat[1][0] = m_vRight.y;	vm->mat[1][1] = m_vUp.y;	vm->mat[1][2] = m_vLook.y;	vm->mat[1][3] = 0.0f;
		vm->mat[2][0] = m_vRight.z;	vm->mat[2][1] = m_vUp.z;	vm->mat[2][2] = m_vLook.z;	vm->mat[2][3] = 0.0f;
		vm->mat[3][0] = x;			vm->mat[3][1] = y;			vm->mat[3][2] = z;			vm->mat[3][3] = 1.0f;
	}

	void Camera::ResetCamera()
	{
		m_vPos		= Vec3::ZERO;
		m_vRight	= Vec3::XUNIT;
		m_vUp		= Vec3::YUNIT;
		m_vLook		= Vec3::ZUNIT;
	}
}