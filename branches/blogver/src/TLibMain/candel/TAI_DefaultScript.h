#ifndef __TAI_DEFAULTNODESCRIPT__
#define __TAI_DEFAULTNODESCRIPT__

#include "TCore_Types.h"
#include "TAI_Node.h"
#include "TAI_NodeScript.h"

namespace TsiU
{
	class AINode;
	class AIWorkingMemory;

	class DefaultSelectorScript : public SelectorScript
	{
	public:
		DefaultSelectorScript(AINode* _poNode)
			:SelectorScript(_poNode)
		{}
		virtual AINode* GetNode(const AIWorkingMemory* _poWM)
		{
			//Find the first action node
			AINode* poRet = NULL;
			for(s32 i = 0;;++i)
			{
				poRet = m_poNode->GetChild(i);
				if(poRet == NULL || poRet->IsActionNode())
					return poRet;
			}
			return NULL;
		}
	};

	class DefaultConditionScript : public ConditionScript
	{
	public:
		DefaultConditionScript(AINode* _poNode)
			:ConditionScript(_poNode)
		{}
		virtual Bool IsOK(const AIWorkingMemory* _poWM) const
		{
			return true;
		}
	};

	class DefaultTerminalScript : public TerminalScript
	{
	public:
		DefaultTerminalScript(AINode* _poNode)
			:TerminalScript(_poNode)
		{}
		virtual AIRet_t Execute(const AIWorkingMemory* _poWM)
		{
			return E_AI_Finish;
		}
		virtual void Exit()
		{
		}
	};
}


#endif