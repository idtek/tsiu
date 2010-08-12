#ifndef __TRENDER_RENDER__
#define __TRENDER_RENDER__

#include "TCore_Types.h"
#include "TRender_Color.h"
#include "TRender_RenderWindow.h"
#include "TMath_Quaternion.h"
#include "TMath_Matrix4.h"

namespace TsiU
{
	class Mat4;
	class Light;
	class Texture;

	enum EBlendMode
	{
		EBlendMode_Normal =0, 
	};

	enum EMaskMode
	{
		EMaskMode_NoMask =0,
	};

	enum EAddressMode
	{
		EAddressMode_UWrap = 0,
		EAddressMode_UMirror = 1,
		EAddressMode_UClamp = 2,
		EAddressMode_UBorder = 3,
		EAddressMode_UMask = 0x3,

		EAddressMode_VWrap = 0,
		EAddressMode_VMirror = 4,
		EAddressMode_VClamp = 8,
		EAddressMode_VBorder = 12,
		EAddressMode_VMask = 0xC,
	};

	class Renderer
	{
	public:
		Renderer();
		virtual ~Renderer(){}

		virtual void InitRender(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter) = 0;
		virtual void Clear(const D_Color& p_Color, u32 p_uiClearFlag) = 0;
		virtual Bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Flip() = 0;
		virtual void ShowFPS(f32 fps) = 0;

		virtual void SetWorldMatrix(const Mat4 &m) = 0;
		virtual void SetViewMatrix(const Mat4 &m) = 0;
		virtual void SetProjectionMatrix(const Mat4 &m) = 0;

		//virtual Viewport GetViewport() = 0;

		virtual void SetLight(u32 p_uiIdx, const Light& p_Light) = 0;
		//virtual void MakeProjectionMatrix(f32 p_fFOV, f32 p_fAspect, f32 p_fNearPlane, f32 p_fFarPlane, Mat4& p_Result) = 0;

		virtual Texture* CreateTexture(const Char * filename) = 0;

		inline u32 GetWidth()					{ return m_poMainWindow->GetWindowWidth();			}
		inline u32 GetHeight()					{ return m_poMainWindow->GetWindowHeight();			}
		inline RenderWindow* GetMainWindow()	{ return m_poMainWindow;							}

		inline void SetFullScreen(Bool _bFull)		{ return m_poMainWindow->SetFullScreen(_bFull);		}

		inline void SetRotationFromGUI(const Quat& q)		{ m_qRotationFromGUI = q;		_UpdateMatrixFromGUI();}
		inline void SetTranslationFromGUI(const Vec3& v)	{ m_vTranslationFromGUI = v;	_UpdateMatrixFromGUI();}
		inline void SetZoomFromGUI(const f32& z)			{ m_fZoomFromGUI = z;			}
		inline Quat& GetRotaionFromGUI()			{ return m_qRotationFromGUI;	}
		inline Vec3& GetTranslationFromGUI()		{ return m_vTranslationFromGUI;	}
		inline f32&  GetZoomFromGUI()				{ return m_fZoomFromGUI;		}
		inline Mat4& GetMatrixFromGUI()				{ return m_mMatrixFromGUI;		}

	protected:
		void _UpdateMatrixFromGUI();

	protected:
		RenderWindow* m_poMainWindow;
		
		Mat4		  m_mMatrixFromGUI;
		Quat	      m_qRotationFromGUI;
		Vec3		  m_vTranslationFromGUI;
		f32		      m_fZoomFromGUI;
	};
}

#endif