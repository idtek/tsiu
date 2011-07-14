#ifndef __TGUI_WINGDI_SLIDINGBAR__
#define __TGUI_WINGDI_SLIDINGBAR__

#include "TCore_Types.h"
#include "TGui_SlidingBar.h"
#include "TRender_Color.h"

namespace TsiU
{
	class SimpleRenderObjectUtility;

	class WinGDIGuiSlidingBar : public GuiSlidingBar
	{
	public:
		WinGDIGuiSlidingBar(SimpleRenderObjectUtility* _poUtility,
							s32 _x, 
							s32 _y, 
							u32 _width, 
							u32 _height, 
							f32 _start, 
							f32 _end, 
							f32 _step, 
							Bool _isVer = false,
							D_Color _Clr = D_Color(192,192,192),
							D_Color _OutClr = D_Color(0,0,0),
							D_Color _FontClr = D_Color(0,0,0));

		virtual void Draw();

	protected:
		D_Color						m_Clr;
		D_Color						m_OutClr;
		D_Color						m_FontClr;
		SimpleRenderObjectUtility*	m_poSRO;
	};
}

#endif