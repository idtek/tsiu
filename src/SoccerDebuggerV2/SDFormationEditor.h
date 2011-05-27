#ifndef __SDFORMATION_EDITOR_H__
#define __SDFORMATION_EDITOR_H__

#include "SDGlobalDef.h"

class TiXmlDocument;
class TiXmlElement;
//----------------------------------------------------------------------------
class ORefValueUpdater : public Object
{
public:
	virtual void Create()
	{}
	virtual void Tick(f32 _fDeltaTime);
};
//----------------------------------------------------------------------------
class IOperator
{
public:
	virtual void Select(const Vec2& pos)	= 0;
	virtual void Deselect(const Vec2& pos)	= 0;
	virtual void Move(const Vec2& pos)		= 0;
	virtual void Delete()					= 0;
};
class ISerializer
{
public:
	virtual void Serialize	(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent = NULL){}
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent = NULL){}
};
//----------------------------------------------------------------------------
class FEElement : public DrawableObject, public IOperator, public ISerializer
{
protected:
	enum{
		EFEFlag_Selected	= (1<<0),
		EFEFlag_Deletable	= (1<<1),
		EFEFlag_Movable		= (1<<2),
	};

public:
	FEElement();
	virtual ~FEElement();

	virtual void Select(const Vec2& pos);
	virtual void Deselect(const Vec2& pos);
	virtual void Move(const Vec2& pos);
	virtual void Delete(){}

	void AddFlags(u32 flags);
	void RemoveFlags(u32 flags);
	Bool HasFlags(u32 flags) const;

	virtual bool InBounds(const Vec2& checkPoint) const { return false;	}

protected:
	Vec2 ClampToPitch(const Vec2& unclamppedPos);

protected:
	BitArray m_Flag;
};
//----------------------------------------------------------------------------
class FEEditableElement : public FEElement
{
public:
	FEEditableElement(s32 id)
		: FEElement()
		, m_id(id)
	{}

	void SetColor(const D_Color& clr)			{ m_Color = clr;	}		
	s32	 GetID()						const	{ return m_id;		}

protected:
	D_Color		m_Color;
	s32			m_id;
};

class FEBall : public FEEditableElement
{
public:
	FEBall(s32 id);

	//from FEElement
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual bool InBounds(const Vec2& checkPoint) const;

	virtual void Move(const Vec2& pos);

	virtual void Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);

protected:
	f32		m_fBallRadius;
};

class FESimulatedBall : public FEBall
{
public:
	FESimulatedBall(s32 id);

	virtual void Tick(f32 _fDeltaTime);
	virtual void Move(const Vec2& pos);

protected:
	AI::RefValue<Vec2, AI::ERefValuFlag_Writable> m_RfPosition;
};
//----------------------------------------------------------------------------
class FEPlayer : public FEEditableElement
{
public:
	FEPlayer(s32 id);

	//from FEElement
	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual bool InBounds(const Vec2& checkPoint) const;

	virtual void Move(const Vec2& pos);

	virtual void Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);

protected:
	f32 m_fPlayerRadius;
};
class FESimulatedPlayer : public FEPlayer
{
public:
	FESimulatedPlayer(s32 id);

	//from FEElement
	virtual void Tick(f32 _fDeltaTime);

protected:
	AI::RefValue<Vec2, AI::ERefValuFlag_ReadOnly> m_RfPosition;
};
//----------------------------------------------------------------------------
class FECanvas : public FEElement
{
public:
	FECanvas();

	virtual void Deselect(const Vec2& pos);
	virtual void Move(const Vec2& pos);

protected:
	FEEditableElement* m_SelectedElement;
};
class FEEditorCanvas : public FECanvas
{
	struct ElementPair
	{
		FEBall*		m_RefElement;
		FEPlayer*	m_FormationElement;
	};
	typedef std::map<s32, ElementPair*>				ElementPairMap;
	typedef std::map<s32, ElementPair*>::iterator	ElementPairMapIterator;
	typedef std::map<s32, ElementPair*>::iterator	ElementPairMapConstIterator;

public:
	FEEditorCanvas();

	virtual void Create(){}
		    void Create(f32 length, f32 width);
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual void Select(const Vec2& pos);
	virtual void Delete();

	virtual void Export(File* _pFile, u32 pitchType, u32 teamState, u32 position);
	virtual void Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);

	void AddElementPair(const Vec2& pos);

private:
	u32			   m_NextID;
	ElementPairMap m_Elements;
};
class FESimulatedCanvas : public FECanvas
{
public:
	FESimulatedCanvas();

	struct SimulatedSettings
	{
		u32			m_PitchType;
		u32			m_TeamState;
		Array<u32>	m_PlayerPosition;
	};
	void Setup(const SimulatedSettings& setup);
	void Stop();

	virtual void Create(){}
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual void Select(const Vec2& pos);

private:
	Array<FEEditableElement*> m_Elements;
};
//----------------------------------------------------------------------------
class FormationEditor : public FEElement
{
	static const s32 kMaxPositionCount = 15;
public:
	enum{
		EEditorMode_Edit,
		EEditorMode_Simulation
	};
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

public:
	FormationEditor();

	virtual void Create();
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

	virtual void Select(const Vec2& pos);
	virtual void Deselect(const Vec2& pos);
	virtual void Move(const Vec2& pos);
	virtual void Delete();

	virtual void Export(File* pFile);
	virtual void Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);

	Bool IsSimulating() const { return m_Mode == EEditorMode_Simulation;	}

	void StartSimulation(const FESimulatedCanvas::SimulatedSettings& setup);
	void StopSimulation();
	void AddElementPair();
	void SetCurrentCanvas(u32 pitchType, u32 teamState, u32 pos);

private:
	u32					m_Mode;
	FESimulatedCanvas*	m_SimulatedCanvas;
	FEEditorCanvas*		m_CurrentCanvas;
	FEEditorCanvas*		m_Canvases[EPitchType_Num][ETeamState_Num][kMaxPositionCount]; 

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