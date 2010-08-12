#ifndef __TRENDER_COLOR__
#define __TRENDER_COLOR__

#include "TCore_Types.h"
#include "TCore_Exception.h"
#include "TCore_Memory.h"

namespace TsiU
{
	class Color32
	{
	public:
		inline Color32()
		{
			r = g = b = a = 0;
		}
		inline Color32(u8 p_r, u8 p_g, u8 p_b, u8 p_a)
			: r(p_r),g(p_g),b(p_b),a(p_a)
		{}
		inline Color32(u8 p_r, u8 p_g, u8 p_b)
			: r(p_r),g(p_g),b(p_b),a(0xff)
		{}
		inline Color32(u32 p_uiColor)
		{
			r = (u8)(p_uiColor & 0x000000ff);
			g = (u8)((p_uiColor & 0x0000ff00) >> 8);
			b = (u8)((p_uiColor & 0x00ff0000) >> 16);
			a = (u8)((p_uiColor & 0xff000000) >> 24); 
		}
		inline Color32(const Color32& p_Color)
			:r(p_Color.r),g(p_Color.g),b(p_Color.b),a(p_Color.a)
		{}

		Bool operator == (const Color32& p_Color) const
		{
			return a == p_Color.a && g == p_Color.g && b == p_Color.b && a == p_Color.a;
		}
		Bool operator != (const Color32& p_Color) const
		{
			return !operator==(p_Color);
		}
		inline Color32 operator * (const f32 fv)
		{
			return Color32((u8)(r * fv), (u8)(g * fv), (u8)(b * fv), (u8)(a * fv));
		}
		inline friend Color32 operator * (const f32 fv, const Color32& v)
		{
			return Color32((u8)(fv * v.r), (u8)(fv * v.g), (u8)(fv * v.b), (u8)(fv * v.a));
		}
		inline Color32 operator / (const f32 fv)
		{
			return Color32((u8)(r / fv), (u8)(g / fv), (u8)(b / fv), (u8)(a / fv));
		}
		inline friend Color32 operator / (const f32 fv, const Color32& v)
		{
			return Color32((u8)(fv / v.r), (u8)(fv / v.g), (u8)(fv / v.b), (u8)(fv / v.a));
		}

	public:
		u8 r, g, b, a;
	};

	#define D_Color	Color32
}

#endif