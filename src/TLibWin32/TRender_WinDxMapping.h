#ifndef __TRENDER_WINDX_MAPPING__
#define __TRENDER_WINDX_MAPPING__

#include "TCore_Types.h"
#include "TRender_Enum.h"

#include "TRender_Color.h"
#include "TWin32_Private.h"

namespace TsiU
{
	class Mat4;
	
	class WinDxMapping
	{
	public:
		static u32			MappingClearFlag(u32 p_uiTFlag);
		static D3DXCOLOR	MappingColor(const D_Color& p_Color);
		static D3DXMATRIX	MappingMat4(const Mat4& p_Mat4);
		static Mat4			MappingDXMat4(const D3DXMATRIX& _DxMat4);
	};
}

#endif