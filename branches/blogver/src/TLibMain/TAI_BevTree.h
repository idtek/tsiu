#ifndef __TAI_BEVTREE_H__
#define __TAI_BEVTREE_H__

#include <string>
#include "TUtility_AnyData.h"

namespace TsiU{

	class Object;

	namespace AI{namespace BehaviorTree{

	#define k_BLimited_MaxChildNodeCnt              16
	#define k_BLimited_InvalidChildNodeIndex        k_BLimited_MaxChildNodeCnt

	enum E_BehaviourNodeType
	{
		k_BNT_None,
		k_BNT_Terminal,
		k_BNT_Sequence,
		k_BNT_Selector,
		k_BNT_Parallel,
		k_BNT_Loop,
	};

	enum E_ParallelFinishCondition
	{
		k_PFC_OR = 1,
		k_PFC_AND
	};

	enum BevRunningStatus
	{
		k_BRS_Executing					= 0,
		k_BRS_Finish					= 1,
		k_BRS_ERROR_PreconditionFailed  = -1,
		k_BRS_ERROR_Transition			= -2,
	};

	enum E_TerminalNodeStaus
	{
		k_TNS_Ready         = 1,
		k_TNS_Running       = 2,
		k_TNS_Finish        = 3,
	};

	typedef AnyData BevNodeInputParam;
	typedef AnyData BevNodeOutputParam;
	typedef Object Entity;

