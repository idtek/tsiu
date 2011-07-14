#ifndef __TMATH_POINT2__
#define __TMATH_POINT2__

namespace TsiU
{
	template<typename T>
	class Point2
	{
	public:
		Point2()
		{
			x = 0;
			y = 0;
		}
		Point2(T _x, T _y)
			: x(_x), y(_y)
		{
		}
		D_Inline void X(T _newX)  { x = _newX;	}
		D_Inline void Y(T _newY)  { y = _newY;	}

		D_Inline T X() const{ return x;	}
		D_Inline T Y() const{ return y;	}

	private:
		T x, y;
	};
}

#endif