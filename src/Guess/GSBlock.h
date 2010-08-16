#ifndef __GS_BLOCK_H__
#define __GS_BLOCK_H__

#include "GSGlobalDef.h"
#include <string>

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

	void InitBlock(u32 _x, u32 _y, u32 _width, u32 _height, StringPtr _backImage, StringPtr _frontImage, u32 _controlFlag = 0);
	u32 GetBlockWidth()		const { return m_uiWidth;	}
	u32 GetBlockHeight()	const { return m_uiHeight;	}

	void OpUpdateBlock(const Event* _poEvent);

private:
	u32	m_uiX, m_uiY;
	u32	m_uiWidth, m_uiHeight;	
	u32 m_uiControlFlag;

	enum{
		EImageState_Back = 0,
		EImageState_Front,
		EImageState_Num
	};
	u32				m_uiCurrentState;
	WinGDIBitmap*	m_poImages[EImageState_Num];
};

class GSBlockManager : public Singleton<GSBlockManager>
{
	static const int kRowCount		= 3;
	static const int kColumnCount	= 3;
public:
	void Init(s32 _wWidth, s32 _wHeight);

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
		ConstPicInfo(StringPtr _back, StringPtr _front)
		{
			m_FrontPicName = _front;
			m_BackPicName = _back;
		}
		std::string m_FrontPicName;
		std::string m_BackPicName;
	};
	GSBlock* m_BlockMap[kRowCount][kColumnCount];
};

#endif