#include "TRender_WinDxMapping.h"
#include "TMath_Matrix4.h"

#ifndef TLIB_NOWINDX
namespace TsiU
{
	u32 WinDxMapping::MappingClearFlag(u32 p_uiTFlag)
	{
		u32 l_uiMapResult = 0;
		if(p_uiTFlag & E_ClearFlagTarget)
			l_uiMapResult |= D3DCLEAR_TARGET;
		if(p_uiTFlag & E_ClearFlagZBuffer)
			l_uiMapResult |= D3DCLEAR_ZBUFFER;
		if(p_uiTFlag & E_ClearFlagStencil)
			l_uiMapResult |= D3DCLEAR_STENCIL;
		return l_uiMapResult;
	}
	D3DXCOLOR WinDxMapping::MappingColor(const D_Color& p_Color)
	{
		return D3DXCOLOR((f32)p_Color.r / 0xFF, 
						 (f32)p_Color.g / 0xFF, 
						 (f32)p_Color.b / 0xFF,
						 (f32)p_Color.a / 0xFF);
	}
	D3DXMATRIX WinDxMapping::MappingMat4(const Mat4& p_Mat4)
	{
		return D3DXMATRIX(
			p_Mat4[0][0], p_Mat4[0][1], p_Mat4[0][2], p_Mat4[0][3],
			p_Mat4[1][0], p_Mat4[1][1], p_Mat4[1][2], p_Mat4[1][3],
			p_Mat4[2][0], p_Mat4[2][1], p_Mat4[2][2], p_Mat4[2][3],
			p_Mat4[3][0], p_Mat4[3][1], p_Mat4[3][2], p_Mat4[3][3]);
	}
	Mat4 WinDxMapping::MappingDXMat4(const D3DXMATRIX& _DxMat4)
	{
		return Mat4(
			_DxMat4.m[0][0], _DxMat4.m[0][1], _DxMat4.m[0][2], _DxMat4.m[0][3],
			_DxMat4.m[1][0], _DxMat4.m[1][1], _DxMat4.m[1][2], _DxMat4.m[1][3],
			_DxMat4.m[2][0], _DxMat4.m[2][1], _DxMat4.m[2][2], _DxMat4.m[2][3],
			_DxMat4.m[3][0], _DxMat4.m[3][1], _DxMat4.m[3][2], _DxMat4.m[3][3]);
	}
}
#endif