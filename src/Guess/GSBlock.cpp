#include "GSBlock.h"

//-----------------------------------------------------------------------
GSEffectController::KeyFrame::FrameInfo GSEffectController::KeyFrame::FrameInfo::Blend(
	const GSEffectController::KeyFrame::FrameInfo& frame1, 
	const GSEffectController::KeyFrame::FrameInfo& frame2, 
	f32 coef)
{
	D_CHECK(coef >= 0.f && coef <= 1.f);
	FrameInfo ret;
	ret.m_UpperLeft.X(frame1.m_UpperLeft.X() * (1.f - coef) + frame2.m_UpperLeft.X() * coef);
	ret.m_UpperLeft.Y(frame1.m_UpperLeft.Y() * (1.f - coef) + frame2.m_UpperLeft.Y() * coef);
	ret.m_Width		= frame1.m_Width * (1.f - coef) + frame2.m_Width * coef;
	ret.m_Height	= frame1.m_Height * (1.f - coef) + frame2.m_Height * coef;

	return ret;
}

GSEffectController::GSEffectController()
	:m_HasStarted(false)
	,m_CurTime(0.f)
{

}
void GSEffectController::AddFrame(const KeyFrame& frame)
{
	m_Frames.PushBack(frame);
}

const GSEffectController::KeyFrame::FrameInfo& GSEffectController::GetCurFrame() const
{
	return m_CurFrame;
}

void GSEffectController::Start()
{
	m_HasStarted = true;
	m_CurTime = 0.f;
}

void GSEffectController::Clear()
{
	m_Frames.Clear();
}

void GSEffectController::Tick(f32 _fDeltaTime)
{
	s32 frame1 = -1, frame2 = -1;

	if(m_HasStarted)
	{
		m_CurTime += _fDeltaTime;
		if(m_Frames.Size() > 0)
		{
			if(m_Frames.Size() == 1)
			{
				frame1 = frame2 = 0;
			}
			else
			{
				for(s32 i = 0; i < m_Frames.Size(); ++i)
				{
					const KeyFrame& frame = m_Frames[i];
					if(frame.m_Time >= m_CurTime)
					{
						frame2 = i;
						break;
					}
				}
				if(frame2 < 0)
					frame1 = frame2 = m_Frames.Size() - 1;
				else
					frame1 = Math::Max<s32>(0, frame2 - 1);
			}
			f32 frame1Time = m_Frames[frame1].m_Time;
			f32 frame2Time = m_Frames[frame2].m_Time;
			f32 coef = 0;
			if(!Math::IsEqual(frame1Time, frame2Time))
			{
				D_CHECK(frame2Time > frame1Time);
				coef = (m_CurTime - frame1Time) / (frame2Time - frame1Time);
			}
			m_CurFrame = KeyFrame::FrameInfo::Blend(m_Frames[frame1].m_FrameInfo, m_Frames[frame2].m_FrameInfo, coef);
		}
	}
	if(frame1 < 0)
		m_HasStarted = false;
}

//-----------------------------------------------------------------------
void GSBlock::Create()
{
}

void GSBlock::Draw()
{
	const GSEffectController::KeyFrame::FrameInfo& info = m_EffectController.GetCurFrame();
	m_uiX = info.m_UpperLeft.X();
	m_uiY = info.m_UpperLeft.Y();
	m_uiWidth = info.m_Width;
	m_uiHeight = info.m_Height;

	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();

	D_CHECK(m_uiCurrentState == EImageState_Back || m_uiCurrentState == EImageState_Front);
	WinGDIJpeg* curBmp = m_poImages[m_uiCurrentState];
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
	m_EffectController.Tick(_fDeltaTime);
}

void GSBlock::OpUpdateBlock(const Event* _poEvent)
{
	m_uiX = _poEvent->GetParam<u32>(0);
	m_uiY = _poEvent->GetParam<u32>(1);
	m_uiWidth = _poEvent->GetParam<u32>(2);
	m_uiHeight = _poEvent->GetParam<u32>(3);
}

