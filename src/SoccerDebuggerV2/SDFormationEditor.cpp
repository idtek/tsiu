#include "SDFormationEditor.h"

//----------------------------------------------------------
void ORefValueUpdater::Tick(f32 _fDeltaTime)
{
	AI::RefValueManager::Get().Flush();
}
//----------------------------------------------------------
f32 CoordinateInfo::sLength = 0.f;
f32 CoordinateInfo::sWidth = 0.f;

f32 CoordinateInfo::GetPixelPerMeter()
{
	f32 xAxis = (kCANVAS_WIDTH - (kWATCH_WIDTH + kCANVAS_START_X * 2 + 16)) / sLength;
	f32 yAxis = (kCANVAS_HEIGHT - (kCANVAS_START_Y + 32)) / sWidth;
	return (xAxis > yAxis ? yAxis : xAxis);
}
f32 CoordinateInfo::GetMeterPerPixel()
{
	f32 xAxis = sLength / (kCANVAS_WIDTH - (kWATCH_WIDTH + kCANVAS_START_X * 2 + 16));
	f32 yAxis = sWidth / (kCANVAS_HEIGHT - (kCANVAS_START_Y + 32));
	return (xAxis > yAxis ? xAxis : yAxis);
}
Vec2 CoordinateInfo::WorldToScreen(const Vec2& worldPos)
{
	Vec2 screenPos;
	screenPos.x = kCANVAS_START_X + (worldPos.x + sLength / 2.f) * GetPixelPerMeter();
	screenPos.y = kCANVAS_START_Y + (sWidth / 2.f - worldPos.y) * GetPixelPerMeter();
	return screenPos;
}
Vec2 CoordinateInfo::ScreenToWorld(const Vec2& screenPos)
{
	Vec2 worldPos;
	worldPos.x = (screenPos.x - kCANVAS_START_X) * GetMeterPerPixel() -  sLength / 2.f;
	worldPos.y = sWidth / 2.f - (screenPos.y - kCANVAS_START_Y) * GetMeterPerPixel();
	return worldPos;
}

//----------------------------------------------------------
FEElement::FEElement()
	: m_Flag(0)
{
}
FEElement::~FEElement()
{
}
void FEElement::Select(const Vec2& pos)
{
	D_AddFlag(m_Flag, EFEFlag_Selected);
}
void FEElement::Deselect(const Vec2& pos)
{
	D_RemoveFlag(m_Flag, EFEFlag_Selected);
}
void FEElement::Move(const Vec2& pos)
{
	SetPosition(Vec3::FromVec2(pos));
}
//----------------------------------------------------------
FERefBall::FERefBall()
	: m_RfPosition("RefBall", Vec2()) 
{
	m_fBallRadius = 0.9f;
}
void FERefBall::Create()
{
}
void FERefBall::Tick(f32 _fDeltaTime)
{
	m_vPos = Vec3::FromVec2(m_RfPosition.As());
}
void FERefBall::Draw()
{
	D_Color bodyColor(255,128,64);
	if(D_TestFlag(m_Flag, EFEFlag_Selected))
		bodyColor = bodyColor / 1.5f;

	Vec2 screenPos = CoordinateInfo::WorldToScreen(Vec2::FromVec3(m_vPos));
	g_poSROU->DrawFillCircle((f32)screenPos.x, (f32)screenPos.y, m_fBallRadius * CoordinateInfo::GetPixelPerMeter(), bodyColor, D_Color(255, 255, 255));

	Vec2 indicatorPos(m_vPos.x - m_fBallRadius, m_vPos.y + m_fBallRadius);
	Vec2 indicatorScreenPos = CoordinateInfo::WorldToScreen(indicatorPos);
	Char indicator[] = "B";
	g_poSROU->DrawStringEx(
		indicatorScreenPos.x, 
		indicatorScreenPos.y,
		2 * m_fBallRadius * CoordinateInfo::GetPixelPerMeter(), 
		2 * m_fBallRadius * CoordinateInfo::GetPixelPerMeter(),
		kPLAYER_NUMSIZE * CoordinateInfo::GetPixelPerMeter(),
		indicator, NULL, D_Color(0,0,0));
}
void FERefBall::Move(const Vec2& pos)
{
	Vec2 unclamppedPos = pos;
	unclamppedPos.x = Math::Clamp(unclamppedPos.x, -CoordinateInfo::sLength / 2.f, CoordinateInfo::sLength / 2.f);
	unclamppedPos.y = Math::Clamp(unclamppedPos.y, -CoordinateInfo::sWidth / 2.f, CoordinateInfo::sWidth / 2.f);
	m_RfPosition = unclamppedPos;
}
bool FERefBall::InBounds(const Vec2& checkPoint) const
{
	Vec2 pos2D = Vec2::FromVec3(m_vPos);
	if((checkPoint - pos2D).LengthSq() <= Math::Square(m_fBallRadius))
	{
		return true;
	}
	return false;
}
//----------------------------------------------------------
FERefPlayer::FERefPlayer(s32 id)
	: m_id(id)
{
	Char refName[16] = {0};
	sprintf(refName, "Player%d_Pos", m_id);
	m_RfPosition.RegisterValue(refName,Vec2());
	m_fPlayerRadius = 0.9f;
}
void FERefPlayer::Create()
{
}
void FERefPlayer::Tick(f32 _fDeltaTime)
{
	m_vPos = Vec3::FromVec2(m_RfPosition.As());
}
void FERefPlayer::Draw()
{
	D_Color bodyColor(255, 64, 64);
	if(D_TestFlag(m_Flag, EFEFlag_Selected))
		bodyColor = bodyColor * 1.5f;

	Vec2 screenPos = CoordinateInfo::WorldToScreen(Vec2::FromVec3(m_vPos));
	g_poSROU->DrawFillCircle((f32)screenPos.x, (f32)screenPos.y, m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(), bodyColor, D_Color(255, 255, 255));

	Vec2 indicatorPos(m_vPos.x - m_fPlayerRadius, m_vPos.y + m_fPlayerRadius);
	Vec2 indicatorScreenPos = CoordinateInfo::WorldToScreen(indicatorPos);
	Char indicator[4];
	itoa(m_id, indicator, 10);
	g_poSROU->DrawStringEx(
		indicatorScreenPos.x, 
		indicatorScreenPos.y,
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(), 
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(),
		kPLAYER_NUMSIZE * CoordinateInfo::GetPixelPerMeter(),
		indicator, NULL, D_Color(0,0,0));
}
//----------------------------------------------------------
FECanvas::FECanvas()
	: m_SelectedElements(NULL)
{}
void FECanvas::Create()
{
	static Bool enter_once = false;
	if(enter_once == false)
	{
		FEElement* playerRef = new FERefPlayer(0);
		playerRef->Create();
		playerRef->SetPosition(0.f, 0.f, 0.f);
		m_Elements.PushBack(playerRef);

		FEElement* ballRef = new FERefBall;
		ballRef->Create();
		ballRef->SetPosition(0.f, 0.f, 0.f);
		m_Elements.PushBack(ballRef);

		enter_once = true;
	}
}
void FECanvas::Tick(f32 _fDeltaTime)
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		m_Elements[i]->Tick(_fDeltaTime);
	}
}
void FECanvas::Draw()
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		m_Elements[i]->Draw();
	}
}
void FECanvas::Select(const Vec2& pos)
{
	m_SelectedElements = NULL;
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		if(m_Elements[i]->InBounds(pos))
		{
			m_SelectedElements = m_Elements[i];
			break;
		}
	}
	if(m_SelectedElements)
		m_SelectedElements->Select(pos);
}
void FECanvas::Move(const Vec2& pos)
{
	if(m_SelectedElements)
	{
		m_SelectedElements->Move(pos);
	}
}
void FECanvas::Deselect(const Vec2& pos)
{
	if(m_SelectedElements)
		m_SelectedElements->Deselect(pos);
	m_SelectedElements = NULL;
}

