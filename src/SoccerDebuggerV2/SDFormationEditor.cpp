#include "SDFormationEditor.h"
#include "fxkeys.h"
#include "tinyxml.h"
#include <sstream>

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
	: m_Flag(EFEFlag_Movable | EFEFlag_Deletable)
{
}
FEElement::~FEElement()
{
}
void FEElement::Select(const Vec2& pos)
{
	AddFlags(EFEFlag_Selected);
}
void FEElement::Deselect(const Vec2& pos)
{
	RemoveFlags(EFEFlag_Selected);
}
void FEElement::Move(const Vec2& pos)
{
	SetPosition(Vec3::FromVec2(pos));
}
void FEElement::AddFlags(u32 flags)
{
	D_AddFlag(m_Flag, flags);
}
void FEElement::RemoveFlags(u32 flags)
{
	D_RemoveFlag(m_Flag, flags);
}
Bool FEElement::HasFlags(u32 flags) const
{
	return D_TestFlag(m_Flag, flags) ? true : false;
}
Vec2 FEElement::ClampToPitch(const Vec2& unclamppedPos)
{
	Vec2 result = unclamppedPos;
	result.x = Math::Clamp(unclamppedPos.x, -CoordinateInfo::sLength / 2.f, CoordinateInfo::sLength / 2.f);
	result.y = Math::Clamp(unclamppedPos.y, -CoordinateInfo::sWidth / 2.f, CoordinateInfo::sWidth / 2.f);
	return result;
}
//----------------------------------------------------------
FEBall::FEBall(s32 id)
	: FEEditableElement(id)
{
	m_fBallRadius = 0.9f;
	m_Color = D_Color(255, 128, 64);
}
void FEBall::Create()
{
}
void FEBall::Tick(f32 _fDeltaTime)
{
}
void FEBall::Draw()
{
	D_Color bodyColor = m_Color;
	if(HasFlags(EFEFlag_Selected))
		bodyColor = bodyColor / 1.5f;

	Vec2 screenPos = CoordinateInfo::WorldToScreen(Vec2::FromVec3(m_vPos));
	g_poSROU->DrawFillCircle((f32)screenPos.x, (f32)screenPos.y, m_fBallRadius * CoordinateInfo::GetPixelPerMeter(), bodyColor, D_Color(0, 0, 0));

	Vec2 indicatorPos(m_vPos.x - m_fBallRadius, m_vPos.y + m_fBallRadius);
	Vec2 indicatorScreenPos = CoordinateInfo::WorldToScreen(indicatorPos);
	Char indicator[4];
	itoa(m_id, indicator, 10);
	g_poSROU->DrawStringEx(
		indicatorScreenPos.x, 
		indicatorScreenPos.y,
		2 * m_fBallRadius * CoordinateInfo::GetPixelPerMeter(), 
		2 * m_fBallRadius * CoordinateInfo::GetPixelPerMeter(),
		kPLAYER_NUMSIZE * CoordinateInfo::GetPixelPerMeter(),
		indicator, NULL, D_Color(0,0,0));
}
void FEBall::Move(const Vec2& pos)
{
	if(!HasFlags(EFEFlag_Movable))
		return;
	m_vPos = Vec3::FromVec2(ClampToPitch(pos));
}
bool FEBall::InBounds(const Vec2& checkPoint) const
{
	Vec2 pos2D = Vec2::FromVec3(m_vPos);
	if((checkPoint - pos2D).LengthSq() <= Math::Square(m_fBallRadius))
	{
		return true;
	}
	return false;
}
void FEBall::Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poBallElement = new TiXmlElement("Ball");
	_poParent->LinkEndChild(poBallElement);

	poBallElement->SetAttribute("id", m_id);
	poBallElement->SetAttribute("is_movable",	HasFlags(EFEFlag_Movable));
	poBallElement->SetAttribute("is_deletable", HasFlags(EFEFlag_Deletable));

	TiXmlElement* poPosElement = new TiXmlElement("Position");
	poBallElement->LinkEndChild(poPosElement);
	poPosElement->SetDoubleAttribute("x", m_vPos.x);
	poPosElement->SetDoubleAttribute("y", m_vPos.y);
}
void FEBall::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{

}

