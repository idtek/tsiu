#ifndef __TINPUT_INPUTDEVICE__
#define __TINPUT_INPUTDEVICE__


#include "TUtility_Array.h"
#include "TInput_InputEvent.h"
#include "TInput_Enum.h"

namespace TsiU
{
	class InputDevice
	{
	public:
		InputDevice()
			:m_bIsActive(false),m_bIsPlugged(false),m_eInputDeviceType(E_IDT_Undefined)
		{}

		virtual void Tick(f32 _fDeltaTime)		= 0;

		inline void SetActive(Bool _bVal)		{	m_bIsActive = _bVal;		}
		inline void SetPlugged(Bool _bVal)		{	m_bIsPlugged = _bVal;		}
		inline Bool IsPlugged()					{	return m_bIsPlugged;		}
		inline Bool IsActive()					{	return m_bIsActive;			}
		inline InputDeviceType_t GetDeviceType(){	return m_eInputDeviceType;	}

	protected:
		InputDeviceType_t	m_eInputDeviceType;
		Bool				m_bIsPlugged;
		Bool				m_bIsActive;
	};
}


#endif