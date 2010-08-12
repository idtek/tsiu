#ifndef __TRENDER_WINDXUITEXTURE_H__
#define __TRENDER_WINDXUITEXTURE_H__

#include "TWin32_Private.h"
#include "TRender_UITexture.h"
namespace TsiU
{
	class WinDxUITexture: public UITexture
	{
	public:
		WinDxUITexture(): mHandler(NULL), mWidth(0), mHeight(0)
		{}
		WinDxUITexture(const char * filename)
		{
			Load(filename);
		}

		/// This is the core loading function. It must be overwritten by your application.
        virtual void Load(const char * fileName) ;

		virtual short GetWidth() const { return mWidth; }
		virtual short GetHeight() const { return mHeight;}

		LPDIRECT3DTEXTURE9  GetHandle() { return mHandler; }

	private:
		short mWidth, mHeight;
		LPDIRECT3DTEXTURE9 mHandler;

	};
}


#endif