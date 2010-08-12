#include "TGui_WinGDICheckBox.h"
#include "TRender_SimpleRenderObject.h"

namespace TsiU
{
	WinGDIGuiCheckBox::WinGDIGuiCheckBox(SimpleRenderObjectUtility* p_poUtility,
										u32 p_uiX, u32 p_uiY, 
										const Char* p_strName,
										u32 p_uiWidth, u32 p_uiHeight,
										D_Color p_Clr,
										D_Color p_OutClr,
										D_Color p_FontClr)
		:GuiCheckBox(p_uiX,p_uiY,p_uiWidth,p_uiHeight,p_strName),
		m_poSRO(p_poUtility),
		m_Clr(p_Clr),
		m_OutClr(p_OutClr),
		m_FontClr(p_FontClr)
	{
		if(!m_poSRO)
			D_Warning("Do you forget to set SRO Utility?");
	}

	void WinGDIGuiCheckBox::Draw()
	{
		if(!m_poSRO)
			return;

		m_poSRO->DrawRectangle(m_vPos.x, m_vPos.y, m_uiSquare, m_uiSquare, m_OutClr, 2);

		if(m_bIsChecked)
		{
			m_poSRO->DrawFillRectangle(m_vPos.x + 3.f, m_vPos.y + 3.f, m_uiSquare - 6.f, m_uiSquare - 6.f, m_Clr, m_Clr );
		}

		m_poSRO->DrawStringEx(m_vPos.x + m_uiSquare + 3.f, m_vPos.y, m_uiWidth - (m_uiSquare + 3.f),
			m_uiHeight, 0, m_strName, NULL, m_FontClr);
	}
}