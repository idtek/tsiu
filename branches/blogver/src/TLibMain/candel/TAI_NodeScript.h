#ifndef __TAI_NODESCRIPT__
#define __TAI_NODESCRIPT__

#include "TCore_Types.h"

namespace TsiU
{
	class AINode;
	class AIWorkingMemory;

	class AINodeScript
	{
	public:
		AINodeScript()
			:m_poNode(NULL)
		{}

		AINodeScript(AINode* _poNode)
		{
			Init(_poNode);
		}
		virtual ~AINodeScript(){}

		void Init(AINode* _poNode)
		{
			m_poNode = _poNode;
			Validate();
		}

		virtual void Validate(){};

	protected:
		AINode* m_poNode;
	};

	class SelectorScript : public AINodeScript
	{
	public:
		SelectorScript(AINode* _poNode)
			:AINodeScript(_poNode)
		{}

		virtual void Validate()
		{
			D_CHECK(m_poNode != NULL && m_poNode->IsSelectorNode());
		}

		virtual AINode* GetNode(const AIWorkingMemory* _poWM) = 0;
	};

	class ConditionScript : public AINodeScript
	{
	public:
		ConditionScript(AINode* _poNode)
			:AINodeScript(_poNode)
		{}
		virtual Bool IsOK(const AIWorkingMemory* _poWM) const = 0;

		virtual void Validate()
		{
			D_CHECK(m_poNode != NULL && m_poNode->IsConditionNode());
		}
	};

	class SequenceScript : public AINodeScript
	{
	public:
		SequenceScript(AINode* _poNode)
			:AINodeScript(_poNode)
		{}
		virtual void Validate()
		{
			D_CHECK(m_poNode != NULL && m_poNode->IsSequenceNode());
		}
	};

	class TerminalScript : public AINodeScript
	{
	public:
		TerminalScript(AINode* _poNode)
			:AINodeScript(_poNode)
		{}
		virtual void Validate()
		{
			D_CHECK(m_poNode != NULL && m_poNode->IsTerminalNode());
		}

		virtual AIRet_t Execute(const AIWorkingMemory* _poWM) = 0;
		virtual void Exit() = 0;
	};
}


#endif