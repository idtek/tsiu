#include "LDImage.h"

const float LDImage::EffectControl::kSTOP = -1.f;
const float LDImage::EffectControl::kFastest = 0.07f;
const float LDImage::EffectControl::kSlowest = 1.f;
const float LDImage::EffectControl::kVel = 0.2f;

void LDImage::Create()
{
	m_Time = 0.f;
	m_CurrentImage = -1;
	m_ImageSize = GameEngine::GetGameEngine()->GetImageCount();
	m_PrevState = m_CurState = EState_Idle;
	m_ShowInfo = false;
}

void LDImage::Tick(f32 _fDeltaTime)
{
	switch(m_CurState)
	{
	case EState_Idle:
		_OnIdle(_fDeltaTime);
		break;
	case EState_Running:
		_OnRunning(_fDeltaTime);
		break;
	case EState_Stopping:
		_OnStopping(_fDeltaTime);
		break;
	}
}

void LDImage::_OnIdle(f32 _fDeltaTime)
{

}
void LDImage::_OnRunning(f32 _fDeltaTime)
{
	m_ShowInfo = false;

	m_Time += _fDeltaTime;
	if(m_Time >= m_Effect.GetCurTime())
	{
		FXJPGImage* image = _GetNextImage();
		if(image)
		{
			GameEngine::GetGameEngine()->GetMainPage()->SetNextImage(image);
			m_Time = 0.f;
			m_Effect.StepToFast();

			//printf("cur id = %d\n", m_CurrentImage);
		}
		else
		{
			_ChangeState(EState_Idle);
		}
	}
}
void LDImage::_OnStopping(f32 _fDeltaTime)
{
	m_Time += _fDeltaTime;
	if(m_Time >= m_Effect.GetCurTime())
	{
		FXJPGImage* image = _GetNextImage();
		if(image)
		{
			GameEngine::GetGameEngine()->GetMainPage()->SetNextImage(image);
			m_Time = 0.f;
			m_Effect.StepToSlow();
			if(m_Effect.IsStop())
			{
				_ChangeState(EState_Idle);
				m_ShowInfo = true;

				//auto remove player who is last winner
				GameEngine::GetGameEngine()->RemoveImageByIndex(m_CurrentImage);
			}
		}
		else
		{
			_ChangeState(EState_Idle);
		}
	}
}

void LDImage::_OnReset()
{
	if(m_CurState == EState_Running)
	{
		m_Time = 0.f;
		m_Effect.StepToFast();
	}
}

FXJPGImage* LDImage::_GetNextImage()
{
	m_ImageSize = GameEngine::GetGameEngine()->GetImageCount();

	if(m_ImageSize == 0)
		return NULL;

	++m_CurrentImage;
	m_CurrentImage = m_CurrentImage % m_ImageSize;

	return GameEngine::GetGameEngine()->GetImageByIndex(m_CurrentImage);
}

void LDImage::TriggerStartStop()
{
	if(m_CurState == EState_Idle)
	{
		_ChangeState(EState_Running);
	}
	else if(m_CurState == EState_Running)
	{
		_ChangeState(EState_Stopping);
	}
}

void LDImage::_ChangeState(int _state)
{
	if(_state != m_CurState)
	{
		m_PrevState = m_CurState;
		m_CurState = _state;

		_OnReset();
	}
}
void LDImage::Draw()
{
	if(m_ShowInfo)
	{
		int viewerWidth, viewerHeight;
		GameEngine::GetGameEngine()->GetMainPage()->GetGDIViewerSize(viewerWidth, viewerHeight);
		GameEngine::GetGameEngine()->GetGDIBrush()->DrawStringEx(0, 0, viewerWidth, viewerHeight, viewerHeight, "¹§Ï²Äã£¡", NULL, D_Color(255, 0, 0) );
	}
}