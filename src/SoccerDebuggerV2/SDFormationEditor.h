#ifndef __SDFORMATION_EDITOR_H__
#define __SDFORMATION_EDITOR_H__

#include "SDGlobalDef.h"

static const f32 kPithLenghNormal	= 50.f;
static const f32 kPitchWidthNormal	= 30.f;

//----------------------------------------------------------------------------
class ORefValueUpdater : public Object
{
public:
	virtual void Create()
	{}
	virtual void Tick(f32 _fDeltaTime);
};

//----------------------------------------------------------------------------
struct CoordinateInfo
{
	static f32	GetPixelPerMeter();
	static f32	GetMeterPerPixel();
	static Vec2 WorldToScreen(const Vec2& worldPos);
	static Vec2 ScreenToWorld(const Vec2& screenPos);

	static f32 sLength;
	static f32 sWidth;
};

//----------------------------------------------------------------------------
class IOperator
{
public:
	virtual void Select(const Vec2& pos)	= 0;
	virtual void Deselect(const Vec2& pos)	= 0;
	virtual void Move(const Vec2& pos)		= 0;
};
//----------------------------------------------------------------------------
class FEElement : public DrawableObject, IOperator
{
protected:
	enum{
		EFEFlag_Selected = (1<<0),
	};

public:
	FEElement();
	virtual ~FEElement();

	virtual void Select(const Vec2& pos);
	virtual void Deselect(const Vec2& pos);
	virtual void Move(const Vec2& pos);

	virtual bool InBounds(const Vec2& checkPoint) const { return false;	}

protected:
	BitArray m_Flag;
};
//----------------------------------------------------------------------------
class FERefBall : public FEElement
{
public:
	FERefBall();

	//from FEElement
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual bool InBounds(const Vec2& checkPoint) const;

	virtual void Move(const Vec2& pos);

private:
	f32 m_fBallRadius;
	AI::RefValue<Vec2, AI::ERefValuFlag_Writable> m_RfPosition;
};
//----------------------------------------------------------------------------
class FERefPlayer : public FEElement
{
public:
	FERefPlayer(s32 id);

	//from FEElement
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

private:
	s32 m_id;
	f32 m_fPlayerRadius;
	AI::RefValue<Vec2, AI::ERefValuFlag_ReadOnly> m_RfPosition;
};
//----------------------------------------------------------------------------
class FECanvas : public FEElement
{
public:
	FECanvas();
	
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

	virtual void Select(const Vec2& pos);
	virtual void Deselect(const Vec2& pos);
	virtual void Move(const Vec2& pos);

private:
	FEElement*		  m_SelectedElements;
	Array<FEElement*> m_Elements;
};
//----------------------------------------------------------------------------
class FormationEditor : public FEElement
{
	enum{
		EPitchType_Normal,
		EPitchType_Large,
		EPitchType_Num,
	};
	enum{
		ETeamState_Attack,
		ETeamState_Defend,
		ETeamState_Num,
	};
	static const s32 kMaxPositionCount = 15;

public:
	FormationEditor();

	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

	virtual void Select(const Vec2& pos);
	virtual void Deselect(const Vec2& pos);
	virtual void Move(const Vec2& pos);

	void SetCurrentCanvas(u32 pitchType, u32 teamState, u32 pos);

private:
	FECanvas* m_CurrentCanvas;
	FECanvas* m_Canvases[EPitchType_Num][ETeamState_Num][kMaxPositionCount]; 

};
//----------------------------------------------------------------------------
class FEWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnMouseLDown(s32 x, s32 y);
	virtual void OnMouseLUp(s32 x, s32 y);
	virtual void OnMouseLDrag(s32 x, s32 y, s32 relX, s32 relY);
	virtual void OnKeyDown(u32 key, s64 info);
};

#endif