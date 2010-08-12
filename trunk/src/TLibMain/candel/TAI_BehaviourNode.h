#ifndef __BEHAVIOUR_NODE__
#define __BEHAVIOUR_NODE__

#include "TCore_Types.h"
#include "TAI_ActionNode.h"

namespace TsiU
{
	class BehaviourNode
	{
	public:
		BehaviourNode(ActionNode* _poStartup)
			:m_poStartupActionNode(_poStartup)
		{}

		Bool Tick(f32 _fDt)
		{
			D_CHECK(m_poStartupActionNode);
			return m_poStartupActionNode->Tick(_fDt);
		}

	private:
		ActionNode* m_poStartupActionNode;
	};
}

#endif