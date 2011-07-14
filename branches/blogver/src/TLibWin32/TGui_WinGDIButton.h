#ifndef __TGUI_WINGDI_BUTTON__
#define __TGUI_WINGDI_BUTTON__

#include "TCore_Types.h"
#include "TGui_Button.h"
#include "TRender_Color.h"

namespace TsiU
{
	class SimpleRenderObjectUtility;

	class WinGDIGuiButton : public GuiButton
	{
	public:
		WinGDIGuiButton(SimpleRenderObjectUtility* p_poUtility,
						u32 p_uiX, u32 p_uiY, 
						const Char* p_strName = "Button",
						u32 p_uiWidth = 80, 
						u32 p_uiHeight = 25,
						D_Color p_Clr = D_Color(192,192,192),
						D_Color p_OutClr = D_Color(0,0,0),
						D_Color p_FontClr = D_Color(0,0,0));

		virtual void Draw();

	protected:
		D_Color						m_Clr;
		D_Color						m_OutClr;
		D_Color						m_FontClr;
		SimpleRenderObjectUtility*	m_poSRO;
	};
}

#endif