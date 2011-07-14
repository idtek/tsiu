#include "TCore_LibSettings.h"
#include "TCore_Allocator.h"
#include <new>

namespace TsiU
{	
	TsiULibSettings::TsiULibSettings()
		: m_poAllocator(NULL)
		, m_uiMacro(0)
	{
		//Setup default allocator
		SetupAllocator(NULL);
		for(u32 i = 0; i < E_CreatorType_Number; ++i)
		{
			m_poCreatorList[i] = NULL;
		}
		m_uiMacro = 0;
	}
	TsiULibSettings::~TsiULibSettings()
	{
		for(u32 i = 0; i < E_CreatorType_Number; ++i)
		{
			D_SafeDelete(m_poCreatorList[i]);
		}
		//TJQ: not free
		if(m_poAllocator)
		{
			m_poAllocator->~Allocator();
			free(m_poAllocator);
			m_poAllocator = NULL;
		}
	}
	void TsiULibSettings::SetupAllocator(Allocator* alloc)
	{
		if(m_poAllocator)
		{
			m_poAllocator->~Allocator();
			free(m_poAllocator);
			m_poAllocator = NULL;
		}
		D_CHECK(!m_poAllocator);
		if(!alloc)
		{
			//Use Default allocator
			void* pMalloc = malloc(sizeof(DefaultAllocator));
			m_poAllocator = new(pMalloc) DefaultAllocator;
		}
		else
		{
			m_poAllocator = alloc;
		}
		D_CHECK(m_poAllocator);
	}
	Allocator* TsiULibSettings::GetAllocator()
	{
		return m_poAllocator;
	}
	void TsiULibSettings::SetupCreator(Creator* _poCreator, u32 _uiCreatorType)
	{
		D_CHECK(_poCreator);
		D_CHECK(_uiCreatorType < E_CreatorType_Number);

		D_SafeDelete(m_poCreatorList[_uiCreatorType]);

		m_poCreatorList[_uiCreatorType] = _poCreator;
	}
	Creator* TsiULibSettings::GetCreator(u32 _uiCreatorType)
	{
		D_CHECK(_uiCreatorType < E_CreatorType_Number);
		return m_poCreatorList[_uiCreatorType];
	}
	void TsiULibSettings::DefineMacro(LibSettingMacro_t _uiMacro)
	{
		m_uiMacro |= _uiMacro;
	}
	Bool TsiULibSettings::IsDefined(LibSettingMacro_t _uiMacro)
	{
		return (m_uiMacro & _uiMacro) != 0;
	}
	
	/************************************************************************/
	/* Global Func                                                          */
	/************************************************************************/

	static TsiULibSettings* g_spoLibSettings = NULL;

	void InitLibSettings(TsiULibSettings* _poLibSetting)
	{
		g_spoLibSettings = _poLibSetting;
		g_spoLibSettings->GetAllocator()->Init();
		for(u32 i = 0; i < E_CreatorType_Number; ++i)
		{
			g_spoLibSettings->SetupCreator(new NullCreator, i);
		}
	}
	TsiULibSettings* GetLibSettings()
	{
		return g_spoLibSettings;
	}
	void UninitLibSettings(TsiULibSettings* _poLibSetting)
	{
		g_spoLibSettings = NULL;
	}
	Bool HasInited()
	{
		return g_spoLibSettings != NULL;
	}
}