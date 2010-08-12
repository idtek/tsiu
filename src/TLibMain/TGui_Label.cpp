#include "TGui_Label.h"

namespace TsiU
{
	GuiLabel::GuiLabel(s32 _x, s32 _y, u32 _width, u32 _height, const Char* p_strName)
		:m_uiWidth(_width),m_uiHeight(_height)
	{
		m_vPos = Vec3(_x, _y, 0);
		strncpy(m_strName, p_strName, kMaxNameLength);
	}
}