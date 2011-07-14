#ifndef __TGUI_LABEL__
#define __TGUI_LABEL__

#include "TGui_Object.h"

namespace TsiU
{
	class GuiLabel : public GuiObject
	{
		static const int kMaxNameLength = 32;

	public:
		GuiLabel(s32 _x, s32 _y, u32 _width, u32 _height, const Char* p_strName);

		virtual void Tick(f32 p_fDeltaTime){};

	protected:
		u32		m_uiWidth;
		u32		m_uiHeight;
		Char	m_strName[kMaxNameLength];
	};
}

#endif