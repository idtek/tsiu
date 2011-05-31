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
Bool CoordinateInfo::sAwayView = false;

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
Vec2 CoordinateInfo::WorldToScreen(const Vec2& worldPos, bool awayView)
{
	Vec2 screenPos;
	if(!awayView)
	{
		screenPos.x = kCANVAS_START_X + (worldPos.x + sLength / 2.f) * GetPixelPerMeter();
		screenPos.y = kCANVAS_START_Y + (sWidth / 2.f - worldPos.y) * GetPixelPerMeter();
	}
	else
	{
		screenPos.x = kCANVAS_START_X + (-worldPos.x + sLength / 2.f) * GetPixelPerMeter();
		screenPos.y = kCANVAS_START_Y + (sWidth / 2.f + worldPos.y) * GetPixelPerMeter();
	}
	return screenPos;
}
Vec2 CoordinateInfo::ScreenToWorld(const Vec2& screenPos, bool awayView)
{
	Vec2 worldPos;
	worldPos.x = (screenPos.x - kCANVAS_START_X) * GetMeterPerPixel() -  sLength / 2.f;
	worldPos.y = sWidth / 2.f - (screenPos.y - kCANVAS_START_Y) * GetMeterPerPixel();
	if(awayView)
	{
		worldPos = -worldPos;
	}
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

	DrawWithColor(bodyColor);
}
void FEBall::DrawWithColor(const D_Color& clr)
{
	D_Color bodyColor = clr;
	Vec3 realPos = m_vPos;
	if(CoordinateInfo::sAwayView)
		realPos = -realPos;

	Vec2 screenPos = CoordinateInfo::WorldToScreen(Vec2::FromVec3(realPos));
	g_poSROU->DrawFillCircle((f32)screenPos.x, (f32)screenPos.y, m_fBallRadius * CoordinateInfo::GetPixelPerMeter(), bodyColor, D_Color(0, 0, 0));

	Vec2 indicatorPos(realPos.x - m_fBallRadius, realPos.y + m_fBallRadius);
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
	if(CoordinateInfo::sAwayView)
		m_vPos = -m_vPos;
}
bool FEBall::InBounds(const Vec2& checkPoint) const
{
	Vec3 realPos = m_vPos;
	if(CoordinateInfo::sAwayView)
		realPos = -realPos;

	Vec2 pos2D = Vec2::FromVec3(realPos);
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

	poBallElement->SetAttribute("is_movable",	HasFlags(EFEFlag_Movable));
	poBallElement->SetAttribute("is_deletable", HasFlags(EFEFlag_Deletable));

	TiXmlElement* poPosElement = new TiXmlElement("Position");
	poBallElement->LinkEndChild(poPosElement);
	poPosElement->SetDoubleAttribute("x", m_vPos.x);
	poPosElement->SetDoubleAttribute("y", m_vPos.y);
}
void FEBall::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poElement = _poParent->FirstChildElement("Ball");
	if(poElement)
	{
		int isMovable, isDeletable;
		poElement->QueryIntAttribute("is_movable", &isMovable);
		poElement->QueryIntAttribute("is_deletable", &isDeletable);
		if(isMovable)
			AddFlags(EFEFlag_Movable);
		else
			RemoveFlags(EFEFlag_Movable);

		if(isDeletable)
			AddFlags(EFEFlag_Deletable);
		else
			RemoveFlags(EFEFlag_Deletable);

		TiXmlElement* poPosElement = poElement->FirstChildElement("Position");
		D_CHECK(poPosElement);

		double x, y;
		poPosElement->QueryDoubleAttribute("x", &x);
		poPosElement->QueryDoubleAttribute("y", &y);
		SetPosition(Vec3(x, y, 0.f));
		SetColor(GetColor(m_id));
	}
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
void FESimulatedBall::Draw()
{
	D_Color bodyColor = m_Color;
	if(HasFlags(EFEFlag_Selected))
		bodyColor = bodyColor / 1.5f;

	Vec2 screenPos = CoordinateInfo::WorldToScreen(Vec2::FromVec3(m_vPos));
	g_poSROU->DrawFillCircle((f32)screenPos.x, (f32)screenPos.y, m_fBallRadius * CoordinateInfo::GetPixelPerMeter(), bodyColor, D_Color(0, 0, 0));

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
bool FESimulatedBall::InBounds(const Vec2& checkPoint) const
{
	Vec3 realPos = m_vPos;
	Vec2 pos2D = Vec2::FromVec3(realPos);
	if((checkPoint - pos2D).LengthSq() <= Math::Square(m_fBallRadius))
	{
		return true;
	}
	return false;
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

	DrawWithColor(bodyColor);
}
void FEPlayer::DrawWithColor(const D_Color& clr)
{
	D_Color bodyColor = clr;
	Vec3 realPos = m_vPos;
	if(CoordinateInfo::sAwayView)
		realPos = -realPos;

	Vec2 playerPos(realPos.x - m_fPlayerRadius, realPos.y + m_fPlayerRadius);
	Vec2 playerScreenPos = CoordinateInfo::WorldToScreen(playerPos);
	g_poSROU->DrawFillRectangle(
		(f32)playerScreenPos.x,
		(f32)playerScreenPos.y,
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(), 
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(),
		bodyColor, D_Color(255, 255, 255));

	Vec2 indicatorPos(realPos.x - m_fPlayerRadius, realPos.y + m_fPlayerRadius);
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
	Vec3 realPos = m_vPos;
	if(CoordinateInfo::sAwayView)
		realPos = -realPos;

	Vec2 pos2D = Vec2::FromVec3(realPos);
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
	if(CoordinateInfo::sAwayView)
		m_vPos = -m_vPos;
}
void FEPlayer::Serialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poPlayerElement = new TiXmlElement("Player");
	_poParent->LinkEndChild(poPlayerElement);

	poPlayerElement->SetAttribute("is_movable",	HasFlags(EFEFlag_Movable));
	poPlayerElement->SetAttribute("is_deletable", HasFlags(EFEFlag_Deletable));

	TiXmlElement* poPosElement = new TiXmlElement("Position");
	poPlayerElement->LinkEndChild(poPosElement);
	poPosElement->SetDoubleAttribute("x", m_vPos.x);
	poPosElement->SetDoubleAttribute("y", m_vPos.y);
}
void FEPlayer::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poElement = _poParent->FirstChildElement("Player");
	if(poElement)
	{
		int isMovable, isDeletable;
		poElement->QueryIntAttribute("is_movable", &isMovable);
		poElement->QueryIntAttribute("is_deletable", &isDeletable);
		if(isMovable)
			AddFlags(EFEFlag_Movable);
		else
			RemoveFlags(EFEFlag_Movable);

		if(isDeletable)
			AddFlags(EFEFlag_Deletable);
		else
			RemoveFlags(EFEFlag_Deletable);

		TiXmlElement* poPosElement = poElement->FirstChildElement("Position");
		D_CHECK(poPosElement);

		double x, y;
		poPosElement->QueryDoubleAttribute("x", &x);
		poPosElement->QueryDoubleAttribute("y", &y);
		SetPosition(Vec3(x, y, 0.f));
		SetColor(GetColor(m_id));
	}
}

