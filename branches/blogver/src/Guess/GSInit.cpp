#include "GSGlobalDef.h"
#include "GSBlock.h"
#include <fmod.hpp>
#include <fmod_errors.h>

//-----------------------------------------------------------------------------------------------------
Engine*						g_poEngine	= NULL;
//--------------------------------------------------------------------------------------------------------------

static FMOD::System		*gSystem	= NULL;
static FMOD::Sound      *gSound		= NULL;
static FMOD::Channel    *gChannel	= NULL;

GameEngine::GameEngine(u32 _width, u32 _height, const Char* _title, Bool _isWindowed)
	:Engine(_width, _height, _title, _isWindowed)
{
	GSBlockManager::Create();

	FMOD::System_Create(&gSystem);
}

void GameEngine::DoInit()
{
	//Add window msg call back
	RenderWindowMsg::RegisterMsgListener(new GSWindowMsgCallBack);

	int wndWidth  = GetRenderMod()->GetRenderer()->GetWidth();
	int wndHeight = GetRenderMod()->GetRenderer()->GetHeight();

	GSBlockManager::GetPtr()->Init(wndWidth, wndHeight);

	GameEngine::GetGameEngine()->GetSceneMod()->AddObject("background", new GSBackground);

	gSystem->init(32, FMOD_INIT_NORMAL, 0);
	gSystem->createStream("bg.mp3", FMOD_HARDWARE, 0, &gSound);
	gSystem->playSound(FMOD_CHANNEL_FREE, gSound, false, &gChannel);
	//gChannel->setVolume(0.2f);
}

void GameEngine::DoUnInit()
{
	GSBlockManager::Destroy();

	gSound->release();
	gSystem->close();
	gSystem->release();
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