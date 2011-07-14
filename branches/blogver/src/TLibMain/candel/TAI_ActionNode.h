#ifndef __TAI_ACTIONNODE__
#define __TAI_ACTIONNODE__

#include "TCore_Types.h"
#include "TAI_Node.h"

namespace TsiU
{
	class AIWorkingMemory;
	class ConditionNode;

	class ActionNode : public AINode
	{
	public:
		ActionNode(AINodeType_t _eType)
			:AINode(_eType)
		{}

		virtual Bool	CanRun(const AIWorkingMemory* _poWM) = 0;
		virtual AIRet_t Tick(const AIWorkingMemory* _poWM)	 = 0;
		virtual void	Cancel(const AIWorkingMemory* _poWM) = 0;
	};
	
	class TerminalActionNode : public ActionNode
	{
	public:
		TerminalActionNode();
		TerminalActionNode(ConditionNode* _poCon);
		virtual ~TerminalActionNode();

		//Terminal Node cannot have leaf node
		virtual AINode* AddNode(AINode* _poNode){ return this;	};
		void	AddCondition(ConditionNode* _poCon);
		
		virtual Bool	CanRun(const AIWorkingMemory* _poWM);
		virtual AIRet_t Tick(const AIWorkingMemory* _poWM);
		virtual void	Cancel(const AIWorkingMemory* _poWM);

	protected:
		virtual AIRet_t	_DoExecute(const AIWorkingMemory* _poWM)
		{
			return E_AI_Finish;
		}
		virtual void	_DoCancel(const AIWorkingMemory* _poWM){}

		void _CheckValidCondition();

	protected:
		ConditionNode* m_poCondition;
	};

	class SelectorActionNode : public ActionNode
	{
	public:
		SelectorActionNode();

		virtual Bool	CanRun(const AIWorkingMemory* _poWM);
		virtual AIRet_t Tick(const AIWorkingMemory* _poWM);
		virtual void	Cancel(const AIWorkingMemory* _poWM);

	protected:
		AINode* m_poCurrentNode;
	};

	/*class SequenceActionNode : public ActionNode
	{
	public:
		SequenceActionNode(AINodeScript* _poScript = NULL);

		virtual AIRet_t Tick(const AIWorkingMemory* _poWM);
		virtual void	Cancel();

	protected:
		s32	m_uiCurrentNode;
	};*/
}

#endif