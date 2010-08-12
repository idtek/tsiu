#include "TGui_SlidingBar.h"
#include "TGui_EventHandler.h"
#include "TGui_Event.h"

namespace TsiU
{
	GuiSlidingBar::GuiSlidingBar(s32 _x, s32 _y, u32 _width, u32 _height, f32 _start, f32 _end, f32 _step, Bool _isVer)
		:m_uiWidth(_width),m_uiHeight(_height),m_fStart(_start),m_fEnd(_end),m_fStep(_step),m_bIsVertical(_isVer)
	{
		m_vPos = Vec3(_x, _y, 0);

		m_bDragStart = false;
		m_fCurrent = m_fStart;
		m_uiBlockDelta = 0;

		m_fEnd -= m_fStep;

		if(m_bIsVertical)
		{
			m_uiBlockRadius = m_uiWidth / 2;
		}
		else
		{
			m_uiBlockRadius = m_uiHeight / 2;
		}

		RenderWindowMsg::RegisterMsgListener(new GuiSlidingBarMsgCallBack(this));
	}

	void GuiSlidingBar::Tick(f32 p_fDeltaTime)
	{
		if(m_bIsVertical)
		{
			m_uiBlockCenter = Vec3(m_vPos.x + m_uiBlockRadius, m_vPos.y + m_uiBlockDelta + m_uiBlockRadius, 0);
		}
		else
		{
			m_uiBlockCenter = Vec3(m_vPos.x + m_uiBlockDelta + m_uiBlockRadius, m_vPos.y + m_uiBlockRadius, 0);
		}
	}

	Bool GuiSlidingBar::IsInSlidingBlock(s32 _x, s32 _y)
	{
		return (m_uiBlockCenter.x - _x) * (m_uiBlockCenter.x - _x) + 
				(m_uiBlockCenter.y - _y) * (m_uiBlockCenter.y - _y) <= m_uiBlockRadius * m_uiBlockRadius;
	}
	Bool GuiSlidingBar::IsInSlidingBar(s32 _x, s32 _y)
	{
		return _x > m_vPos.x && _x < m_vPos.x + m_uiWidth &&
				_y > m_vPos.y && _y < m_vPos.y + m_uiHeight;
	}
	Bool GuiSlidingBar::MoveSlidingBlock(s32 _newX, s32 _newY)
	{
		f32 fNewValue;

		if(m_bIsVertical) 
		{
			m_uiMinLimited	= m_vPos.y + m_uiBlockRadius;
			m_uiMaxLimited	= m_vPos.y + m_uiHeight - m_uiBlockRadius;
			m_fValuePerPixel = (m_fEnd - m_fStart) / (m_uiMaxLimited - m_uiMinLimited);

			m_uiBlockCenter.y = Math::Clamp<s32>(_newY, m_uiMinLimited, m_uiMaxLimited);
			m_uiBlockDelta = m_uiBlockCenter.y - m_vPos.y - m_uiBlockRadius;
			fNewValue = m_fStart + (m_uiBlockCenter.y - m_uiMinLimited) * m_fValuePerPixel;
		}
		else
		{
			m_uiMinLimited	= m_vPos.x + m_uiBlockRadius;
			m_uiMaxLimited	= m_vPos.x + m_uiWidth - m_uiBlockRadius;
			m_fValuePerPixel = (m_fEnd - m_fStart) / (m_uiMaxLimited - m_uiMinLimited);

			m_uiBlockCenter.x = Math::Clamp<s32>(_newX, m_uiMinLimited, m_uiMaxLimited);
			m_uiBlockDelta = m_uiBlockCenter.x - m_vPos.x - m_uiBlockRadius;
			fNewValue = m_fStart + (m_uiBlockCenter.x - m_uiMinLimited) * m_fValuePerPixel;
		}

		fNewValue = (s32)((fNewValue - m_fStart) / m_fStep) * m_fStep + m_fStart;
		if(Math::IsZero(fNewValue - m_fCurrent))
		{
			return false;
		}
		else
		{
			m_fCurrent = fNewValue;
			return true;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	GuiSlidingBarMsgCallBack::GuiSlidingBarMsgCallBack(GuiSlidingBar* p_poSB)
		:m_poSBar(p_poSB)
	{
	}
	void GuiSlidingBarMsgCallBack::OnMouseLDrag(s32 x, s32 y)
	{
		if(m_poSBar->IsInSlidingBlock(x, y) || m_poSBar->IsDragStart())
		{
			m_poSBar->SetDragStart(true);
			if(m_poSBar->MoveSlidingBlock(x, y))
			{
				GuiEvent* pEvent = new GuiEvent;
				pEvent->AddParam(m_poSBar->GetCurrentValue());
				m_poSBar->ExecuteEventHandler(E_GUI_EVENT_SLIDINGBAR_VALUECHANGE, pEvent);
				delete pEvent;
			}
		}
	}
	void GuiSlidingBarMsgCallBack::OnMouseLUp(s32 x, s32 y)
	{
		m_poSBar->SetDragStart(false);
	}
	void GuiSlidingBarMsgCallBack::OnMouseLDown(s32 x, s32 y)
	{
		if(m_poSBar->IsInSlidingBar(x, y))
		{
			if(m_poSBar->MoveSlidingBlock(x, y))
			{
				GuiEvent* pEvent = new GuiEvent;
				pEvent->AddParam(m_poSBar->GetCurrentValue());
				m_poSBar->ExecuteEventHandler(E_GUI_EVENT_SLIDINGBAR_VALUECHANGE, pEvent);
				delete pEvent;
			}
		}
	}
}