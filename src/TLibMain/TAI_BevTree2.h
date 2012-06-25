#ifndef __TAI_BEVTREE2_H__
#define __TAI_BEVTREE2_H__

#include <string>
#include "TUtility_AnyData.h"
#include <vector>

namespace TsiU{
	namespace AI{namespace BehaviorTree{
	
	enum BevRunningStatus
	{
		k_BRS_Invalid					= -1,
		k_BRS_Executing					= 0,
		k_BRS_Finish					= 1,
		k_BRS_Failure					= 2
	};

	typedef AnyData BevNodeInputParam;
	typedef AnyData BevNodeOutputParam;

	class Node;
	class Task;

	class Node
	{
	public:
		Node()
			:m_pParent(NULL)
		{}
		virtual ~Node(){};
		virtual Task* CreateTask() = 0;
		virtual void DestroyTask(Task* pTask) = 0;

		void SetParent(Node* node)
		{
			if(m_pParent)
			{
				D_Output("has parent already?\n");
			}
			m_pParent = node;
		}

	protected:
		Node* m_pParent;
	};
	class Task
	{
	public:
		Task(Node* pNode)
			:m_pNode(pNode)
		{}
		virtual ~Task(){};
		virtual void			 OnInit(const BevNodeInputParam& inputParam)									= 0;
		virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)	= 0;
		virtual void			 OnTerminate(const BevNodeInputParam& inputParam)								= 0;

	protected:
		Node* m_pNode;
	};
	class Behavior
	{
	public:
		Behavior()
			:m_pTask(NULL)
			,m_pNode(NULL)
			,m_eStatus(k_BRS_Invalid)
		{
		}
		Behavior(Node& node)
			:m_pTask(NULL)
			,m_eStatus(k_BRS_Invalid)
		{
			Install(node);
		}
		virtual ~Behavior()
		{
			Uninstall();
		}

		bool HasInstalled() const
		{
			return m_pTask != NULL;
		}

		void Install(Node& node)
		{
			Uninstall();

			m_pNode = &node;
			m_pTask = m_pNode->CreateTask();
		}
		void Uninstall()
		{
			if(!m_pTask) return;
			if(!m_pNode) return;
			m_pNode->DestroyTask(m_pTask);
			m_pTask = NULL;
			m_eStatus = k_BRS_Invalid;
		}
		BevRunningStatus Update(const BevNodeInputParam& inputParam,BevNodeOutputParam& outputParam)
		{
			if(m_eStatus == k_BRS_Invalid)
				m_pTask->OnInit(inputParam);
			BevRunningStatus ret = m_pTask->OnUpdate(inputParam, outputParam);
			if(ret != k_BRS_Executing)
			{
				m_pTask->OnTerminate(inputParam);
				m_eStatus = k_BRS_Invalid;
			}
			else
			{
				m_eStatus = ret;
			}
			return ret;
		}

	private:
		Node* m_pNode;
		Task* m_pTask;
		BevRunningStatus m_eStatus;
	};
	typedef std::vector<Node*> Nodes;

	class CompositeNode : public Node
	{
	public:
		virtual ~CompositeNode()
		{
			for(int i = 0; i < (int)m_Children.size(); ++i)
			{
				D_SafeDelete(m_Children[i]);
			}
		}
		Node* GetChild(int idx){
			if(idx < 0 || idx >= (int)m_Children.size())
				return NULL;
			return m_Children[idx];
		}
		void AddChild(Node* node)
		{
			node->SetParent(this);
			m_Children.push_back(node);
		}
		int GetChildCount() const{
			return m_Children.size();
		}
	protected:
		Nodes m_Children;
	};

	class SequenceTask : public Task
	{
	public:
		SequenceTask(Node* pNode)
			: Task(pNode)
			, m_iCurrentChild(0)
		{}
		CompositeNode& GetCompositeNode(){
			return *dynamic_cast<CompositeNode*>(m_pNode);
		}
		virtual void OnInit(const BevNodeInputParam& inputParam)
		{

		}
		virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
		{
			CompositeNode& comNode = GetCompositeNode();
			if(comNode.GetChildCount() == 0)
				return k_BRS_Failure;;

			if(!m_CurrentBehavior.HasInstalled())
			{
				m_iCurrentChild = 0;
				m_CurrentBehavior.Install(*(comNode.GetChild(m_iCurrentChild))); 
			}
			while(true)
			{
				BevRunningStatus status = m_CurrentBehavior.Update(inputParam, outputParam);
				if(status == k_BRS_Finish)
				{
					m_iCurrentChild++;
					if(m_iCurrentChild >= comNode.GetChildCount())
					{
						return status;
					}
					m_CurrentBehavior.Install(*(comNode.GetChild(m_iCurrentChild))); 
				}
				else
				{
					return status;
				}
			}
			return k_BRS_Invalid;
		}
		virtual void OnTerminate(const BevNodeInputParam& inputParam)
		{
			m_CurrentBehavior.Uninstall();
		};

