#include "TEngine_GUIModule.h"

namespace TsiU
{
	GUIModule::GUIModule(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle)
	{
		m_poApp = new FXApp(p_strTitle, p_strTitle);

		s32 argc = 1;
		Char *argv[2]={"aaa","bbb"};
		m_poApp->init(argc, argv);
		m_poMainWindow = new FXMainWindow(m_poApp, p_strTitle, NULL, NULL, DECOR_ALL,
										   0, 0, p_uiWidth, p_uiHeight);
	}

	GUIModule::~GUIModule()
	{
	}

	void GUIModule::Init()
	{
		m_poApp->create();
		m_poMainWindow->show(PLACEMENT_SCREEN);
	}

	void GUIModule::RunOneFrame(float _fDeltaTime)
	{
		m_poApp->runOneEvent();
	}

	void GUIModule::UnInit()
	{
	}
}