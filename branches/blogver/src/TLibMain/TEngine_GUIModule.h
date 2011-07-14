#ifndef __TENGINE_GUIMODULE__
#define __TENGINE_GUIMODULE__

#include "TCore_Types.h"
#include "TEngine_Module.h"

//use fox as gui library
#include "fx.h"
#include "FXExpression.h"

namespace TsiU
{
	class GUIModule : public IModule
	{
	public:
		GUIModule(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle);
		~GUIModule();

		virtual void Init();
		virtual void RunOneFrame(float _fDeltaTime);
		virtual void UnInit();

	private:
		FXApp*			m_poApp;
		FXMainWindow*	m_poMainWindow;
	};
}

#endif