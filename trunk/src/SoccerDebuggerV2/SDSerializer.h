#ifndef __SD_SERIALIZER_H__
#define __SD_SERIALIZER_H__

#include "tinyxml.h"
#include "SDGlobalDef.h"

class Serializer
{
public:
	Serializer(TiXmlDocument* p_poXmlDoc);

	virtual void	Serialize(TiXmlElement* p_poParent = NULL) = 0;
	virtual void	Deserialize(TiXmlElement* p_poParent = NULL) = 0;

protected:
	TiXmlDocument*	m_poXmlDoc;
};

class SoccerSerializer : public Serializer
{
public:
	SoccerSerializer(TiXmlDocument* p_poXmlDoc);

	virtual void	Serialize(TiXmlElement* p_poParent = NULL);
	virtual void	Deserialize(TiXmlElement* p_poParent = NULL);
};

class BallSerializer : public Serializer
{
public:
	BallSerializer(TiXmlDocument* p_poXmlDoc);

	virtual void	Serialize(TiXmlElement* p_poParent = NULL);
	virtual void	Deserialize(TiXmlElement* p_poParent = NULL);
};

class TeamSerializer : public Serializer
{
public:
	TeamSerializer(TiXmlDocument* p_poXmlDoc, const char* p_poTeamName);

	virtual void	Serialize(TiXmlElement* p_poParent = NULL);
	virtual void	Deserialize(TiXmlElement* p_poParent = NULL);

private:
	const char*		m_poTeamName;
};

class PlayerSerializer : public Serializer
{
public:
	PlayerSerializer(TiXmlDocument* p_poXmlDoc, const char* p_poTeamName, s32 p_iIdx);

	virtual void	Serialize(TiXmlElement* p_poParent = NULL);
	virtual void	Deserialize(TiXmlElement* p_poParent = NULL);

private:
	const char*		m_poTeamName;
	s32				m_iIdx;
};

class Vec3Serializer : public Serializer
{
public:
	Vec3Serializer(TiXmlDocument* p_poXmlDoc, Vec3& p_vVector, const char* p_poName);

	virtual void	Serialize(TiXmlElement* p_poParent = NULL);
	virtual void	Deserialize(TiXmlElement* p_poParent = NULL);

private:
	Vec3&			m_vVector;
	const char*		m_poName;
};
#endif