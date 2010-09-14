#include "ATGlobalDef.h"

#if PLATFORM_TYPE == PLATFORM_WIN32
int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
#else
int main(int argc, char* argv[])
#endif
{
#if PLATFORM_TYPE == PLATFORM_WIN32
	::SetPriorityClass(::GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif

	InitLibrary();

#if PLATFORM_TYPE == PLATFORM_WIN32
	GetLibSettings()->SetCreator(new T_Creator<DefaultAllocator>,		E_CreatorType_Allocator);
	GetLibSettings()->SetCreator(new T_Creator<WinPanic>,				E_CreatorType_Panic);
	GetLibSettings()->SetCreator(new T_Creator<WinTick>,				E_CreatorType_Tick);
	GetLibSettings()->SetCreator(new T_Creator<WinGDIRenderWindow>,		E_CreatorType_RenderWindow);
	GetLibSettings()->SetCreator(new T_Creator<WinGDIRenderer>,			E_CreatorType_Renderer);
	GetLibSettings()->SetCreator(new T_Creator<WinRenderWindowMsg>,		E_CreatorType_RenderWindowMsg);
	GetLibSettings()->SetCreator(new T_Creator<WinInputManager>,		E_CreatorType_Input);
	GetLibSettings()->SetCreator(new T_Creator<DefaultFile>,			E_CreatorType_File);
#endif
	GetLibSettings()->DefineMacro(E_LS_Has_GDI);

	g_poEngine = new GameEngine(800, 600, "AITest", false);

	D_CHECK(g_poEngine);

	if(!g_poEngine->Init())
		D_FatalError("Init Scene Error");

	g_poEngine->MainLoop();

	if(!g_poEngine->UnInit())
		D_FatalError("UnInit Scene Error");

	D_SafeDelete(g_poEngine);

	UnInitLibrary();

	return 0;
}