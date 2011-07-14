#ifndef __TENGINE_CAMERA__
#define __TENGINE_CAMERA__

#include "TCore_Types.h"
#include "TMath_Vector3.h"

namespace TsiU
{
	class Mat4;

	class Camera
	{
	public:
		Camera();
		~Camera();

		void MoveLeftRight( f32 p_fUnits );
		void MoveUpDown( f32 p_fUnits );
		void MoveForwardBack( f32 p_fUnits );

		void Pitch( f32 p_fAngle);
		void Yaw( f32 p_fAngle );
		void Roll( f32 p_fAngle );

		void CalViewMatrix(Mat4 *vm);

		const Vec3& GetLookDirection() const	{ return m_vLook;	};
		const Vec3& GetRightDirection() const	{ return m_vRight;	};
		const Vec3& GetUpDirection() const		{ return m_vUp;		};
		const Vec3& GetPosition() const			{ return m_vPos;	};

		void SetPosition(const Vec3& l_vPos)					{ m_vPos	= l_vPos;	};
		//当心！！每次必须设定两个，要不会导致坐标错误
		void SetLookDirection(const Vec3& p_vLook)				{ m_vLook	= p_vLook;	};
		void SetRightDirection(const Vec3& p_vRight)			{ m_vRight	= p_vRight;	};
		void SetUpDirection(const Vec3& p_vUp)					{ m_vUp		= p_vUp;	};

		void ResetCamera();

	private:
		Vec3 m_vLook;
		Vec3 m_vUp;
		Vec3 m_vRight;
		Vec3 m_vPos;
	};
}

#endif