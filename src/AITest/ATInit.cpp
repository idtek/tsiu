#include "ATGlobalDef.h"

//--------------------------------------------------------------------------------------------
Engine*	g_poEngine	= NULL;
SimpleRenderObjectUtility*	g_poSROU = NULL;
//---------------------------------------------------------------------------------------------
#define WRITABLE_APP

#ifdef WRITABLE_APP
class TestObject : public Object
{
public:
	TestObject()
		: m_X("x", 0)
		, m_Y("y", 0)
	{
	}	
	virtual void Create()
	{}
	virtual void Tick(f32 _fDeltaTime)
	{
		AI::RefValueManager::Get().Flush();
	}
	void SetNewXY(int x, int y)
	{
		m_X = x;
		m_Y = y;
	}
private:
	AI::RefValue<int, AI::ERefValuFlag_Writable> m_X;
	AI::RefValue<int, AI::ERefValuFlag_Writable> m_Y;
};
#else
class TestObject : public DrawableObject
{
public:
	TestObject()
		: m_X("x", 0)
		, m_Y("y", 0)
	{
	}	
	virtual void Create()
	{}
	virtual void Tick(f32 _fDeltaTime)
	{
		AI::RefValueManager::Get().Flush();
	}
	virtual void Draw()
	{
		g_poSROU->DrawFillCircle((f32)m_X.As(), (f32)m_Y.As(), 10, D_Color(255, 0, 0), D_Color(255, 0, 0));
	}
private:
	AI::RefValue<int, AI::ERefValuFlag_ReadOnly> m_X;
	AI::RefValue<int, AI::ERefValuFlag_ReadOnly> m_Y;
};
#endif
class TestWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	virtual void OnMouseLDrag(s32 x, s32 y, s32 relX, s32 relY)
	{
#ifdef WRITABLE_APP
		TestObject* pObj = GameEngine::GetGameEngine()->GetSceneMod()->GetSceneObject<TestObject>("TestObject");
		pObj->SetNewXY(x, y);
#endif
	}
};
//---------------------------------------------------------------------------------------------
GameEngine::GameEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow)
	:Engine(_uiWidth, _uiHeight, _strTitle, _bIsWindow, 60)
{
}
void GameEngine::DoInit()
{
	RenderWindowMsg::RegisterMsgListener(new TestWindowMsgCallBack);
	g_poSROU = new WinGDISimpleRenderObjectUtility((WinGDIRenderer*)GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer());
	GameEngine::GetGameEngine()->GetSceneMod()->AddObject("TestObject", new TestObject);
}

void GameEngine::DoUnInit()
{
}