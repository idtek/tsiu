#ifndef __TCORE_LIBSETTINGS__ 
#define __TCORE_LIBSETTINGS__


#include "TCore_Creator.h"

namespace TsiU
{
	enum LibSettingMacro_t
	{
		E_LS_Has_GUI		= (1<<1),
		E_LS_Has_D3D		= (1<<2),
		E_LS_Has_GDI		= (1<<3),
		E_LS_Has_Network	= (1<<4),
		E_LS_Has_Script		= (1<<5)
	};

	class LibSettings
	{
	public:
		LibSettings();
		~LibSettings();

		void SetCreator(Creator* _poCreator, u32 _uiCreatorType);
		Creator* GetCreator(u32 _uiCreatorType);

		void DefineMacro(LibSettingMacro_t _uiMacro);
		Bool IsDefined(LibSettingMacro_t _uiMacro);

	private:
		u32			m_uiMacro;
		Creator*	m_poCreatorList[E_CreatorType_Number];
	};
	/************************************************************************/
	/* Global Func                                                          */
	/************************************************************************/
	
	extern void InitLibSettings();
	extern LibSettings* GetLibSettings();
	extern void UnInitLibSettings();

	template<typename T>
	T* CallCreator(u32 _uiType)
	{
		return (T*)GetLibSettings()->GetCreator(_uiType)->Create();
	}
}

#endif