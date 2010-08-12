#include "TRender_WinDxUITexture.h"

#include "TRender_WinDxRenderer.h"


namespace TsiU
{
	void WinDxUITexture::Load(const char *fileName)
	{
		WinDxRenderer * pRender = static_cast<WinDxRenderer *>( mpRenderer);
		LPDIRECT3DDEVICE9 device = pRender->GetActiveDevice();
		HRESULT result = D3DXCreateTextureFromFile(device, fileName, &mHandler);
		if(D3D_OK != result)
		{
			mHandler = NULL;
		}
	}
}