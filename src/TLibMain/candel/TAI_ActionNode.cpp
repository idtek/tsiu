#include "TAI_ActionNode.h"
#include "TAI_ConditionNode.h"

namespace TsiU
{
	////////////////////////////////Terminal Node////////////////////////////////////////////
	TerminalActionNode::TerminalActionNode()
		:ActionNode(E_ANT_Terminal), m_poCondition(NULL)
	{
		_CheckValidCondition();
	}
	TerminalActionNode::TerminalActionNode(ConditionNode* _poCon)
		:ActionNode(E_ANT_Terminal), m_poCondition(_poCon)
	{
		_CheckValidCondition();
	}
	TerminalActionNode::~TerminalActionNode()
	{
		D_SafeDelete(m_poCondition);
	}
	void TerminalActionNode::AddCondition(ConditionNode* _poCon)
	{
		D_SafeDelete(m_poCondition);
		m_poCondition = _poCon;
	}
	Bool TerminalActionNode::CanRun(const AIWorkingMemory* _poWM)
	{
		D_CHECK(m_poCondition);
		return m_poCondition->IsTrue(_poWM);
	}
	AIRet_t TerminalActionNode::Tick(const AIWorkingMemory* _poWM)
	{
		AIRet_t eFinish = _DoExecute(_poWM);
		if(eFinish != E_AI_Executing)
			Cancel(_poWM);
		return eFinish;
	}
	void TerminalActionNode::Cancel(const AIWorkingMemory* _poWM)
	{
		_DoCancel(_poWM);
	}
	void TerminalActionNode::_CheckValidCondition()
	{
		if(!m_poCondition)
		{
			D_Output("[AI] No condition node, use TRUE as default\n");
			m_poCondition = new ConditionTRUE;
			D_CHECK(m_poCondition);
		}
	}
	////////////////////////////////Selector Node////////////////////////////////////////////
	SelectorActionNode::SelectorActionNode()
		:ActionNode(E_ANT_Selector), m_poCurrentNode(NULL)
	{
	}
	Bool SelectorActionNode::CanRun(const AIWorkingMemory* _poWM)
	{
		//Check Current Node 
		if(m_poCurrentNode)
		{
			ActionNode* poActionNode = (ActionNode*)m_poCurrentNode;
			if(!poActionNode->CanRun(_poWM))
			{
				poActionNode->Cancel(_poWM);
				m_poCurrentNode = NULL;
			}
		}
		//if current node is null, try to find a runnable node
		if(m_poCurrentNode == NULL)
		{
			for(s32 i = 0; i < m_arNodeList.Size(); ++i)
			{
				D_CHECK(m_arNodeList[i]->IsActionNode());
				ActionNode* poActionNode = (ActionNode*)m_arNodeList[i];
				if(poActionNode->CanRun(_poWM))
				{
					m_poCurrentNode = poActionNode;
					break;
				}
			}
		}
		return m_poCurrentNode != NULL;
	}
	AIRet_t SelectorActionNode::Tick(const AIWorkingMemory* _poWM)
	{
		AIRet_t eFinish = E_AI_Finish;
		//Tick this node
		if(m_poCurrentNode)
		{
			ActionNode* poActionNode = (ActionNode*)m_poCurrentNode;
			eFinish = poActionNode->Tick(_poWM);
		}
		//Current node has finished
		if(eFinish != E_AI_Executing)
			m_poCurrentNode = NULL;

		return eFinish;
	}
	void SelectorActionNode::Cancel(const AIWorkingMemory* _poWM)
	{
		if(m_poCurrentNode != NULL && m_poCurrentNode->IsActionNode())
		{	
			ActionNode* poNode = (ActionNode*)m_poCurrentNode;
			poNode->Cancel(_poWM);
			m_poCurrentNode = NULL;
		}
	}

	////////////////////////////////Sequence Node////////////////////////////////////////////
	/*SequenceActionNode::SequenceActionNode(AINodeScript* _poScript)
		:ActionNode(E_ANT_Sequence, _poScript), m_uiCurrentNode(-1)
	{
		if(m_poScript == NULL)
		{
			//m_poScript = new DefaultSelectorScript(this);
		} 
	}
	AIRet_t SequenceActionNode::Tick(const AIWorkingMemory* _poWM)
	{	
		AIRet_t eFinish = E_AI_Finish;

		//if first time
		if(m_uiCurrentNode < 0)
			m_uiCurrentNode = 0;

		u32	uiSize = m_arNodeList.Size();
		if(m_uiCurrentNode >= 0 && m_uiCurrentNode < uiSize)
		{
			for(u32 i = m_uiCurrentNode; i < uiSize; ++i)
			{
				AINode* poNode = m_arNodeList[m_uiCurrentNode];
				if(poNode->IsActionNode())
				{
					ActionNode* poActionNode = (ActionNode *)poNode;
					eFinish = poActionNode->Tick(_poWM);
					if(eFinish == E_AI_Executing)
						break;
				}
				m_uiCurrentNode++;
				if(m_uiCurrentNode == uiSize)
					break;
			}
		}
		if(eFinish != E_AI_Executing)
			m_uiCurrentNode = -1;

		return eFinish;
	}

	void SequenceActionNode::Cancel()
	{
		if(m_uiCurrentNode >= 0 && m_uiCurrentNode < m_arNodeList.Size())
		{	
			AINode* poNode = m_arNodeList[m_uiCurrentNode];
			if(poNode->IsActionNode())
			{
				ActionNode* poActionNode = (ActionNode *)poNode;
				poActionNode->Cancel();
			}
		}
	}*/
}