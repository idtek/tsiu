#include "TAI_ConditionNode.h"
#include "TAI_DefaultScript.h"

namespace TsiU
{
	ConditionNode::ConditionNode()
		:AINode(E_ANT_Condition)
	{
	}

	////////////////////////////////ConditionNOT////////////////////////////////////////////
	ConditionNOT::ConditionNOT(ConditionNode* _poNode)
	{
		if(_poNode)
			AddNode(_poNode);
		else
			AddNode(new ConditionFALSE);
	}
	Bool ConditionNOT::IsTrue(const AIWorkingMemory* _poWM) const
	{
		ConditionNode* poNode = (ConditionNode*)m_arNodeList[0];
		return !poNode->IsTrue(_poWM);
	}
}