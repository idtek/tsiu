#include "ATGlobalDef.h"

//--------------------------------------------------------------------------------------------
Engine*	g_poEngine	= NULL;
SimpleRenderObjectUtility*	g_poSROU = NULL;
//---------------------------------------------------------------------------------------------
#define WRITABLE_APP

#ifdef WRITABLE_APP
class TestObject : public DrawableObject
{
public:
	TestObject()
		: m_X("x1", 0)
		, m_Y("y1", 0)
		, m_cX("x2", 0)
		, m_cY("y2", 0)
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
		g_poSROU->DrawFillCircle((f32)m_cX.As(), (f32)m_cY.As(), 10, D_Color(0, 255, 0), D_Color(0, 255, 0));
	}
	void SetNewXY(int x, int y)
	{
		m_X = x;
		m_Y = y;
	}
private:
	AI::RFInt m_X;
	AI::RFInt m_Y;
	AI::RFCInt m_cX;
	AI::RFCInt m_cY;
};
#else
class TestObject : public DrawableObject
{
public:
	TestObject()
		: m_cX("x1", 0)
		, m_cY("y1", 0)
		, m_X("x2", 0)
		, m_Y("y2", 40)
	{
	}	
	virtual void Create()
	{}
	virtual void Tick(f32 _fDeltaTime)
	{
		AI::RefValueManager::Get().Flush();

		m_X = m_X.As() + (100 * _fDeltaTime);
		if(m_X.As() > GameEngine::GetGameEngine()->GetRenderMod()->GetRenderer()->GetWidth())
		{
			m_X = 0;
		}
	}
	virtual void Draw()
	{
		g_poSROU->DrawFillCircle((f32)m_X.As(), (f32)m_Y.As(), 10, D_Color(0, 255, 0), D_Color(0, 255, 0));
		g_poSROU->DrawFillCircle((f32)m_cX.As(), (f32)m_cY.As(), 10, D_Color(255, 0, 0), D_Color(255, 0, 0));
	}
private:
	AI::RFCInt m_cX;
	AI::RFCInt m_cY;
	AI::RFInt m_X;
	AI::RFInt m_Y;
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