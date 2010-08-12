#include "TInput_WinInputDevice.h"
#include "TCore_Exception.h"
#include "TInput_WinInputManager.h"

namespace TsiU
{
	Key_t _Convert[256] = {
		E_INVALID_INPUT_ID,
		E_KB_ESCAPE,         
		E_KB_1,               
		E_KB_2,               
		E_KB_3,               
		E_KB_4,               
		E_KB_5,               
		E_KB_6,               
		E_KB_7,               
		E_KB_8,               
		E_KB_9,               
		E_KB_0,               
		E_KB_MINUS,       
		E_KB_EQUALS,     
		E_KB_BACK,        
		E_KB_TAB,           
		E_KB_Q,              
		E_KB_W,              
		E_KB_E,               
		E_KB_R,               
		E_KB_T,               
		E_KB_Y,               
		E_KB_U,               
		E_KB_I,               
		E_KB_O,              
		E_KB_P,               
		E_KB_LBRACKET, 
		E_KB_RBRACKET, 
		E_KB_RETURN,     
		E_KB_LCONTROL, 
		E_KB_A,               
		E_KB_S,               
		E_KB_D,              
		E_KB_F ,              
		E_KB_G,              
		E_KB_H,               
		E_KB_J ,              
		E_KB_K,               
		E_KB_L,               
		E_KB_SEMICOLON,   
		E_KB_APOSTROPHE, 
		E_KB_GRAVE,
		E_KB_LSHIFT,          
		E_KB_BACKSLASH,   
		E_KB_Z,               
		E_KB_X,               
		E_KB_C,               
		E_KB_V,               
		E_KB_B,               
		E_KB_N,               
		E_KB_M,               
		E_KB_COMMA,      
		E_KB_PERIOD,      
		E_KB_SLASH,       
		E_KB_RSHIFT,      
		E_KB_MULTIPLY ,  
		E_KB_LMENU,       
		E_KB_SPACE,       
		E_KB_CAPITAL,    
		E_KB_F1,              
		E_KB_F2,              
		E_KB_F3,              
		E_KB_F4,              
		E_KB_F5,              
		E_KB_F6,              
		E_KB_F7,              
		E_KB_F8,              
		E_KB_F9,              
		E_KB_F10,            
		E_KB_NUMLOCK ,  
		E_KB_SCROLL,     
		E_KB_NUMPAD7,   
		E_KB_NUMPAD8,   
		E_KB_NUMPAD9,   
		E_KB_SUBTRACT, 
		E_KB_NUMPAD4,   
		E_KB_NUMPAD5,   
		E_KB_NUMPAD6,   
		E_KB_ADD,          
		E_KB_NUMPAD1,       
		E_KB_NUMPAD2,       
		E_KB_NUMPAD3,       
		E_KB_NUMPAD0,       
		E_KB_DECIMAL,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_OEM_102,       
		E_KB_F11,             
		E_KB_F12,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_F13,             
		E_KB_F14,             
		E_KB_F15,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_KANA,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_ABNT_C1,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_CONVERT,
		E_INVALID_INPUT_ID,
		E_KB_NOCONVERT,
		E_INVALID_INPUT_ID,
		E_KB_YEN,            
		E_KB_ABNT_C2,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_NUMPADEQUALS,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_PREVTRACK,   
		E_KB_AT,              
		E_KB_COLON,          
		E_KB_UNDERLINE,    
		E_KB_KANJI,           
		E_KB_STOP,            
		E_KB_AX,              
		E_KB_UNLABELED,
		E_INVALID_INPUT_ID,
		E_KB_NEXTTRACK,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_NUMPADENTER,
		E_KB_RCONTROL,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_MUTE,            
		E_KB_CALCULATOR, 
		E_KB_PLAYPAUSE,
		E_INVALID_INPUT_ID,
		E_KB_MEDIASTOP,
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_VOLUMEDOWN,
		E_INVALID_INPUT_ID,
		E_KB_VOLUMEUP,
		E_INVALID_INPUT_ID,
		E_KB_WEBHOME,      
		E_KB_DIVIDE, 
		E_INVALID_INPUT_ID,
		E_KB_SYSRQ,           
		E_KB_RMENU, 
		E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,E_INVALID_INPUT_ID,
		E_KB_PAUSE,
		E_INVALID_INPUT_ID,
		E_KB_HOME,            
		E_KB_UP,              
		E_KB_PRIOR, 
		E_INVALID_INPUT_ID,
		E_KB_LEFT,           
		E_KB_RIGHT, 
		E_INVALID_INPUT_ID,
		E_KB_END,            
		E_KB_DOWN,        
		E_KB_NEXT,          
		E_KB_INSERT,       
		E_KB_DELETE
	};

	/************************************************************************/
	/*     WinMouse															*/
	/************************************************************************/

