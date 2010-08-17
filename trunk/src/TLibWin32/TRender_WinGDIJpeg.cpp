#include "TRender_WinGDIJpeg.h"
#include "TRender_WinGDIRenderer.h"

#undef FAR
extern "C" {
	/* Theo Veenker <Theo.Veenker@let.uu.nl> says this is needed for CYGWIN */
#if (defined(__CYGWIN__) || defined(__MINGW32__) || defined(_MSC_VER)) && !defined(XMD_H)
#define XMD_H
	typedef short INT16;
	typedef int INT32;
#include "jpeglib.h"
#undef XMD_H
#elif defined __WINE__
#define XMD_H
#include "jpeglib.h"
#else
#include "jpeglib.h"
#endif
}

namespace TsiU
{
	WinGDIJpeg::WinGDIJpeg()
		: m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
	{
	}

	WinGDIJpeg::WinGDIJpeg(WinGDIRenderer* _poRenderer, StringPtr szFileName)
		: m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
	{
		Create(_poRenderer, szFileName);
	}

	WinGDIJpeg::~WinGDIJpeg()
	{
		Free();
	}

	void WinGDIJpeg::Free()
	{
		if (m_hBitmap != NULL)
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
	}

	Bool WinGDIJpeg::Create(WinGDIRenderer* _poRenderer, StringPtr szFileName)
	{
		Free();

		FILE* file = fopen(szFileName, "rb");
		D_CHECK(file);

		jpeg_error_mgr jerr;
		jpeg_decompress_struct cinfo;
		
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, file);
		jpeg_read_header(&cinfo, true);
		jpeg_start_decompress(&cinfo);
		
		m_iWidth = cinfo.output_width;
		m_iHeight = cinfo.output_height;

		u32 *bmpBuffer = new u32[m_iHeight * m_iWidth];
		JSAMPLE *buffer[1];
		buffer[0]= new JSAMPLE[sizeof(JSAMPLE) * cinfo.output_width * cinfo.output_components];

		register u32 *pp;
		register JSAMPLE *qq;

		pp = bmpBuffer;
		while(cinfo.output_scanline < cinfo.output_height) 
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			qq = buffer[0];
			for(s32 i = 0; i< m_iWidth; ++i, ++pp){
				((u8*)pp)[0] = *qq++;
				((u8*)pp)[1] = *qq++;
				((u8*)pp)[2] = *qq++;
				((u8*)pp)[3] = 0xff;
			}
		}       
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		delete[] buffer[0];
		fclose(file);

		register s32 bytes_per_line,skip,h,w;
		register u8 *src,*dst;
		BITMAPINFO bmi;
		u8 *pixels;

		// Set up the bitmap info
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_iWidth;
		bmi.bmiHeader.biHeight = m_iHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;

		// DIB format pads to multiples of 4 bytes...
		bytes_per_line = (m_iWidth * 3 + 3 ) & ~3;
		pixels = (u8*)VirtualAlloc(0, bytes_per_line * m_iHeight, MEM_COMMIT, PAGE_READWRITE);
		D_CHECK(pixels)
		skip = -bytes_per_line - m_iWidth * 3;
		src = (u8*)bmpBuffer;
		dst = pixels + m_iHeight * bytes_per_line + m_iWidth * 3;
		h = m_iHeight;
		do{
			dst += skip;
			w = m_iWidth;
			do{
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				src += 4;
				dst += 3;
			}
			while(--w);
		}
		while(--h);

		HDC hDC = _poRenderer->GetOffscreenDC();
		m_hBitmap = CreateCompatibleBitmap(hDC, Math::Max(m_iWidth,1), Math::Max(m_iHeight, 1));
		if(!SetDIBits(hDC, m_hBitmap, 0, m_iHeight, pixels, &bmi, DIB_RGB_COLORS))
		{
			return false;
		}
		VirtualFree(pixels, 0, MEM_RELEASE);
		return true;
	}

	void WinGDIJpeg::Draw(WinGDIRenderer* _poRenderer, s32 x, s32 y, s32 width, s32 height)
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
}