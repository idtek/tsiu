#include "GSBlock.h"
#include <float.h>

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
	ret.m_PicIndex	= frame2.m_PicIndex;

	return ret;
}

GSEffectController::GSEffectController(GSBlock* _pOwner)
	:m_HasStarted(false)
	,m_CurTime(0.f)
	,m_LastTime(0.f)
	,m_HasInit(false)
	,m_Owner(_pOwner)
{
}
void GSEffectController::AddFrame(const KeyFrame& frame)
{
	D_CHECK(m_HasInit);
	m_Frames.PushBack(frame);
}

const GSEffectController::OutputKeyFrame& GSEffectController::GetCurFrame() const
{
	return m_CurFrame;
}

void GSEffectController::Start()
{
	m_HasStarted = true;
	m_CurTime = 0.f;
	m_LastTime = 0.f;
}

void GSEffectController::Init()
{
	m_HasInit = true;

	m_Frames.Clear();

	KeyFrame kf;
	kf.m_FrameInfo.m_UpperLeft.X(m_Owner->m_uiX);
	kf.m_FrameInfo.m_UpperLeft.Y(m_Owner->m_uiY);
	kf.m_FrameInfo.m_Width = m_Owner->m_uiWidth;
	kf.m_FrameInfo.m_Height = m_Owner->m_uiHeight;
	kf.m_FrameInfo.m_PicIndex = m_Owner->m_uiPicIndex;
	kf.m_Time = 0.f;
	AddFrame(kf);
}

void GSEffectController::Tick(f32 _fDeltaTime)
{
	s32 frame1 = -1, frame2 = -1;

	//Clear
	m_CurFrame.m_HasValidData = false;

	if(m_HasStarted)
	{
		m_LastTime = m_CurTime;
		m_CurTime += _fDeltaTime;

		if(m_Frames.Size() > 0)
		{
			if(m_Frames.Size() == 1)
			{
				const KeyFrame& frame = m_Frames[0];
				D_CHECK(Math::IsEqual(frame.m_Time, 0.f));
				if(Math::IsEqual(m_LastTime, 0.f))
				{
					m_CurFrame.m_HasValidData = true;
					m_CurFrame.m_FrameInfo = frame.m_FrameInfo;
				}
			}
			else
			{
				s32 sizeOfFrames = m_Frames.Size();
				f32 timeUsed = m_CurTime;

				for(s32 i = 0; i < sizeOfFrames; ++i)
				{
					const KeyFrame& frame = m_Frames[i];
					if(frame.m_Time >= timeUsed)
					{
						frame2 = i;
						break;
					}
				}
				if(frame2 < 0)
				{
					if(m_Frames[sizeOfFrames - 1].m_Time >= m_LastTime)
					{
						frame2 = sizeOfFrames - 1;
						timeUsed = m_Frames[frame2].m_Time;
					}
				}
				if(frame2 >= 0)
					frame1 = Math::Max<s32>(0, frame2 - 1);

				if(frame1 >= 0 && frame2 >= 0)
				{
					f32 frame1Time = m_Frames[frame1].m_Time;
					f32 frame2Time = m_Frames[frame2].m_Time;
					f32 coef = 0;
					if(!Math::IsEqual(frame1Time, frame2Time))
					{
						D_CHECK(frame2Time > frame1Time);
						coef = (timeUsed - frame1Time) / (frame2Time - frame1Time);
					}
					m_CurFrame.m_HasValidData = true;
					m_CurFrame.m_FrameInfo = KeyFrame::FrameInfo::Blend(m_Frames[frame1].m_FrameInfo, m_Frames[frame2].m_FrameInfo, coef);
				}
			}
		}
	}
	if(!m_CurFrame.m_HasValidData)
	{
		m_HasStarted = false;

		Event evt((EventType_t)(GameEngine::E_ET_EffectFinished));
		evt.AddParam(this);
		GameEngine::GetGameEngine()->GetEventMod()->SendEvent(&evt);
	}
}

//-----------------------------------------------------------------------
void GSBlock::Create()
{
}

void GSBlock::Draw()
{
	u32 width = 0, height = 0, x = 0, y = 0;
	u32 picIndex = EImageIndex_Num;

	if(m_bReadFromEffect)
	{
		const GSEffectController::OutputKeyFrame& info = m_EffectController->GetCurFrame();
		D_CHECK(info.m_HasValidData);
		x = info.m_FrameInfo.m_UpperLeft.X();
		y = info.m_FrameInfo.m_UpperLeft.Y();
		width = info.m_FrameInfo.m_Width;
		height = info.m_FrameInfo.m_Height;
		picIndex = info.m_FrameInfo.m_PicIndex;

		m_uiX = x;
		m_uiY = y;
		m_uiWidth = width;
		m_uiHeight = height;
		m_uiPicIndex = picIndex;
	}
	else
	{
		width = m_uiWidth;
		height = m_uiHeight;
		x = m_uiX;
		y = m_uiY;
		picIndex = m_uiPicIndex;
	}

	if(picIndex >= 0 && picIndex < EImageIndex_Num)
	{
		WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
		WinGDIJpeg* curBmp = m_poImages[picIndex];
		if(curBmp)
		{
			switch(m_uiControlFlag)
			{
			case EControlFlag_WidthPrefered:
				height = (int)((float)curBmp->GetHeight() / curBmp->GetWidth() * width);
				break;
			case EControlFlag_HeightPrefered:
				width = (int)((float)curBmp->GetWidth() / curBmp->GetHeight() * height);
				break;
			}
			if(height != 0 && width != 0)
				curBmp->Draw(pRender, x, y, width, height);
		}
	}
}

