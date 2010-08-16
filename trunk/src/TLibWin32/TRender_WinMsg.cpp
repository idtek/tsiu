#include "TRender_WinMsg.h"

namespace TsiU
{
	s32 WinRenderWindowMsg::LastMousePosX = 0;
	s32 WinRenderWindowMsg::LastMousePosY = 0;

	Bool WinRenderWindowMsg::MessageBump()
	{
		MSG  msg;
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			if(msg.message == WM_QUIT)
			{
				return false;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );

			return true;
		}
		return true;
	}

	LRESULT CALLBACK WinRenderWindowMsg::WndProc(HWND _poHwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
	{
		HDC hdc;
		PAINTSTRUCT ps;

		switch(_uiMsg)             
		{
		case WM_DESTROY:
			PostQuitMessage (0);
			break;
		case WM_PAINT:
			hdc = BeginPaint(_poHwnd, &ps);
			EndPaint(_poHwnd, &ps);	
			break;
		case WM_LBUTTONDBLCLK:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnMouseLDBClick(((int)(short)LOWORD(_lParam)), ((int)(short)HIWORD(_lParam)));
			break;
		case WM_LBUTTONDOWN:
			LastMousePosX = (int)(short)LOWORD(_lParam);
			LastMousePosY = (int)(short)HIWORD(_lParam);
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnMouseLDown(((int)(short)LOWORD(_lParam)), ((int)(short)HIWORD(_lParam)));
			break;
		case WM_LBUTTONUP:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnMouseLUp(((int)(short)LOWORD(_lParam)), ((int)(short)HIWORD(_lParam)));
			break;
		case WM_MOUSEMOVE:
			if(_wParam & MK_LBUTTON)
			{
				for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
					s_poMsgCallBack[i]->OnMouseLDrag(((int)(short)LOWORD(_lParam)), ((int)(short)HIWORD(_lParam)), ((int)(short)LOWORD(_lParam)) - LastMousePosX, ((int)(short)HIWORD(_lParam)) - LastMousePosY);
				LastMousePosX = (int)(short)LOWORD(_lParam);
				LastMousePosY = (int)(short)HIWORD(_lParam);
			}
			break;
		case WM_SIZE:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnWindowResize(((int)(short)LOWORD(_lParam)), ((int)(short)HIWORD(_lParam)));
			break;
		/*case WM_KEYDOWN:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyDown(_wParam, _lParam);
			break;
		case WM_SYSKEYDOWN:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyDown(_wParam, _lParam);
			break;
		case WM_KEYUP:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyUp(_wParam, _lParam);
			break;
		case WM_SYSKEYUP:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyUp(_wParam, _lParam);
			break;*/
		default:            
			return DefWindowProc(_poHwnd, _uiMsg, _wParam, _lParam);
		}
		return 0;
	}
}