	//-------------------------------------------------------------------------------------------------------------------------------------
	class BevNodePrecondition
	{
	public:
		virtual bool ExternalCondition(const BevNodeInputParam& input) const = 0;
	};
	class BevNodePreconditionTRUE : public BevNodePrecondition
	{
	public:
		virtual bool ExternalCondition(const BevNodeInputParam& input) const{
			return true;
		}
	};
	class BevNodePreconditionFALSE : public BevNodePrecondition
	{
	public:
		virtual bool ExternalCondition(const BevNodeInputParam& input) const{
			return false;
		}
	};
	class BevNodePreconditionNOT : public BevNodePrecondition
	{
	public:
		BevNodePreconditionNOT(BevNodePrecondition* lhs)
			: m_lhs(lhs)
		{
			D_CHECK(m_lhs);
		}
		~BevNodePreconditionNOT(){
			D_SafeDelete(m_lhs);
		}
		virtual bool ExternalCondition(const BevNodeInputParam& input) const{
			return !m_lhs->ExternalCondition(input);
		}
	private:
		BevNodePrecondition* m_lhs;
	};
	class BevNodePreconditionAND : public BevNodePrecondition
	{
	public:
		BevNodePreconditionAND(BevNodePrecondition* lhs, BevNodePrecondition* rhs)
			: m_lhs(lhs)
			, m_rhs(rhs)
		{
			D_CHECK(m_lhs && m_rhs);
		}
		~BevNodePreconditionAND(){
			D_SafeDelete(m_lhs);
			D_SafeDelete(m_rhs);
		}
		virtual bool ExternalCondition(const BevNodeInputParam& input) const{
			return m_lhs->ExternalCondition(input) && m_rhs->ExternalCondition(input);
		}
	private:
		BevNodePrecondition* m_lhs;
		BevNodePrecondition* m_rhs;
	};
	class BevNodePreconditionOR : public BevNodePrecondition
	{
	public:
		BevNodePreconditionOR(BevNodePrecondition* lhs, BevNodePrecondition* rhs)
			: m_lhs(lhs)
			, m_rhs(rhs)
		{
			D_CHECK(m_lhs && m_rhs);
		}
		~BevNodePreconditionOR(){
			D_SafeDelete(m_lhs);
			D_SafeDelete(m_rhs);
		}
		virtual bool ExternalCondition(const BevNodeInputParam& input) const{
			return m_lhs->ExternalCondition(input) || m_rhs->ExternalCondition(input);
		}
	private:
		BevNodePrecondition* m_lhs;
		BevNodePrecondition* m_rhs;
	};
	class BevNodePreconditionXOR : public BevNodePrecondition
	{
	public:
		BevNodePreconditionXOR(BevNodePrecondition* lhs, BevNodePrecondition* rhs)
			: m_lhs(lhs)
			, m_rhs(rhs)
		{
			D_CHECK(m_lhs && m_rhs);
		}
		~BevNodePreconditionXOR(){
			D_SafeDelete(m_lhs);
			D_SafeDelete(m_rhs);
		}
		virtual bool ExternalCondition(const BevNodeInputParam& input) const{
			return m_lhs->ExternalCondition(input) ^ m_rhs->ExternalCondition(input);
		}
	private:
		BevNodePrecondition* m_lhs;
		BevNodePrecondition* m_rhs;
	};
	//-------------------------------------------------------------------------------------------------------------------------------------
	class BevNode
	{
	public:
		BevNode(const E_BehaviourNodeType& _type, Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodeScript = NULL)
			: mul_ChildNodeCount(0)
			, mz_DebugName("UNNAMED")
			, mo_ActiveNode(NULL)
			, mo_LastActiveNode(NULL)
			, mo_NodePrecondition(NULL)
		{
			for(int i = 0; i < k_BLimited_MaxChildNodeCnt; ++i)
				mao_ChildNodeList[i] = NULL;

			_SetNodeType(_type);
			_SetOwner(_o_Owner);
			_SetParentNode(_o_ParentNode);
			SetNodePrecondition(_o_NodeScript);
		}
		virtual ~BevNode()
		{
			for(unsigned int i = 0; i < mul_ChildNodeCount; ++i)
			{
				D_SafeDelete(mao_ChildNodeList[i]);
			}
			D_SafeDelete(mo_NodePrecondition);
		}
		bool Evaluate(const BevNodeInputParam& input)
		{
			return (mo_NodePrecondition == NULL || mo_NodePrecondition->ExternalCondition(input)) && _DoEvaluate(input);
		}
		void Transition(const BevNodeInputParam& input)
		{
			_DoTransition(input);
		}
		BevRunningStatus Tick(const BevNodeInputParam& input, BevNodeOutputParam& output)
		{
			return _DoTick(input, output);
		}
		//---------------------------------------------------------------
		BevNode& AddChildNode(BevNode* _o_ChildNode)
		{
			if(mul_ChildNodeCount == k_BLimited_MaxChildNodeCnt)
			{
				D_Output("The number of child nodes is up to 16");
				D_CHECK(0);
				return (*this);
			}
			mao_ChildNodeList[mul_ChildNodeCount] = _o_ChildNode;
			++mul_ChildNodeCount;
			return (*this);
		}
		BevNode& SetNodePrecondition(BevNodePrecondition* _o_NodePrecondition)
		{
			if(mo_NodePrecondition != _o_NodePrecondition)
			{
				if(mo_NodePrecondition)
					delete mo_NodePrecondition;

				mo_NodePrecondition = _o_NodePrecondition;
			}
			return (*this);
		}
		BevNode& SetDebugName(const char* _debugName)
		{
			mz_DebugName = _debugName;
			return (*this);
		}
		const BevNode* oGetLastActiveNode() const
		{
			return mo_LastActiveNode;
		}
		void SetActiveNode(BevNode* _o_Node)
		{
			mo_LastActiveNode = mo_ActiveNode;
			mo_ActiveNode = _o_Node;
			if(mo_ParentNode != NULL)
				mo_ParentNode->SetActiveNode(_o_Node);
		}
		const char* GetDebugName() const
		{
			return mz_DebugName.c_str();
		}
	protected:
		//--------------------------------------------------------------
		// virtual function
		//--------------------------------------------------------------
		virtual bool _DoEvaluate(const BevNodeInputParam& input)
		{
			return true;
		}
		virtual void _DoTransition(const BevNodeInputParam& input)
		{
		}
		virtual BevRunningStatus _DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output)
		{
			return k_BRS_Finish;
		}
	protected:
		void _SetNodeType(E_BehaviourNodeType _e_NodeType)
		{
			mui_NodeType = _e_NodeType;
		}
		void _SetOwner(Entity* _o_Owner)
		{
			mo_Owner = _o_Owner;
		}
		void _SetParentNode(BevNode* _o_ParentNode)
		{
			mo_ParentNode = _o_ParentNode;
		}
		bool _bCheckIndex(u32 _ui_Index) const
		{
			return _ui_Index >= 0 && _ui_Index < mul_ChildNodeCount;
		}
	protected:
		E_BehaviourNodeType     mui_NodeType;
		BevNode*                mao_ChildNodeList[k_BLimited_MaxChildNodeCnt];
		u32						mul_ChildNodeCount;
		BevNode*                mo_ParentNode;
		BevNode*                mo_ActiveNode;
		BevNode*				mo_LastActiveNode;
		Entity*				    mo_Owner;
		BevNodePrecondition*    mo_NodePrecondition;
		std::string				mz_DebugName;
	};