void GSBlock::Tick(f32 _fDeltaTime)
{
	if(m_bReadFromEffect)
		m_EffectController->Tick(_fDeltaTime);
}

void GSBlock::OpResizeBlock(u32 x, u32 y, u32 width, u32 height)
{
	if(m_StateMachine.GetState() != EImageState_Invisible)
	{
		m_uiX = x;
		m_uiY = y;
		m_uiWidth = width;
		m_uiHeight = height;
	}
}

void GSBlock::OpClickBlock()
{
	if(!m_bReadFromEffect)
		m_StateMachine.Handle(this);
}

void GSBlock::OpEffectFinished(const Event* _poEvent)
{
	GSEffectController* pController = (GSEffectController*)_poEvent->GetParam<void*>(0);
	if(pController == m_EffectController)
	{
		m_bReadFromEffect = false;
	}
}

void GSBlock::InitBlock(u32 _x, u32 _y, u32 _width, u32 _height, StringPtr _backImage, StringPtr _frontImage, StringPtr _answerImage, u32 _controlFlag)
{
	m_uiX = _x;
	m_uiY = _y;

	m_uiWidth = _width;
	m_uiHeight = _height;
	
	m_uiControlFlag = _controlFlag;
	m_iDefaultZOrder = GetZOrder();
	m_bReadFromEffect = false;

	m_uiPicIndex = EImageIndex_Back;

	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();

	m_poImages[EImageIndex_Back]		= new WinGDIJpeg(pRender, _backImage);
	m_poImages[EImageIndex_Front]		= new WinGDIJpeg(pRender, _frontImage);
	m_poImages[EImageIndex_Answer]		= new WinGDIJpeg(pRender, _answerImage);

	m_EffectController = new GSEffectController(this);
}

void GSBlock::STATE_HandleBack()
{
	m_EffectController->Init();

	GSEffectController::KeyFrame kf;
	kf.m_FrameInfo.m_UpperLeft.X(m_uiX + m_uiWidth / 2.f);
	kf.m_FrameInfo.m_UpperLeft.Y(m_uiY);
	kf.m_FrameInfo.m_Width = 0.f;
	kf.m_FrameInfo.m_Height = m_uiHeight;
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Back;
	kf.m_Time = 0.5f;
	m_EffectController->AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(m_uiX);
	kf.m_FrameInfo.m_UpperLeft.Y(m_uiY);
	kf.m_FrameInfo.m_Width = m_uiWidth;
	kf.m_FrameInfo.m_Height = m_uiHeight;
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Front;
	kf.m_Time = 1.f;
	m_EffectController->AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(m_uiX);
	kf.m_FrameInfo.m_UpperLeft.Y(m_uiY);
	kf.m_FrameInfo.m_Width = m_uiWidth;
	kf.m_FrameInfo.m_Height = m_uiHeight;
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Front;
	kf.m_Time = 1.5f;
	m_EffectController->AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(0);
	kf.m_FrameInfo.m_UpperLeft.Y(0);
	kf.m_FrameInfo.m_Width = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetWidth();
	kf.m_FrameInfo.m_Height = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetHeight();
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Front;
	kf.m_Time = 2.5f;
	m_EffectController->AddFrame(kf);
	
	m_EffectController->Start();
	SetZOrder(EZOrder_Top);
	m_bReadFromEffect = true;

	m_StateMachine.StepForward();	//To Front

	GSBlockManager::GetPtr()->SetSelectedBlock(this);
}
void GSBlock::STATE_HandleFront()
{
	m_EffectController->Init();

	GSEffectController::KeyFrame kf;
	kf.m_FrameInfo.m_UpperLeft.X(GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetWidth() / 2.f);
	kf.m_FrameInfo.m_UpperLeft.Y(0);
	kf.m_FrameInfo.m_Width = 0.f;
	kf.m_FrameInfo.m_Height = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetHeight();
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Front;
	kf.m_Time = 0.5f;
	m_EffectController->AddFrame(kf);

	kf.m_FrameInfo.m_UpperLeft.X(0);
	kf.m_FrameInfo.m_UpperLeft.Y(0);
	kf.m_FrameInfo.m_Width = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetWidth();
	kf.m_FrameInfo.m_Height = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetHeight();
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Answer;
	kf.m_Time = 1.f;
	m_EffectController->AddFrame(kf);

	m_EffectController->Start();

	SetZOrder(EZOrder_Top);
	m_bReadFromEffect = true;

	m_StateMachine.StepForward();	//To Answer
}
void GSBlock::STATE_HandleAnswer()
{
	m_EffectController->Init();

	GSEffectController::KeyFrame kf;
	kf.m_FrameInfo.m_UpperLeft.X(GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetWidth() / 2);
	kf.m_FrameInfo.m_UpperLeft.Y(GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetHeight() / 2);
	kf.m_FrameInfo.m_Width = 0;
	kf.m_FrameInfo.m_Height = 0;
	kf.m_FrameInfo.m_PicIndex = EImageIndex_Answer;
	kf.m_Time = 1.f;
	m_EffectController->AddFrame(kf);

	m_EffectController->Start();

	SetZOrder(EZOrder_Top);
	m_bReadFromEffect = true;

	m_StateMachine.StepForward();	//To Invisible

	GSBlockManager::GetPtr()->SetSelectedBlock(NULL);
}
void GSBlock::STATE_HandleInvisible()
{

}