int FESimulatedPlayer::sHomePlayerCount = 0;
int FESimulatedPlayer::sAwayPlayerCount = 0;

FESimulatedPlayer::FESimulatedPlayer(s32 id, int team)
	: FEPlayer(id)
	, m_Team(team)
{
	Char refName[16] = {0};
	sprintf(refName, "Player%d_Pos", m_id);
	m_RfPosition.RegisterValue(refName,Vec2());

	if(m_Team == kHOME_TEAM)
		m_Color = D_Color(255,64,64);
	else
		m_Color = D_Color(64,159,255);
}
void FESimulatedPlayer::Tick(f32 _fDeltaTime)
{
	m_vPos = Vec3::FromVec2(m_RfPosition.As());
}
void FESimulatedPlayer::Draw()
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
	itoa(m_Team == kHOME_TEAM ? (m_id - 0 + 1) : (m_id - FESimulatedPlayer::sHomePlayerCount + 1), indicator, 10);
	g_poSROU->DrawStringEx(
		indicatorScreenPos.x, 
		indicatorScreenPos.y,
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(), 
		2 * m_fPlayerRadius * CoordinateInfo::GetPixelPerMeter(),
		kPLAYER_NUMSIZE * CoordinateInfo::GetPixelPerMeter(),
		indicator, NULL, D_Color(0,0,0));
}
bool FESimulatedPlayer::InBounds(const Vec2& checkPoint) const
{
	Vec3 realPos = m_vPos;
	Vec2 pos2D = Vec2::FromVec3(realPos);
	if((checkPoint - pos2D).LengthSq() <= Math::Square(m_fPlayerRadius))
	{
		return true;
	}
	return false;
}
void FESimulatedPlayer::Select(const Vec2& pos)
{
	FEPlayer::Select(pos);

	Event evt((EventType_t)E_ET_SIM_SelectPlayer);
	evt.AddParam(m_id);
	evt.AddParam(m_Team);
	g_poEngine->GetEventMod()->SendEvent(&evt);
}
//----------------------------------------------------------
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
FEEditorCanvas::~FEEditorCanvas()
{
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		D_SafeDelete(ePair);
		++it;
	}
	m_Elements.clear();
}
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

	unsigned int zOrder = EZOrder_Bottom;
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		if(ePair->m_RefElement->InBounds(pos))
		{
			if(zOrder >= ePair->m_RefElement->GetZOrder())
			{
				zOrder = ePair->m_RefElement->GetZOrder();
				m_SelectedElement = ePair->m_RefElement;
			}
		}
		if(ePair->m_FormationElement->InBounds(pos))
		{
			if(zOrder >= ePair->m_FormationElement->GetZOrder())
			{
				zOrder = ePair->m_FormationElement->GetZOrder();
				m_SelectedElement = ePair->m_FormationElement;
			}
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

		TiXmlElement* poPairData = new TiXmlElement("PairData");
		_poParent->LinkEndChild(poPairData);
		
		poPairData->SetAttribute("id", (*it).first);

		ePair->m_RefElement->Serialize(_poXmlDoc, poPairData);
		ePair->m_FormationElement->Serialize(_poXmlDoc, poPairData);

		++it;
	}
}
void FEEditorCanvas::Deserialize(TiXmlDocument* _poXmlDoc, TiXmlElement* _poParent)
{
	TiXmlElement* poPairData = _poParent->FirstChildElement("PairData");
	while(poPairData)
	{
		int id;
		poPairData->QueryIntAttribute("id", &id);
		ElementPairMapIterator it = m_Elements.find(id);
		if(it == m_Elements.end())
		{
			ElementPair* pNewPair = new ElementPair;
			pNewPair->m_RefElement = new FEBall(id);
			pNewPair->m_FormationElement = new FEPlayer(id);

			if(m_NextID <= id)
				m_NextID = id + 1;

			pNewPair->m_RefElement->Deserialize(_poXmlDoc, poPairData);
			pNewPair->m_FormationElement->Deserialize(_poXmlDoc, poPairData);

			m_Elements.insert(std::pair<s32, ElementPair*>(id, pNewPair));
		}
		poPairData = poPairData->NextSiblingElement();
	}
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
void FEEditorCanvas::DrawWithColor(const D_Color& clr)
{
	ElementPairMapIterator it = m_Elements.begin();
	while(it != m_Elements.end())
	{
		ElementPair* ePair = (*it).second;
		static_cast<FEBall*>(ePair->m_RefElement)->DrawWithColor(clr);
		static_cast<FEPlayer*>(ePair->m_FormationElement)->DrawWithColor(clr);

		++it;
	}
}

FESimulatedCanvas::FESimulatedCanvas()
	: m_SimBall(NULL)
{}
FESimulatedCanvas::~FESimulatedCanvas()
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		D_SafeDelete(m_Elements[i]);
	}
	m_Elements.Clear();
	D_SafeDelete(m_SimBall);
}
void FESimulatedCanvas::Setup(const FESimulatedCanvas::SimulatedSettings& setup)
{
	m_SimBall = new FESimulatedBall(0);
	m_SimBall->Move(Vec2(0.f, 2.f));

	FEEditableElement* pNewElement = NULL;
	FESimulatedPlayer::sHomePlayerCount = setup.m_HomePlayerPosition.Size();
	FESimulatedPlayer::sAwayPlayerCount = setup.m_AwayPlayerPosition.Size();

	for(int i = 0; i < FESimulatedPlayer::sHomePlayerCount; ++i)
	{
		pNewElement = new FESimulatedPlayer(i, kHOME_TEAM);
		m_Elements.PushBack(pNewElement);
	}
	for(int i = 0; i < FESimulatedPlayer::sAwayPlayerCount; ++i)
	{
		pNewElement = new FESimulatedPlayer(i + FESimulatedPlayer::sHomePlayerCount, kAWAY_TEAM);
		m_Elements.PushBack(pNewElement);
	}
}
void FESimulatedCanvas::Stop()
{
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		D_SafeDelete(m_Elements[i]);
	}
	m_Elements.Clear();
	D_SafeDelete(m_SimBall);
	m_SelectedElement = NULL;
}
void FESimulatedCanvas::Tick(f32 _fDeltaTime)
{
	m_SimBall->Tick(_fDeltaTime);
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		m_Elements[i]->Tick(_fDeltaTime);
	}
}
void FESimulatedCanvas::Draw()
{
	m_SimBall->Draw();
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		m_Elements[i]->Draw();
	}
}
void FESimulatedCanvas::Select(const Vec2& pos)
{
	if(m_SimBall->InBounds(pos))
	{
		m_SimBall->Select(pos);
	}
	else
	{
		m_SimBall->Deselect(pos);
	}
	FEEditableElement* curSelect = NULL;
	unsigned int zOrder = EZOrder_Bottom;
	for(int i = 0; i < m_Elements.Size(); ++i)
	{
		if(m_Elements[i]->InBounds(pos))
		{
			if(zOrder >= m_Elements[i]->GetZOrder())
			{
				zOrder = m_Elements[i]->GetZOrder();
				curSelect = m_Elements[i];
			}
		}
	}
	if(curSelect)
	{
		if(m_SelectedElement)
		{
			m_SelectedElement->Deselect(pos);
		}
		m_SelectedElement = curSelect;
		m_SelectedElement->Select(pos);
	}
}
void FESimulatedCanvas::Move(const Vec2& pos)
{
	if(m_SimBall->HasFlags(EFEFlag_Selected))
	{
		m_SimBall->Move(pos);
	}
}
//----------------------------------------------------------
FormationEditor::FormationEditor()
	: m_CurrentCanvas(NULL)
	, m_Mode(EEditorMode_Edit)
	, m_InSimulatedMode("ToSimulation", false)
	, m_IsHomeAttacking("InHomeAttacking", false)
	, m_SimulatedRefCanvas(NULL)
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
	char refName[16] = {0};
	for(int i = 0; i < 10; ++i)
	{
		sprintf(refName, "Player%d_PosInTB", i);
		m_RefPlayerPositionInTB[i].RegisterValue(refName, -1);
	}
	m_SimulatedCanvas = new FESimulatedCanvas;
}
FormationEditor::~FormationEditor()
{
	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				D_SafeDelete(m_Canvases[i][j][k]);
			}
		}
	}
	D_SafeDelete(m_SimulatedCanvas);
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

	g_poEngine->GetEventMod()->RegisterHandler(
		(EventType_t)(E_ET_SIM_SelectRefCanvas), 
		new MEventHandler<FormationEditor>(this, &FormationEditor::onSIM_SelectRefCanvas));

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
		if(m_SimulatedRefCanvas)
			m_SimulatedRefCanvas->DrawWithColor(D_Color(192, 192, 192));
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

	if(teamState != ETeamState_Attack && teamState != ETeamState_Defend)
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
bool FormationEditor::StartSimulation(const FESimulatedCanvas::SimulatedSettings& setup)
{	
	if(!setup.m_AwayPlayerPosition.Size() && !setup.m_HomePlayerPosition.Size())
		return false;

	std::string fullPath = setup.m_RootDir + "\\Media\\scripts\\ai\\formationsimulation.lua";
	File* pLuaFile = FileManager::Get().OpenFile(fullPath.c_str(), E_FOM_Write | E_FOM_Text);
	if(!pLuaFile)
		return false;

	std::stringstream ss;

	ss <<	"DefaultMatchSetup()"																								<< "\r\n";
	
	ss <<	"package.path = \"scripts:/?.lua\""																					<< "\r\n";
	ss <<	"require \"MiniGameSharedFunctions\""																				<< "\r\n";
	
	ss <<	"MatchSetup['Init'] = function(newGame)"																			<< "\r\n";
	ss <<		"\tgameId = newGame.CreateGame();"																				<< "\r\n";

	if(setup.m_PitchType == EPitchType_Normal)
		ss <<	"\tnewGame.AddPitch(\"Street_SH_Night_01\");"																	<< "\r\n";
	else
		ss <<	"\tnewGame.AddPitch(\"stadium_BJ_60_40\");"																		<< "\r\n";
	ss <<		"\tnewGame.AddBall(\"CommonObjects\");"																			<< "\r\n";
	for(int i = 0; i < setup.m_HomePlayerPosition.Size(); ++i)
	{
		m_RefPlayerPositionInTB[i] = setup.m_HomePlayerPosition[i];

		ss <<	"\tMatchSetup.player" << i + 1 << " = newGame.AddPlayer('SM_M_Vs_Red', ETeam.ETeam_HOME);"						<< "\r\n";
	}
	for(int i = 0; i < setup.m_AwayPlayerPosition.Size(); ++i)
	{
		m_RefPlayerPositionInTB[i + setup.m_HomePlayerPosition.Size()] = setup.m_AwayPlayerPosition[i];

		ss <<	"\tMatchSetup.player" << i + 1 + setup.m_HomePlayerPosition.Size() 
									  << " = newGame.AddPlayer('SM_M_Vs_Blue', ETeam.ETeam_AWAY);"								<< "\r\n";
	}
	ss <<		"\tnewGame.SetHasNoHumanPlayer(ETeam.ETeam_HOME);"																<< "\r\n";	
	ss <<		"\tnewGame.SetHasNoHumanPlayer(ETeam.ETeam_AWAY);"																<< "\r\n";	
	ss <<		"\tnewGame.SetHasNoGoalkeeper(ETeam.ETeam_HOME);"																<< "\r\n";	
	ss <<		"\tnewGame.SetHasNoGoalkeeper(ETeam.ETeam_AWAY);"																<< "\r\n";	
	ss <<		"\tnewGame.RunGame(gameId)"																						<< "\r\n";	
	ss <<	"end"																												<< "\r\n";	

	ss <<	"MatchSetup['Setup'] = function(newGame)"																			<< "\r\n";	
	for(int i = 0; i < setup.m_HomePlayerPosition.Size(); ++i)
	{
		ss <<	"\tGameLogic.ClearPlayerAI( MatchSetup.player" << i + 1 << " )"													<< "\r\n";	
		ss <<	"\tGameLogic.AddBehaviourModule(MatchSetup.player" << i + 1
		   <<	", EBehaviourModule.EBehaviourModule_IDLE);"																	<< "\r\n";
		ss <<	"\tGameLogic.AddBehaviourModule(MatchSetup.player" << i + 1
		   <<	", EBehaviourModule.EBehaviourModule_OFF_BALL_ATTACK_ZONAL_POSITIONING);"										<< "\r\n";
		ss <<	"\tGameLogic.AddBehaviourModule(MatchSetup.player" << i + 1
		   <<	", EBehaviourModule.EBehaviourModule_DEFENSIVE_MODULE_OUTFIELD);"												<< "\r\n";
	}
	for(int i = 0; i < setup.m_AwayPlayerPosition.Size(); ++i)
	{
		ss <<	"\tGameLogic.ClearPlayerAI( MatchSetup.player" << i + 1 + setup.m_HomePlayerPosition.Size() << " )"				<< "\r\n";	
		ss <<	"\tGameLogic.AddBehaviourModule(MatchSetup.player" << i + 1 + setup.m_HomePlayerPosition.Size()
		   <<	", EBehaviourModule.EBehaviourModule_IDLE);"																	<< "\r\n";
		ss <<	"\tGameLogic.AddBehaviourModule(MatchSetup.player" << i + 1 + setup.m_HomePlayerPosition.Size()
		   <<	", EBehaviourModule.EBehaviourModule_OFF_BALL_ATTACK_ZONAL_POSITIONING);"										<< "\r\n";
		ss <<	"\tGameLogic.AddBehaviourModule(MatchSetup.player" << i + 1 + setup.m_HomePlayerPosition.Size()
		   <<	", EBehaviourModule.EBehaviourModule_DEFENSIVE_MODULE_OUTFIELD);"												<< "\r\n";
	}
	ss <<		"\tSequenceManager.PlaySequence( \"Camera.Side\" );"															<< "\r\n";	
	ss <<		"\tSequenceManager.PlaySequence( \"Game.SinglePlayer\");"														<< "\r\n";	
	ss <<	"end"																												<< "\r\n";	

	ss <<	"MatchSetup['Reset'] = function(newGame)"																			<< "\r\n";	
	ss <<		"\tGameLogic.ReinitializeBallAtEx( -5.5, 0, 0 );"																<< "\r\n";	
	ss <<		"\tGameLogic.ReinitializeBallState(ERequestBallState.ERequestBallState_FREE, MatchSetup.player1);"				<< "\r\n";
	ss <<	"end"																												<< "\r\n";

	Bool isOk = pLuaFile->Write(ss.str().c_str(), ss.str().length());
	if(!isOk)
	{
		FileManager::Get().CloseFile(pLuaFile);
		return false;
	}

	std::stringstream ss2;

	std::string fullPathStartupFile = setup.m_RootDir + "\\Media\\scripts\\ui_configuration.lua";
	File* pStartupFile = FileManager::Get().OpenFile(fullPathStartupFile.c_str(), E_FOM_Write | E_FOM_Text);
	if(!pStartupFile)
		return false;

	ss2 <<	"do"																																			<< "\r\n";
	ss2 <<		"\tGameControl.skip_menu = true;"																											<< "\r\n";
	ss2 <<		"\tGameControl.set_startup_by_lua (\"Media/Scripts/ai/formationsimulation.lua\", GameType.GameType_MINIGAME);"	<< "\r\n";
	ss2 <<	"end"																																			<< "\r\n";

	isOk = pStartupFile->Write(ss2.str().c_str(), ss2.str().length());
	if(!isOk)
	{
		FileManager::Get().CloseFile(pStartupFile);
		FileManager::Get().CloseFile(pLuaFile);
		return false;
	}

	FileManager::Get().CloseFile(pStartupFile);
	FileManager::Get().CloseFile(pLuaFile);

	if(setup.m_PitchType == EPitchType_Normal)
	{
		CoordinateInfo::sLength = kPithLenghNormal;
		CoordinateInfo::sWidth = kPitchWidthNormal;
	}
	else
	{
		CoordinateInfo::sLength = kPithLenghLarge;
		CoordinateInfo::sWidth = kPitchWidthLarge;
	}
	m_IsHomeAttacking = (setup.m_TeamState == ETeamState_Attack);

	m_Mode = EEditorMode_Simulation;
	m_SimulatedCanvas->Setup(setup);

	//notify start
	m_InSimulatedMode = true;

	return true;
}
bool FormationEditor::StopSimulation()
{
	m_InSimulatedMode = false;
	m_SimulatedCanvas->Stop();
	m_Mode = EEditorMode_Edit;
	m_SimulatedRefCanvas = NULL;

	return true;
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
	TiXmlElement* poRootData = _poXmlDoc->FirstChildElement("FormationData");
	if(poRootData)
	{
		TiXmlElement* poCanvas = poRootData->FirstChildElement("Canvas");
		while(poCanvas)
		{
			int i, j, k;
			poCanvas->QueryIntAttribute("pitch_type",		&i);
			poCanvas->QueryIntAttribute("team_state",		&j);
			poCanvas->QueryIntAttribute("position_in_team", &k);
			m_Canvases[i][j][k] = new FEEditorCanvas;
			m_Canvases[i][j][k]->Deserialize(_poXmlDoc, poCanvas);
			poCanvas = poCanvas->NextSiblingElement();
		}
	}
}
void FormationEditor::Clear()
{
	for(s32 i = 0; i < EPitchType_Num; ++i)
	{
		for(s32 j = 0; j < ETeamState_Num; ++j)
		{
			for(s32 k = 0; k < kMaxPositionCount; ++k)
			{
				D_SafeDelete(m_Canvases[i][j][k]);
			}
		}
	}
}

