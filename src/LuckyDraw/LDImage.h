#ifndef __LD_IMAGE_H__
#define __LD_IMAGE_H__

#include "LDGlobalDef.h"

class LDImage : public DrawableObject
{
public:
	virtual void Create();
	virtual void Tick(f32 p_fDeltaTime);
	virtual void Draw();

public:
	void TriggerStartStop();

private:
	FXJPGImage* _GetNextImage();

	void _ChangeState(int _state);
	void _OnIdle(f32 _fDeltaTime);
	void _OnRunning(f32 _fDeltaTime);
	void _OnStopping(f32 _fDeltaTime);
	void _OnReset();

private:
	enum{
		EState_Idle,
		EState_Running,
		EState_Stopping,
	};
	
	class EffectControl{
		static const float kSTOP; //= -1.f;
		static const float kFastest; //= 0.07f;
		static const float kSlowest; //= 3.f;
		static const float kVel;// = 0.5f;

	public:
		EffectControl()
			:m_CurTime(kSTOP)
		{}

		float GetCurTime() const { return m_CurTime; }
		void  StepToSlow() {
			if(IsStop())
				m_CurTime = kFastest;

			m_CurTime += kVel;
			if(m_CurTime > kSlowest)
				m_CurTime = kSTOP;
		}
		bool  IsStop() const{
			return m_CurTime < 0.f;
		}
		void  StepToFast() {
			if(IsStop())
				m_CurTime = kSlowest;

			m_CurTime -= kVel;
			if(m_CurTime < kFastest)
				m_CurTime = kFastest;
		}

	private:
		float m_CurTime;
	};

	int				m_CurState;
	int				m_PrevState;
		
	float			m_Time;
	int				m_CurrentImage;
	int				m_ImageSize;

	EffectControl	m_Effect;

	bool			m_ShowInfo;
};

#endif