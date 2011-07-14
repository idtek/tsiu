#ifndef __TGUI_SLIDINGBAR__
#define __TGUI_SLIDINGBAR__

#include "TGui_Object.h"
#include "TRender_RenderWindowMsg.h"

namespace TsiU
{
	class GuiSlidingBar : public GuiObject
	{
	public:
		GuiSlidingBar(s32 _x, s32 _y, u32 _width, u32 _height, f32 _start, f32 _end, f32 _step, Bool _isVer);

		virtual void Tick(f32 p_fDeltaTime);

		Bool IsInSlidingBlock(s32 _x, s32 _y);
		Bool IsInSlidingBar(s32 _x, s32 _y);

		Bool MoveSlidingBlock(s32 _newX, s32 _newY);

		inline Bool IsDragStart()				{ return m_bDragStart;	};
		inline void SetDragStart(Bool _bVal)	{ m_bDragStart = _bVal;	};

		inline float GetCurrentValue()			{ return m_fCurrent;	};

	protected:
		u32		m_uiWidth;
		u32		m_uiHeight;
		f32		m_fStart;
		f32		m_fEnd;
		f32		m_fValuePerPixel;
		f32		m_fStep;
		f32		m_fCurrent;
		Bool	m_bIsVertical;

		s32		m_uiBlockDelta;
		Vec3	m_uiBlockCenter;
		u32		m_uiBlockRadius;
		u32		m_uiMinLimited;
		u32		m_uiMaxLimited;

		Bool	m_bDragStart;
	};

	class GuiSlidingBarMsgCallBack : public RenderWindowMsgListener
	{
	public:
		GuiSlidingBarMsgCallBack(GuiSlidingBar* p_poSB);

		virtual void OnMouseLDrag(s32 x, s32 y);
		virtual void OnMouseLDown(s32 x, s32 y);
		virtual void OnMouseLUp(s32 x, s32 y);

	protected:
		GuiSlidingBar* m_poSBar;
	};
}

#endif