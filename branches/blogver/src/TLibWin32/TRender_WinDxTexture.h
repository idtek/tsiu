#ifndef __TRENDER_WINDXTEXTURE_H__
#define __TRENDER_WINDXTEXTURE_H__


#include "TRender_Texture.h"

#ifndef TLIB_NOWINDX

namespace TsiU
{
	class WinDxTexture: public Texture
	{
	public:
		WinDxTexture()
			: Texture(),
			  m_poHandler(NULL), 
			  m_iWidth(0), 
			  m_iHeight(0)
		{}
		WinDxTexture(const Char* _strFilename)
		{
			Load(_strFilename);
		}
		virtual ~WinDxTexture();

		/// This is the core loading function. It must be overwritten by your application.
        virtual void Load(const Char* _strFilename) ;

		virtual s16 GetWidth() const	{ return m_iWidth;		}
		virtual s16 GetHeight() const	{ return m_iHeight;	}

		LPDIRECT3DTEXTURE9  GetHandle() { return m_poHandler;		}

	private:
		s16 m_iWidth;
		s16 m_iHeight;

		LPDIRECT3DTEXTURE9 m_poHandler;
	};
}
#endif

#endif