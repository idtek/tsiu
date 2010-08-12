#ifndef __TENGINE_SCRIPTMODULE__
#define __TENGINE_SCRIPTMODULE__

#include "TCore_Types.h"
#include "TEngine_Module.h"
#include <string>
#include <map>

namespace TsiU
{
	class ScriptMachine;
	class ScriptFunction;

	class ScriptModule : public IModule
	{
	public:
		ScriptModule();
		~ScriptModule();

		virtual void Init();
		virtual void RunOneFrame(float _fDeltaTime);
		virtual void UnInit();

	private:
		typedef std::map<std::string, ScriptFunction*> SFList;

		ScriptMachine*	m_poMachine;
		SFList			m_aFunctionList;

		Char*			m_poMainBuffer;
		u32				m_uiBufferSize;
	};
}

#endif