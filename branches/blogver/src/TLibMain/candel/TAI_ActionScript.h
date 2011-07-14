#ifndef __TAI_ACTIONSCRIPT__
#define __TAI_ACTIONSCRIPT__

#include "TCore_Types.h"
#include "TUtility_Singleton.h"

namespace TsiU
{
	class ActionNode;

	class ActionScript
	{
	public:
		virtual void DoReset(){}
		virtual u32 DoGetNext(ActionNode* _poNode) = 0;
	};

	class ActionScriptFactory
	{
	public:
		virtual ActionScript* CreateScript(u32 _uiId) = 0;
		virtual void DestroyScript(ActionScript* _poScript)
		{
			if(_poScript != 0)
			{
				delete _poScript;
				_poScript = NULL;
			}
		}
	};
}


#endif