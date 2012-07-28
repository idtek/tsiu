#include "BTTGlobalDef.h"
#include <time.h>
#include "TAI_BevTree.h"

//--------------------------------------------------------------------------------------------
Engine*	g_poEngine	= NULL;
SimpleRenderObjectUtility*	g_poSROU = NULL;
//---------------------------------------------------------------------------------------------
struct BevInputData 
{
	f32		m_TimeStep;
	Vec2	m_TargetPosition2D;
	Object*	m_Owner;
	Vec2	m_CurrentFacing;
};
struct BevOutputData
{
	D_Color m_BodyColor;
	f32     m_BodySize;
	Vec3	m_NextPosition;
	Vec2	m_NextFacing;
};

using namespace TsiU::AI::BehaviorTree;

class CON_HasReachedTarget : public BevNodePrecondition
{
public:
	virtual bool ExternalCondition(const BevNodeInputParam& input) const
	{
		const BevInputData&  inputData	= input.GetRealDataType<BevInputData>();
		Vec2 targetPoint2D = inputData.m_TargetPosition2D;
		Vec3 curPosition3D = inputData.m_Owner->GetPosition();
		Vec2 curPosition2D(curPosition3D.x, curPosition3D.y);
		Vec2 dir = targetPoint2D - curPosition2D;
		if(dir.LengthSq() < 0.5f)
		{
			return true;
		}
		return false;
	}
};
class CON_HasFacedToTarget : public BevNodePrecondition
{
public:
	virtual bool ExternalCondition(const BevNodeInputParam& input) const
	{
		const BevInputData& inputData = input.GetRealDataType<BevInputData>();
		f32 timeStep = inputData.m_TimeStep;
		Vec2 targetPoint2D = inputData.m_TargetPosition2D;
		Vec3 curPosition3D = inputData.m_Owner->GetPosition();
		Vec2 curPosition2D(curPosition3D.x, curPosition3D.y);
		Vec2 dir = targetPoint2D - curPosition2D;
		if(Math::IsZero(dir.LengthSq()))
		{
			return true;
		}
		else
		{
			Vec2 curFacing = inputData.m_CurrentFacing;
			dir.Nomalize();
			f32 dotValue = dir.DotProduct(curFacing);
			f32 angle = Math::ACos(Math::Clamp<f32>(dotValue, -1.f, 1.f));
			if(angle < 0.1f)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;
	}
};

class NOD_MoveTo : public BevNodeTerminal
{
public:
	NOD_MoveTo(BevNode* _o_ParentNode)
		:BevNodeTerminal(_o_ParentNode)
	{}
protected:
	virtual BevRunningStatus _DoExecute(const BevNodeInputParam& input, BevNodeOutputParam& output)	
	{
		const BevInputData& inputData = input.GetRealDataType<BevInputData>();
		BevOutputData& outputData = output.GetRealDataType<BevOutputData>();

		f32 timeStep = inputData.m_TimeStep;
		Vec2 targetPoint2D = inputData.m_TargetPosition2D;
		Vec3 curPosition3D = inputData.m_Owner->GetPosition();

		Vec2 curPosition2D(curPosition3D.x, curPosition3D.y);
		Vec2 dir = targetPoint2D - curPosition2D;
		if(dir.LengthSq() < 2.f)
		{
			outputData.m_NextPosition = Vec3(targetPoint2D.x, targetPoint2D.y, 0);
			return k_BRS_Finish;
		}
		else
		{
			dir.Nomalize();
			Vec2 nextPos2D = curPosition2D + dir * timeStep * 100.f;
			outputData.m_NextPosition = Vec3(nextPos2D.x, nextPos2D.y, 0);
		}
		return k_BRS_Executing;
	}
};

class NOD_TurnTo : public BevNodeTerminal
{
public:
	NOD_TurnTo(BevNode* _o_ParentNode)
		:BevNodeTerminal(_o_ParentNode)
	{}
protected:
	virtual BevRunningStatus _DoExecute(const BevNodeInputParam& input, BevNodeOutputParam& output)	
	{
		const BevInputData& inputData = input.GetRealDataType<BevInputData>();
		BevOutputData& outputData = output.GetRealDataType<BevOutputData>();

		f32 timeStep = inputData.m_TimeStep;
		Vec2 targetPoint2D = inputData.m_TargetPosition2D;
		Vec3 curPosition3D = inputData.m_Owner->GetPosition();
		Vec2 curPosition2D(curPosition3D.x, curPosition3D.y);
		Vec2 dir = targetPoint2D - curPosition2D;
		if(Math::IsZero(dir.LengthSq()))
		{
			return k_BRS_Finish;
		}
		else
		{
			Vec2 curFacing = inputData.m_CurrentFacing;
			dir.Nomalize();
			f32 dotValue = dir.DotProduct(curFacing);
			f32 angle = Math::ACos(Math::Clamp<f32>(dotValue, -1.f, 1.f));
			if(angle < 0.1f)
			{
				outputData.m_NextFacing = dir;
				return k_BRS_Finish;
			}
			Vec3 vA(curFacing.x, curFacing.y, 0);
			Vec3 vB(dir.x, dir.y, 0);
			Vec3 vC = vA.CrossProduct(vB);

			f32 angleToTurn = Math::Min(timeStep * 3.f, angle);
			if(vC.z < 0)
				angleToTurn = -angleToTurn;
			Quat rotAxis( angleToTurn, Vec3::ZUNIT );
			Vec3 ret = vA * rotAxis;
			outputData.m_NextFacing = Vec2(ret.x, ret.y);
		}
		return k_BRS_Executing;
	}
};

class NOD_Idle : public BevNodeTerminal
{
public:
	NOD_Idle(BevNode* _o_ParentNode)
		:BevNodeTerminal(_o_ParentNode)
	{}
protected:
	virtual void _DoEnter(const BevNodeInputParam& input)
	{
		m_WaitingTime = 0.5f;
	}
	virtual BevRunningStatus _DoExecute(const BevNodeInputParam& input, BevNodeOutputParam& output)	
	{
		const BevInputData& inputData = input.GetRealDataType<BevInputData>();
		BevOutputData& outputData = output.GetRealDataType<BevOutputData>();

		f32 timeStep = inputData.m_TimeStep;
		m_WaitingTime -= timeStep;
		if(m_WaitingTime < 0)
		{
			outputData.m_BodyColor = D_Color(rand() % 256, rand() % 256, rand() % 256);
			return k_BRS_Finish;
		}
		return k_BRS_Executing;
	}
private:
	float m_WaitingTime;
};

class NOD_Breathe : public BevNodeTerminal
{
public:
	NOD_Breathe(BevNode* _o_ParentNode)
		:BevNodeTerminal(_o_ParentNode)
	{}
protected:
	virtual BevRunningStatus _DoExecute(const BevNodeInputParam& input, BevNodeOutputParam& output)	
	{
		const BevInputData& inputData = input.GetRealDataType<BevInputData>();
		BevOutputData& outputData = output.GetRealDataType<BevOutputData>();

		f32 timeStep = inputData.m_TimeStep;
		if(m_IsIncreasing)
		{
			outputData.m_BodySize += (timeStep * 10);
			if(outputData.m_BodySize > 12){
				outputData.m_BodySize = 12;
				m_IsIncreasing = false;
			}
		}
		else
		{
			outputData.m_BodySize -= (timeStep * 10);
			if(outputData.m_BodySize < 10){
				outputData.m_BodySize = 10;
				m_IsIncreasing = true;
			}
		}
		return k_BRS_Executing;
	}
private:
	bool m_IsIncreasing;
};

class TestObject : public DrawableObject
{
public:
	TestObject()
		: m_BevTreeRoot(NULL)
	{
		SetPosition(0,0,0);
		m_BodyColor = D_Color(0, 255, 0);
		m_BodySize = 10;
		m_TimeToFindNewTargetPos = -1;
	}	
	virtual void Create()
	{
		//init bev tree
		BevNode& ret = 
			BevNodeFactory::oCreatePrioritySelectorNode(NULL, "root");
		BevNodeFactory::oCreateTeminalNode<NOD_MoveTo>(&ret, "move to")
			.SetNodePrecondition(new BevNodePreconditionNOT(new CON_HasReachedTarget()));
		BevNodeFactory::oCreateTeminalNode<NOD_Idle>(&ret, "idle")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		m_BevTreeRoot = &ret;
	}
	virtual void Tick(f32 _fDeltaTime)
	{
		int width = 800;
		int height = 600;
		WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
		if(pRender && pRender->GetMainWindow())
		{
			width = pRender->GetWidth();
			height = pRender->GetHeight();
		}
		//preparing data
		m_BevTreeInputData.m_Owner = this;
		m_BevTreeInputData.m_TimeStep = _fDeltaTime;

		m_TimeToFindNewTargetPos -= _fDeltaTime;
		if(m_TimeToFindNewTargetPos <= 0)
		{
			m_BevTreeInputData.m_TargetPosition2D = Vec2(rand() % width, rand() % (height - 100));
			m_TimeToFindNewTargetPos = rand() % 5 + 2;
		}
		m_BevTreeOutputdata.m_BodySize = m_BodySize;
		m_BevTreeOutputdata.m_BodyColor = m_BodyColor;
		m_BevTreeOutputdata.m_NextPosition = GetPosition();

		//tick behavior tree
		BevNodeInputParam input(&m_BevTreeInputData);
		BevNodeOutputParam output(&m_BevTreeOutputdata);
		if(m_BevTreeRoot->Evaluate(input))
		{
			m_BevTreeRoot->Tick(input, output);
		}

		//update object
		m_BodySize = m_BevTreeOutputdata.m_BodySize;
		m_BodyColor = m_BevTreeOutputdata.m_BodyColor;
		SetPosition(m_BevTreeOutputdata.m_NextPosition);
	}
	virtual void Draw()
	{
		Vec3 curPosition3D = GetPosition();

		g_poSROU->DrawFillCircle((f32)m_BevTreeInputData.m_TargetPosition2D.x, (f32)m_BevTreeInputData.m_TargetPosition2D.y, 15, D_Color(128, 128, 128), D_Color(128, 128, 128));
		g_poSROU->DrawLine((f32)m_BevTreeInputData.m_TargetPosition2D.x, (f32)m_BevTreeInputData.m_TargetPosition2D.y, 
			(f32)curPosition3D.x, (f32)curPosition3D.y,
			D_Color(128, 128, 128), 1);
		g_poSROU->DrawFillCircle((f32)curPosition3D.x, (f32)curPosition3D.y, m_BodySize, m_BodyColor, m_BodyColor);

		//draw debug info
		const BevNode* activeNode = m_BevTreeRoot->oGetLastActiveNode();
		if(activeNode)
		g_poSROU->DrawString(3, 0, activeNode->GetDebugName(), D_Color(255, 255, 255));
		else
		g_poSROU->DrawString(3, 0, "No Active Node", D_Color(255, 255, 255));
		g_poSROU->DrawString(3, 550, GetInfo(), D_Color(255, 255, 255));
	}
	virtual const char* GetInfo(){
		return "Example1: priority selector(mouse click to next sample)";
	}
protected:
	D_Color			m_BodyColor;
	float			m_BodySize;
	BevNode*		m_BevTreeRoot;
	BevInputData	m_BevTreeInputData;
	BevOutputData	m_BevTreeOutputdata;
	float			m_TimeToFindNewTargetPos;
};
class TestObject2 : public TestObject
{
protected:
	virtual void Create()
	{
		//init bev tree
		BevNode& ret =	BevNodeFactory::oCreatePrioritySelectorNode(NULL, "root");
		BevNode& p =	BevNodeFactory::oCreateParallelNode(&ret, k_PFC_OR, "parallel")
			.SetNodePrecondition(new BevNodePreconditionNOT(new CON_HasReachedTarget()));
		BevNodeFactory::oCreateTeminalNode<NOD_MoveTo>(&p, "move to")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		BevNodeFactory::oCreateTeminalNode<NOD_Breathe>(&p,"breathing")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		BevNodeFactory::oCreateTeminalNode<NOD_Idle>(&ret, "idle")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		m_BevTreeRoot = &ret;
	}
	virtual const char* GetInfo(){
		return "Example2: priority selector & parallel(mouse click to next sample)";
	}
};
class TestObject3 : public TestObject
{
public:
	TestObject3()
		: TestObject()
	{
		m_Facing = Vec2(0, 1);
	}
public:
	virtual void Create()
	{
		//init bev tree
		BevNode& ret =	BevNodeFactory::oCreatePrioritySelectorNode(NULL, "root");
		BevNode& p =	BevNodeFactory::oCreateParallelNode(&ret, k_PFC_OR, "parallel")
			.SetNodePrecondition(new BevNodePreconditionNOT(new CON_HasReachedTarget()));
		BevNode& sq =	BevNodeFactory::oCreateSequenceNode(&p, "sequence");
		BevNodeFactory::oCreateTeminalNode<NOD_TurnTo>(&sq, "turn to")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		BevNodeFactory::oCreateTeminalNode<NOD_MoveTo>(&sq, "move to")
			.SetNodePrecondition(new CON_HasFacedToTarget());
		BevNodeFactory::oCreateTeminalNode<NOD_Breathe>(&p, "breathing")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		BevNodeFactory::oCreateTeminalNode<NOD_Idle>(&ret, "idle")
			.SetNodePrecondition(new BevNodePreconditionTRUE());
		m_BevTreeRoot = &ret;
	}
	virtual void Tick(f32 _fDeltaTime)
	{
		m_BevTreeInputData.m_CurrentFacing = m_Facing;
		m_BevTreeOutputdata.m_NextFacing = m_Facing;
		TestObject::Tick(_fDeltaTime);
		m_Facing = m_BevTreeOutputdata.m_NextFacing;
	}
	virtual void Draw()
	{
		TestObject::Draw();
		Vec3 curPosition3D = GetPosition();
		Vec3 nosePosition3D = curPosition3D + Vec3(m_Facing.x, m_Facing.y, 0) * 20;
		g_poSROU->DrawLine((f32)curPosition3D.x, (f32)curPosition3D.y, (f32)nosePosition3D.x, (f32)nosePosition3D.y, m_BodyColor, 1);
	}
	virtual const char* GetInfo(){
		return "Example3: priority selector & parallel & sequence(mouse click to next sample)";
	}
protected:
	Vec2 m_Facing;
};
class Level : public DrawableObject
{
	enum{
		LEVEL_1 = 0,
		LEVEL_2,
		LEVEL_3,
		LEVEL_NUM
	};
public:
	Level()
		:m_pCurrentObj(NULL)
	{
		m_CurrentLevel = -1;
	}
	~Level()
	{
		D_SafeDelete(m_pCurrentObj);
	}
	void ChangeLevel()
	{
		D_SafeDelete(m_pCurrentObj);

		m_CurrentLevel++;
		m_CurrentLevel %= LEVEL_NUM;
		switch (m_CurrentLevel)
		{
		case LEVEL_1:	{m_pCurrentObj = new TestObject();	break;}
		case LEVEL_2:	{m_pCurrentObj = new TestObject2();	break;}
		case LEVEL_3:	{m_pCurrentObj = new TestObject3();	break;}
		}
		D_CHECK(m_pCurrentObj);
		m_pCurrentObj->Create();
	}
protected:
	virtual void Create()
	{
		ChangeLevel();
	}
	virtual void Tick(f32 _fDeltaTime)
	{
		D_CHECK(m_pCurrentObj);
		m_pCurrentObj->Tick(_fDeltaTime);
	}
	virtual void Draw()
	{
		D_CHECK(m_pCurrentObj);
		m_pCurrentObj->Draw();
	}
private:
	s32 m_CurrentLevel;
	DrawableObject* m_pCurrentObj;
};
//---------------------------------------------------------------------------------------------
class TestWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnMouseLDown(s32 x, s32 y)
	{
		Level* pObj = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<Level>("Level");
		pObj->ChangeLevel();
	}
};
//---------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow)
:Engine(_uiWidth, _uiHeight, _strTitle, _bIsWindow, 60)
{
}
void GameEngine::DoInit()
{
	srand(time(NULL));

	RenderWindowMsg::RegisterMsgListener(new TestWindowMsgCallBack);
	g_poSROU = new WinGDISimpleRenderObjectUtility((WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer());
	GameEngine::GetGameEngine()->GetSceneMod()->AddObject("Level", new Level);
}
void GameEngine::DoUnInit()
{
}