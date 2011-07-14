#ifndef __TRENDER_RENDERWINDOW__
#define __TRENDER_RENDERWINDOW__



namespace TsiU
{
	class RenderWindow
	{
	public:
		virtual ~RenderWindow(){}

		virtual void InitWindow(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindowed, void* _poOuter) = 0;
		virtual void SetFullScreen(Bool _bFull) = 0;

		inline u32 GetWindowWidth()	{ return m_uiWidth;		}
		inline u32 GetWindowHeight(){ return m_uiHeight;	}
		inline u32 IsWindowed()		{ return m_bIsWindowed;	}
		inline void SetWindowWidth(u32 _uiWidth)
		{
			m_uiWidth = _uiWidth;
		}
		inline void SetWindowHeight(u32 _uiHeight)
		{
			m_uiHeight = _uiHeight;
		}

	protected:
		u32 m_uiWidth;
		u32 m_uiHeight;
		u32 m_bIsWindowed;
	};
}

#endif