#include "TRender_WinGDIMapping.h"

namespace TsiU
{
	COLORREF WinGDIMapping::MappingColor(const D_Color& _color)
	{
		return RGB(_color.r, _color.g, _color.b);
	}
}