FESimulatedBall::FESimulatedBall(s32 id)
	: FEBall(id)
	, m_RfPosition("RefBall", Vec2())
{
}
void FESimulatedBall::Tick(f32 _fDeltaTime)
{
	m_vPos = Vec3::FromVec2(m_RfPosition.As());
}
void FESimulatedBall::Move(const Vec2& pos)
{
	if(!HasFlags(EFEFlag_Movable))
		return;
	m_RfPosition = ClampToPitch(pos);
}
//----------------------------------------------------------
FEPlayer::FEPlayer(s32 id)
	: FEEditableElement(id)
{
	m_fPlayerRadius = 0.9f;
	m_Color = D_Color(255, 64, 64);
}
void FEPlayer::Create()
{
}
void FEPlayer::Tick(f32 _fDeltaTime)
{
}
void FEPlayer::Draw()
{
	D_Color bodyColor = m_Color;
	if(HasFlags(EFEFlag_Selected))
		bodyColor = bodyColor / 1.5f;

	Vec2 playerPos(m_vPos.x - m_fPlayerRadius, m_vPos.y + m_fPlayerRadius);
	Vec2 playerScreenPos = CoordinateInfo::WorldToScreen(playerPos);
	g_poSROU->DrawFillRectangle(
		(f32)playerScreenPos.x,
		(f32)playerScreenPos.y,
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(), 
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(),
		bodyColor, D_Color(255, 255, 255));

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
bool FEPlayer::InBounds(const Vec2& checkPoint) const
{
	Vec2 pos2D = Vec2::FromVec3(m_vPos);
	if((checkPoint - pos2D).LengthSq() <= Math::Square(m_fPlayerRadius))
	{
		return true;
	}
	return false;
}
void FEPlayer::Move(const Vec2& pos)
{
	if(!HasFlags(EFEFlag_Movable))
		return;
	m_vPos = Vec3::FromVec2(ClampToPitch(pos));
}
void FEPlayer::Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poPlayerElement = new TiXmlElement("Player");
	_poParent->LinkEndChild(poPlayerElement);

	poPlayerElement->SetAttribute("id", m_id);
	poPlayerElement->SetAttribute("is_movable",	HasFlags(EFEFlag_Movable));
	poPlayerElement->SetAttribute("is_deletable", HasFlags(EFEFlag_Deletable));

	TiXmlElement* poPosElement = new TiXmlElement("Position");
	poPlayerElement->LinkEndChild(poPosElement);
	poPosElement->SetDoubleAttribute("x", m_vPos.x);
	poPosElement->SetDoubleAttribute("y", m_vPos.y);
}
void FEPlayer::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{

}

FESimulatedPlayer::FESimulatedPlayer(s32 id)
	: FEPlayer(id)
{
	Char refName[16] = {0};
	sprintf(refName, "Player%d_Pos", m_id);
	m_RfPosition.RegisterValue(refName,Vec2());
}
void FESimulatedPlayer::Tick(f32 _fDeltaTime)
{
	m_vPos = Vec3::FromVec2(m_RfPosition.As());
}
//----------------------------------------------------------
namespace
{
	static const u32 kColor[] = {
		0xffff0000,
		0xff00ff00,
		0xff0000ff,
		0xffffff00,
		0xffff00ff,
		0xff00ffff,
		0xff00ff7f,
		0xff7f00ff,
		0xffff7f00,
		0xff7f7f00,
		0xff7f007f,
		0xff007f7f,
		0xff7f7f7f
	};
	D_Color GetColor(s32 id)
	{
		static s32 kSizeOfColor = sizeof(kColor) / sizeof(kColor[0]);
		s32 idx = id % kSizeOfColor;
		return D_Color(kColor[idx]);
	}
}

