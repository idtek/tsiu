#include "TRender_WinDxRenderWindow.h"
#include "TCore_Exception.h"
#include "TRender_WinMsg.h"
#include "TRender_WinDxDevice.h"

#ifndef TLIB_NOWINDX
namespace TsiU
{
	WinDxRenderWindow::WinDxRenderWindow()
		:WinRenderWindow()
	{
		m_poDevice = new WinDxDevice();
	}

	WinDxRenderWindow::~WinDxRenderWindow()
	{
		D_SafeDelete(m_poDevice);
	}
	
	void WinDxRenderWindow::InitWindow(u32 p_uiWidth, u32 p_uiHeight, const Char* p_strTitle, Bool p_bIsWindowed, void* p_poOuter)
	{
		m_uiWidth		= p_uiWidth;
		m_uiHeight		= p_uiHeight;
		m_bIsWindowed	= p_bIsWindowed;

		if(!p_poOuter)
		{
			WNDCLASSEX l_Wincl;

			/* The Window structure */
			l_Wincl.hInstance = m_poInst;
			l_Wincl.lpszClassName = p_strTitle;
			l_Wincl.lpfnWndProc = WinRenderWindowMsg::WndProc;
			l_Wincl.style = CS_DBLCLKS;
			l_Wincl.cbSize = sizeof(WNDCLASSEX);

			l_Wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
			l_Wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
			l_Wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
			l_Wincl.lpszMenuName = NULL;
			l_Wincl.cbClsExtra = 0;
			l_Wincl.cbWndExtra = 0; 
			l_Wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

			if( !::RegisterClassEx(&l_Wincl) )
			{			
				D_FatalError("Register Window Error");
			}
			m_poHwnd = ::CreateWindowEx(0, p_strTitle, p_strTitle, 
				WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0,
				m_uiWidth, m_uiHeight, NULL, NULL, m_poInst, NULL );
			::ShowWindow(m_poHwnd, SW_SHOW);
			::UpdateWindow(m_poHwnd);
		}
		else
		{
			m_poHwnd = (HWND)p_poOuter;
		}

		InitD3D();
	}

	void WinDxRenderWindow::InitD3D()
	{
		LPDIRECT3D9 l_poD3D = m_poDevice->GetD3D();

		if( NULL == ( l_poD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		{
			D_FatalError("Direct3DCreate9 failed");
		}

		D3DPRESENT_PARAMETERS l_D3Dpp;

		ZeroMemory( &l_D3Dpp, sizeof(l_D3Dpp) );
		l_D3Dpp.BackBufferWidth            = m_uiWidth;
		l_D3Dpp.BackBufferHeight           = m_uiHeight;
		l_D3Dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
		l_D3Dpp.BackBufferCount            = 1;
		l_D3Dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
		l_D3Dpp.MultiSampleQuality         = 0;
		l_D3Dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
		l_D3Dpp.hDeviceWindow              = m_poHwnd;
		l_D3Dpp.Windowed                   = m_bIsWindowed;
		l_D3Dpp.EnableAutoDepthStencil     = true; 
		l_D3Dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
		l_D3Dpp.Flags                      = 0;
		l_D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		l_D3Dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;

		D3DCAPS9 l_Caps;
		l_poD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &l_Caps);

		int l_iVp = 0;
		if( l_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			l_iVp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			l_iVp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		LPDIRECT3DDEVICE9 l_poD3DDevice = m_poDevice->GetD3DDevice();

		HRESULT l_HR = l_poD3D->CreateDevice(D3DADAPTER_DEFAULT, 
					D3DDEVTYPE_HAL, m_poHwnd, l_iVp, &l_D3Dpp, &l_poD3DDevice);

		if( FAILED(l_HR) )
		{
			// try again using a 16-bit depth buffer
			l_D3Dpp.AutoDepthStencilFormat = D3DFMT_D16;

			l_HR = l_poD3D->CreateDevice( D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL, m_poHwnd, l_iVp, &l_D3Dpp, &l_poD3DDevice);
			if( FAILED(l_HR) )
			{
				D_FatalError("CreateDevice failed");
			}
		}
		m_poDevice->SetD3DDevice(l_poD3DDevice);
	}
}
#endif