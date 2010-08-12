#ifndef __TMATH_BASIC__
#define __TMATH_BASIC__

#include "TCore_Types.h"
#include <math.h>

namespace TsiU
{
	class Math
	{
	public:
		template<typename T> 
		static inline T Abs(const T& p_X)
		{ 
			return p_X > 0 ? p_X : -p_X;	
		}
		template<typename T> 
		static inline T Min(const T& p_X, const T& p_Y)
		{ 
			return p_X < p_Y ? p_X : p_Y;
		}
		template<typename T> 
		static inline T Max(const T& p_X, const T& p_Y)
		{ 
			return p_X > p_Y ? p_X : p_Y;
		}
		template<typename T> 
		static inline T Clamp(const T& p_X, const T& p_Low, const T& p_High)
		{
			return p_X > p_High ? p_High : ( p_X < p_Low ? p_Low : p_X );
		}

		static inline Bool IsEqual(f32 p_fLVal, f32 p_fRVal)
		{ 
			return Abs<f32>(p_fLVal - p_fRVal) < kFLOAT_EPSILON ? true : false; 
		}
		static inline Bool IsZero(f32 p_fVal){ return IsEqual(p_fVal, 0.f);	}

		static inline f32 Sqrt(f32 p_fVal)					{ return sqrt(p_fVal);			}
		static inline f32 InvSqrt(f32 p_fVal)				{ return 1.f/sqrt(p_fVal);		}
		static inline f32 Pow(f32 p_fVal, f32 p_fPower=2.f)	{ return pow(p_fVal, p_fPower);	}
		static inline f32 Sin(f32 p_fVal)					{ return sin(p_fVal);			}
		static inline f32 Cos(f32 p_fVal)					{ return cos(p_fVal);			}
		static inline f32 Tan(f32 p_fVal)					{ return tan(p_fVal);			}
		static inline f32 DegToRad(f32 p_fVal)				{ return p_fVal / 180.f * kTPI;	}
		static inline f32 RadToDeg(f32 p_fVal)				{ return p_fVal / kTPI * 180.f;	}
		static inline f32 ASin(f32 p_fVal)					{ return asin(p_fVal);			}
		static inline f32 ACos(f32 p_fVal)					{ return acos(p_fVal);			}
		static inline f32 Log(f32 p_fVal)					{ return log(p_fVal);			}
		static inline f32 Sign(f32 p_fVal)
		{
			if (p_fVal > 0)
				return 1.f;
			else if (p_fVal < 0)
				return -1.f;
			return 0.f;
		}

		static const f32 kTPI;

	private:
		static const f32 kFLOAT_EPSILON;
	};
}

#endif