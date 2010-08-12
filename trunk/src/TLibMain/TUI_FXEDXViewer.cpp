#include "TUI_FXEDXViewer.h"
#include "TEngine_Private.h"
#include "TRender_Camera.h"
#include "TEngine_SceneModule.h"
#include "TEngine_RenderModule.h"
#include "TRender_Light.h"
#include "TMath_Quaternion.h"
#include "TMath_Basic.h"
#include "TRender_Renderer.h"
#include "fxkeys.h"

namespace TsiU
{
	FXDEFMAP(FXEDXViewer) FXEDXViewerMap[]={
		FXMAPFUNC(SEL_MOTION,				0,		FXEDXViewer::onMotion),
		FXMAPFUNC(SEL_LEFTBUTTONPRESS,		0,		FXEDXViewer::onLeftBtnPress),
		FXMAPFUNC(SEL_LEFTBUTTONRELEASE,	0,		FXEDXViewer::onLeftBtnRelease),
		FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,	0,		FXEDXViewer::onMiddleBtnPress),
		FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,	0,		FXEDXViewer::onMiddleBtnRelease),
		FXMAPFUNC(SEL_RIGHTBUTTONPRESS,		0,		FXEDXViewer::onRightBtnPress),
		FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,		FXEDXViewer::onRightBtnRelease),
		FXMAPFUNC(SEL_KEYPRESS,				0,		FXEDXViewer::onKeyPress)
	};
	// Object implementation
	FXIMPLEMENT(FXEDXViewer,FXCanvas, FXEDXViewerMap,ARRAYNUMBER(FXEDXViewerMap))

	FXEDXViewer::FXEDXViewer()
	{
		flags |= FLAG_ENABLED;
	}

	FXEDXViewer::FXEDXViewer(FX::FXComposite *p, 
							 FX::FXObject *tgt, 
							 FX::FXSelector sel, 
							 FX::FXuint opts, 
							 FX::FXint x, 
							 FX::FXint y, 
							 FX::FXint w, 
							 FX::FXint h)
		:FXCanvas(p,tgt,sel,opts,x,y,w,h)
	{
		flags |= FLAG_ENABLED;

		poGetEngine()->GetRenderMod()->SetRendererWin(this);

		m_poCameraInGUI = new Camera;
		m_poCameraInGUI->SetPosition(Vec3(0.f, 50.f, -50.f));
		m_poCameraInGUI->SetLookAt(Vec3(0,0,0));

		defaultCursor=getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
	}

	long FXEDXViewer::onMotion(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		flags &= ~FLAG_TIP;

		if(isEnabled())
		{
			if(target && target->tryHandle(this,FXSEL(SEL_MOTION, message),ptr)) 
				return 1;
			switch(m_eMode)
			{
			case ROTATING:
				_Rotating(event->last_x,event->last_y,event->win_x,event->win_y);
				break;
			case POSTING:
				_Posting(event->last_x,event->last_y,event->win_x,event->win_y);
				break;
			case ZOOMING:
				_Zooming(event->last_x,event->last_y,event->win_x,event->win_y);
				break;
			}
		}
		return 1;
	}

	long FXEDXViewer::onLeftBtnPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		flags &= ~FLAG_TIP;

		handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
		
		if(isEnabled())
		{
			grab();
			flags &= ~FLAG_UPDATE;
			if(target && target->tryHandle(this,FXSEL(SEL_LEFTBUTTONPRESS,message),ptr))
				return 1;
			_SetOp(ROTATING);
		}
		return 1;
	}
	long FXEDXViewer::onLeftBtnRelease(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		if(isEnabled())
		{
			ungrab();
			flags|=FLAG_UPDATE;
			if(target && target->tryHandle(this,FXSEL(SEL_LEFTBUTTONRELEASE,message),ptr)) 
				return 1;
			
			_SetOp(HOVERING);
		}
		return 1;
	}
	long FXEDXViewer::onMiddleBtnPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		flags &= ~FLAG_TIP;

		handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
		
		if(isEnabled())
		{
			grab();
			flags &= ~FLAG_UPDATE;
			if(target && target->tryHandle(this,FXSEL(SEL_MIDDLEBUTTONPRESS,message),ptr))
				return 1;
			_SetOp(ZOOMING);
		}
		return 1;
	}
	long FXEDXViewer::onMiddleBtnRelease(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		if(isEnabled())
		{
			ungrab();
			flags|=FLAG_UPDATE;
			if(target && target->tryHandle(this,FXSEL(SEL_MIDDLEBUTTONRELEASE,message),ptr)) 
				return 1;
			
			_SetOp(HOVERING);
		}
		return 1;
	}
	long FXEDXViewer::onRightBtnPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		flags &= ~FLAG_TIP;

		handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
		
		if(isEnabled())
		{
			grab();
			flags &= ~FLAG_UPDATE;
			if(target && target->tryHandle(this,FXSEL(SEL_RIGHTBUTTONPRESS,message),ptr))
				return 1;
			_SetOp(POSTING);
		}
		return 1;
	}
	long FXEDXViewer::onRightBtnRelease(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		if(isEnabled())
		{
			ungrab();
			flags|=FLAG_UPDATE;
			if(target && target->tryHandle(this,FXSEL(SEL_RIGHTBUTTONRELEASE,message),ptr)) 
				return 1;
			
			_SetOp(HOVERING);
		}
		return 1;
	}

	long FXEDXViewer::onKeyPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		flags &= ~FLAG_TIP;
		if(isEnabled())
		{
			if(target && target->tryHandle(this,FXSEL(SEL_KEYPRESS,message),ptr)) 
				return 1;

			switch(event->code)
			{
			case KEY_F6:
				poGetEngine()->GetSceneMod()->SetDefaultCamera((u32)0);
				poGetEngine()->GetRenderMod()->GetRenderer()->SetRotationFromGUI(Quat::IDENTITY);
				poGetEngine()->GetRenderMod()->GetRenderer()->SetTranslationFromGUI(Vec3::ZERO);
				poGetEngine()->GetRenderMod()->GetRenderer()->SetZoomFromGUI(1.0f);
				break;
			default:
				break;
			}
		}
		return 1;
	}

	void FXEDXViewer::_SetOp(Mode_t _eMode)
	{
		switch(_eMode)
		{
		case HOVERING:
			setDragCursor(getDefaultCursor());
			break;
		case ROTATING:
			setDragCursor(getApp()->getDefaultCursor(DEF_ROTATE_CURSOR));
			break;
		case ZOOMING:
			setDragCursor(getApp()->getDefaultCursor(DEF_DRAGH_CURSOR));
			break;
		case POSTING:
			setDragCursor(getApp()->getDefaultCursor(DEF_MOVE_CURSOR));
			break;
		}
		m_eMode = _eMode;
	}
	void FXEDXViewer::_Rotating(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY)
	{
		Camera* poCamera = poGetEngine()->GetSceneMod()->GetCamera();
		Quat q;

		q.ArcToQuat(_SpherePoint(_iX, _iY), _SpherePoint(_iLastX, _iLastY)) ;
		q = q * poGetEngine()->GetRenderMod()->GetRenderer()->GetRotaionFromGUI();
		//q.Inverse();

		poGetEngine()->GetRenderMod()->GetRenderer()->SetRotationFromGUI(q);
		poGetEngine()->GetSceneMod()->SetDefaultCamera(m_poCameraInGUI);
	}
	void FXEDXViewer::_Posting(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY)
	{
		Vec3 v = poGetEngine()->GetRenderMod()->GetRenderer()->GetTranslationFromGUI();
		v += _ScreenTo3D(_iLastX, _iLastY, _iX, _iY);
		poGetEngine()->GetRenderMod()->GetRenderer()->SetTranslationFromGUI(v);
		poGetEngine()->GetSceneMod()->SetDefaultCamera(m_poCameraInGUI);
	}
	void FXEDXViewer::_Zooming(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY)
	{
		//f32 fZoom = poGetEngine()->GetRenderMod()->GetRenderer()->GetZoomFromGUI();
		f32 fDelta = 0.1 * (_iY - _iLastY);
		//poGetEngine()->GetRenderMod()->GetRenderer()->SetZoomFromGUI(fZoom);
		poGetEngine()->GetSceneMod()->SetDefaultCamera(m_poCameraInGUI);

		//move camera to zoom
		m_poCameraInGUI->MoveForwardBack(fDelta);
	}

	Vec3 FXEDXViewer::_ScreenTo3D(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY)
	{
		Vec3 v(0.f,0.f,0.f);

		Vec3 vSrc(_iLastX, _iLastY, 0.f);
		Vec3 vDst(_iX, _iY, 0.f);

		/*Camera* poCam = m_poCameraInGUI;//poGetEngine()->GetSceneMod()->GetCamera();
		Mat4 mProj = poCam->GetProjectionMatrix();
		Mat4 mIProj = mProj.Inverse();
		Mat4 mView = poCam->GetViewMatrix();
		Mat4 mIView = mView.Inverse();
		Mat4 mIRet = mIProj * mIView;

		vSrc = vSrc * mIRet;
		vDst = vDst * mIRet;*/

		v = vDst - vSrc;
	
		u32 w = getWidth();
		u32 h = getHeight();
		u32 rw = poGetEngine()->GetRenderMod()->GetRenderer()->GetWidth();
		u32 rh = poGetEngine()->GetRenderMod()->GetRenderer()->GetHeight();
		
		v.x = v.x / w * rw / 10;
		v.y = -v.y / h * rh / 10;

		return v;
	}

	Vec3 FXEDXViewer::_SpherePoint(s32 _sX, s32 _sY)
	{
		f32 d, t, screenmin;
		Vec3 v;
		u32 w = getWidth();
		u32 h = getHeight();
		if(w > h)
			screenmin = (f32)h;
		else
			screenmin = (f32)w;
		v.x = 2.0f * (_sX - 0.5f * w) / screenmin;
		v.y = 2.0f * (0.5f * h - _sY) / screenmin;
		d = v.x * v.x + v.y * v.y;
		if(d < 0.75)
		{
			v.z = Math::Sqrt(1.0 - d);
		}
		else if( d < 3.0f)
		{
			d = 1.7320508008f - Math::Sqrt(d);
			t = 1.0f - d * d;
			if(t < 0.0f)
				t = 0.0f;
			v.z = 1.0f - Math::Sqrt(t);
		}
		else
		{
			v.z=0.0f;
		}
		v.Normalize();
		return v;
	}
}