void FormationEditor::onSIM_SelectRefCanvas(const Event* _poEvent)
{
	s32 pitchList	= _poEvent->GetParam<s32>(0);
	s32 teamState	= _poEvent->GetParam<s32>(1);
	s32 playerID	= _poEvent->GetParam<s32>(2);
	s32 teamID		= _poEvent->GetParam<s32>(3);

	if(teamID == kAWAY_TEAM)
	{
		teamState = (teamState == ETeamState_Attack ? ETeamState_Defend : ETeamState_Attack);
	}
	s32 posID		= m_RefPlayerPositionInTB[teamID == kHOME_TEAM ? playerID : playerID + FESimulatedPlayer::sHomePlayerCount].As();

	m_SimulatedRefCanvas = m_Canvases[pitchList][teamState][posID];
}

//----------------------------------------------------------------
#define kOneTurnTime 2.f

FEDebuggerInfo::FEDebuggerInfo()
	: m_Turn(kOneTurnTime)
	, m_GoUp(false)
	, m_SelectedPosition(-1)
{
}
void FEDebuggerInfo::Tick(f32 _fDeltaTime)
{
	if(m_SelectedPosition >= 0)
	{
		if(m_GoUp)
		{
			m_Turn += _fDeltaTime;
			if(m_Turn >= kOneTurnTime)
			{
				m_GoUp = false;
				m_Turn = kOneTurnTime;
			}
		}
		else
		{
			m_Turn -= _fDeltaTime;
			if(m_Turn <= 0)
			{
				m_GoUp = true;
				m_Turn = 0;
			}
		}
	}
	else
	{
		m_Turn = kOneTurnTime;
		m_GoUp = false;
	}
}
void FEDebuggerInfo::Draw()
{
	const float kWidthInterval	= CoordinateInfo::sWidth / kGridWidth;
	const float kLengthInterval	= CoordinateInfo::sLength / kGridLength;
	for(int i = 0; i < kGridWidth - 1; ++i)
	{
		Vec2 vPos1(-CoordinateInfo::sLength / 2.f,  -CoordinateInfo::sWidth / 2.f + kWidthInterval * (i + 1));
		Vec2 vPos2( CoordinateInfo::sLength / 2.f,  -CoordinateInfo::sWidth / 2.f + kWidthInterval * (i + 1));
		Vec2 vScreenPos1 = CoordinateInfo::WorldToScreen(vPos1);
		Vec2 vScreenPos2 = CoordinateInfo::WorldToScreen(vPos2);
		g_poSROU->DrawLine(vScreenPos1.x, vScreenPos1.y, vScreenPos2.x, vScreenPos2.y, D_Color(128, 128, 128), 1.f);
	}	
	for(int i = 0; i < kGridLength - 1; ++i)
	{
		Vec2 vPos1(-CoordinateInfo::sLength / 2.f + kLengthInterval * (i + 1),  CoordinateInfo::sWidth / 2.f);
		Vec2 vPos2(-CoordinateInfo::sLength / 2.f + kLengthInterval * (i + 1), -CoordinateInfo::sWidth / 2.f);
		Vec2 vScreenPos1 = CoordinateInfo::WorldToScreen(vPos1);
		Vec2 vScreenPos2 = CoordinateInfo::WorldToScreen(vPos2);
		g_poSROU->DrawLine(vScreenPos1.x, vScreenPos1.y, vScreenPos2.x, vScreenPos2.y, D_Color(128, 128, 128), 1.f);
	}

	if(m_SelectedPosition >= 0)
	{
		s32 realPos = m_SelectedPosition;
		if(CoordinateInfo::sAwayView)
		{
			s32 column	= (kGridLength - 1) - m_SelectedPosition / kGridWidth;
			s32 row		= (kGridWidth - 1) - m_SelectedPosition % kGridWidth;
			realPos = column * kGridWidth + row;
		}

		Vec2 indicatorPos(CoordinateInfo::sLength / 2.f - (realPos / kGridWidth + 1) * kLengthInterval, 
						  CoordinateInfo::sWidth / 2.f - (realPos % kGridWidth) * kWidthInterval);
		Vec2 indicatorScreenPos = CoordinateInfo::WorldToScreen(indicatorPos);
		Char indicator[4];
		itoa(m_SelectedPosition, indicator, 10);

		D_Color clr = Math::Blend(D_Color(128, 128, 128), D_Color(255, 255, 255), Math::Clamp(m_Turn / kOneTurnTime, 0.f, 1.f));
		g_poSROU->DrawStringEx(
			indicatorScreenPos.x, 
			indicatorScreenPos.y,
			kLengthInterval * CoordinateInfo::GetPixelPerMeter(), 
			kWidthInterval  * CoordinateInfo::GetPixelPerMeter(),
			5.f * CoordinateInfo::GetPixelPerMeter(),
			indicator, NULL, clr);
	}
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