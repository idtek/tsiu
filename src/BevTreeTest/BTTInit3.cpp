#include "BTTGlobalDef.h"
#include <time.h>
#include "TAI_BevTree2.h"

//--------------------------------------------------------------------------------------------
Engine*	g_poEngine	= NULL;
SimpleRenderObjectUtility*	g_poSROU = NULL;
//---------------------------------------------------------------------------------------------
using namespace TsiU::AI::BehaviorTree;

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
//----------------------------------------------------------------------------------
/*class TestTask : public Task
{
public:
	TestTask(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
		D_Output("Test init\n");
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		D_Output("Test update\n");
		return k_BRS_Finish;
	}
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
		D_Output("Test terminate\n");
	}
};
class TestTask2 : public Task
{
public:
	TestTask2(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
		D_Output("Test2 init\n");
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		const BevInputData& inputData = inputParam.GetRealDataType<BevInputData>();
		if(inputData.m_ConditionValue != 2)
			return k_BRS_Failure;

		D_Output("Test2 update\n");
		return k_BRS_Finish;
	}
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
		D_Output("Test2 terminate\n");
	}
};
class TestTask3 : public Task
{
public:
	TestTask3(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
		D_Output("Test3 init\n");
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		const BevInputData& inputData = inputParam.GetRealDataType<BevInputData>();
		if(inputData.m_ConditionValue != 3)
			return k_BRS_Failure;

		D_Output("Test3 update\n");
		return k_BRS_Finish;
	}
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
		D_Output("Test3 terminate\n");
	}
};

DEF_TERMINATE_NODE(Test,  TestTask);
DEF_TERMINATE_NODE(Test2, TestTask2);
DEF_TERMINATE_NODE(Test3, TestTask3);*/
//----------------------------------------------------------------------------------
// Behavior task
class TASK_MoveTo : public Task
{
public:
	TASK_MoveTo(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		const BevInputData& inputData = inputParam.GetRealDataType<BevInputData>();
		BevOutputData& outputData = outputParam.GetRealDataType<BevOutputData>();
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
			Vec2 curFacing = inputData.m_CurrentFacing;
			dir.Nomalize();
			f32 dotValue = dir.DotProduct(curFacing);
			f32 angle = Math::ACos(Math::Clamp<f32>(dotValue, -1.f, 1.f));
			if(angle >= 0.1f)
			{
				return k_BRS_Failure;
			}
			Vec2 nextPos2D = curPosition2D + dir * timeStep * 100.f;
			outputData.m_NextPosition = Vec3(nextPos2D.x, nextPos2D.y, 0);
		}
		return k_BRS_Executing;
	}
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
	}
};
DEF_TERMINATE_NODE(MoveTo, TASK_MoveTo);

