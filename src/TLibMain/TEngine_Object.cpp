#include "TEngine_Object.h"
#include "TEngine_Private.h"
#include "TRender_Renderer.h"
#include "TEngine_RenderModule.h"
#include "TCore_LibSettings.h"

namespace TsiU
{
	Mat4& Object::UpdateMatrix()
	{
		//if(IsMatOutOfDate())
		{
			m_mMatrix = Mat4::IDENTITY;

			m_mMatrix.Rotation(m_qRotation);			
			m_mMatrix.Scale(m_vScale.x, m_vScale.y, m_vScale.z);
			m_mMatrix.Translation(m_vPos);

			if(m_Parent)
				m_mMatrix = m_mMatrix * m_Parent->GetMatrix();
			else
			{
				if(GetLibSettings()->IsDefined(E_LS_Has_GUI))
				{
					Renderer* poRenderer = poGetEngine()->GetRenderMod()->GetRenderer();
					if(poRenderer)
						m_mMatrix = m_mMatrix * poRenderer->GetMatrixFromGUI();
				}
			}
			m_bNeedRecalc = false;
		}
		for(s32 i = 0; i < m_poChildList.Size(); ++i)
		{
			Object* poObj = m_poChildList[i];
			poObj->UpdateMatrix();
		}
		return m_mMatrix;
	}

	void Object::InvalidateMat()
	{
		m_bNeedRecalc = true;
		for(s32 i = 0; i < m_poChildList.Size(); ++i)
		{
			Object* poObj = m_poChildList[i];
			poObj->InvalidateMat();
		}
	}
}