	WinInputDeviceMouse::WinInputDeviceMouse(HINSTANCE _poHInst, HWND _poHWnd, WinInputManager* _poParent)
		:m_poInst(_poHInst)
		,m_poWnd(_poHWnd)
		,m_poManager(_poParent)
	{
		m_eInputDeviceType = E_IDT_Mouse;

		//set mouse active for default
		SetActive(true);

		HRESULT l_HR;
		l_HR = DirectInput8Create(_poHInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_poDXInput, NULL);
		if(FAILED(l_HR))
			D_FatalError("DirectInputCreateEx failed");

		l_HR = m_poDXInput->CreateDevice(GUID_SysMouse, &m_poDXMouseDevice, NULL);
		if(FAILED(l_HR))
			D_FatalError("Mouse CreateDeviceEx failed");

		l_HR = m_poDXMouseDevice->SetDataFormat(&c_dfDIMouse2);
		if(FAILED(l_HR))
			D_FatalError("Mouse SetDataFormat failed");

		l_HR = m_poDXMouseDevice->SetCooperativeLevel(_poHWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
		if(FAILED(l_HR))
			D_FatalError("Mouse SetCooperativeLevel failed");

		DIPROPDWORD dipdw;
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = 16;

		l_HR = m_poDXMouseDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
		if(FAILED(l_HR))
			D_FatalError("SetProperty failed");

		l_HR = m_poDXMouseDevice->Acquire();
		if(FAILED(l_HR))
		{
			SetPlugged(false);
		}
		else
		{
			SetPlugged(true);
		}
	}
	WinInputDeviceMouse::~WinInputDeviceMouse()
	{
		D_SafeRelease(m_poDXInput);
		D_SafeRelease(m_poDXMouseDevice);
	}
	void WinInputDeviceMouse::Tick(f32 _fDeltaTime)
	{
		if(IsPlugged() == false)
		{
			HRESULT	hr = m_poDXMouseDevice->Acquire();
			if(FAILED(hr))
				return;
			else
				SetPlugged(true);
		}

		//InvalidateCursorRect(m_poWnd);
		Bool	bDone = false;
		HRESULT	hr;
		s16		x = 0, y = 0;
		while (!bDone) 
		{
			DIDEVICEOBJECTDATA od;
			DWORD dwElements = 1;   // number of items to be retrieved
			hr = m_poDXMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
			if(hr == DIERR_INPUTLOST) 
			{
				//SetAcquire();
				SetPlugged(false);
				break;
			}
			if( FAILED(hr) || dwElements == 0 ) 
			{
				//D_Output("[WinInputDeviceMouse::Tick] Can't get Data\n");
				break;
			}
			switch(od.dwOfs) 
			{
				// Mouse horizontal motion
				case DIMOFS_X:
					x += (s16)od.dwData;
					break;
				// Mouse vertical motion
				case DIMOFS_Y:
					y += (s16)od.dwData;
					break; 
				case DIMOFS_BUTTON0:
				case DIMOFS_BUTTON1:
				{
					m_poManager->PostButtonEvent((Key_t)(E_MOUSE_BUTTON1 + (od.dwOfs - DIMOFS_BUTTON0)),
												 od.dwData & 0x80 ? E_KS_Down : E_KS_Up);
					break;
				}
			}
		}
		m_poManager->PostMouseEvent(x, y);
		//InvalidateCursorRect(m_poWnd);
	}

	/************************************************************************/
	/*     WinKeyBoard														*/
	/************************************************************************/

	WinInputDeviceKeyBoard::WinInputDeviceKeyBoard(HINSTANCE _poHInst, HWND _poHWnd, WinInputManager* _poParent)
		:m_poInst(_poHInst)
		,m_poWnd(_poHWnd)
		,m_poManager(_poParent)
	{
		m_eInputDeviceType = E_IDT_KeyBoard;

		//set keyboard active for default
		SetActive(true);

		HRESULT l_HR;
		l_HR = DirectInput8Create(_poHInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_poDXInput, NULL);
		if(FAILED(l_HR))
			D_FatalError("DirectInputCreateEx failed");

		l_HR = m_poDXInput->CreateDevice(GUID_SysKeyboard, &m_poDXKBDevice, NULL);
		if(FAILED(l_HR))
			D_FatalError("KeyBoard CreateDeviceEx failed");

		l_HR = m_poDXKBDevice->SetDataFormat(&c_dfDIKeyboard);
		if(FAILED(l_HR))
			D_FatalError("KeyBoard SetDataFormat failed");

		l_HR = m_poDXKBDevice->SetCooperativeLevel(_poHWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
		if(FAILED(l_HR))
			D_FatalError("KeyBoard SetCooperativeLevel failed");

		l_HR = m_poDXKBDevice->Acquire();
		if(FAILED(l_HR))
		{
			SetPlugged(false);
		}
		else
		{
			SetPlugged(true);
		}
	}
	WinInputDeviceKeyBoard::~WinInputDeviceKeyBoard()
	{
		D_SafeRelease(m_poDXInput);
		D_SafeRelease(m_poDXKBDevice);
	}
	void WinInputDeviceKeyBoard::Tick(f32 _fDeltaTime)
	{
		if(IsPlugged() == false)
		{
			HRESULT	hr = m_poDXKBDevice->Acquire();
			if(FAILED(hr))
				return;
			else
				SetPlugged(true);
		}
		Char     buffer[256]; 
		HRESULT  hr; 
		hr = m_poDXKBDevice->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
		if(FAILED(hr))
		{ 
			if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) 
			{
				//SetAcquire();
				SetPlugged(false);
				return;
			}
			return; 
		} 
		for(s32 i = 1; i <= DIK_DELETE; i++)
		{
			m_poManager->PostButtonEvent(_Convert[i], buffer[i] & 0x80 ? E_KS_Down : E_KS_Up);
		}
	}
}