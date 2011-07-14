#ifndef __TUI_MATERIAL_H__
#define __TUI_MATERIAL_H__
#include "TUtility_UIUtility.h"

namespace TsiU
{
	class UITexture;
	class UIMaterial
	{
	public:
		UIMaterial():mWidth(0), mHeight(0), mTopLeftU(0.f), mTopLeftV(0.f), mBottomRightU(1.f), mBottomRightV(1.f){}
		virtual ~UIMaterial(){}
		inline void SetTexture(UITexture * tex){ mTexture = tex; }	
		inline UITexture * GetTexture() { return mTexture; }

		inline void SetSize(short w, short h){mWidth = w, mHeight = h; }
		inline void SetWidth(short w){mWidth =w;}
		inline void SetHeight(short h){mHeight =h;}
		inline short GetWdith() { return mWidth; }
		inline short GetHeight() { return mHeight; }

		
		inline void SetTopLeftUV(float u, float v){mTopLeftU = u, mTopLeftV=v;}
		inline void SetBottomRightUV(float u, float v){mBottomRightU = u, mBottomRightV=v;}
		inline float GetTopLeftU(){ return mTopLeftU;}
		inline float GetTopLeftV(){ return mTopLeftV;}
		inline float GetBottomRightU(){ return mBottomRightU;}
		inline float GetBottomRightV(){ return mBottomRightV;}

	protected:
		UITexture * mTexture;
		short mWidth, mHeight;

		float mTopLeftU, mTopLeftV;
		float mBottomRightU, mBottomRightV;

	};
}


#endif