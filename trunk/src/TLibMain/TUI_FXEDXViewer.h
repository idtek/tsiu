#ifndef __TUI_FXEDXVIEWER__
#define __TUI_FXEDXVIEWER__

#include "TCore_Types.h"
#include "TMath_Vector3.h"
#include "TMath_Quaternion.h"
#include "fx.h"

namespace TsiU
{
	class Camera;

	class FXEDXViewer : public FXCanvas
	{
		FXDECLARE(FXEDXViewer)
	protected:
		// Mouse actions when in viewing window
		enum Mode_t
		{
			HOVERING,                       // Hovering mouse w/o doing anything
			PICKING,                        // Pick mode
			ROTATING,                       // Rotating camera around target
			POSTING,                        // Posting right-mouse menu
			TRANSLATING,                    // Translating camera
			ZOOMING,                        // Zooming
			FOVING,                         // Change field-of-view
			DRAGGING,                       // Dragging objects
			TRUCKING,                       // Trucking camera
			GYRATING,                       // Rotation of camera around eye
			DO_LASSOSELECT,                 // Lasso select when mouse pressed
			LASSOSELECT,                    // Anchor of lasso rectangle
			DO_LASSOZOOM,                   // Zoom when mouse pressed
			LASSOZOOM                       // Zoom rectangle
		};
	protected:
		FXEDXViewer();

		void _SetOp(Mode_t _eMode);
		
		Vec3 _SpherePoint(s32 _sX, s32 _sY);
		Vec3 _ScreenTo3D(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY);

		void _Rotating(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY);
		void _Posting(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY);
		void _Zooming(s32 _iLastX, s32 _iLastY, s32 _iX, s32 _iY);

	public:
		//Construct DX viewer widget
		FXEDXViewer(FXComposite* p,
				    FXObject* tgt = NULL,
				    FXSelector sel = 0,
				    FXuint opts = 0,
				    FXint x = 0,
				    FXint y = 0,
				    FXint w = 0,
				    FXint h = 0);

		//Msg
		long onLeftBtnPress(FXObject* sender, FXSelector sel, void *ptr);
		long onLeftBtnRelease(FXObject* sender, FXSelector sel, void *ptr);
		long onMiddleBtnPress(FXObject* sender, FXSelector sel, void *ptr);
		long onMiddleBtnRelease(FXObject* sender, FXSelector sel, void *ptr);
		long onRightBtnPress(FXObject* sender, FXSelector sel, void *ptr);
		long onRightBtnRelease(FXObject* sender, FXSelector sel, void *ptr);
		long onKeyPress(FXObject* sender, FXSelector sel, void *ptr);

		long onMotion(FXObject* sender, FXSelector sel, void *ptr);
	public:
		enum{
			//SEL_RIGHTBUTTONPRESS = FXCanvas::ID_LAST,
			ID_LAST
		};

	protected:
		Mode_t			m_eMode;
		Camera*			m_poCameraInGUI;
	};
}

#endif