#ifndef __TENGINE_ENGINE__
#define __TENGINE_ENGINE__

#include "TCore_Types.h"

namespace TsiU
{
	class ClockModule;
	class SceneModule;
	class InputModule;
	class NetworkModule;
	class EventModule;
	class ScriptModule;
	class RenderModule;

#ifdef TLIB_DEBUG
	class DebugConsole;
#endif

	class Engine
	{
	public:
		Engine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow);
		virtual ~Engine();

		Bool Init();
		void MainLoop();
		Bool UnInit();

		virtual void DoInit(){};
		virtual void DoUnInit(){};
		virtual void DoPreFrame(){};
		virtual void DoPostFrame(){};

		inline SceneModule*		GetSceneMod()		{ return m_poSceneModule;	}
		inline InputModule*		GetInputMod()		{ return m_poInputModule;	}
		inline NetworkModule*	GetNetworkMod()		{ return m_poNetworkModule;	}
		inline EventModule*		GetEventMod()		{ return m_poEventModule;	}
		inline ScriptModule*	GetScriptMod()		{ return m_poScriptModule;	}
		inline RenderModule*	GetRenderMod()		{ return m_poRenderModule;	}
		inline ClockModule*		GetClockMod()		{ return m_poClockModule;	}

	protected:
		//Modules
		ClockModule*		m_poClockModule;
		SceneModule*		m_poSceneModule;
		InputModule*		m_poInputModule;
		NetworkModule*		m_poNetworkModule;
		EventModule*		m_poEventModule;
		ScriptModule*		m_poScriptModule;
		RenderModule*		m_poRenderModule;

#ifdef TLIB_DEBUG
		DebugConsole*		m_poDebugConsole;
#endif
		Bool m_bQuitEngine;
	};
}

#endif
