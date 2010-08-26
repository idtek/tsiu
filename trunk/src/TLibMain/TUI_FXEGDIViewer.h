#ifndef __TUI_FXEGDIVIEWER__
#define __TUI_FXEGDIVIEWER__


#include "TUtility_Array.h"
#include "fx.h"

namespace TsiU
{
	class RenderWindowMsgListener;

	class FXEGDIViewer : public FXCanvas
	{
		FXDECLARE(FXEGDIViewer)

	protected:
		FXEGDIViewer();

	public:
		//Construct DX viewer widget
		FXEGDIViewer(FXComposite* p,
			FXObject* tgt = NULL,
			FXSelector sel = 0,
			FXuint opts = 0,
			FXint x = 0,
			FXint y = 0,
			FXint w = 0,
			FXint h = 0);

		void RegisterListener(RenderWindowMsgListener* _pListener){
			m_poMsgCallBack.PushBack(_pListener);
		}

		//Msg
		long onLeftBtnPress(FXObject* sender, FXSelector sel, void *ptr);
		long onLeftBtnRelease(FXObject* sender, FXSelector sel, void *ptr);
		long onMiddleBtnPress(FXObject* sender, FXSelector sel, void *ptr);
		long onMiddleBtnRelease(FXObject* sender, FXSelector sel, void *ptr);
		long onRightBtnPress(FXObject* sender, FXSelector sel, void *ptr);
		long onRightBtnRelease(FXObject* sender, FXSelector sel, void *ptr);
		long onKeyPress(FXObject* sender, FXSelector sel, void *ptr);
		long onMotion(FXObject* sender, FXSelector sel, void *ptr);
		long onConfigure(FXObject* sender, FXSelector sel, void *ptr);

	public:
		enum{
			ID_LAST
		};

	private:
		Array<RenderWindowMsgListener*> m_poMsgCallBack;
	};
}

#endif