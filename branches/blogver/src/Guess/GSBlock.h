#ifndef __GS_BLOCK_H__
#define __GS_BLOCK_H__

#include "GSGlobalDef.h"
#include <string>

class GSBlock;

class GSEffectController
{
public:
	enum{
		EEffectController_Loop,
		EEffectController_Once,
	};	

	struct KeyFrame
	{
		struct FrameInfo
		{
			static FrameInfo Blend(const FrameInfo& frame1, const FrameInfo& frame2, f32 coef);
			Point2<u32> m_UpperLeft;
			u32			m_Width;
			u32			m_Height;
			u32			m_PicIndex;
		} m_FrameInfo;
		f32			m_Time;
	};

	struct OutputKeyFrame
	{
		KeyFrame::FrameInfo m_FrameInfo;
		Bool				m_HasValidData;
	};

	GSEffectController(GSBlock* _pOwner);

	void						AddFrame(const KeyFrame& frame);
	const OutputKeyFrame&		GetCurFrame() const;
	void						Init();

	void Start();
	void Tick(f32 _fDeltaTime);
	
private:
	Array<KeyFrame>		m_Frames;
	f32					m_CurTime;
	f32					m_LastTime;
	OutputKeyFrame		m_CurFrame;
	Bool				m_HasStarted;
	GSBlock*			m_Owner;
	Bool				m_HasInit;
};


class GSBlock : public DrawableObject
{
public:
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

	enum{
		EControlFlag_WidthPrefered,
		EControlFlag_HeightPrefered,
		EControlFlag_FreeSize,
	};

	void InitBlock(u32 _x, u32 _y, u32 _width, u32 _height, StringPtr _backImage, StringPtr _frontImage, StringPtr _answerImage, u32 _controlFlag = 0);
	u32 GetBlockWidth()		const { return m_uiWidth;	}
	u32 GetBlockHeight()	const { return m_uiHeight;	}
	u32 GetBlockState()		const { return m_StateMachine.GetState();	}

	void OpResizeBlock(u32 x, u32 y, u32 width, u32 height);
	void OpClickBlock();
	void OpEffectFinished(const Event* _poEvent);

	enum{
		EImageState_Back = 0,
		EImageState_Front,
		EImageState_Answer,
		EImageState_Invisible,
		EImageState_Num
	};
	enum{
		EImageIndex_Back,
		EImageIndex_Front,
		EImageIndex_Answer,

		EImageIndex_Num
	};

private:
	typedef void (GSBlock::*StateHandler)();

	void STATE_HandleBack();
	void STATE_HandleFront();
	void STATE_HandleAnswer();
	void STATE_HandleInvisible();

	class BlockStateMachine{
	public:	
		BlockStateMachine()
			: m_CurState(EImageState_Back)
		{
			m_StateHandler[EImageState_Back]		= &GSBlock::STATE_HandleBack;
			m_StateHandler[EImageState_Front]		= &GSBlock::STATE_HandleFront;
			m_StateHandler[EImageState_Answer]		= &GSBlock::STATE_HandleAnswer;
			m_StateHandler[EImageState_Invisible]	= &GSBlock::STATE_HandleInvisible;
		}
		u32		GetState() const{ return m_CurState; }
		void	StepForward() { m_CurState++;	}
		void    Handle(GSBlock* obj) { 
			if(m_CurState >= EImageState_Back && m_CurState <= EImageState_Invisible)
			{
				(obj->*m_StateHandler[m_CurState])();
			}
		}

	private:
		u32				m_CurState;
		StateHandler	m_StateHandler[EImageState_Num];
	};

	friend class GSEffectController;

	u32					m_uiX, m_uiY;
	u32					m_uiWidth, m_uiHeight;
	u32					m_uiPicIndex;
	u32					m_uiControlFlag;
	WinGDIJpeg*			m_poImages[EImageIndex_Num];
	GSEffectController*	m_EffectController;
	u8					m_iDefaultZOrder;
	Bool				m_bReadFromEffect;
	BlockStateMachine	m_StateMachine;
};	

class GSBlockManager : public Singleton<GSBlockManager>
{
	static const int kRowCount		= 3;
	static const int kColumnCount	= 3;
public:
	GSBlockManager();
	void Init(s32 _wWidth, s32 _wHeight);
	void SetSelectedBlock(GSBlock* _pBlock){
		m_SelectedBlock = _pBlock;
	}
	

	void OnResizeWindow(s32 _wWidth, s32 _wHeight);
	void OnClick(s32 x, s32 y);

private:
	D_Inline s32 _XYToIndex(s32 row, s32 column){
		return row * kColumnCount + column;
	}
	D_Inline Point2<u32> _XYToUpperLeft(s32 width, s32 height, s32 row, s32 column){
		return Point2<u32>(column * width, row * height);
	}

private:
	struct ConstPicInfo{
		ConstPicInfo(StringPtr _back, StringPtr _front, StringPtr _answer)
		{
			m_FrontPicName = _front;
			m_BackPicName = _back;
			m_AnswerPicName = _answer;
		}
		std::string m_FrontPicName;
		std::string m_BackPicName;
		std::string m_AnswerPicName;
	};
	GSBlock*			m_BlockMap[kRowCount][kColumnCount];
	u32					m_BlockWidth;
	u32					m_BlockHeight;
	GSBlock*			m_SelectedBlock;
	
};

class GSBackground : public DrawableObject
{
public:
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

private:
	WinGDIJpeg*	 m_poImage;
};

#endif