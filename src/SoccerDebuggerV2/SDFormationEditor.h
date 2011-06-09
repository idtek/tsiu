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
	virtual void Serialize	(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent = NULL)	{}
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent = NULL)	{}
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

	void Resize(f32 ratioX, f32 ratioY);
	void DrawWithColor(const D_Color& clr);

protected:
	f32		m_fBallRadius;
};

class FESimulatedBall : public FEBall
{
public:
	FESimulatedBall(s32 id);

	virtual bool InBounds(const Vec2& checkPoint) const;
	virtual void Tick(f32 _fDeltaTime);
	virtual void Move(const Vec2& pos);
	virtual void Draw();

protected:
	AI::RefValue<Vec2, AI::ERefValuFlag_Writable> m_RfPosition;
};

class FERealBall : public FEBall
{
public:
	FERealBall();

	virtual bool InBounds(const Vec2& checkPoint) const;
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

protected:
	AI::RefValue<Vec2, AI::ERefValuFlag_ReadOnly> m_RfPosition;
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

	void Resize(f32 ratioX, f32 ratioY);
	void DrawWithColor(const D_Color& clr);

protected:
	f32 m_fPlayerRadius;
};
class FESimulatedPlayer : public FEPlayer
{
public:
	static int sHomePlayerCount;
	static int sAwayPlayerCount;

	FESimulatedPlayer(s32 id, int team);

	//from FEElement
	virtual void Select(const Vec2& pos); 
	virtual bool InBounds(const Vec2& checkPoint) const;
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

protected:
	int m_Team;
	AI::RefValue<Vec2, AI::ERefValuFlag_ReadOnly> m_RfPosition;
};

class FERealPlayer : public FESimulatedPlayer
{
public:
	FERealPlayer(s32 id, int team, bool isGK);
	virtual void Draw();
	virtual void Select(const Vec2& pos); 

private:
	Bool m_bIsGK;
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
	typedef std::map<s32, ElementPair*>					ElementPairMap;
	typedef std::map<s32, ElementPair*>::iterator		ElementPairMapIterator;
	typedef std::map<s32, ElementPair*>::const_iterator	ElementPairMapConstIterator;

public:
	FEEditorCanvas();
	~FEEditorCanvas();

	FEEditorCanvas& operator=(const FEEditorCanvas& rhs);

	virtual void Create(){}
		    void Create(f32 length, f32 width);
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual void Select(const Vec2& pos);
	virtual void Delete();

	void Resize(f32 ratioX, f32 ratioY);

	virtual void Export(File* _pFile, u32 pitchType, u32 teamState, u32 position);
	virtual void Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);
	virtual void Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent);

	void DrawWithColor(const D_Color& clr);
	void AddElementPair(const Vec2& pos);

private:
	u32			   m_NextID;
	ElementPairMap m_Elements;
};
class FESimulatedCanvas : public FECanvas
{
public:
	FESimulatedCanvas();
	~FESimulatedCanvas();

	struct SimulatedSettings
	{
		std::string m_RootDir;
		u32			m_PitchType;
		u32			m_TeamState;
		Array<u32>	m_HomePlayerPosition;
		Array<u32>	m_AwayPlayerPosition;
	};
	void Setup(const SimulatedSettings& setup);
	void Stop();

	virtual void Move(const Vec2& pos);
	virtual void Create(){}
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();
	virtual void Select(const Vec2& pos);

protected:
	FESimulatedBall*		  m_SimBall;
	Array<FEEditableElement*> m_Elements;
};

class FERealGameCanvas : public FECanvas
{
public:
	FERealGameCanvas();
	~FERealGameCanvas();

	void Setup(const RealGameInfo& rgInfo);
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
		EEditorMode_Simulation,
		EEditorMode_RealGame
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
	enum{
		ECopyDirection_L2N,
		ECopyDirection_N2L,
	};
public:
	FormationEditor();
	~FormationEditor();

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

	Bool IsEditor() const				{ return m_Mode.As() == EEditorMode_Edit;		}
	Bool IsSimulating() const			{ return m_Mode.As() == EEditorMode_Simulation;	}
	Bool IsInRealGame() const			{ return m_Mode.As() == EEditorMode_RealGame;	}

	void SetIsHomeAttacking(bool val)	{ m_IsHomeAttacking = val;	}

	void Clear();
	Bool StartSimulation(const FESimulatedCanvas::SimulatedSettings& setup);
	Bool StopSimulation();
	void AddElementPair();
	void SetCurrentCanvas(u32 pitchType, u32 teamState, u32 pos);

	Bool StartRealGame(const RealGameInfo& rgInfo);
	Bool StopRealGame();

	void ShowRef(bool show, u32 pitchType, u32 teamState, u32 pos);

	bool CanvasCopy(u32 dir, bool isAll = true, s32 teamState = -1, s32 pos = -1);

private:
	void CanvasCopySingle(FEEditorCanvas* to, const FEEditorCanvas* from);

	void onSIM_SelectRefCanvas(const Event* _poEvent);
	void onSIM_SelectRefCanvasInRealGame(const Event* _poEvent);

private:
	AI::RFInt			m_Mode;
	FESimulatedCanvas*	m_SimulatedCanvas;
	FERealGameCanvas*	m_RealGameCanvas;
	FEEditorCanvas*		m_SimulatedRefCanvas;
	FEEditorCanvas*		m_CurrentCanvas;
	FEEditorCanvas*		m_CurrentRefCanvas;
	FEEditorCanvas*		m_Canvases[EPitchType_Num][ETeamState_Num][kMaxPositionCount]; 

	AI::RFInt			m_RefPlayerPositionInTB[10];
	AI::RFBool			m_IsHomeAttacking;
};
//----------------------------------------------------------------------------
class FEDebuggerInfo : public DrawableObject
{
	static const int	kGridWidth = 5;
	static const int	kGridLength = 3;

public:
	FEDebuggerInfo();

	virtual void Create(){};
	virtual void Tick(f32 _fDeltaTime);
	virtual void Draw();

	void SetSelectedPosition(s32 pos) { m_SelectedPosition = pos;	}

private:
	f32   m_Turn;
	bool  m_GoUp;
	s32	  m_SelectedPosition;
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