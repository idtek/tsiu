#include "TAI_Thinker.h"
#include "TAI_ActionNode.h"

namespace TsiU
{
	AIThinker::AIThinker()
		:m_poRoot(NULL), 
		 m_poWM(NULL),
		 m_LastAIUpdateResult(E_AI_Finish)
	{
	}

	AIThinker::~AIThinker()
	{
		D_SafeDelete(m_poRoot);
		D_SafeDelete(m_poWM);
	}

	void AIThinker::Create()
	{
	}

	void AIThinker::Tick(f32 _fDeltaTime)
	{
		if(m_LastAIUpdateResult == E_AI_Finish)
			_DoCollectWorkingParam(_fDeltaTime);

		if(m_poRoot != NULL && m_poRoot->IsActionNode())
		{
			ActionNode *poActionNode = (ActionNode *)m_poRoot;
			if(poActionNode->CanRun(m_poWM))
				m_LastAIUpdateResult = poActionNode->Tick(m_poWM);
		}

		_DoUpdate(_fDeltaTime);
	}
	
	void AIThinker::Draw()
	{
	}
}