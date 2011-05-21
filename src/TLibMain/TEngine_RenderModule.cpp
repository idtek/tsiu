#include "TRender_Renderer.h"
#include "TRender_RenderWindowMsg.h"
#include "TCore_LibSettings.h"
#include "TEngine_Engine.h"
#include "TEngine_Private.h"
#include "TEngine_ClockModule.h"
#include "TEngine_RenderModule.h"

namespace TsiU
{
	RenderModule::FpsTimer::FpsTimer()
		: m_fLastTime(0.f)
		, m_fFPS(0.f)
	{
	}

	f32 RenderModule::FpsTimer::GetFPS() const
	{
		return m_fFPS;
	}
	void RenderModule::FpsTimer::SetTimer(f32 _fTotalTime)
	{
		f32 fRealDt = _fTotalTime - m_fLastTime;
		if(fRealDt)
			m_fFPS = 1.f / fRealDt;
		m_fLastTime = _fTotalTime;
	}

	RenderModule::RenderModule(u32 _uiWidth, u32 _uiHeight, StringPtr _strTitle, Bool _bIsWindow)
		:m_poApp(NULL), 
		 m_poMainWindow(NULL), 
		 m_poRenderWindowMsg(NULL), 
		 m_poRendererWin(NULL),
		 m_poRenderer(NULL)
	{
		if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
		{
			m_poApp			= new FXApp(_strTitle, _strTitle);
			D_CHECK(m_poApp);

			m_poMainWindow	= new FXMainWindow(m_poApp, _strTitle, NULL, NULL, DECOR_ALL, 0, 0, _uiWidth, _uiHeight);
			D_CHECK(m_poMainWindow);
		}

		if(GetLibSettings()->IsDefined(E_LS_Has_D3D) || GetLibSettings()->IsDefined(E_LS_Has_GDI))
		{
			m_poRenderer = CallCreator<Renderer>(E_CreatorType_Renderer);
			D_CHECK(m_poRenderer);

			if(!GetLibSettings()->IsDefined(E_LS_Has_GUI))
			{
				m_poRenderer->InitRender(_uiWidth, _uiHeight, _strTitle, _bIsWindow, NULL);
			}
		}

		m_poRenderWindowMsg = CallCreator<RenderWindowMsg>(E_CreatorType_RenderWindowMsg);
		D_CHECK(m_poRenderWindowMsg);
	}

	RenderModule::~RenderModule()
	{	
		D_SafeDelete(m_poRenderer);
		D_SafeDelete(m_poRenderWindowMsg);
		D_SafeDelete(m_poApp);

		//Fox will delete this automatically
		//D_SafeDelete(m_poMainWindow);
	}

	void RenderModule::Init()
	{
		if(m_poApp)
		{
			//dummy arguments
			s32 argc = 1;
			Char* argv[2] = {"x", "x"};	

			m_poApp->init(argc,argv);
			m_poApp->create();
		}

		//if we use gui we should make sure to renderer window is not null
		if(!m_poRendererWin && !m_poMainWindow)
		{
			m_poRendererWin = m_poMainWindow;
		}

		if(m_poRenderer)
		{
			if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
			{
				D_CHECK(m_poRendererWin);
				FXWindow* pWin = (FXWindow*)m_poRendererWin;
				m_poRenderer->InitRender(pWin->getWidth(), pWin->getHeight(), "RendererWin", true, pWin->id());
			}
			//else
			//{
			//	//use null to let render to create a window
			//	//refect here!!!
			//	m_poRenderer->InitRender(800, 600, "RendererWin", false, NULL);
			//}
		}
		
		if(m_poMainWindow)
			m_poMainWindow->show(PLACEMENT_SCREEN);
	}

	void RenderModule::RunOneFrame(float _fDeltaTime)
	{
#ifdef TLIB_DEBUG
		m_FpsTimer.SetTimer(poGetEngine()->GetClockMod()->GetTotalElapsedSeconds());

		//Show fps only if we have renderer part
		if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
		{
			Char strBuff[64]={0};
			sprintf(strBuff, "FPS = %.2f", m_FpsTimer.GetFPS() );
			m_poMainWindow->setTitle(strBuff);
		}
		else
		{
			if(GetLibSettings()->IsDefined(E_LS_Has_D3D) || GetLibSettings()->IsDefined(E_LS_Has_GDI))
			{
				m_poRenderer->ShowFPS(m_FpsTimer.GetFPS());
			}
		}
#endif
	}

	void RenderModule::UnInit()
	{
	}
}