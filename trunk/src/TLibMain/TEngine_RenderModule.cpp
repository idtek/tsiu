#include "TRender_Renderer.h"
#include "TRender_RenderWindowMsg.h"
#include "TCore_LibSettings.h"

#include "TEngine_RenderModule.h"

namespace TsiU
{
	RenderModule::RenderModule(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindow)
		:m_poApp(NULL), 
		 m_poMainWindow(NULL), 
		 m_poRenderWindowMsg(NULL), 
		 m_poRendererWin(NULL),
		 m_poRenderer(NULL)
	{
		if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
		{
			m_poApp			= new FXApp(p_strTitle, p_strTitle);
			D_CHECK(m_poApp);

			m_poMainWindow	= new FXMainWindow(m_poApp, p_strTitle, NULL, NULL, DECOR_ALL, 0, 0, p_uiWidth, p_uiHeight);
			D_CHECK(m_poMainWindow);
		}

		if(GetLibSettings()->IsDefined(E_LS_Has_D3D) || GetLibSettings()->IsDefined(E_LS_Has_GDI))
		{
			m_poRenderer = CallCreator<Renderer>(E_CreatorType_Renderer);
			D_CHECK(m_poRenderer);
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
			else
			{
				//use null to let render to create a window
				//refect here!!!
				m_poRenderer->InitRender(800, 600, "RendererWin", true, NULL);
			}
		}
		
		if(m_poMainWindow)
			m_poMainWindow->show(PLACEMENT_SCREEN);
	}

	void RenderModule::RunOneFrame(float _fDeltaTime)
	{
#ifdef TLIB_DEBUG
		//Show fps only if we have renderer part
		if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
		{
			if(GetLibSettings()->IsDefined(E_LS_Has_D3D) || GetLibSettings()->IsDefined(E_LS_Has_GDI))
			{
				if(_fDeltaTime > 0)
				{	
					Char l_strBuff[64]={0};
					sprintf(l_strBuff, "FPS = %.2f", 1.f / _fDeltaTime);
					m_poMainWindow->setTitle(l_strBuff);
					//m_poRenderer->ShowFPS(1.f / _fDeltaTime);
				}
			}
			else
			{
				if(_fDeltaTime > 0)
				{	
					Char l_strBuff[64]={0};
					sprintf(l_strBuff, "FPS = %.2f", 1.f / _fDeltaTime);
					m_poMainWindow->setTitle(l_strBuff);
					//m_poRenderer->ShowFPS(1.f / _fDeltaTime);
				}
			}
		}
#endif
	}

	void RenderModule::UnInit()
	{
	}
}