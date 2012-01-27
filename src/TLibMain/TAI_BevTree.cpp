#include "TAI_BevTree.h"

namespace TsiU{
	namespace AI{namespace BehaviorTree{
	//-------------------------------------------------------------------------------------
	// BevNodePrioritySelector
	//-------------------------------------------------------------------------------------
	bool BevNodePrioritySelector::_DoEvaluate(const BevNodeInputParam& input)
	{
		mui_CurrentSelectIndex = k_BLimited_InvalidChildNodeIndex;
		for(unsigned int i = 0; i < mul_ChildNodeCount; ++i)
		{
			BevNode* oBN = mao_ChildNodeList[i];
			if(oBN->Evaluate(input))
			{
				mui_CurrentSelectIndex = i;
				return true;
			}
		}
		return false;
	}
	void BevNodePrioritySelector::_DoTransition(const BevNodeInputParam& input)
	{
		if(_bCheckIndex(mui_LastSelectIndex))
		{
			BevNode* oBN = mao_ChildNodeList[mui_LastSelectIndex];
			oBN->Transition(input);
		}
		mui_LastSelectIndex = k_BLimited_InvalidChildNodeIndex;
	}
	BevRunningStatus BevNodePrioritySelector::_DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output)
	{
		BevRunningStatus bIsFinish = k_BRS_Finish;
		if(_bCheckIndex(mui_CurrentSelectIndex))
		{
			if(mui_LastSelectIndex != mui_CurrentSelectIndex)  //new select result
			{
				if(_bCheckIndex(mui_LastSelectIndex))
				{
					BevNode* oBN = mao_ChildNodeList[mui_LastSelectIndex];
					oBN->Transition(input);   //we need transition
				}
				mui_LastSelectIndex = mui_CurrentSelectIndex;
			}
		}
		if(_bCheckIndex(mui_LastSelectIndex))
		{
			//Running node
			BevNode* oBN = mao_ChildNodeList[mui_LastSelectIndex];
			bIsFinish = oBN->Tick(input, output);
			//clear variable if finish
			if(bIsFinish)
				mui_LastSelectIndex = k_BLimited_InvalidChildNodeIndex;
		}
		return bIsFinish;
	}
	//-------------------------------------------------------------------------------------
	// BevNodeNonePrioritySelector
	//-------------------------------------------------------------------------------------
	bool BevNodeNonePrioritySelector::_DoEvaluate(const BevNodeInputParam& input)
	{
		if(_bCheckIndex(mui_CurrentSelectIndex))
		{
			BevNode* oBN = mao_ChildNodeList[mui_CurrentSelectIndex];
			if(oBN->Evaluate(input))
			{
				return true;
			}
		}
		return BevNodePrioritySelector::_DoEvaluate(input);
	}
	//-------------------------------------------------------------------------------------
	// BevNodeSequence
	//-------------------------------------------------------------------------------------
	bool BevNodeSequence::_DoEvaluate(const BevNodeInputParam& input)
	{
		unsigned int testNode;
		if(mui_CurrentNodeIndex == k_BLimited_InvalidChildNodeIndex)
			testNode = 0;
		else
			testNode = mui_CurrentNodeIndex;

		if(_bCheckIndex(testNode))
		{
			BevNode* oBN = mao_ChildNodeList[testNode];
			if(oBN->Evaluate(input))
				return true;
		}
		return false;
	}
	void BevNodeSequence::_DoTransition(const BevNodeInputParam& input)
	{
		if(_bCheckIndex(mui_CurrentNodeIndex))
		{
			BevNode* oBN = mao_ChildNodeList[mui_CurrentNodeIndex];
			oBN->Transition(input);
		}
		mui_CurrentNodeIndex = k_BLimited_InvalidChildNodeIndex;
	}
	BevRunningStatus BevNodeSequence::_DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output)
	{
		BevRunningStatus bIsFinish = k_BRS_Finish;

		//First Time
		if(mui_CurrentNodeIndex == k_BLimited_InvalidChildNodeIndex)
			mui_CurrentNodeIndex = 0;

		BevNode* oBN = mao_ChildNodeList[mui_CurrentNodeIndex];
		bIsFinish = oBN->Tick(input, output);
		if(bIsFinish == k_BRS_Finish)
		{
			++mui_CurrentNodeIndex;
			//sequence is over
			if(mui_CurrentNodeIndex == mul_ChildNodeCount)
			{
				mui_CurrentNodeIndex = k_BLimited_InvalidChildNodeIndex;
			}
			else
			{
				bIsFinish = k_BRS_Executing;
			}
		}
		if(bIsFinish < 0)
		{
			mui_CurrentNodeIndex = k_BLimited_InvalidChildNodeIndex;
		}
		return bIsFinish;
	}