	class BevNodePrioritySelector : public BevNode
	{
	public:
		BevNodePrioritySelector(Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodePrecondition = NULL)
			: BevNode(k_BNT_Selector, _o_Owner, _o_ParentNode, _o_NodePrecondition)
			, mui_LastSelectIndex(k_BLimited_InvalidChildNodeIndex)
			, mui_CurrentSelectIndex(k_BLimited_InvalidChildNodeIndex)
		{}
		virtual bool _DoEvaluate(const BevNodeInputParam& input);
		virtual void _DoTransition(const BevNodeInputParam& input);
		virtual BevRunningStatus _DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output);

	protected:
		u32 mui_CurrentSelectIndex;
		u32 mui_LastSelectIndex;
	};
	
	class BevNodeNonePrioritySelector : public BevNodePrioritySelector
	{
	public:
		BevNodeNonePrioritySelector(Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodePrecondition = NULL)
			: BevNodePrioritySelector(_o_Owner, _o_ParentNode, _o_NodePrecondition)
		{}
		virtual bool _DoEvaluate(const BevNodeInputParam& input);
	};

	class BevNodeSequence : public BevNode
	{
	public:
		BevNodeSequence(Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodePrecondition = NULL)
			: BevNode(k_BNT_Sequence, _o_Owner, _o_ParentNode, _o_NodePrecondition)
			, mui_CurrentNodeIndex(k_BLimited_InvalidChildNodeIndex)
		{}
		virtual bool _DoEvaluate(const BevNodeInputParam& input);
		virtual void _DoTransition(const BevNodeInputParam& input);
		virtual BevRunningStatus _DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output);

