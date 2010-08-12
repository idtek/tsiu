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

	LRESULT CALLBACK WinRenderWindowMsg::WndProc(HWND p_poHwnd, UINT p_uiMsg, WPARAM p_wParam, LPARAM p_lParam)
	{
		HDC hdc;
		PAINTSTRUCT ps;

		switch(p_uiMsg)             
		{
		case WM_DESTROY:
			PostQuitMessage (0);
			break;

		case WM_PAINT:
			hdc = BeginPaint(p_poHwnd, &ps);
			EndPaint(p_poHwnd, &ps);	
			break;
		/*case WM_LBUTTONDBLCLK:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnMouseLDBClick(((int)(short)LOWORD(p_lParam)), ((int)(short)HIWORD(p_lParam)));
			break;
		case WM_LBUTTONDOWN:
			LastMousePosX = (int)(short)LOWORD(p_lParam);
			LastMousePosY = (int)(short)HIWORD(p_lParam);
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnMouseLDown(((int)(short)LOWORD(p_lParam)), ((int)(short)HIWORD(p_lParam)));
			break;
		case WM_LBUTTONUP:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnMouseLUp(((int)(short)LOWORD(p_lParam)), ((int)(short)HIWORD(p_lParam)));
			break;
		case WM_MOUSEMOVE:
			if(p_wParam & MK_LBUTTON)
			{
				for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
					s_poMsgCallBack[i]->OnMouseLDrag(((int)(short)LOWORD(p_lParam)), ((int)(short)HIWORD(p_lParam)), ((int)(short)LOWORD(p_lParam)) - LastMousePosX, ((int)(short)HIWORD(p_lParam)) - LastMousePosY);
				LastMousePosX = (int)(short)LOWORD(p_lParam);
				LastMousePosY = (int)(short)HIWORD(p_lParam);
			}
			break;*/
		case WM_SIZE:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnWindowResize(((int)(short)LOWORD(p_lParam)), ((int)(short)HIWORD(p_lParam)));
			break;
		/*case WM_KEYDOWN:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyDown(p_wParam, p_lParam);
			break;
		case WM_SYSKEYDOWN:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyDown(p_wParam, p_lParam);
			break;
		case WM_KEYUP:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyUp(p_wParam, p_lParam);
			break;
		case WM_SYSKEYUP:
			for(s32 i = 0; i < s_poMsgCallBack.Size(); ++i)
				s_poMsgCallBack[i]->OnKeyUp(p_wParam, p_lParam);
			break;*/
		default:            
			return DefWindowProc(p_poHwnd, p_uiMsg, p_wParam, p_lParam);
		}
		return 0;
	}
}