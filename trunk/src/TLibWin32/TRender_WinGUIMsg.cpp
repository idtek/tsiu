#include "TRender_WinGUIMsg.h"
#include "TEngine_Private.h"
#include "TCore_LibSettings.h"
#include "TEngine_RenderModule.h"

#include "fx.h"

namespace TsiU
{
	Bool WinRenderGUIWindowMsg::MessageBump()
	{
		D_CHECK(GetLibSettings()->IsDefined(E_LS_Has_GUI));

		FXApp* pApp = poGetEngine()->GetRenderMod()->GetGUIApp();
		s32 ulRet = pApp->runWhileEvents();
		if(!ulRet)
			return false;	//close window

		return true;
	}
}