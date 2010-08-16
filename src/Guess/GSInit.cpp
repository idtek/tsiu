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

static const int kRowCount		= 3;
static const int kColumnCount	= 3;

void GameEngine::DoInit()
{
	//Add window msg call back
	RenderWindowMsg::RegisterMsgListener(new GSWindowMsgCallBack);

	int wndWidth  = GetRenderMod()->GetRenderer()->GetWidth();
	int wndHeight = GetRenderMod()->GetRenderer()->GetHeight();

	GSBlockManager::GetPtr()->Init(wndWidth, wndHeight);
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

		GSBlockManager::GetPtr()->OnResizeWindow(wndWidth, wndHeight);
	}
}