	private:
		int m_iCurrentChild;
		Behavior m_CurrentBehavior;
	};
	class CompositeNode_Sequence : public CompositeNode
	{
	public:
		virtual Task* CreateTask()
		{
			return new SequenceTask(this);
		}
		virtual void DestroyTask(Task* pTask)
		{
			SequenceTask* pTest = dynamic_cast<SequenceTask*>(pTask);
			D_CHECK(pTest);
			D_SafeDelete(pTest);
		}
	};

	class SelectorTask : public Task
	{
	public:
		SelectorTask(Node* pNode)
			: Task(pNode)
			, m_LastBehavior(-1)
		{}
		CompositeNode& GetCompositeNode(){
			return *dynamic_cast<CompositeNode*>(m_pNode);
		}
		virtual void OnInit(const BevNodeInputParam& inputParam)
		{}
		virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
		{
			CompositeNode& comNode = GetCompositeNode();
			if(comNode.GetChildCount() == 0)
				return k_BRS_Failure;

			if(!m_CurrentBehavior.HasInstalled())
			{
				m_LastBehavior = 0;
				m_CurrentBehavior.Install(*(comNode.GetChild(m_LastBehavior))); 
			}
			BevRunningStatus status = m_CurrentBehavior.Update(inputParam, outputParam);
			if(status != k_BRS_Failure)
			{
				return status;
			}
			for(int i = 0; i < comNode.GetChildCount(); ++i)
			{
				if(m_LastBehavior == i)
					continue;

				m_CurrentBehavior.Install(*(comNode.GetChild(i)));
				BevRunningStatus status = m_CurrentBehavior.Update(inputParam, outputParam);
				if(status != k_BRS_Failure)
				{
					m_LastBehavior = i;
					return status;
				}
			}
			return k_BRS_Failure;;
		}
		virtual void OnTerminate(const BevNodeInputParam& inputParam)
		{
			m_LastBehavior = -1;
			m_CurrentBehavior.Uninstall();
		};

	private:
		int			 m_LastBehavior;
		Behavior m_CurrentBehavior;
	};
	class CompositeNode_Selector : public CompositeNode
	{
	public:
		virtual Task* CreateTask()
		{
			return new SelectorTask(this);
		}
		virtual void DestroyTask(Task* pTask)
		{
			SelectorTask* pTest = dynamic_cast<SelectorTask*>(pTask);
			D_CHECK(pTest);
			D_SafeDelete(pTest);
		}
	};

	class ParallelTask : public Task
	{
	public:
		ParallelTask(Node* pNode)
			: Task(pNode)
		{
			CompositeNode& comNode = GetCompositeNode();
			for(int i = 0; i < comNode.GetChildCount(); ++i)
			{
				m_Behaviors.push_back(new Behavior());
			}
		}
		~ParallelTask()
		{
			for(int i = 0; i < m_Behaviors.size(); ++i)
			{
				D_SafeDelete(m_Behaviors[i]);
			}
		}
		CompositeNode& GetCompositeNode(){
			return *dynamic_cast<CompositeNode*>(m_pNode);
		}
		virtual void OnInit(const BevNodeInputParam& inputParam)
		{
			CompositeNode& comNode = GetCompositeNode();
			D_CHECK(comNode.GetChildCount() == (int)m_Behaviors.size());
			for(int i = 0; i < (int)m_Behaviors.size(); ++i)
			{
				m_Behaviors[i]->Install(*(comNode.GetChild(i)));
			}
		}
		virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
		{
			//todo, or
			for(int i = 0; i < (int)m_Behaviors.size(); ++i)
			{
				BevRunningStatus ret = m_Behaviors[i]->Update(inputParam, outputParam);
				if(ret != k_BRS_Executing)
				{
					return ret;
				}
			}
			return k_BRS_Executing;
		}
		virtual void OnTerminate(const BevNodeInputParam& inputParam)
		{
		}
	private:
		std::vector<Behavior*> m_Behaviors;
	};
	class CompositeNode_Parallel : public CompositeNode
	{
	public:
		virtual Task* CreateTask()
		{
			return new ParallelTask(this);
		}
		virtual void DestroyTask(Task* pTask)
		{
			ParallelTask* pTest = dynamic_cast<ParallelTask*>(pTask);
			D_CHECK(pTest);
			D_SafeDelete(pTest);
		}
	};

#define DEF_TERMINATE_NODE(name, task) \
	class Node##_##name : public Node {\
	public:\
		virtual Task* CreateTask(){\
			return new task(this);\
		}\
		virtual void DestroyTask(Task* pTask){ \
			task* pTest = dynamic_cast<task*>(pTask);\
			D_CHECK(pTest);\
			D_SafeDelete(pTest);	\
		};\
	};

#define CREATE_TERMINATE_NODE(name) new Node##_##name()

}}}	

#endif