//--------------------------------------------------------------------------------------------------------------
GSBlockManager::GSBlockManager()
	:m_SelectedBlock(NULL)
{
	memset(m_BlockMap, 0, sizeof(GSBlock*) * kColumnCount * kRowCount);
}
void GSBlockManager::Init(s32 _wWidth, s32 _wHeight)
{
	m_BlockWidth  = _wWidth / kColumnCount;
	m_BlockHeight = _wHeight / kRowCount; 

	static const ConstPicInfo kPicList[] = {
		ConstPicInfo("back1.jpg", "1.jpg", "3-a.jpg"), 
		ConstPicInfo("back2.jpg", "2.jpg", "3-a.jpg"),
		ConstPicInfo("back3.jpg", "3.jpg", "3-a.jpg"),
		ConstPicInfo("back4.jpg", "4.jpg", "3-a.jpg"),
		ConstPicInfo("back5.jpg", "5.jpg", "3-a.jpg"),
		ConstPicInfo("back6.jpg", "6.jpg", "3-a.jpg"),
		ConstPicInfo("back7.jpg", "7.jpg", "3-a.jpg"),
		ConstPicInfo("back8.jpg", "8.jpg", "3-a.jpg"),
		ConstPicInfo("back9.jpg", "9.jpg", "3-a.jpg")
	};

	GSBlock* newBlock = NULL;
	for(s32 i = 0; i < kRowCount; ++i)
	{
		for(s32 j = 0; j < kColumnCount; ++j)
		{
			Point2<u32> upperLeft = _XYToUpperLeft(m_BlockWidth, m_BlockHeight, i, j);

			s32 flatIndex = _XYToIndex(i, j);
			newBlock = new GSBlock;
			newBlock->SetZOrder(EZOrder_Bottom - 1);
			newBlock->InitBlock(upperLeft.X(), 
								upperLeft.Y(), 
								m_BlockWidth, 
								m_BlockHeight, 
								kPicList[flatIndex].m_BackPicName.c_str(), 
								kPicList[flatIndex].m_FrontPicName.c_str(),
								kPicList[flatIndex].m_AnswerPicName.c_str(),
								GSBlock::EControlFlag_FreeSize);
			m_BlockMap[i][j] = newBlock;

			//Add to engine
			char objName[64];
			sprintf(objName, "Image%d", flatIndex);
			GameEngine::GetGameEngine()->GetSceneMod()->AddObject(objName, newBlock);

			//Register event handler
			GameEngine::GetGameEngine()->GetEventMod()->RegisterHandler(
				(EventType_t)(GameEngine::E_ET_EffectFinished), 
				new MEventHandler<GSBlock>(newBlock, &GSBlock::OpEffectFinished));
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
			if(m_BlockMap[i][j] == m_SelectedBlock)
				m_BlockMap[i][j]->OpResizeBlock(0, 0, _wWidth, _wHeight);
			else
				m_BlockMap[i][j]->OpResizeBlock(upperLeft.X(), upperLeft.Y(), m_BlockWidth, m_BlockHeight);
		}
	}
}

void GSBlockManager::OnClick(s32 x, s32 y)
{
	if(!m_SelectedBlock)
	{
		s32 row = y / m_BlockHeight;
		s32 column = x / m_BlockWidth;

		D_CHECK(row >= 0 && row < kRowCount);
		D_CHECK(column >= 0 && column < kRowCount);

		GSBlock* curBlock = m_BlockMap[row][column];
		if(curBlock)
			curBlock->OpClickBlock();
	}
	else
	{
		m_SelectedBlock->OpClickBlock();
	}
}

//---------------------------------------------------------------------------------------------------------
void GSBackground::Create()
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
	m_poImage = new WinGDIJpeg(pRender, "bg.jpg");

	SetZOrder(EZOrder_Bottom);
}
void GSBackground::Tick(f32 _fDeltaTime)
{

}
void GSBackground::Draw()
{
	WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();

	u32 width = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetWidth();
	u32 height = GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetHeight();

	m_poImage->Draw(pRender, 0, 0, width, height);
}