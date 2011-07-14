#include "SDSerializer.h"
#include "SDObject.h"

/********************************************************************************/
/* Serializer
/********************************************************************************/

Serializer::Serializer(TiXmlDocument* p_poXmlDoc)
:	m_poXmlDoc(p_poXmlDoc)
{
}

/********************************************************************************/
/* Soccer Serializer
/********************************************************************************/

SoccerSerializer::SoccerSerializer(TiXmlDocument* p_poXmlDoc)
:	Serializer(p_poXmlDoc)
{
}

void SoccerSerializer::Serialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = new TiXmlElement("Soccer");
	m_poXmlDoc->LinkEndChild(poElement);

	BallSerializer* poBallSerializer = new BallSerializer(m_poXmlDoc);
	poBallSerializer->Serialize(poElement);
	delete poBallSerializer;

	TeamSerializer* poTeamSerializer = new TeamSerializer(m_poXmlDoc, "TeamHome");
	poTeamSerializer->Serialize(poElement);
	delete poTeamSerializer;

	poTeamSerializer = new TeamSerializer(m_poXmlDoc, "TeamAway");
	poTeamSerializer->Serialize(poElement);
	delete poTeamSerializer;
}

void SoccerSerializer::Deserialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = m_poXmlDoc->FirstChildElement("Soccer");

	BallSerializer* poBallSerializer = new BallSerializer(m_poXmlDoc);
	poBallSerializer->Deserialize(poElement);
	delete poBallSerializer;

	TeamSerializer* poTeamSerializer = new TeamSerializer(m_poXmlDoc, "TeamHome");
	poTeamSerializer->Deserialize(poElement);
	delete poTeamSerializer;

	poTeamSerializer = new TeamSerializer(m_poXmlDoc, "TeamAway");
	poTeamSerializer->Deserialize(poElement);
	delete poTeamSerializer;
}

/********************************************************************************/
/* Ball Serializer
/********************************************************************************/

BallSerializer::BallSerializer(TiXmlDocument* p_poXmlDoc)
:	Serializer(p_poXmlDoc)
{
}

void BallSerializer::Serialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = new TiXmlElement("Ball");
	p_poParent->LinkEndChild(poElement);

	OBall* poBall = g_poEngine->GetSceneMod()->GetSceneObject<OBall>("ZBall");

	Vec3 vBallPos = poBall->GetPosition();
	Vec3Serializer* poSerializer = new Vec3Serializer(m_poXmlDoc, vBallPos, "Pos");
	poSerializer->Serialize(poElement);
	delete poSerializer;
}

void BallSerializer::Deserialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = p_poParent->FirstChildElement("Ball");
	if (poElement)
	{
		Vec3 vBallPos;
		Vec3Serializer* poSerializer = new Vec3Serializer(m_poXmlDoc, vBallPos, "Pos");
		poSerializer->Deserialize(poElement);
		delete poSerializer;

		OBall* poBall = g_poEngine->GetSceneMod()->GetSceneObject<OBall>("ZBall");
		poBall->SetPosition(vBallPos);
	}
}

/********************************************************************************/
/* Team Serializer
/********************************************************************************/

TeamSerializer::TeamSerializer(TiXmlDocument* p_poXmlDoc, const char* p_poTeamName)
:	Serializer(p_poXmlDoc)
,	m_poTeamName(p_poTeamName)
{
}

void TeamSerializer::Serialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = new TiXmlElement("Team");
	p_poParent->LinkEndChild(poElement);

	poElement->SetAttribute("Name", m_poTeamName);
	
	for (int i = 0; i < kPLAYER_COUNT; ++i)
	{
		PlayerSerializer* serializer = new PlayerSerializer(m_poXmlDoc, m_poTeamName, i);
		serializer->Serialize(poElement);
		delete serializer;
	}
}