FECanvas::FECanvas()
	: m_SelectedElement(NULL)
{}
void FECanvas::Move(const Vec2& pos)
{
	if(m_SelectedElement)
	{
		m_SelectedElement->Move(pos);
	}
}
void FECanvas::Deselect(const Vec2& pos)
{
}
FEEditorCanvas::FEEditorCanvas()
	: m_NextID(0)
{}
void FEEditorCanvas::Create(f32 length, f32 width)
{
	ElementPair* pNewPair = new ElementPair;
	pNewPair->m_RefElement = new FEBall(m_NextID);
	pNewPair->m_RefElement->SetColor(GetColor(m_NextID));
	pNewPair->m_RefElement->SetPosition(length / 2.f, width / 2.f, 0.f);
	pNewPair->m_RefElement->RemoveFlags(EFEFlag_Movable | EFEFlag_Deletable);
	pNewPair->m_FormationElement = new FEPlayer(m_NextID);
	pNewPair->m_FormationElement->SetColor(GetColor(m_NextID));
	pNewPair->m_FormationElement->SetPosition(length / 4.f, width / 4.f, 0.f);
	pNewPair->m_FormationElement->RemoveFlags(EFEFlag_Deletable);
	m_Elements.insert(std::pair<s32, ElementPair*>(m_NextID, pNewPair));
	
	m_NextID++;

	pNewPair = new ElementPair;
	pNewPair->m_RefElement = new FEBall(m_NextID);
	pNewPair->m_RefElement->SetColor(GetColor(m_NextID));
	pNewPair->m_RefElement->SetPosition(-length / 2.f, -width / 2.f, 0.f);
	pNewPair->m_RefElement->RemoveFlags(EFEFlag_Movable | EFEFlag_Deletable);
	pNewPair->m_FormationElement = new FEPlayer(m_NextID);
	pNewPair->m_FormationElement->SetColor(GetColor(m_NextID));
	pNewPair->m_FormationElement->SetPosition(-length / 4.f, -width / 4.f, 0.f);
	pNewPair->m_FormationElement->RemoveFlags(EFEFlag_Deletable);
	m_Elements.insert(std::pair<s32, ElementPair*>(m_NextID, pNewPair));

	m_NextID++;
}
void FEEditorCanvas::Tick(f32 _fDeltaTime)
{
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		ePair->m_RefElement->Tick(_fDeltaTime);
		ePair->m_FormationElement->Tick(_fDeltaTime);

		++it;
	}
}
void FEEditorCanvas::Draw()
{
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		ePair->m_RefElement->Draw();
		ePair->m_FormationElement->Draw();

		++it;
	}
}
void FEEditorCanvas::Select(const Vec2& pos)
{
	if(m_SelectedElement)
	{
		m_SelectedElement->Deselect(pos);
	}
	m_SelectedElement = NULL;
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		if(ePair->m_RefElement->InBounds(pos))
		{
			m_SelectedElement = ePair->m_RefElement;
			break;
		}
		if(ePair->m_FormationElement->InBounds(pos))
		{
			m_SelectedElement = ePair->m_FormationElement;
			break;
		}
		++it;
	}
	if(m_SelectedElement)
		m_SelectedElement->Select(pos);
}
void FEEditorCanvas::Delete()
{
	if(m_SelectedElement)
	{
		if(m_SelectedElement->HasFlags(EFEFlag_Deletable))
		{
			ElementPairMapIterator it = m_Elements.find(m_SelectedElement->GetID());
			D_CHECK(it != m_Elements.end())
			ElementPair* pPair = (*it).second;
			delete pPair->m_RefElement;
			delete pPair->m_FormationElement;
			delete pPair;
			m_Elements.erase(it);

			m_SelectedElement = NULL;
		}
	}
}
void FEEditorCanvas::AddElementPair(const Vec2& pos)
{
	ElementPair* pNewPair = new ElementPair;
	pNewPair->m_RefElement = new FEBall(m_NextID);
	pNewPair->m_RefElement->SetColor(GetColor(m_NextID));
	pNewPair->m_RefElement->SetPosition(pos.x - 2.f, 0.f, 0.f);
	pNewPair->m_FormationElement = new FEPlayer(m_NextID);
	pNewPair->m_FormationElement->SetColor(GetColor(m_NextID));
	pNewPair->m_FormationElement->SetPosition(pos.x + 2.f, 0.f, 0.f);
	m_Elements.insert(std::pair<s32, ElementPair*>(m_NextID, pNewPair));

	m_NextID++;
}
void FEEditorCanvas::Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		ePair->m_RefElement->Serialize(_poXmlDoc, _poParent);
		ePair->m_FormationElement->Serialize(_poXmlDoc, _poParent);
		++it;
	}
}
void FEEditorCanvas::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{

}
void FEEditorCanvas::Export(File* _pFile, u32 pitchType, u32 teamState, u32 position)
{
	D_CHECK(_pFile->IsValid());

	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		std::stringstream ss;
		ss << "FormationStaticDatabase.AddFormationPoint(";
		if(pitchType == FormationEditor::EPitchType_Normal)
			ss << "EFormationPitchType.EPitchType_Normal,";
		else
			ss << "EFormationPitchType.EPitchType_Large,";
		if(teamState == FormationEditor::ETeamState_Attack)
			ss << "EFormationTeamState.ETeamState_Attack,";
		else
			ss << "EFormationTeamState.ETeamState_Defend,";
		ss << position << ",";

		ElementPair* ePair = (*it).second;
		Vec3 refPos = ePair->m_RefElement->GetPosition();
		Vec3 formationPos = ePair->m_FormationElement->GetPosition();

		ss << "Vector3(" << refPos.x << "," << refPos.y << "," << refPos.z << "),";
		ss << "Vector3(" << formationPos.x << "," << formationPos.y << "," << formationPos.z << "));\r\n";

		_pFile->Write(ss.str().c_str(), ss.str().length());

		++it;
	}
}

