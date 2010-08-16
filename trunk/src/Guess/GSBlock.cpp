#include "GSBlock.h"

void GSBlock::Create()
{
}

void GSBlock::Draw()
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();

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

void GSBlock::OpUpdateBlock(const Event* _poEvent)
{
	m_uiX = _poEvent->GetParam<u32>(0);
	m_uiY = _poEvent->GetParam<u32>(1);
	m_uiWidth = _poEvent->GetParam<u32>(2);
	m_uiHeight = _poEvent->GetParam<u32>(3);
}

void GSBlock::InitBlock(u32 _x, u32 _y, u32 _width, u32 _height, StringPtr _backImage, StringPtr _frontImage, u32 _controlFlag)
{
	m_uiX = _x;
	m_uiY = _y;

	m_uiWidth = _width;
	m_uiHeight = _height;
	
	m_uiControlFlag = _controlFlag;
	m_uiCurrentState = EImageState_Front;

	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();

	m_poImages[EImageState_Back]	= new WinGDIBitmap(pRender, _backImage);
	m_poImages[EImageState_Front]	= new WinGDIBitmap(pRender, _frontImage);
}

//--------------------------------------------------------------------------------------------------------------
void GSBlockManager::Init(s32 _wWidth, s32 _wHeight)
{
	s32 blockWidth  = _wWidth / kColumnCount;
	s32 blockHeight = _wHeight / kRowCount;

	static const ConstPicInfo kPicList[] = {
		ConstPicInfo("back.bmp", "1.bmp"), 
		ConstPicInfo("back.bmp", "2.bmp"),
		ConstPicInfo("back.bmp", "3.bmp"),
		ConstPicInfo("back.bmp", "4.bmp"),
		ConstPicInfo("back.bmp", "5.bmp"),
		ConstPicInfo("back.bmp", "6.bmp"),
		ConstPicInfo("back.bmp", "7.bmp"),
		ConstPicInfo("back.bmp", "8.bmp"),
		ConstPicInfo("back.bmp", "9.bmp")
	};

	GSBlock* newBlock = NULL;
	for(s32 i = 0; i < kRowCount; ++i)
	{
		for(s32 j = 0; j < kColumnCount; ++j)
		{
			Point2<u32> upperLeft = _XYToUpperLeft(blockWidth, blockHeight, i, j);

			s32 flatIndex = _XYToIndex(i, j);
			newBlock = new GSBlock;
			newBlock->InitBlock(upperLeft.X(), 
								upperLeft.Y(), 
								blockWidth, 
								blockHeight, 
								kPicList[flatIndex].m_BackPicName.c_str(), 
								kPicList[flatIndex].m_FrontPicName.c_str(), 
								GSBlock::EControlFlag_FreeSize);
			m_BlockMap[i][j] = newBlock;

			//Add to engine
			char objName[64];
			sprintf(objName, "Image%d", flatIndex);
			GameEngine::GetGameEngine()->GetSceneMod()->AddObject(objName, newBlock);

			//Register event handler
			GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler((EventType_t)(GameEngine::E_ET_UpdateBlock1 + flatIndex), 
																	     new MEventHandler<GSBlock>(newBlock, &GSBlock::OpUpdateBlock));
			
		}
	}
}

void GSBlockManager::OnResizeWindow(s32 _wWidth, s32 _wHeight)
{
	int blockWidth  = _wWidth / kColumnCount;
	int blockHeight = _wHeight / kRowCount; 

	for(int i = 0; i < kRowCount; ++i)
	{
		for(int j = 0; j < kColumnCount; ++j)
		{
			Point2<u32> upperLeft = _XYToUpperLeft(blockWidth, blockHeight, i, j);

			s32 flatIndex = _XYToIndex(i, j);
			Event evt((EventType_t)(GameEngine::E_ET_UpdateBlock1 + flatIndex));
			evt.AddParam(upperLeft.X())
			   .AddParam(upperLeft.Y())
			   .AddParam(blockWidth)
			   .AddParam(blockHeight);
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
		}
	}
}