#ifndef __TRENDER_RENDERWINDOW__
#define __TRENDER_RENDERWINDOW__

#include "TCore_Types.h"

namespace TsiU
{
	class RenderWindow
	{
	public:
		virtual ~RenderWindow(){}

		virtual void InitWindow(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter) = 0;
		virtual void SetFullScreen(Bool p_bFull) = 0;

		inline u32 GetWindowWidth()	{ return m_uiWidth;		}
		inline u32 GetWindowHeight(){ return m_uiHeight;	}
		inline u32 IsWindowed()		{ return m_bIsWindowed;	}
		inline void SetWindowWidth(u32 p_uiWidth)
		{
			m_uiWidth = p_uiWidth;
		}
		inline void SetWindowHeight(u32 p_uiHeight)
		{
			m_uiHeight = p_uiHeight;
		}

	protected:
		u32 m_uiWidth;
		u32 m_uiHeight;
		u32 m_bIsWindowed;
	};
}

#endif