#include "TRender_Renderer.h"

namespace TsiU
{
	Renderer::Renderer()
	{
		m_poMainWindow = NULL;
				
		m_mMatrixFromGUI = Mat4::ZERO;
		m_qRotationFromGUI = Quat::IDENTITY;
		m_vTranslationFromGUI = Vec3::ZERO;
		m_fZoomFromGUI = 1.f;

		_UpdateMatrixFromGUI();
	}

	void Renderer::_UpdateMatrixFromGUI()
	{
		m_mMatrixFromGUI = Mat4::IDENTITY;

		m_mMatrixFromGUI.Rotation(m_qRotationFromGUI);			
		m_mMatrixFromGUI.Translation(m_vTranslationFromGUI);
	}
}