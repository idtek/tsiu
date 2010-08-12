#ifndef __TINPUT_WIN_INPUTDEVICE__
#define __TINPUT_WIN_INPUTDEVICE__

#include "TCore_Types.h"
#include "TInput_InputDevice.h"
#include "TWin32_Private.h"

namespace TsiU
{
	class WinInputManager;

	class WinInputDeviceMouse : public InputDevice
	{
	public:
		WinInputDeviceMouse(HINSTANCE _poHInst, HWND _poHWnd, WinInputManager* _poParent);
		~WinInputDeviceMouse();

		virtual void Tick(f32 _fDeltaTime);

	private:
		WinInputManager*		m_poManager;

		HINSTANCE				m_poInst;
		HWND					m_poWnd;
		LPDIRECTINPUT8			m_poDXInput;
		LPDIRECTINPUTDEVICE8	m_poDXMouseDevice;
	};

	class WinInputDeviceKeyBoard : public InputDevice
	{
	public:
		WinInputDeviceKeyBoard(HINSTANCE p_poHInst, HWND p_poHWnd, WinInputManager* _poParent);
		~WinInputDeviceKeyBoard();

		virtual void Tick(f32 _fDeltaTime);

	private:
		WinInputManager*		m_poManager;

		HINSTANCE				m_poInst;
		HWND					m_poWnd;
		LPDIRECTINPUT8			m_poDXInput;
		LPDIRECTINPUTDEVICE8	m_poDXKBDevice;
	};
}

#endif