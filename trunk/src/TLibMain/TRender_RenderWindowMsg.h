#ifndef __TRENDER_RENDERWINDOW_MSG__
#define __TRENDER_RENDERWINDOW_MSG__


#include "TUtility_Array.h"

namespace TsiU
{
	class RenderWindowMsgListener
	{
	public:
		virtual void OnMouseLDBClick(s32 x, s32 y){};
		virtual void OnMouseLDown(s32 x, s32 y){};
		virtual void OnMouseLUp(s32 x, s32 y){};
		virtual void OnWindowResize(s32 width, s32 height){};
		virtual void OnMouseLDrag(s32 x, s32 y, s32 relX, s32 relY){};
		virtual void OnKeyDown(u32 key, s64 info){};
		virtual void OnKeyUp(u32 key, s64 info){};
		virtual void OnKeyPress(u32 key, s64 info){};
	};

	class RenderWindowMsg
	{
	public:
		virtual Bool MessageBump() = 0;

		static void RegisterMsgListener(RenderWindowMsgListener* _poMsgCallBack)
		{
			s_poMsgCallBack.PushBack(_poMsgCallBack);;
		}
		static Array<RenderWindowMsgListener*> s_poMsgCallBack;
	};
}

#endif