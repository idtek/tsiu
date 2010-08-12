#include "TInput_WinInputManager.h"
#include "TRender_WinRenderWindow.h"
#include "TEngine_Private.h"
#include "TInput_WinInputDevice.h"
#include "TRender_Renderer.h"
#include "TEvent_EventObject.h"
#include "TEvent_EventID.h"
#include "TEngine_EventModule.h"
#include "TEngine_RenderModule.h"
#include "TCore_LibSettings.h"

namespace TsiU
{
	WinInputManager::WinInputManager()
		:InputManager()
	{
		if(!GetLibSettings()->IsDefined(E_LS_Has_GUI))
		{
			WinRenderWindow* pMainWind = (WinRenderWindow*)(poGetEngine()->GetRenderMod()->GetRenderer()->GetMainWindow());

			m_DeviceList.PushBack(new WinInputDeviceMouse(pMainWind->GetHINST(), pMainWind->GetHWND(), this));
			m_DeviceList.PushBack(new WinInputDeviceKeyBoard(pMainWind->GetHINST(), pMainWind->GetHWND(), this));
		}
		//else
		//{
		//	//todo
		//	FXMainWindow *poMain = poGetEngine()->GetRenderMod()->GetGUIMainWindow();

		//	m_DeviceList.PushBack(new WinInputDeviceMouse(pMainWind->GetHINST(), (HWND)poMain->id(), this));
		//	m_DeviceList.PushBack(new WinInputDeviceKeyBoard(pMainWind->GetHINST(), (HWND)poMain->id(), this));
		//}
	}

	void WinInputManager::PostMouseEvent(s16 _x, s16 _y)
	{
		//TJQ: We may add some filter for mouse later
		Event evtX(E_ET_Input, E_EST_Input_Mouse);
		evtX.AddParam<u32>((u32)E_MOUSE_X).AddParam<s32>((s32)_x);
		poGetEngine()->GetEventMod()->PostEvent(&evtX);

		Event evtY(E_ET_Input, E_EST_Input_Mouse);
		evtY.AddParam<u32>((u32)E_MOUSE_Y).AddParam<s32>((s32)_y);
		poGetEngine()->GetEventMod()->PostEvent(&evtY);
	}

	void WinInputManager::PostButtonEvent(Key_t _key, KeyState_t _ks)
	{
		if(_key == E_INVALID_INPUT_ID || _key >= E_INPUT_ID_NUM)
			return;

		if(m_KeyState[_key] != _ks)
		{
			m_KeyState[_key] = _ks;
			Event evt(E_ET_Input);

			if(_key > E_INVALID_INPUT_ID && _key < E_JOYSTK_ENDITEMS)
				evt.SetEventSubType(E_EST_Input_Pad);
			else if(_key > E_JOYSTK_ENDITEMS && _key < E_MOUSE_ENDITEMS)
				evt.SetEventSubType(E_EST_Input_Mouse);
			else
				evt.SetEventSubType(E_EST_Input_KeyBoard);

			evt.AddParam<u32>((u32)_key).AddParam<u32>((u32)m_KeyState[_key]);
			poGetEngine()->GetEventMod()->PostEvent(&evt);
		}
	}
}