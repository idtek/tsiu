#ifndef __TGUI_BUTTON__
#define __TGUI_BUTTON__

#include "TGui_Object.h"
#include "TRender_RenderWindowMsg.h"

namespace TsiU
{
	class GuiButton : public GuiObject
	{
		static const int kMaxNameLength = 32;

	public:
		GuiButton(u32 p_uiX, u32 p_uiY, u32 p_uiWidth, u32 p_uiHeight, const Char* p_strName);

		virtual void Tick(f32 p_fDeltaTime);

		Bool bIsInArea(u32 x, u32 y);
		inline void SetClick(Bool p_bVal)	{ m_bIsClicked = p_bVal;}
		inline Bool IsClick()				{ return m_bIsClicked;	}
		inline void SetText(const Char* p_poName)
		{
			strncpy(m_strName, p_poName, kMaxNameLength);
		}

	protected:
		//Appearance
		u32		m_uiWidth;
		u32		m_uiHeight;
		Char	m_strName[kMaxNameLength];
		Bool	m_bIsClicked;
	};

	class GuiButtonMsgCallBack : public RenderWindowMsgListener
	{
	public:
		GuiButtonMsgCallBack(GuiButton* p_poBtn);

		virtual void OnMouseLDown(s32 x, s32 y);
		virtual void OnMouseLUp(s32 x, s32 y);

	protected:
		GuiButton* m_poBtn;
	};
}

#endif