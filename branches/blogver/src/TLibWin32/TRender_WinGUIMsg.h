#ifndef __TRENDER_WIN_RENDERWINDOW_GUI_MSG__
#define __TRENDER_WIN_RENDERWINDOW_GUI_MSG__

#include "TCore_Types.h"
#include "TRender_RenderWindowMsg.h"

namespace TsiU
{
	class WinRenderGUIWindowMsg : public RenderWindowMsg
	{
	public:
		virtual Bool MessageBump();
	};
}

#endif
