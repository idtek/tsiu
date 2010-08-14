#ifndef __GS_BLOCK_H__
#define __GS_BLOCK_H__

#include "GSGlobalDef.h"

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

#endif