FESimulatedCanvas::FESimulatedCanvas()
{}
void FESimulatedCanvas::Setup(const FESimulatedCanvas::SimulatedSettings& setup)
{
	FEEditableElement* pNewElement = new FESimulatedBall(0);
	pNewElement->Move(Vec2(0.f, 2.f));
	m_Elements.PushBack(pNewElement);
	for(int i = 0; i < setup.m_PlayerPosition.Size(); ++i)
	{
		pNewElement = new FESimulatedPlayer(i);
		m_Elements.PushBack(pNewElement);
	}
}
void FESimulatedCanvas::Stop()
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		delete m_Elements[i];
	}
	m_Elements.Clear();
}
void FESimulatedCanvas::Tick(f32 _fDeltaTime)
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		m_Elements[i]->Tick(_fDeltaTime);
	}
}
void FESimulatedCanvas::Draw()
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		m_Elements[i]->Draw();
	}
}
void FESimulatedCanvas::Select(const Vec2& pos)
{
	if(m_SelectedElement)
	{
		m_SelectedElement->Deselect(pos);
	}
	m_SelectedElement = NULL;
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		if(m_Elements[i]->InBounds(pos))
		{
			m_SelectedElement = m_Elements[i];
			break;
		}
	}
	if(m_SelectedElement)
		m_SelectedElement->Select(pos);
}

