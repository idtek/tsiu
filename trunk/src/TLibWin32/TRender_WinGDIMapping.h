#ifndef __TRENDER_WINGDI_MAPPING__
#define __TRENDER_WINGDI_MAPPING__

#include "TCore_Types.h"
#include "TRender_Enum.h"

#include "TRender_Color.h"
#include "TWin32_Private.h"

namespace TsiU
{
	class WinGDIMapping
	{
	public:
		static COLORREF	MappingColor(const D_Color& _color);
	};
}

#endif