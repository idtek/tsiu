#ifndef __TRENDER_VIEWPORT__
#define __TRENDER_VIEWPORT__

#include "TCore_Types.h"

namespace TsiU
{
	class Viewport
	{
	private:
		u32 m_uiX;
		u32 m_uiY;
		u32 m_uiWidth;
		u32 m_uiHeight;
		f32 m_fMinZ;
		f32 m_fMaxZ;
	};
}

#endif