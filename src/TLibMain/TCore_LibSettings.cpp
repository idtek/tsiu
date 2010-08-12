#include "TCore_LibSettings.h"

namespace TsiU
{	
	LibSettings::LibSettings()
	{
		for(u32 i = 0; i < E_CreatorType_Number; ++i)
		{
			m_poCreatorList[i] = new NullCreator;
			D_CHECK(m_poCreatorList[i]);
		}
		m_uiMacro = 0;
	}
	LibSettings::~LibSettings()
	{
		for(u32 i = 0; i < E_CreatorType_Number; ++i)
		{
			D_SafeDelete(m_poCreatorList[i]);
		}
	}
	void LibSettings::SetCreator(Creator* _poCreator, u32 _uiCreatorType)
	{
		D_CHECK(_poCreator);
		D_CHECK(_uiCreatorType < E_CreatorType_Number);

		D_SafeDelete(m_poCreatorList[_uiCreatorType]);

		m_poCreatorList[_uiCreatorType] = _poCreator;
	}
	Creator* LibSettings::GetCreator(u32 _uiCreatorType)
	{
		D_CHECK(_uiCreatorType < E_CreatorType_Number);
		return m_poCreatorList[_uiCreatorType];
	}
	void LibSettings::DefineMacro(LibSettingMacro_t _uiMacro)
	{
		m_uiMacro |= _uiMacro;
	}
	Bool LibSettings::IsDefined(LibSettingMacro_t _uiMacro)
	{
		return (m_uiMacro & _uiMacro) != 0;
	}
	
	/************************************************************************/
	/* Global Func                                                          */
	/************************************************************************/

	LibSettings* g_poLibSettings = NULL;

	void InitLibSettings()
	{
		g_poLibSettings = new LibSettings;
		D_CHECK(g_poLibSettings);
	}
	LibSettings* GetLibSettings()
	{
		return g_poLibSettings;
	}
	void UnInitLibSettings()
	{
		D_SafeDelete(g_poLibSettings);
	}
}