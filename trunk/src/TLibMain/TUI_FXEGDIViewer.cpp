#include "TUI_FXEGDIViewer.h"
#include "TEngine_Private.h"
#include "TEngine_RenderModule.h"
#include "TRender_RenderWindowMsg.h"
#include "fxkeys.h"

namespace TsiU
{
	FXDEFMAP(FXEGDIViewer) FXEGDIViewerMap[]={
		FXMAPFUNC(SEL_MOTION,				0,		FXEGDIViewer::onMotion),
		FXMAPFUNC(SEL_LEFTBUTTONPRESS,		0,		FXEGDIViewer::onLeftBtnPress),
		FXMAPFUNC(SEL_LEFTBUTTONRELEASE,	0,		FXEGDIViewer::onLeftBtnRelease),
		FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,	0,		FXEGDIViewer::onMiddleBtnPress),
		FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,	0,		FXEGDIViewer::onMiddleBtnRelease),
		FXMAPFUNC(SEL_RIGHTBUTTONPRESS,		0,		FXEGDIViewer::onRightBtnPress),
		FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,	0,		FXEGDIViewer::onRightBtnRelease),
		FXMAPFUNC(SEL_KEYPRESS,				0,		FXEGDIViewer::onKeyPress),
		FXMAPFUNC(SEL_CONFIGURE,			0,		FXEGDIViewer::onConfigure)
	};
	// Object implementation
	FXIMPLEMENT(FXEGDIViewer,FXCanvas, FXEGDIViewerMap,ARRAYNUMBER(FXEGDIViewerMap))

	FXEGDIViewer::FXEGDIViewer()
	{
		flags |= FLAG_ENABLED;
	}

	FXEGDIViewer::FXEGDIViewer(FX::FXComposite *p, 
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
	}

	long FXEGDIViewer::onMotion(FXObject* sender, FXSelector sel, void *ptr)
	{	
		return 1;
	}

	long FXEGDIViewer::onLeftBtnPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		for(s32 i = 0 ; i < m_poMsgCallBack.Size(); ++i)
			m_poMsgCallBack[i]->OnMouseLDown(event->click_x, event->click_y);
		return 1;
	}
	long FXEGDIViewer::onLeftBtnRelease(FXObject* sender, FXSelector sel, void *ptr)
	{
		return 1;
	}
	long FXEGDIViewer::onMiddleBtnPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		return 1;
	}
	long FXEGDIViewer::onMiddleBtnRelease(FXObject* sender, FXSelector sel, void *ptr)
	{
		return 1;
	}
	long FXEGDIViewer::onRightBtnPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		return 1;
	}
	long FXEGDIViewer::onRightBtnRelease(FXObject* sender, FXSelector sel, void *ptr)
	{
		return 1;
	}
	long FXEGDIViewer::onKeyPress(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		for(s32 i = 0 ; i < m_poMsgCallBack.Size(); ++i)
			m_poMsgCallBack[i]->OnKeyPress(event->code, 0);
		return 1;
	}
	long FXEGDIViewer::onConfigure(FXObject* sender, FXSelector sel, void *ptr)
	{
		FXEvent* event=(FXEvent*)ptr;
		for(s32 i = 0 ; i < m_poMsgCallBack.Size(); ++i)
			m_poMsgCallBack[i]->OnWindowResize(event->rect.w, event->rect.h);
		return 1;
	}
}