void GSBlock::OpClickBlock()
{
	//m_uiCurrentState == EImageState_Front ? 
	//	m_uiCurrentState = EImageState_Back :
	//	m_uiCurrentState = EImageState_Front;

	m_EffectController.Clear();

	GSEffectController::KeyFrame kf;
	kf.m_FrameInfo.m_UpperLeft.X(m_uiX);
	kf.m_FrameInfo.m_UpperLeft.Y(m_uiY);
	kf.m_FrameInfo.m_Width = m_uiWidth;
	kf.m_FrameInfo.m_Height = m_uiHeight;
	kf.m_Time = 0.f;
	m_EffectController.AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(0);
	kf.m_FrameInfo.m_UpperLeft.Y(0);
	kf.m_FrameInfo.m_Width = 900;
	kf.m_FrameInfo.m_Height = 675;
	kf.m_Time = 3.f;
	m_EffectController.AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(0);
	kf.m_FrameInfo.m_UpperLeft.Y(0);
	kf.m_FrameInfo.m_Width = 900;
	kf.m_FrameInfo.m_Height = 675;
	kf.m_Time = 6.f;
	m_EffectController.AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(m_uiX);
	kf.m_FrameInfo.m_UpperLeft.Y(m_uiY);
	kf.m_FrameInfo.m_Width = m_uiWidth;
	kf.m_FrameInfo.m_Height = m_uiHeight;
	kf.m_Time = 9.f;
	m_EffectController.AddFrame(kf);

	m_uiCurrentState = EImageState_Front;

	m_EffectController.Start();
}

void GSBlock::InitBlock(u32 _x, u32 _y, u32 _width, u32 _height, StringPtr _backImage, StringPtr _frontImage, u32 _controlFlag)
{
	m_uiX = _x;
	m_uiY = _y;

	m_uiWidth = _width;
	m_uiHeight = _height;

	GSEffectController::KeyFrame kf;
	kf.m_FrameInfo.m_UpperLeft.X(m_uiX);
	kf.m_FrameInfo.m_UpperLeft.Y(m_uiY);
	kf.m_FrameInfo.m_Width = m_uiWidth;
	kf.m_FrameInfo.m_Height = m_uiHeight;
	kf.m_Time = 0.f;
	m_EffectController.AddFrame(kf);
	m_EffectController.Start();
	
	m_uiControlFlag = _controlFlag;
	m_uiCurrentState = EImageState_Back;

	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();

	m_poImages[EImageState_Back]	= new WinGDIJpeg(pRender, _backImage);
	m_poImages[EImageState_Front]	= new WinGDIJpeg(pRender, _frontImage);
}

//--------------------------------------------------------------------------------------------------------------
GSBlockManager::GSBlockManager()
{
	memset(m_BlockMap, 0, sizeof(GSBlock*) * kColumnCount * kRowCount);
}
void GSBlockManager::Init(s32 _wWidth, s32 _wHeight)
{
	m_BlockWidth  = _wWidth / kColumnCount;
	m_BlockHeight = _wHeight / kRowCount; 

	static const ConstPicInfo kPicList[] = {
		ConstPicInfo("back.jpg", "1.jpg"), 
		ConstPicInfo("back.jpg", "2.jpg"),
		ConstPicInfo("back.jpg", "3.jpg"),
		ConstPicInfo("back.jpg", "4.jpg"),
		ConstPicInfo("back.jpg", "1.jpg"),
		ConstPicInfo("back.jpg", "2.jpg"),
		ConstPicInfo("back.jpg", "3.jpg"),
		ConstPicInfo("back.jpg", "4.jpg"),
		ConstPicInfo("back.jpg", "1.jpg")
	};

	GSBlock* newBlock = NULL;
	for(s32 i = 0; i < kRowCount; ++i)
	{
		for(s32 j = 0; j < kColumnCount; ++j)
		{
			Point2<u32> upperLeft = _XYToUpperLeft(m_BlockWidth, m_BlockHeight, i, j);

			s32 flatIndex = _XYToIndex(i, j);
			newBlock = new GSBlock;
			newBlock->InitBlock(upperLeft.X(), 
								upperLeft.Y(), 
								m_BlockWidth, 
								m_BlockHeight, 
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
	m_BlockWidth  = _wWidth / kColumnCount;
	m_BlockHeight = _wHeight / kRowCount; 

	for(int i = 0; i < kRowCount; ++i)
	{
		for(int j = 0; j < kColumnCount; ++j)
		{
			Point2<u32> upperLeft = _XYToUpperLeft(m_BlockWidth, m_BlockHeight, i, j);

			s32 flatIndex = _XYToIndex(i, j);
			Event evt((EventType_t)(GameEngine::E_ET_UpdateBlock1 + flatIndex));
			evt.AddParam(upperLeft.X())
			   .AddParam(upperLeft.Y())
			   .AddParam(m_BlockWidth)
			   .AddParam(m_BlockHeight);
			GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
		}
	}
}

void GSBlockManager::OnClick(s32 x, s32 y)
{
	s32 row = y / m_BlockHeight;
	s32 column = x / m_BlockWidth;

	D_CHECK(row >= 0 && row < kRowCount);
	D_CHECK(column >= 0 && column < kRowCount);

	GSBlock* curBlock = m_BlockMap[row][column];
	if(curBlock)
		curBlock->OpClickBlock();
}