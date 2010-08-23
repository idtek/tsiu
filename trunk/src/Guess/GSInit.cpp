#include "GSGlobalDef.h"
#include "GSBlock.h"

//-----------------------------------------------------------------------------------------------------
Engine*						g_poEngine	= NULL;
//--------------------------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed)
	:Engine(_width, _height, _title, _isWindowed)
{
	GSBlockManager::Create();
}

void GameEngine::DoInit()
{
	//Add window msg call back
	RenderWindowMsg::RegisterMsgListener(new GSWindowMsgCallBack);

	int wndWidth  = GetRenderMod()->GetRenderer()->GetWidth();
	int wndHeight = GetRenderMod()->GetRenderer()->GetHeight();

	GSBlockManager::GetPtr()->Init(wndWidth, wndHeight);

	GameEngine::GetGameEngine()->GetSceneMod()->AddObject("background", new GSBackground);
}

void GameEngine::DoUnInit()
{
	GSBlockManager::Destroy();
}

//---------------------------------------------------------------------------------------
void GSWindowMsgCallBack::OnWindowResize(s32 width, s32 height)
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
	if(pRender && pRender->GetMainWindow())
	{
		pRender->OnWindowResize(width, height);

		u32 wndWidth  = pRender->GetWidth();
		u32 wndHeight = pRender->GetHeight();

		if(GSBlockManager::GetPtr())
			GSBlockManager::GetPtr()->OnResizeWindow(wndWidth, wndHeight);
	}
}
void GSWindowMsgCallBack::OnMouseLDown(s32 x, s32 y)
{
	if(GSBlockManager::GetPtr())
		GSBlockManager::GetPtr()->OnClick(x, y);
}