//----------------------------------------------------------
FormationEditor::FormationEditor()
	: m_CurrentCanvas(NULL)
{
	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				m_Canvases[i][j][k] = new FECanvas;
			}
		}
	}
	SetCurrentCanvas(0, 0, 0);

	CoordinateInfo::sLength = kPithLenghNormal;
	CoordinateInfo::sWidth = kPitchWidthNormal;
}
void FormationEditor::Create()
{
	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				m_Canvases[i][j][k]->Create();
			}
		}
	}
}
void FormationEditor::Tick(f32 _fDeltaTime)
{
	if(m_CurrentCanvas)
		m_CurrentCanvas->Tick(_fDeltaTime);
}
void FormationEditor::Draw()
{
	if(m_CurrentCanvas)
		m_CurrentCanvas->Draw();
}
void FormationEditor::Select(const Vec2& pos)
{
	if(m_CurrentCanvas)
	{
		m_CurrentCanvas->Select(pos);
	}
}
void FormationEditor::Move(const Vec2& pos)
{
	if(m_CurrentCanvas)
	{
		m_CurrentCanvas->Move(pos);
	}
}
void FormationEditor::Deselect(const Vec2& pos)
{
	if(m_CurrentCanvas)
	{
		m_CurrentCanvas->Deselect(pos);
	}
}
void FormationEditor::SetCurrentCanvas(u32 pitchType, u32 teamState, u32 pos)
{
	if(pitchType != EPitchType_Normal && pitchType != EPitchType_Large)
		return;

	if(teamState != ETeamState_Attack && pitchType != ETeamState_Defend)
		return;

	if(pos < 0 || pos >= kMaxPositionCount)
		return;

	m_CurrentCanvas = m_Canvases[pitchType][teamState][pos];
}
//----------------------------------------------------------------
void FEWindowMsgCallBack::OnMouseLDown(s32 x, s32 y)
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	Vec2 worldPos = CoordinateInfo::ScreenToWorld(Vec2(x, y));
	pEditor->Select(worldPos);
}
void FEWindowMsgCallBack::OnMouseLUp(s32 x, s32 y)
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	Vec2 worldPos = CoordinateInfo::ScreenToWorld(Vec2(x, y));
	pEditor->Deselect(worldPos);
}
void FEWindowMsgCallBack::OnMouseLDrag(s32 x, s32 y, s32 relX, s32 relY)
{
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	Vec2 worldPos = CoordinateInfo::ScreenToWorld(Vec2(x, y));
	pEditor->Move(worldPos);
}
void FEWindowMsgCallBack::OnKeyDown(u32 key, s64 info)
{
}