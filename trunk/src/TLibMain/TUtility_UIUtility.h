#ifndef __TUTILITY_UIUTILITY_H__
#define __TUTILITY_UIUTILITY_H__

namespace TsiU
{
	class UIPoint
	{
	public:
		UIPoint(): mX(0), mY(0)
		{
		}

		UIPoint(short x, short y): mX(x), mY(y)
		{
		}
		
		inline short GetX() const { return mX; }
		inline short GetY() const { return mY; }
		inline void  SetX(short x) { mX = x; }
		inline void  SetY(short y) { mY = y; }
		inline void  Set(short x, short y) { mX = x; mY = y;}

		//static const UIPoint	InvalidUIPoint;

	protected:
		short	mX, mY;
	};

	class UIRect
	{
	public:
		UIRect(): mTop(0), mLeft(0), mBottom(0), mRight(0)
		{
		}

		UIRect(short t, short l, short b, short r)
		{
			if( t>b ) b^=t^=b^=t;
			if( l>r ) l^=r^=l^=r;

			mTop=t;
			mBottom=b;
			mLeft=l;
			mRight=r;
		}

		inline short GetTop() const { return mTop;} 
		inline short GetBottom() const { return mBottom;}
		inline short GetLeft() const { return mLeft;}
		inline short GetRight() const { return mRight;}
		
		UIRect& operator += ( const UIPoint& p)
		{
			mLeft =  mLeft> p.GetX()? p.GetX():mLeft;
			mRight = mRight>p.GetX()? mRight: p.GetX();
			mTop   = mTop> p.GetY()? p.GetY(): mTop;
			mBottom = mBottom>p.GetY()? mBottom: p.GetY();

			return *this;
		}

		UIRect& operator +=( const UIRect & r)
		{
			mLeft =  mLeft> r.GetLeft()? r.GetLeft():mLeft;
			mRight = mRight>r.GetRight()? mRight: r.GetRight();
			mTop   = mTop> r.GetTop()? r.GetTop(): mTop;
			mBottom = mBottom>r.GetBottom()? mBottom: r.GetBottom();

			return * this;
		}

		UIRect operator + ( const UIPoint& p)
		{
			short l, r, t, b;
			l =  mLeft> p.GetX()? p.GetX():mLeft;
			r = mRight>p.GetX()? mRight: p.GetX();
			t   = mTop> p.GetY()? p.GetY(): mTop;
			b = mBottom>p.GetY()? mBottom: p.GetY();

			return UIRect(t,l,r,b);
		}

		UIRect operator +( const UIRect & r)
		{
			short l, ri, t, b;
			l =  mLeft> r.GetLeft()? r.GetLeft():mLeft;
			ri = mRight>r.GetRight()? mRight: r.GetRight();
			t   = mTop> r.GetTop()? r.GetTop(): mTop;
			b = mBottom>r.GetBottom()? mBottom: r.GetBottom();

			return UIRect(t,l,ri,b);
		}


	protected:
		short	mTop, mLeft, mBottom, mRight;
	};

	struct UITextureUV
	{
		float mU, mV;
	};

	struct UIColor
	{

	};
}

#endif