class TASK_TurnTo : public Task
{
public:
	TASK_TurnTo(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		const BevInputData& inputData = inputParam.GetRealDataType<BevInputData>();
		BevOutputData& outputData = outputParam.GetRealDataType<BevOutputData>();

		f32 timeStep = inputData.m_TimeStep;
		Vec2 targetPoint2D = inputData.m_TargetPosition2D;
		Vec3 curPosition3D = inputData.m_Owner->GetPosition();
		Vec2 curPosition2D(curPosition3D.x, curPosition3D.y);
		Vec2 dir = targetPoint2D - curPosition2D;
		if(Math::IsZero(dir.LengthSq()))
		{
			return k_BRS_Failure;
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
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
	}
};
DEF_TERMINATE_NODE(TurnTo, TASK_TurnTo);

class TASK_Breathe : public Task
{
public:
	TASK_Breathe(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
		m_IsIncreasing = true;
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		const BevInputData& inputData = inputParam.GetRealDataType<BevInputData>();
		BevOutputData& outputData = outputParam.GetRealDataType<BevOutputData>();

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
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
	}
private:
	bool m_IsIncreasing;
};
DEF_TERMINATE_NODE(Breathe, TASK_Breathe);

class TASK_Idle : public Task
{
public:
	TASK_Idle(Node* pNode)
		:Task(pNode)
	{}
	virtual void OnInit(const BevNodeInputParam& inputParam)
	{
		m_WaitingTime = 0.5f;
	}
	virtual BevRunningStatus OnUpdate(const BevNodeInputParam& inputParam, BevNodeOutputParam& outputParam)
	{
		const BevInputData& inputData = inputParam.GetRealDataType<BevInputData>();
		BevOutputData& outputData = outputParam.GetRealDataType<BevOutputData>();

		f32 timeStep = inputData.m_TimeStep;
		m_WaitingTime -= timeStep;
		if(m_WaitingTime < 0)
		{
			outputData.m_BodyColor = D_Color(rand() % 256, rand() % 256, rand() % 256);
			return k_BRS_Finish;
		}
		return k_BRS_Executing;
	}
	virtual void OnTerminate(const BevNodeInputParam& inputParam)
	{
	}
private:
	float m_WaitingTime;
};
DEF_TERMINATE_NODE(Idle, TASK_Idle);
//----------------------------------------------------------------------------------
class TestObject : public DrawableObject
{
	static Node* sBevTree;

public:
	TestObject()
	{
		SetPosition(0,0,0);
		m_BodyColor = D_Color(0, 255, 0);
		m_BodySize = 10;
		m_TimeToFindNewTargetPos = -1;
		m_Facing = Vec2(0, 1);
	}	
	~TestObject()
	{
		m_BevTreeRoot.Uninstall();
		D_SafeDelete(sBevTree);
	}
	virtual void Create()
	{
		if(!sBevTree)
		{
			CompositeNode* pSe = new CompositeNode_Sequence();
				pSe->AddChild(CREATE_TERMINATE_NODE(TurnTo));
				pSe->AddChild(CREATE_TERMINATE_NODE(MoveTo));
			CompositeNode* pPa = new CompositeNode_Parallel();
				pPa->AddChild(pSe);
				pPa->AddChild(CREATE_TERMINATE_NODE(Breathe));
			CompositeNode* pRo = new CompositeNode_Selector();
				pRo->AddChild(pPa);
				pRo->AddChild(CREATE_TERMINATE_NODE(Idle));
			sBevTree = pRo;
		}
		m_BevTreeRoot.Install(*sBevTree);
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
		m_BevTreeInputData.m_CurrentFacing = m_Facing;
		m_BevTreeOutputdata.m_NextFacing = m_Facing;

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

		m_BevTreeRoot.Update(input, output);

		//update object
		m_BodySize = m_BevTreeOutputdata.m_BodySize;
		m_BodyColor = m_BevTreeOutputdata.m_BodyColor;
		SetPosition(m_BevTreeOutputdata.m_NextPosition);
		m_Facing = m_BevTreeOutputdata.m_NextFacing;
	}
	virtual void Draw()
	{
		Vec3 curPosition3D = GetPosition();

		g_poSROU->DrawFillCircle((f32)m_BevTreeInputData.m_TargetPosition2D.x, (f32)m_BevTreeInputData.m_TargetPosition2D.y, 15, D_Color(128, 128, 128), D_Color(128, 128, 128));
		g_poSROU->DrawLine((f32)m_BevTreeInputData.m_TargetPosition2D.x, (f32)m_BevTreeInputData.m_TargetPosition2D.y, 
			(f32)curPosition3D.x, (f32)curPosition3D.y,
			D_Color(128, 128, 128), 1);
		g_poSROU->DrawFillCircle((f32)curPosition3D.x, (f32)curPosition3D.y, m_BodySize, m_BodyColor, m_BodyColor);
		Vec3 nosePosition3D = curPosition3D + Vec3(m_Facing.x, m_Facing.y, 0) * 20;
		g_poSROU->DrawLine((f32)curPosition3D.x, (f32)curPosition3D.y, (f32)nosePosition3D.x, (f32)nosePosition3D.y, m_BodyColor, 1);
	}
protected:
	Vec2			m_Facing;
	D_Color			m_BodyColor;
	float			m_BodySize;
	Behavior	m_BevTreeRoot;
	BevInputData	m_BevTreeInputData;
	BevOutputData	m_BevTreeOutputdata;
	float			m_TimeToFindNewTargetPos;
};
Node* TestObject::sBevTree(0);

class Level : public DrawableObject
{
public:
	Level()
	{}
	~Level()
	{}
protected:
	virtual void Create()
	{
		for(int i = 0; i < 10; ++i)
		{
			TestObject* pObj = new TestObject();
			pObj->Create();
			m_Objs.push_back(pObj);
		}
	}
	virtual void Tick(f32 _fDeltaTime)
	{
		for(int i = 0; i < (int)m_Objs.size(); ++i)
		{
			m_Objs[i]->Tick(_fDeltaTime);
		}
	}
	virtual void Draw()
	{
		for(int i = 0; i < (int)m_Objs.size(); ++i)
		{
			m_Objs[i]->Draw();
		}
		int height = 600;
		WinGDIRenderer* pRender = (WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer();
		if(pRender && pRender->GetMainWindow())
		{
			height = pRender->GetHeight();
		}
		char info[256] = {0};
		sprintf_s(info, 256, "Total: %d (click to add 10 objects), Memory Used: %.2f KB", m_Objs.size(), GetMemUsed() / 1024.f);
		g_poSROU->DrawString(3, height - 50, info, D_Color(255, 255, 255));
	}
public:
	void AddObj()
	{
		for(int i = 0; i < 10; ++i)
		{
			TestObject* pObj = new TestObject();
			pObj->Create();
			m_Objs.push_back(pObj);
		}
	}

private:
	std::vector<TestObject*> m_Objs;
};

//---------------------------------------------------------------------------------------------
class TestWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnMouseLDown(s32 x, s32 y)
	{
		Level* pObj = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<Level>("Level");
		pObj->AddObj();
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