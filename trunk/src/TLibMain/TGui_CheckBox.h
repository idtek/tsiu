#ifndef __TGUI_CHECKBOX__
#define __TGUI_CHECKBOX__

#include "TGui_Object.h"
#include "TRender_RenderWindowMsg.h"

namespace TsiU
{
	class GuiCheckBox : public GuiObject
	{
		static const int kMaxNameLength = 32;

	public:
		GuiCheckBox(s32 _x, s32 _y, u32 _width, u32 _height, const Char* p_strName);

		virtual void Tick(f32 p_fDeltaTime);

		Bool IsInCheckBox(s32 _x, s32 _y);

		inline Bool IsChecked()				{ return m_bIsChecked;	};
		inline void SetCheck(Bool _bVal)	{ m_bIsChecked = _bVal;	};

	protected:
		u32		m_uiWidth;
		u32		m_uiHeight;
		u32		m_uiSquare;
		Char	m_strName[kMaxNameLength];

		Bool	m_bIsChecked;
	};

	class GuiCheckBoxMsgCallBack : public RenderWindowMsgListener
	{
	public:
		GuiCheckBoxMsgCallBack(GuiCheckBox* p_poCB);

		virtual void OnMouseLDown(s32 x, s32 y);

	protected:
		GuiCheckBox* m_poCBox;
	};
}

#endif