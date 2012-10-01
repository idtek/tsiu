#include "TEngine_Engine.h"
#include "TCore_LibSettings.h"
#include "TCore_Panic.h"
#include "TCore_Allocator.h"

#include "TRender_Enum.h"
#include "TRender_Renderer.h"
#include "TRender_RenderWindowMsg.h"

#include "TEngine_ClockModule.h"
#include "TEngine_SceneModule.h"
#include "TEngine_InputModule.h"
#include "TEngine_NetworkModule.h"
#include "TEngine_EventModule.h"
#include "TEngine_ScriptModule.h"
#include "TEngine_RenderModule.h"

#ifndef TLIB_RETAIL
#include "TUtility_DebugConsole.h"
#endif

namespace TsiU
{
	Engine* __g_Root = NULL;
	Engine* poGetEngine(){ D_CHECK(__g_Root); return __g_Root; };

	Engine::Engine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow, s32 _iRateLock)
		: m_BackgroundColor()
	{
		m_poClockModule = NULL;
		m_poSceneModule = NULL;
		m_poInputModule = NULL;
		m_poNetworkModule = NULL;
		m_poEventModule = NULL;
		m_poScriptModule = NULL;
		m_poRenderModule = NULL;

//#ifdef TLIB_DEBUG
		m_poDebugConsole = NULL;
//#endif
		m_bQuitEngine = false;

		g_poPanic = CallCreator<Panic>(E_CreatorType_Panic);
		//g_poAllocator = CallCreator<Allocator>(E_CreatorType_Allocator);

//#ifdef TLIB_DEBUG
		m_poDebugConsole = new DebugConsole();
		D_CHECK(m_poDebugConsole);
//#endif
		m_poClockModule = new ClockModule();
		D_CHECK(m_poClockModule);
		m_poClockModule->SetRateLock(_iRateLock);

		//if(GetLibSettings()->IsDefined(E_LS_Has_D3D) || GetLibSettings()->IsDefined(E_LS_Has_GDI))
		{
			m_poSceneModule = new SceneModule();
			D_CHECK(m_poSceneModule);
		}

		m_poInputModule = new InputModule(1);
		D_CHECK(m_poInputModule);

		if(GetLibSettings()->IsDefined(E_LS_Has_Network))
		{
			m_poNetworkModule = new NetworkModule(8);
			D_CHECK(m_poNetworkModule);
		}

		m_poEventModule = new EventModule();
		D_CHECK(m_poEventModule);

		if(GetLibSettings()->IsDefined(E_LS_Has_Script))
		{
			m_poScriptModule = new ScriptModule();
			D_CHECK(m_poScriptModule);
		}
		
		m_poRenderModule = new RenderModule(_uiWidth, _uiHeight, _strTitle, _bIsWindow);
		D_CHECK(m_poRenderModule);

		__g_Root = this;
	}
	Engine::~Engine()
	{
		D_SafeDelete(m_poClockModule);
		D_SafeDelete(m_poSceneModule);
		D_SafeDelete(m_poInputModule);
		D_SafeDelete(m_poNetworkModule);
		D_SafeDelete(m_poEventModule);
		D_SafeDelete(m_poScriptModule);
		D_SafeDelete(m_poRenderModule);

//#ifdef TLIB_DEBUG
		D_SafeDelete(m_poDebugConsole);
//#endif
	}
	
	Bool Engine::Init()
	{
		if(m_poClockModule)		m_poClockModule->Init();
		if(m_poNetworkModule)	m_poNetworkModule->Init();
		if(m_poEventModule)		m_poEventModule->Init();
		if(m_poScriptModule)	m_poScriptModule->Init();

		DoInit();
		
		if(m_poRenderModule)	m_poRenderModule->Init();
		if(m_poInputModule)		m_poInputModule->Init();
		if(m_poSceneModule)		m_poSceneModule->Init();

		return true;
	}

	Bool Engine::UnInit()
	{
		if(m_poSceneModule)		m_poSceneModule->UnInit();
		if(m_poInputModule)		m_poInputModule->UnInit();
		if(m_poRenderModule)	m_poRenderModule->UnInit();
		
		DoUnInit();

		if(m_poScriptModule)	m_poScriptModule->UnInit();
		if(m_poEventModule)		m_poEventModule->UnInit();
		if(m_poNetworkModule)	m_poNetworkModule->UnInit();
		if(m_poClockModule)		m_poClockModule->UnInit();

		return true;
	}

	void Engine::SetBackgroundColor(const D_Color& clr)
	{
		m_BackgroundColor = clr;
	}

	void Engine::MainLoop()
	{
		while(1)
		{
			m_poClockModule->StartFrame();

			Renderer* poRenderer = m_poRenderModule->GetRenderer();
			RenderWindowMsg* poRenderWindowMsg = m_poRenderModule->GetRenderWindowMsg();

			if(poRenderWindowMsg)
			{
				m_bQuitEngine = !poRenderWindowMsg->MessageBump();
				if(m_bQuitEngine)
					return;
			}

			DoPreFrame();

			m_poClockModule->RunOneFrame(0);

			f32 fDeltaT = m_poClockModule->GetFrameElapsedSeconds();

			//Recv from network
			if(m_poNetworkModule)	m_poNetworkModule->RunOneFrame(fDeltaT);

			if(m_poInputModule)		m_poInputModule->RunOneFrame(fDeltaT);
			if(m_poEventModule)		m_poEventModule->RunOneFrame(fDeltaT);
			if(m_poScriptModule)	m_poScriptModule->RunOneFrame(fDeltaT);	
			if(m_poSceneModule)		m_poSceneModule->RunOneFrame(fDeltaT);
			if(m_poRenderModule)	m_poRenderModule->RunOneFrame(fDeltaT);

			// Begin Draw
			if(poRenderer)
			{
				poRenderer->Clear(m_BackgroundColor, E_ClearFlagTarget | E_ClearFlagZBuffer);
				if(poRenderer->BeginFrame())
				{
					if(m_poSceneModule)
						m_poSceneModule->Draw();
					poRenderer->EndFrame();
				}
			}
			// End Draw

			//Send to network
			if(m_poNetworkModule)	m_poNetworkModule->RunOneFrame(fDeltaT);
			
			if(poRenderer)
				poRenderer->Flip();

			DoPostFrame();

			m_poClockModule->EndFrame();
		}
	}
}
