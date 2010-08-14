#include "GSBlock.h"

void GSBlock::Create()
{
}

void GSBlock::Draw()
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)g_poEngine->GetRenderMod()->GetRenderer();

	D_CHECK(m_uiCurrentState == EImageState_Back || m_uiCurrentState == EImageState_Front);
	WinGDIBitmap* curBmp = m_poImages[m_uiCurrentState];
	int width = m_uiWidth;
	int height = m_uiHeight;
	switch(m_uiControlFlag)
	{
	case EControlFlag_WidthPrefered:
		height = (int)((float)curBmp->GetHeight() / curBmp->GetWidth() * width);
		break;
	case EControlFlag_HeightPrefered:
		width = (int)((float)curBmp->GetWidth() / curBmp->GetHeight() * height);
		break;
	}
	curBmp->Draw(pRender, m_uiX, m_uiY, width, height);
}

void GSBlock::Tick(f32 _fDeltaTime)
{
}

void GSBlock::InitBlock(u32 _x, u32 _y, u32 _width, u32 _height, StringPtr _backImage, StringPtr _frontImage, u32 _controlFlag)
{
	m_uiX = _x;
	m_uiY = _y;

	m_uiWidth = _width;
	m_uiHeight = _height;
	
	m_uiControlFlag = _controlFlag;
	m_uiCurrentState = EImageState_Front;

	WinGDIRenderer* pRender = (WinGDIRenderer*)g_poEngine->GetRenderMod()->GetRenderer();

	m_poImages[EImageState_Back]	= new WinGDIBitmap(pRender, _backImage);
	m_poImages[EImageState_Front]	= new WinGDIBitmap(pRender, _frontImage);
}