//----------------------------------------------------------
FormationEditor::FormationEditor()
	: m_CurrentCanvas(NULL)
	, m_Mode(EEditorMode_Edit)
{
	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				m_Canvases[i][j][k] = new FEEditorCanvas;
			}
		}
	}
	m_SimulatedCanvas = new FESimulatedCanvas;
}
void FormationEditor::Create()
{
	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				if(i == EPitchType_Normal)
					m_Canvases[i][j][k]->Create(kPithLenghNormal, kPitchWidthNormal);
				else
					m_Canvases[i][j][k]->Create(kPithLenghLarge, kPitchWidthLarge);
			}
		}
	}
}
void FormationEditor::Tick(f32 _fDeltaTime)
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
			m_CurrentCanvas->Tick(_fDeltaTime);
	}
	else
	{
		m_SimulatedCanvas->Tick(_fDeltaTime);
	}
}
void FormationEditor::Draw()
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
			m_CurrentCanvas->Draw();
	}
	else
	{
		m_SimulatedCanvas->Draw();
	}
}
void FormationEditor::Select(const Vec2& pos)
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
		{
			m_CurrentCanvas->Select(pos);
		}
	}
	else
	{
		m_SimulatedCanvas->Select(pos);
	}
}
void FormationEditor::Move(const Vec2& pos)
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
		{
			m_CurrentCanvas->Move(pos);
		}
	}
	else
	{
		m_SimulatedCanvas->Move(pos);
	}
}
void FormationEditor::Deselect(const Vec2& pos)
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
		{
			m_CurrentCanvas->Deselect(pos);
		}
	}
	else
	{
		m_SimulatedCanvas->Deselect(pos);
	}
}
void FormationEditor::Delete()
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
		{
			m_CurrentCanvas->Delete();
		}
	}
}
void FormationEditor::AddElementPair()
{
	if(m_Mode == EEditorMode_Edit)
	{
		if(m_CurrentCanvas)
		{
			m_CurrentCanvas->AddElementPair(Vec2());
		}
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

	if(pitchType == EPitchType_Normal)
	{
		CoordinateInfo::sLength = kPithLenghNormal;
		CoordinateInfo::sWidth = kPitchWidthNormal;
	}
	else
	{
		CoordinateInfo::sLength = kPithLenghLarge;
		CoordinateInfo::sWidth = kPitchWidthLarge;
	}
	m_CurrentCanvas = m_Canvases[pitchType][teamState][pos];
}
void FormationEditor::StartSimulation(const FESimulatedCanvas::SimulatedSettings& setup)
{
	m_Mode = EEditorMode_Simulation;
	m_SimulatedCanvas->Setup(setup);
}
void FormationEditor::StopSimulation()
{
	m_SimulatedCanvas->Stop();
	m_Mode = EEditorMode_Edit;
}
void FormationEditor::Export(File* pFile)
{
	D_CHECK(pFile->IsValid());
	
	Char start[] = "do\r\n";
	pFile->Write(start, strlen(start));

	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				m_Canvases[i][j][k]->Export(pFile, i, j, k);
			}
		}
	}
	Char end[] = "end\r\n";
	pFile->Write(end, strlen(end));
}
void FormationEditor::Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poRootData = new TiXmlElement("FormationData");
	_poXmlDoc->LinkEndChild(poRootData);

	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				TiXmlElement* poCanvasElement = new TiXmlElement("Canvas");
				poRootData->LinkEndChild(poCanvasElement);

				poCanvasElement->SetAttribute("pitch_type", i);
				poCanvasElement->SetAttribute("team_state",	j);
				poCanvasElement->SetAttribute("position_in_team", k);

				m_Canvases[i][j][k]->Serialize(_poXmlDoc, poCanvasElement);
			}
		}
	}
}
void FormationEditor::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{

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
	FormationEditor* pEditor = g_poEngine->GetSceneMod()->GetSceneObject<FormationEditor>("FormationEditor");
	switch(key)
	{
	case KEY_Delete:
		pEditor->Delete();
		break;
	case KEY_Insert:
		pEditor->AddElementPair();
	}
}