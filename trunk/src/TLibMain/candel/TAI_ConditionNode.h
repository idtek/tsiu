#ifndef __TAI_CONDITIONNODE__
#define __TAI_CONDITIONNODE__

#include "TCore_Types.h"
#include "TAI_Node.h"

namespace TsiU
{
	class AIWorkingMemory;

	class ConditionNode : public AINode
	{
	public:
		ConditionNode();
		virtual Bool IsTrue(const AIWorkingMemory* _poWM) const = 0;
	};

	class ConditionBinary : public ConditionNode
	{
	public:
		virtual AINode* AddNode(AINode* _poNode)
		{
			if(m_arNodeList.Size() < 2)
			{
				return ConditionNode::AddNode(_poNode);
			}
			return this;
		}
	};

	class ConditionUnary : public ConditionNode
	{
	public:
		virtual AINode* AddNode(AINode* _poNode)
		{
			if(m_arNodeList.Size() < 1)
			{
				return ConditionNode::AddNode(_poNode);
			}
			return this;
		}
	};

	class ConditionTRUE : public ConditionUnary
	{
	public:
		virtual Bool IsTrue(const AIWorkingMemory* _poWM) const
		{
			return true;
		}
	};
	class ConditionFALSE : public ConditionUnary
	{
	public:
		virtual Bool IsTrue(const AIWorkingMemory* _poWM) const
		{
			return false;
		}
	};
	class ConditionNOT : public ConditionUnary
	{
	public:
		ConditionNOT(ConditionNode* _poNode);
		virtual Bool IsTrue(const AIWorkingMemory* _poWM) const;
	};
}

#endif