#include "TRender_WinDxTexture.h"
#include "TRender_WinDxRenderer.h"

#ifndef TLIB_NOWINDX
namespace TsiU
{
	WinDxTexture::~WinDxTexture()
	{
		D_SafeRelease(m_poHandler);
	}
	void WinDxTexture::Load(const Char* _strFilename)
	{
		WinDxRenderer * poRender = static_cast<WinDxRenderer*>(m_poRenderer);
		LPDIRECT3DDEVICE9 poDevice = poRender->GetActiveDevice();
		HRESULT hr = D3DXCreateTextureFromFile(poDevice, _strFilename, &m_poHandler);
		if(FAILED(hr))
		{
			m_poHandler = NULL;
			return;
		}
	}
}
#endif