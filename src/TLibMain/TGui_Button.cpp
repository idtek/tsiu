#include "TGui_Button.h"
#include "TGui_EventHandler.h"

namespace TsiU
{
	GuiButton::GuiButton(u32 p_uiX, u32 p_uiY, u32 p_uiWidth, u32 p_uiHeight,const Char* p_strName)
	   :m_uiWidth(p_uiWidth),
		m_uiHeight(p_uiHeight),
		m_bIsClicked(false)
	{
		m_vPos = Vec3(p_uiX, p_uiY, 0);
		strncpy(m_strName, p_strName, kMaxNameLength);

		RenderWindowMsg::RegisterMsgListener(new GuiButtonMsgCallBack(this));
	}

	void GuiButton::Tick(f32 p_fDeltaTime)
	{
	}

	Bool GuiButton::bIsInArea(u32 x, u32 y)
	{
		return x > m_vPos.x && x < m_vPos.x + m_uiWidth &&
				y > m_vPos.y && y < m_vPos.y + m_uiHeight;
	}
	
	//////////////////////////////////////////////////////////////////////////
	GuiButtonMsgCallBack::GuiButtonMsgCallBack(GuiButton* p_poBtn)
		:m_poBtn(p_poBtn)
	{
	}

	void GuiButtonMsgCallBack::OnMouseLDown(s32 x, s32 y)
	{
		if(m_poBtn->bIsInArea(x, y))
		{
			m_poBtn->SetClick(true);
		}
	}
	void GuiButtonMsgCallBack::OnMouseLUp(s32 x, s32 y)
	{
		if(m_poBtn->bIsInArea(x, y) && m_poBtn->IsClick())
		{
			m_poBtn->ExecuteEventHandler(E_GUI_EVENT_BUTTON_CLICK, NULL);
		}
		m_poBtn->SetClick(false);
	}
}