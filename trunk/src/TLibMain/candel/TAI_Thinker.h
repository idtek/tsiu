#ifndef __TAI_THINKER__
#define __TAI_THINKER__

#include "TCore_Types.h"
#include "TEngine_Object.h"
#include "TAI_Node.h"

namespace TsiU
{
	class AINode;
	class AIWorkingMemory;

	class AIThinker : public DrawableObject
	{
	public:
		AIThinker();
		virtual ~AIThinker();

		virtual void Create();
		virtual void Tick(f32 _fDeltaTime);
		virtual void Draw();

	protected:
		virtual void _DoCollectWorkingParam(f32 _fDeltaTime) = 0;
		virtual void _DoUpdate(f32 _fDeltaTime) = 0;

	protected:
		AINode*				m_poRoot;
		AIWorkingMemory*	m_poWM;
		AIRet_t				m_LastAIUpdateResult;
	};
}

#endif