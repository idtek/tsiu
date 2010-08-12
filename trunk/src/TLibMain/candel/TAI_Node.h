#ifndef __TAI_NODE__
#define __TAI_NODE__

#include "TCore_Types.h"
#include "TUtility_Array.h"

namespace TsiU
{
	enum AINodeType_t
	{
		E_ANT_Terminal,
		E_ANT_Selector,
		E_ANT_Sequence,
		E_ANT_Condition,
	};

	enum AIRet_t
	{
		E_AI_Error = -1,

		E_AI_Finish = 0,
		E_AI_Executing
	};

	class AINode
	{
	public:
		AINode(AINodeType_t _eType)
			:m_eNodeType(_eType), 
			 m_poParent(NULL)
		{}
		virtual ~AINode();

		inline void SetParent(AINode* _poNode)
		{
			m_poParent = _poNode;
		}
		virtual AINode* AddNode(AINode* _poNode)
		{
			D_CHECK(_poNode);
			_poNode->SetParent(this);
			m_arNodeList.PushBack(_poNode);
			return this;
		}
		inline AINode* GetParent() const
		{ 
			return m_poParent;
		};
		inline AINode* GetChild(u32 _uiIdx) const
		{
			if(_uiIdx > 0 && _uiIdx < m_arNodeList.Size())
				return m_arNodeList.Get(_uiIdx);
			return NULL;
		}

		/********************************************************************************************/
		//Helper function
		inline Bool IsActionNode() const
		{
			return IsTerminalNode() ||
				   IsSelectorNode() ||
				   IsSequenceNode();
		}
		inline Bool IsTerminalNode() const	{ return m_eNodeType == E_ANT_Terminal;	}
		inline Bool IsSelectorNode() const	{ return m_eNodeType == E_ANT_Selector;	}
		inline Bool IsSequenceNode() const	{ return m_eNodeType == E_ANT_Sequence;	} 
		inline Bool IsConditionNode() const	{ return m_eNodeType == E_ANT_Condition;} 

	protected:
		AINodeType_t		m_eNodeType;
		Array<AINode*>		m_arNodeList;
		AINode*				m_poParent;
	};
}

#endif