	//-------------------------------------------------------------------------------------
	// BevNodeTerminal
	//-------------------------------------------------------------------------------------
	void BevNodeTerminal::_DoTransition(const BevNodeInputParam& input)
	{
		if(mb_NeedExit)     //call Exit if we have called Enter
			_DoExit(input, k_BRS_ERROR_Transition);

		SetActiveNode(NULL);
		me_Status = k_TNS_Ready;
		mb_NeedExit = false;
	}
	BevRunningStatus BevNodeTerminal::_DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output)
	{
		BevRunningStatus bIsFinish = k_BRS_Finish;

		if(me_Status == k_TNS_Ready)
		{
			_DoEnter(input);
			mb_NeedExit = TRUE;
			me_Status = k_TNS_Running;
			SetActiveNode(this);
		}
		if(me_Status == k_TNS_Running)
		{
			bIsFinish = _DoExecute(input, output);
			SetActiveNode(this);
			if(bIsFinish == k_BRS_Finish || bIsFinish < 0)
				me_Status = k_TNS_Finish;
		}
		if(me_Status == k_TNS_Finish)
		{
			if(mb_NeedExit)     //call Exit if we have called Enter
				_DoExit(input, bIsFinish);

			me_Status = k_TNS_Ready;
			mb_NeedExit = FALSE;
			SetActiveNode(NULL);

			return bIsFinish;
		}
		return bIsFinish;
	}

	//-------------------------------------------------------------------------------------
	// BevNodeParallel
	//-------------------------------------------------------------------------------------
	bool BevNodeParallel::_DoEvaluate(const BevNodeInputParam& input)
	{
		for(unsigned int i = 0; i < mul_ChildNodeCount; ++i)
		{
			BevNode* oBN = mao_ChildNodeList[i];
			if(mab_ChildNodeStatus[i] == 0)
			{
				if(!oBN->Evaluate(input))
				{
					return false;
				}
			}
		}
		return true;
	}
	void BevNodeParallel::_DoTransition(const BevNodeInputParam& input)
	{
		for(unsigned int i = 0; i < k_BLimited_MaxChildNodeCnt; ++i)
			mab_ChildNodeStatus[i] = k_BRS_Executing;

		for(unsigned int i = 0; i < mul_ChildNodeCount; ++i)
		{
			BevNode* oBN = mao_ChildNodeList[i];
			oBN->Transition(input);
		}
	}
	BevNodeParallel& BevNodeParallel::SetFinishCondition(E_ParallelFinishCondition _e_Condition)
	{
		me_FinishCondition = _e_Condition;
		return (*this);
	}
	BevRunningStatus BevNodeParallel::_DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output)
	{   
		unsigned int finishedChildCount = 0;  
		for(unsigned int i = 0; i < mul_ChildNodeCount; ++i)
		{
			BevNode* oBN = mao_ChildNodeList[i];
			if(me_FinishCondition == k_PFC_OR)
			{
				if(mab_ChildNodeStatus[i] == k_BRS_Executing)
				{
					mab_ChildNodeStatus[i] = oBN->Tick(input, output);
				}
				if(mab_ChildNodeStatus[i] != k_BRS_Executing)
				{
					for(unsigned int i = 0; i < k_BLimited_MaxChildNodeCnt; ++i)
						mab_ChildNodeStatus[i] = k_BRS_Executing;
					return k_BRS_Finish;
				}
			}
			else if(me_FinishCondition == k_PFC_AND)
			{
				if(mab_ChildNodeStatus[i] == k_BRS_Executing)
				{
					mab_ChildNodeStatus[i] = oBN->Tick(input, output);
				}
				if(mab_ChildNodeStatus[i] != k_BRS_Executing)
				{
					finishedChildCount++;
				}
			}
			else
			{
				D_CHECK(0);
;			}
		}
		if(finishedChildCount == mul_ChildNodeCount)
		{
			for(unsigned int i = 0; i < k_BLimited_MaxChildNodeCnt; ++i)
				mab_ChildNodeStatus[i] = k_BRS_Executing;
			return k_BRS_Finish;
		}
		return k_BRS_Executing;
	}
	//-------------------------------------------------------------------------------------
	// BevNodeLoop
	//-------------------------------------------------------------------------------------
	bool BevNodeLoop::_DoEvaluate(const BevNodeInputParam& input)
	{
		bool checkLoopCount = (mi_LoopCount == kInfiniteLoop) ||
			mi_CurrentCount < mi_LoopCount;

		if(!checkLoopCount)
			return false;

		if(_bCheckIndex(0))
		{
			BevNode* oBN = mao_ChildNodeList[0];
			if(oBN->Evaluate(input))
				return true;
		}
		return false;				 
	}
	void BevNodeLoop::_DoTransition(const BevNodeInputParam& input)
	{
		if(_bCheckIndex(0))
		{
			BevNode* oBN = mao_ChildNodeList[0];
			oBN->Transition(input);
		}
		mi_CurrentCount = 0;
	}
	BevRunningStatus BevNodeLoop::_DoTick(const BevNodeInputParam& input, BevNodeOutputParam& output)
	{
		BevRunningStatus bIsFinish = k_BRS_Finish;
		if(_bCheckIndex(0))
		{
			BevNode* oBN = mao_ChildNodeList[0];
			bIsFinish = oBN->Tick(input, output);

			if(bIsFinish == k_BRS_Finish)
			{
				if(mi_LoopCount != kInfiniteLoop)
				{
					mi_CurrentCount++;
					if(mi_CurrentCount == mi_LoopCount)
					{
						bIsFinish = k_BRS_Executing;
					}
				}
				else
				{
					bIsFinish = k_BRS_Executing;
				}
			}
		}
		if(bIsFinish)
		{
			mi_CurrentCount = 0;
		}
		return bIsFinish;
	}	
}}}