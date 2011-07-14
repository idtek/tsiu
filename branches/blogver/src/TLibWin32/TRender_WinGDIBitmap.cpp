#include "TRender_WinGDIBitmap.h"
#include "TRender_WinGDIMapping.h"
#include "TRender_WinGDIRenderer.h"

namespace TsiU
{
	WinGDIBitmap::WinGDIBitmap()
		: m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
	{
	}

	WinGDIBitmap::WinGDIBitmap(WinGDIRenderer* _poRenderer, StringPtr szFileName)
		: m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
	{
		Create(_poRenderer, szFileName);
	}

	WinGDIBitmap::WinGDIBitmap(WinGDIRenderer* _poRenderer, u32 uiResID, HINSTANCE hInstance)
		: m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
	{
		Create(_poRenderer, uiResID, hInstance);
	}

	WinGDIBitmap::WinGDIBitmap(WinGDIRenderer* _poRenderer, s32 iWidth, s32 iHeight, D_Color crColor)
		: m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
	{
		Create(_poRenderer, iWidth, iHeight, crColor);
	}

	WinGDIBitmap::~WinGDIBitmap()
	{
		Free();
	}

	void WinGDIBitmap::Free()
	{
		if (m_hBitmap != NULL)
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
	}

	Bool WinGDIBitmap::Create(WinGDIRenderer* _poRenderer, StringPtr szFileName)
	{
		Free();

		HDC hDC = _poRenderer->GetOffscreenDC();

		HANDLE hFile = CreateFile(szFileName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;

		BITMAPFILEHEADER  bmfHeader;
		DWORD             dwBytesRead;
		BOOL bOK = ReadFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER),&dwBytesRead, NULL);
		if((!bOK) || (dwBytesRead != sizeof(BITMAPFILEHEADER)) ||
			(bmfHeader.bfType != 0x4D42))
		{
			CloseHandle(hFile);
			return false;
		}

		BITMAPINFO* pBitmapInfo = (new BITMAPINFO);
		if (pBitmapInfo != NULL)
		{

			bOK = ReadFile(hFile, pBitmapInfo, sizeof(BITMAPINFOHEADER),&dwBytesRead, NULL);
			if((!bOK) || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
			{
				CloseHandle(hFile);
				Free();
				return false;
			}

			m_iWidth = (s32)pBitmapInfo->bmiHeader.biWidth;
			m_iHeight = (s32)pBitmapInfo->bmiHeader.biHeight;

			PBYTE pBitmapBits;
			m_hBitmap = CreateDIBSection(hDC, pBitmapInfo, DIB_RGB_COLORS,
				(PVOID*)&pBitmapBits, NULL, 0);
			if((m_hBitmap != NULL) && (pBitmapBits != NULL))
			{
				SetFilePointer(hFile, bmfHeader.bfOffBits, NULL, FILE_BEGIN);
				bOK = ReadFile(hFile, pBitmapBits, pBitmapInfo->bmiHeader.biSizeImage,
					&dwBytesRead, NULL);
				if (bOK)
					return true;
			}
		}
		Free();
		return false;
	}

	Bool WinGDIBitmap::Create(WinGDIRenderer* _poRenderer, u32 uiResID, HINSTANCE hInstance)
	{
		Free();

		HDC hDC = _poRenderer->GetOffscreenDC();

		HRSRC hResInfo = FindResource(hInstance, MAKEINTRESOURCE(uiResID), RT_BITMAP);
		if(hResInfo == NULL)
			return false;

		HGLOBAL hMemBitmap = LoadResource(hInstance, hResInfo);
		if(hMemBitmap == NULL)
			return false;

		PBYTE pBitmapImage = (BYTE*)LockResource(hMemBitmap);
		if(pBitmapImage == NULL)
		{
			FreeResource(hMemBitmap);
			return false;
		}

		BITMAPINFO* pBitmapInfo = (BITMAPINFO*)pBitmapImage;
		m_iWidth = (s32)pBitmapInfo->bmiHeader.biWidth;
		m_iHeight = (s32)pBitmapInfo->bmiHeader.biHeight;

		PBYTE pBitmapBits;
		m_hBitmap = CreateDIBSection(hDC, pBitmapInfo, DIB_RGB_COLORS,
			(PVOID*)&pBitmapBits, NULL, 0);
		if((m_hBitmap != NULL) && (pBitmapBits != NULL))
		{
			const PBYTE pTempBits = pBitmapImage + pBitmapInfo->bmiHeader.biSize +
				pBitmapInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD);
			CopyMemory(pBitmapBits, pTempBits, pBitmapInfo->bmiHeader.biSizeImage);

			UnlockResource(hMemBitmap);
			FreeResource(hMemBitmap);
			return true;
		}

		UnlockResource(hMemBitmap);
		FreeResource(hMemBitmap);
		Free();
		return false;
	}

	Bool WinGDIBitmap::Create(WinGDIRenderer* _poRenderer, s32 iWidth, s32 iHeight, D_Color crColor)
	{
		HDC hDC = _poRenderer->GetOffscreenDC();

		m_hBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
		if(m_hBitmap == NULL)
			return false;

		m_iWidth = iWidth;
		m_iHeight = iHeight;

		HDC hMemDC = CreateCompatibleDC(hDC);
		HBRUSH hBrush = CreateSolidBrush(WinGDIMapping::MappingColor(crColor));

		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

		RECT rcBitmap = {0, 0, m_iWidth, m_iHeight};
		FillRect(hMemDC, &rcBitmap, hBrush);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		DeleteObject(hBrush);

		return true;
	}

	void WinGDIBitmap::Draw(WinGDIRenderer* _poRenderer, s32 x, s32 y, Bool bTrans, D_Color crTransColor)
	{
		DrawPart(_poRenderer,x,y,0,0,GetWidth(),GetHeight(),bTrans,crTransColor);
	}

	void WinGDIBitmap::Draw(WinGDIRenderer* _poRenderer, s32 x, s32 y, s32 width, s32 height, Bool bTrans, D_Color crTransColor)
	{
		if (m_hBitmap != NULL)
		{
			HDC hDC = _poRenderer->GetOffscreenDC();

			HDC hMemDC = CreateCompatibleDC(hDC);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

			SetStretchBltMode(hDC, COLORONCOLOR);
			StretchBlt(hDC, x, y, width, height, hMemDC, 0, 0, GetWidth(), GetHeight(), SRCCOPY);

			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}

	void WinGDIBitmap::DrawPart(WinGDIRenderer* _poRenderer, s32 x,s32 y, s32 xPart,s32 yPart, s32 wPart,s32 hPart, Bool bTrans, D_Color crTransColor)
	{
		if (m_hBitmap != NULL)
		{
			HDC hDC = _poRenderer->GetOffscreenDC();

			HDC hMemDC = CreateCompatibleDC(hDC);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

			if (bTrans)
				TransparentBlt(hDC, x, y, wPart, hPart, hMemDC, xPart, yPart,
				wPart, hPart, WinGDIMapping::MappingColor(crTransColor));
			else
				BitBlt(hDC, x, y, wPart, hPart, hMemDC, xPart, yPart, SRCCOPY);

			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
}