	private:
		u32 mui_CurrentNodeIndex;
	};

	class BevNodeTerminal : public BevNode
	{
	public:
		BevNodeTerminal(Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodePrecondition = NULL)
			: BevNode(k_BNT_Terminal, _o_Owner, _o_ParentNode, _o_NodePrecondition)
			, me_Status(k_TNS_Ready)
			, mb_NeedExit(false)
		{}
		virtual void _DoTransition(const BevNodeInputParam& input);
		virtual BevRunningStatus _DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output);

	protected:
		virtual void				_DoEnter(const BevNodeInputParam& input)								{}
		virtual BevRunningStatus	_DoExecute(const BevNodeInputParam& input, BevNodeOutputParam& output)	{ return k_BRS_Finish;}
		virtual void				_DoExit(const BevNodeInputParam& input, BevRunningStatus _ui_ExitID)	{}

	private:
		E_TerminalNodeStaus me_Status;
		bool                mb_NeedExit;
	};

	class BevNodeParallel : public BevNode
	{
	public:
		BevNodeParallel(Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodePrecondition = NULL)
			: BevNode(k_BNT_Parallel, _o_Owner, _o_ParentNode, _o_NodePrecondition)
			, me_FinishCondition(k_PFC_OR)
		{
			for(unsigned int i = 0; i < k_BLimited_MaxChildNodeCnt; ++i)
				mab_ChildNodeStatus[i] = k_BRS_Executing;
		}
		virtual bool _DoEvaluate(const BevNodeInputParam& input);
		virtual void _DoTransition(const BevNodeInputParam& input);
		virtual BevRunningStatus _DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output);

		BevNodeParallel& SetFinishCondition(E_ParallelFinishCondition _e_Condition);

	private:
		E_ParallelFinishCondition me_FinishCondition;
		BevRunningStatus		  mab_ChildNodeStatus[k_BLimited_MaxChildNodeCnt];
	};

	class BevNodeLoop : public BevNode
	{
	public:
		static const int kInfiniteLoop = -1;

	public:
		BevNodeLoop(Entity* _o_Owner, BevNode* _o_ParentNode, BevNodePrecondition* _o_NodePrecondition = NULL, int _i_LoopCnt = kInfiniteLoop)
			: BevNode(k_BNT_Loop, _o_Owner, _o_ParentNode, _o_NodePrecondition)
			, mi_LoopCount(_i_LoopCnt)
			, mi_CurrentCount(0)
		{}
		virtual bool _DoEvaluate(const BevNodeInputParam& input);
		virtual void _DoTransition(const BevNodeInputParam& input);
		virtual BevRunningStatus _DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output);

	private:
		int mi_LoopCount;
		int mi_CurrentCount;
	};

	class BevNodeFactory
	{
	public:
		static BevNode& oCreateParallelNode(Entity* _o_Owner, BevNode* _o_Parent, E_ParallelFinishCondition _e_Condition, const char* _debugName)
		{
			BevNodeParallel* pReturn = new BevNodeParallel(_o_Owner, _o_Parent);
			pReturn->SetFinishCondition(_e_Condition);
			oCreateNodeCommon(pReturn, _o_Parent, _debugName);
			return (*pReturn);
		}
		static BevNode& oCreatePrioritySelectorNode(Entity* _o_Owner, BevNode* _o_Parent, const char* _debugName)
		{
			BevNodePrioritySelector* pReturn = new BevNodePrioritySelector(_o_Owner, _o_Parent);
			oCreateNodeCommon(pReturn, _o_Parent, _debugName);
			return (*pReturn);
		}
		static BevNode& oCreateNonePrioritySelectorNode(Entity* _o_Owner, BevNode* _o_Parent, const char* _debugName)
		{
			BevNodeNonePrioritySelector* pReturn = new BevNodeNonePrioritySelector(_o_Owner, _o_Parent);
			oCreateNodeCommon(pReturn, _o_Parent, _debugName);
			return (*pReturn);
		}
		static BevNode& oCreateSequenceNode(Entity* _o_Owner, BevNode* _o_Parent, const char* _debugName)
		{
			BevNodeSequence* pReturn = new BevNodeSequence(_o_Owner, _o_Parent);
			oCreateNodeCommon(pReturn, _o_Parent, _debugName);
			return (*pReturn);
		}
		static BevNode& oCreateLoopNode(Entity* _o_Owner, BevNode* _o_Parent, const char* _debugName, int _i_LoopCount)
		{
			BevNodeLoop* pReturn = new BevNodeLoop(_o_Owner, _o_Parent, NULL, _i_LoopCount);
			oCreateNodeCommon(pReturn, _o_Parent, _debugName);
			return (*pReturn);
		}
		template<typename T>
		static BevNode& oCreateTeminalNode(Entity* _o_Owner, BevNode* _o_Parent, const char* _debugName)
		{
			BevNodeTerminal* pReturn = new T(_o_Owner, _o_Parent);
			oCreateNodeCommon(pReturn, _o_Parent, _debugName);
			return (*pReturn);
		}
	private:
		static void oCreateNodeCommon(BevNode* _o_Me, BevNode* _o_Parent, const char* _debugName)
		{
			if(_o_Parent)
				_o_Parent->AddChildNode(_o_Me);
			_o_Me->SetDebugName(_debugName);
		}
	};
}}}

#endif