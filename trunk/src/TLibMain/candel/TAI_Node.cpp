#include "TAI_Node.h"
#include "TAI_NodeScript.h"

namespace TsiU
{
	AINode::~AINode()
	{
		for(s32 i = 0; i < m_arNodeList.Size(); ++i)
		{
			D_SafeDelete(m_arNodeList[i]);
		}
		m_arNodeList.Clear();
	}
}