void TeamSerializer::Deserialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = p_poParent->FirstChildElement("Team");
	while (poElement)
	{
		std::string strTeamName(m_poTeamName);
		std::string strAttributeName(poElement->Attribute("Name"));
		if (strTeamName == strAttributeName)
		{
			for (int i = 0; i < kPLAYER_COUNT; ++i)
			{
				PlayerSerializer* serializer = new PlayerSerializer(m_poXmlDoc, m_poTeamName, i);
				serializer->Deserialize(poElement);
				delete serializer;
			}
			break;
		}
		poElement = poElement->NextSiblingElement("Team");
	}
}

/********************************************************************************/
/* Player Serializer
/********************************************************************************/

PlayerSerializer::PlayerSerializer(TiXmlDocument* p_poXmlDoc, const char* p_poTeamName, s32 p_iIdx)
:	Serializer(p_poXmlDoc)
,	m_poTeamName(p_poTeamName)
,	m_iIdx(p_iIdx)
{
}

void PlayerSerializer::Serialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = new TiXmlElement("Player");
	p_poParent->LinkEndChild(poElement);

	poElement->SetAttribute("Num", m_iIdx);

	OTeam* poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>(m_poTeamName);
	OPlayer* poPlayer = poTeam->GetPlayer(m_iIdx);

	Vec3 vPlayerPos = poPlayer->GetPosition();
	Vec3Serializer* poSerializer = new Vec3Serializer(m_poXmlDoc, vPlayerPos, "Pos");
	poSerializer->Serialize(poElement);
	delete poSerializer;

	Vec3 vPlayerRot = poPlayer->GetOrientation();
	poSerializer = new Vec3Serializer(m_poXmlDoc, vPlayerRot, "Rot");
	poSerializer->Serialize(poElement);
	delete poSerializer;
}

void PlayerSerializer::Deserialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = p_poParent->FirstChildElement("Player");
	while (poElement)
	{
		s32 iIdx;
		poElement->Attribute("Num", &iIdx);
		if (iIdx == m_iIdx)
		{
			OTeam* poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>(m_poTeamName);
			OPlayer* poPlayer = poTeam->GetPlayer(m_iIdx);

			Vec3 vPlayerPos;
			Vec3Serializer* poSerializer = new Vec3Serializer(m_poXmlDoc, vPlayerPos, "Pos");
			poSerializer->Deserialize(poElement);
			delete poSerializer;
			poPlayer->SetPosition(vPlayerPos);

			Vec3 vPlayerRot;
			poSerializer = new Vec3Serializer(m_poXmlDoc, vPlayerRot, "Rot");
			poSerializer->Deserialize(poElement);
			delete poSerializer;
			poPlayer->SetRotation(vPlayerRot);

			break;
		}
		poElement = poElement->NextSiblingElement("Player");
	}
}

/********************************************************************************/
/* Vec3 Serializer
/********************************************************************************/

Vec3Serializer::Vec3Serializer(TiXmlDocument* p_poXmlDoc, Vec3& p_vVector, const char* p_poName)
:	Serializer(p_poXmlDoc)
,	m_vVector(p_vVector)
,	m_poName(p_poName)
{
}

void Vec3Serializer::Serialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = new TiXmlElement(m_poName);
	p_poParent->LinkEndChild(poElement);

	poElement->SetDoubleAttribute("x", m_vVector.x);
	poElement->SetDoubleAttribute("y", m_vVector.y);
	poElement->SetDoubleAttribute("z", m_vVector.z);
}

void Vec3Serializer::Deserialize(TiXmlElement* p_poParent /* = NULL */)
{
	TiXmlElement* poElement = p_poParent->FirstChildElement(m_poName);
	if (poElement)
	{
		double value;
		poElement->QueryDoubleAttribute("x", &value);
		m_vVector.x = value;
		poElement->QueryDoubleAttribute("y", &value);
		m_vVector.y = value;
		poElement->QueryDoubleAttribute("z", &value);
		m_vVector.z = value;
	}
}
