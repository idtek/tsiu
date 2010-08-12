#include "TGui_CheckBox.h"
#include "TGui_EventHandler.h"
#include "TGui_Event.h"

namespace TsiU
{
	GuiCheckBox::GuiCheckBox(s32 _x, s32 _y, u32 _width, u32 _height, const Char* p_strName)
		:m_uiWidth(_width),m_uiHeight(_height)
	{
		m_vPos = Vec3(_x, _y, 0);
		strncpy(m_strName, p_strName, kMaxNameLength);

		m_bIsChecked = false;
		m_uiSquare = m_uiHeight;

		RenderWindowMsg::RegisterMsgListener(new GuiCheckBoxMsgCallBack(this));
	}

	void GuiCheckBox::Tick(f32 p_fDeltaTime)
	{
	}

	Bool GuiCheckBox::IsInCheckBox(s32 _x, s32 _y)
	{
		return _x > m_vPos.x && _x < m_vPos.x + m_uiHeight &&
			_y > m_vPos.y && _y < m_vPos.y + m_uiHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	GuiCheckBoxMsgCallBack::GuiCheckBoxMsgCallBack(GuiCheckBox* p_poCB)
		:m_poCBox(p_poCB)
	{
	}
	void GuiCheckBoxMsgCallBack::OnMouseLDown(s32 x, s32 y)
	{
		if(m_poCBox->IsInCheckBox(x, y))
		{
			m_poCBox->SetCheck(!m_poCBox->IsChecked());
			GuiEvent* pEvent = new GuiEvent;
			pEvent->AddParam(m_poCBox->IsChecked() ? 1 : 0);
			m_poCBox->ExecuteEventHandler(E_GUI_EVENT_CHECKBOX_VALUECHANGE, pEvent);
			delete pEvent;
		}
	}
}