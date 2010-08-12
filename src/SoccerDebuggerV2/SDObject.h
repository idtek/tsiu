#ifndef __SD_OBJECT_H__
#define __SD_OBJECT_H__

#include "SDGlobalDef.h"
#include <string>
#include "SDMempool.h"

extern Vec3 AdjustPos(const Vec3& _vPos);

class OField : public DrawableObject
{
public:
	virtual void Create(){};
	virtual void Tick(f32 p_fDeltaTime){};
	virtual void Draw();
};

class OWatch : public DrawableObject
{
public:
	virtual void Create(){};
	virtual void Tick(f32 p_fDeltaTime);
	virtual void Draw();

	void AddWatch(UDP_PACK* p_poWatch);

protected:
	struct UDP_PACK_ext
	{
		UDP_PACK_ext(UDP_PACK* rawPack, bool newValue = false)
			:m_Pack(*rawPack)
			,m_NewValue(newValue)
		{}

		UDP_PACK m_Pack;
		bool	 m_NewValue;
	};

	typedef std::map<std::string, UDP_PACK_ext>::iterator WatchIterator;
	std::map<std::string,  UDP_PACK_ext>	m_WatchList;
};

class ODynamicObj;

class OWatchOthers : public OWatch
{
public:
	OWatchOthers()
		:m_wObj(NULL)
	{}
	virtual void Draw();
	
	template<typename T>
	Bool GetValue(const Char* p_poName, T* p_poValue)
	{
		WatchIterator it = m_WatchList.find(p_poName);
		if(it == m_WatchList.end())
		{
			//if(p_poValue)
			//	memset(p_poValue, 0, sizeof(T));
			return false;
		}
		else
		{
			UDP_PACK* p_poPack = &((*it).second.m_Pack);
			if(p_poPack->ulType == MsgType_String)
				strncpy((Char*)p_poValue, p_poPack->unValue._zValue, 32);
			else
				*p_poValue = *(T*)&p_poPack->unValue;
			return true;
		}
	}

	template<typename T>
	Bool SetValue(const Char* p_poName, T* p_poValue)
	{
		WatchIterator it = m_WatchList.find(p_poName);
		if(it == m_WatchList.end())
		{
			return false;
		}
		else
		{
			UDP_PACK* p_poPack = &((*it).second.m_Pack);
			if(p_poPack->ulType == MsgType_String)
				strncpy(p_poPack->unValue._zValue, (Char*)p_poValue, 32);
			else
				*(T*)&p_poPack->unValue = *p_poValue;
			return true;
		}
	}
	void SetWatchObject(const ODynamicObj* wObj) { m_wObj = wObj;	}
	void RemoveWatch();

private:
	const ODynamicObj* m_wObj;
};

class ODynamicObj : public DrawableObject
{
public:
	ODynamicObj()
		:m_bIsShowDetailed(false)
	{
	};
	virtual void Create(){};
	virtual void Tick(f32 p_fDeltaTime){};
	virtual void Draw(){};

	inline void AddWatch(UDP_PACK* p_poWatch)
	{
		m_oWatch.AddWatch(p_poWatch);
	}
	virtual void SetShowDetailed(Bool p_bValue);
	void SetOrientation(const Vec3& v)
	{
		m_Orientation = v;
		Quat q;
		q.ArcToQuat(Vec3::ZUNIT, m_Orientation);
		SetRotation(q);
	}
	const Vec3& GetOrientation()
	{
		return m_Orientation;
	}
protected:
	virtual void TurnOfforOnDetails(bool isOn) = 0;

protected:
	OWatchOthers	m_oWatch;
	Bool			m_bIsShowDetailed;
	Vec3			m_Orientation;
};

class OPlayer : public ODynamicObj
{
public:
	OPlayer(u32 _ul_Team, u32 _ul_Num);

	virtual void	Tick(f32 p_fDeltaTime);
	virtual void	Draw();

	virtual void	SetPosition(const Vec3& v);
	virtual void	SetRotation(const Vec3& v);

protected:
	virtual void TurnOfforOnDetails(bool isOn);

private:
	void			CheckWatchOwner();
	u32				GetFilter();
	u32				m_ulTeam;
	u32				m_ulNum;
};

class OBall : public ODynamicObj
{
public:
	OBall();

	virtual void Tick(f32 p_fDeltaTime);
	virtual void Draw();

	virtual void SetPosition(const Vec3& v);
	virtual void SetRotation(const Vec3& v);

protected:
	virtual void TurnOfforOnDetails(bool isOn);
};

class OTeam : public DrawableObject
{
public:
	OTeam(u32 _ul_Team);

	virtual void Create(){};
	virtual void Tick(f32 p_fDeltaTime);
	virtual void Draw();

	inline OPlayer* GetPlayer(s32 _iIdx)
	{
		return m_PlayerList[_iIdx];
	}
private:
	OPlayer* m_PlayerList[kPLAYER_COUNT];
};

class OParser : public DrawableObject
{
public:
	virtual void Create(){};
	virtual void Tick(f32 p_fDeltaTime);
	virtual void Draw(){};

private:
	void ParserPlayerWatch(OPlayer* p_poPlayer, UDP_PACK* p_poPack);
};

#endif