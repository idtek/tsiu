#include "SDGlobalDef.h"

#if PLATFORM_TYPE == PLATFORM_WIN32
int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
#else
int main(int argc, char* argv[])
#endif
{
#if PLATFORM_TYPE == PLATFORM_WIN32
	::SetPriorityClass(::GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif

	TsiULibSettings libSettings;

	InitLibSettings(&libSettings);

#if PLATFORM_TYPE == PLATFORM_WIN32
	libSettings.SetupCreator(new T_Creator<WinPanic>, E_CreatorType_Panic);
	libSettings.SetupCreator(new T_Creator<WinTick>, E_CreatorType_Tick);
	libSettings.SetupCreator(new T_Creator<WinGDIRenderWindow>, E_CreatorType_RenderWindow);
	libSettings.SetupCreator(new T_Creator<WinGDIRenderer>, E_CreatorType_Renderer);
	libSettings.SetupCreator(new T_Creator<WinRenderGUIWindowMsg>, E_CreatorType_RenderWindowMsg);
	libSettings.SetupCreator(new T_Creator<WinInputManager>, E_CreatorType_Input);
	libSettings.SetupCreator(new T_Creator<DefaultFile>, E_CreatorType_File);
#endif
	libSettings.DefineMacro(E_LS_Has_GDI);
	libSettings.DefineMacro(E_LS_Has_GUI);

	g_poEngine = new MyEngine(g_WindowWidth, g_WindowHeight, "SoccerDebugger(SSO version) V2.5", g_bIsAlwaysOnTop);

	D_CHECK(g_poEngine);

	if(!g_poEngine->Init())
		D_FatalError("Init Scene Error");

	g_poEngine->MainLoop();

	if(!g_poEngine->UnInit())
		D_FatalError("UnInit Scene Error");

	D_SafeDelete(g_poEngine);

	UninitLibSettings(&libSettings);

	return 0;
}