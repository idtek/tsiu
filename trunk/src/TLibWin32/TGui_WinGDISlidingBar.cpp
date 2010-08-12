#include "TGui_WinGDISlidingBar.h"
#include "TRender_SimpleRenderObject.h"

namespace TsiU
{
	WinGDIGuiSlidingBar::WinGDIGuiSlidingBar(SimpleRenderObjectUtility* _poUtility,
												s32 _x, 
												s32 _y, 
												u32 _width, 
												u32 _height, 
												f32 _start, 
												f32 _end, 
												f32 _step, 
												Bool _isVer,
												D_Color _Clr,
												D_Color _OutClr,
												D_Color _FontClr)
		:GuiSlidingBar(_x,_y,_width,_height,_start,_end,_step,_isVer),
		m_poSRO(_poUtility),
		m_Clr(_Clr),
		m_OutClr(_OutClr),
		m_FontClr(_FontClr)
	{
		if(!m_poSRO)
			D_Warning("Do you forget to set SRO Utility?");
	}

	void WinGDIGuiSlidingBar::Draw()
	{
		if(!m_poSRO)
			return;

		if(!m_bIsVertical)
			m_poSRO->DrawFillRectangle(m_vPos.x, m_vPos.y + m_uiBlockRadius - 2.f, m_uiWidth, 4.f, m_Clr, m_OutClr);
		else
			m_poSRO->DrawFillRectangle(m_vPos.x + m_uiBlockRadius - 2.f, m_vPos.y, 4.f, m_uiHeight, m_Clr, m_OutClr);

		if(IsDragStart())
		{
			m_poSRO->DrawFillCircle(m_uiBlockCenter.x, m_uiBlockCenter.y, m_uiBlockRadius, D_Color(128,128,128), m_OutClr);
			m_poSRO->DrawFillCircle(m_uiBlockCenter.x, m_uiBlockCenter.y, m_uiBlockRadius, m_Clr, m_OutClr);
		}
		else
		{
			m_poSRO->DrawFillCircle(m_uiBlockCenter.x, m_uiBlockCenter.y, m_uiBlockRadius, D_Color(128,128,128), m_OutClr);
			m_poSRO->DrawFillCircle(m_uiBlockCenter.x - 1.f, m_uiBlockCenter.y - 1.f, m_uiBlockRadius - 0.5f, m_Clr, m_OutClr);
		}
		
	}
}