#include "ATGlobalDef.h"

//--------------------------------------------------------------------------------------------
Engine*	g_poEngine	= NULL;
//---------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow)
	:Engine(_uiWidth, _uiHeight, _strTitle, _bIsWindow, 60)
{
}
void GameEngine::DoInit()
{
}

void GameEngine::DoUnInit()
{
}