#include "SDObject.h"
#include "SDMempool.h"
#include "SDNetWork.h"

Vec3 AdjustPos(const Vec3& _vPos)
{
	Vec3 vNewPos = _vPos;
	vNewPos.x += kRULE_GOALLINE;
	vNewPos.y = -vNewPos.y + kRULE_SIDELINE;
	if(vNewPos.z < 0)
		vNewPos.z = 0.f;
	vNewPos.z += 1.f;
	return vNewPos;
}
Vec3 AdjustOri(const Vec3& _vOri)
{
	Vec3 vNewOri = _vOri;
	vNewOri.y = -vNewOri.y;
	vNewOri.Normalize();
	return vNewOri;
}

/********************************************************************************/
/* OParser
/********************************************************************************/
void OParser::Tick(f32 p_fDeltaTime)
{
	s32 iSize = g_poMem->GetSize();
	if(iSize != 0)
	{
		UDP_PACK *poPacArray = new UDP_PACK[iSize];
		g_poMem->GetUDPData(poPacArray, iSize);
		for(s32 i = 0; i < iSize; ++i)
		{
			UDP_PACK *poPack = poPacArray + i;
			switch(poPack->ulFilter)
			{
				case kFILTER_TEAM_HOME_0:
				case kFILTER_TEAM_HOME_1:
				case kFILTER_TEAM_HOME_2:
				case kFILTER_TEAM_HOME_3:
				case kFILTER_TEAM_HOME_4:
				case kFILTER_TEAM_HOME_5:
				case kFILTER_TEAM_HOME_6:
				case kFILTER_TEAM_HOME_7:
				case kFILTER_TEAM_HOME_8:
				case kFILTER_TEAM_HOME_9:
				case kFILTER_TEAM_HOME_10:
				{
					OTeam* poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>("TeamHome");
					ParserPlayerWatch(poTeam->GetPlayer(poPack->ulFilter - kFILTER_TEAM_HOME_0), poPack);
					break;
				}
				case kFILTER_TEAM_AWAY_0:
				case kFILTER_TEAM_AWAY_1:
				case kFILTER_TEAM_AWAY_2:
				case kFILTER_TEAM_AWAY_3:
				case kFILTER_TEAM_AWAY_4:
				case kFILTER_TEAM_AWAY_5:
				case kFILTER_TEAM_AWAY_6:
				case kFILTER_TEAM_AWAY_7:
				case kFILTER_TEAM_AWAY_8:
				case kFILTER_TEAM_AWAY_9:
				case kFILTER_TEAM_AWAY_10:
				{
					OTeam* poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>("TeamAway");
					ParserPlayerWatch(poTeam->GetPlayer(poPack->ulFilter - kFILTER_TEAM_AWAY_0), poPack);
					break;
				}
				case kFILTER_BALL:
				{
					OBall* poBall = g_poEngine->GetSceneMod()->GetSceneObject<OBall>("ZBall");
					poBall->AddWatch(poPack);
					break;
				}
				case kFILTER_WATCH:
				{
					OWatch* poWatch = g_poEngine->GetSceneMod()->GetSceneObject<OWatch>("Watch");
					poWatch->AddWatch(poPack);
					break;
				}
			}
		}
		delete[] poPacArray;
	}
}

void OParser::ParserPlayerWatch(OPlayer* p_poPlayer, UDP_PACK* p_poPack)
{
	p_poPlayer->AddWatch(p_poPack);
}

/********************************************************************************/
/* OField
/********************************************************************************/
void OField::Draw()
{
	g_poSROU->DrawFillRectangle(kCANVAS_START_X, 
								kCANVAS_START_Y, 
								2 * kRULE_GOALLINE * kPIXEL_PER_METER,
								2 * kRULE_SIDELINE * kPIXEL_PER_METER,
								D_Color(141,198,63), D_Color(255,255,255));
	g_poSROU->DrawLine(kCANVAS_START_X + kRULE_GOALLINE * kPIXEL_PER_METER, 
					   kCANVAS_START_Y, 
					   kCANVAS_START_X + kRULE_GOALLINE * kPIXEL_PER_METER, 
					   kCANVAS_START_Y + 2 * kRULE_SIDELINE * kPIXEL_PER_METER,
					   D_Color(255,255,255), 1);
	g_poSROU->DrawCircle(kCANVAS_START_X + kRULE_GOALLINE * kPIXEL_PER_METER, 
						 kCANVAS_START_Y + kRULE_SIDELINE * kPIXEL_PER_METER, 
						 kRULE_CENTERCIRCLE * kPIXEL_PER_METER,
						 D_Color(255,255,255), 1);
	g_poSROU->DrawRectangle(kCANVAS_START_X, 
							kCANVAS_START_Y + kRULE_PENALTY_POS * kPIXEL_PER_METER, 
							kPIXEL_PER_METER * kRULE_PENALTY_WIDTH,
							kPIXEL_PER_METER * kRULE_PENALTY_HEIGHT,
							D_Color(255,255,255), 1);
	g_poSROU->DrawRectangle(kCANVAS_START_X + (2 * kRULE_GOALLINE - kRULE_PENALTY_WIDTH) * kPIXEL_PER_METER, 
							kCANVAS_START_Y + kRULE_PENALTY_POS * kPIXEL_PER_METER, 
							kPIXEL_PER_METER * kRULE_PENALTY_WIDTH,
							kPIXEL_PER_METER * kRULE_PENALTY_HEIGHT,
							D_Color(255,255,255), 1);
}
/********************************************************************************/
/* ODynamicObj
/********************************************************************************/
void ODynamicObj::SetShowDetailed(Bool p_bValue)
{
	if(p_bValue == false)
	{
		Util::Clear(E_Tab_Entity);
		TurnOfforOnDetails(false);
	}
	else
	{
		TurnOfforOnDetails(true);
	}

	m_bIsShowDetailed = p_bValue;
}
/********************************************************************************/
/* OPlayer
/********************************************************************************/
OPlayer::OPlayer(u32 _ul_Team, u32 _ul_Num)
	:ODynamicObj()
	,m_ulTeam(_ul_Team)
	,m_ulNum(_ul_Num)
{
	SetOrientation(Vec3::XUNIT);
	m_oWatch.SetWatchObject(this);
	m_vPos = Vec3(1000.f, 1000.f, 0.f);
}

void OPlayer::Tick(f32 p_fDeltaTime)
{
	m_oWatch.GetValue<Vec3>("Position", &m_vPos);
	m_oWatch.GetValue<Vec3>("Orientation", &m_Orientation);

	if (g_bIsWatchOwner)
		CheckWatchOwner();

	m_oWatch.RemoveWatch();
}

void OPlayer::SetPosition(const Vec3& v)
{
	Object::SetPosition(v);

	m_oWatch.SetValue<Vec3>("Position", &m_vPos);

	UDP_PACK stMsg;
	stMsg.ulFilter = GetFilter();
	strcpy(stMsg.zName, "Pos");
	stMsg.ulType = MsgType_Reset;
	stMsg.bIsHidden = true;
	stMsg.unValue._vValue.x = v.x;
	stMsg.unValue._vValue.y = v.y;
	stMsg.unValue._vValue.z = v.z;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

void OPlayer::SetRotation(const Vec3& v)
{
	ODynamicObj::SetOrientation(v);

	m_oWatch.SetValue<Vec3>("Orientation", &m_Orientation);

	UDP_PACK stMsg;
	stMsg.ulFilter = GetFilter();
	strcpy(stMsg.zName, "Rot");
	stMsg.ulType = MsgType_Reset;
	stMsg.bIsHidden = true;
	stMsg.unValue._vValue.x = v.x;
	stMsg.unValue._vValue.y = v.y;
	stMsg.unValue._vValue.z = v.z;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

void OPlayer::TurnOfforOnDetails(bool isOn)
{
	UDP_PACK stMsg;
	stMsg.ulFilter = GetFilter();
	stMsg.ulType = MsgType_DetailsSwitcher;
	stMsg.bIsHidden = true;
	stMsg.unValue._iValue = (int)isOn;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

void OPlayer::CheckWatchOwner()
{
	char zState[32];
	m_oWatch.GetValue<char>("State", zState);
	std::string s1(zState);
	std::string s2("BallCatcher");
	if (s1 == s2)
	{
		if(g_WatchPlayer)
			g_WatchPlayer->SetShowDetailed(false);
		SetShowDetailed(true);
		g_WatchPlayer = this;
	}
}

u32 OPlayer::GetFilter()
{
	return (m_ulTeam == kHOME_TEAM ? kFILTER_TEAM_HOME_0 : kFILTER_TEAM_AWAY_0) + m_ulNum;
}

void OPlayer::Draw()
{
	D_Color ClothClr;
	D_Color OutClr(255,255,255);
	if(m_ulTeam == kHOME_TEAM)
		ClothClr = D_Color(255,64,64);
	else
		ClothClr = D_Color(64,159,255);

	if(m_bIsShowDetailed)
		ClothClr = ClothClr * 1.5;

	Vec3 vNewPos = AdjustPos(m_vPos);
	Vec3 vNewOri = AdjustOri(m_Orientation);
	f32 vNewRadius = kPLAYER_RADIUS + kPLAYER_RADIUS * vNewPos.z / 10;

	s32 iConType;
	m_oWatch.GetValue<s32>("ConType", &iConType);
	if(iConType == 1)
		OutClr = D_Color(128,128,255);		//Input
	else if(iConType == 20)
		OutClr = D_Color(0,0,0);		//Defend
	else if(iConType == 21)
		OutClr = D_Color(255,255,255);		//Attack
	else if(iConType == 3)
		OutClr = D_Color(255,255,0);		//Gk
	else if(iConType == 5)
		OutClr = D_Color(128,128,255);		//Robot

	if(g_bIsShowDetailed)
	{
		s32 iPlayer;
		Char zName[16];
		for(s32 i = 0; i < 3; ++i)
		{
			sprintf(zName, "defend%d_Line", i);
			Bool bRet = m_oWatch.GetValue<s32>(zName, &iPlayer);
			if(bRet && iPlayer != 99)
			{
				OTeam* poTeam;
				if(m_ulTeam == kHOME_TEAM)
				{
					poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>("TeamAway");
				}
				else
				{
					poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>("TeamHome");
				}
				OPlayer* poPlayer = poTeam->GetPlayer(iPlayer);
				Vec3 vDPos = poPlayer->GetPosition();
				Vec3 vDNewPos = AdjustPos(vDPos);
				g_poSROU->DrawLine(kCANVAS_START_X + vNewPos.x * kPIXEL_PER_METER, 
									kCANVAS_START_Y + vNewPos.y * kPIXEL_PER_METER,
									kCANVAS_START_X + vDNewPos.x * kPIXEL_PER_METER,
									kCANVAS_START_Y + vDNewPos.y * kPIXEL_PER_METER,
									D_Color(0,0,0), 1);
			}
		}
	}

	g_poSROU->DrawFillCircle(kCANVAS_START_X + vNewPos.x * kPIXEL_PER_METER, 
							 kCANVAS_START_Y + vNewPos.y * kPIXEL_PER_METER, 
							 vNewRadius * kPIXEL_PER_METER,
							 ClothClr, OutClr);
	if(m_Orientation != Vec3::ZERO)
	{

		Vec3 vStart = vNewPos + vNewOri * vNewRadius;
		Vec3 vEnd = vStart + vNewOri * kPLAYER_DIRLINE;

		g_poSROU->DrawLine(kCANVAS_START_X + vStart.x * kPIXEL_PER_METER, 
							kCANVAS_START_Y + vStart.y * kPIXEL_PER_METER,
							kCANVAS_START_X + vEnd.x * kPIXEL_PER_METER, 
							kCANVAS_START_Y + vEnd.y * kPIXEL_PER_METER,
							OutClr, 1);
	}
	Char zNum[4];
	itoa(m_ulNum, zNum, 10);
	g_poSROU->DrawStringEx(kCANVAS_START_X + (vNewPos.x - vNewRadius) * kPIXEL_PER_METER, 
						   kCANVAS_START_Y + (vNewPos.y - vNewRadius) * kPIXEL_PER_METER,
						   2 * vNewRadius * kPIXEL_PER_METER, 
						   2 * vNewRadius * kPIXEL_PER_METER,
						   kPLAYER_NUMSIZE * kPIXEL_PER_METER,
						   zNum, NULL, D_Color(0,0,0));

	if(m_bIsShowDetailed)
		m_oWatch.Draw();
}

/********************************************************************************/
/* OBall
/********************************************************************************/
OBall::OBall()
	:ODynamicObj()
{
	m_oWatch.SetWatchObject(this);
	m_vPos = Vec3(1000.f, 1000.f, 0.f);
}
void OBall::Tick(f32 p_fDeltaTime)
{
	m_oWatch.GetValue<Vec3>("Position", &m_vPos);
	m_oWatch.RemoveWatch();
}

void OBall::Draw()
{
	D_Color ClothClr(255,128,64);
	if(m_bIsShowDetailed)
		ClothClr = ClothClr / 1.5;

	Vec3 vNewPos = AdjustPos(m_vPos);
	f32 vNewRadius = kBALL_RADIUS + kBALL_RADIUS * vNewPos.z / 10;

	g_poSROU->DrawFillCircle(kCANVAS_START_X + vNewPos.x * kPIXEL_PER_METER, 
							 kCANVAS_START_Y + vNewPos.y * kPIXEL_PER_METER, 
							 vNewRadius * kPIXEL_PER_METER,
							 ClothClr, D_Color(0,0,0));

	if(m_bIsShowDetailed)
		m_oWatch.Draw();
}

void OBall::SetPosition(const Vec3& v)
{
	Object::SetPosition(v);

	m_oWatch.SetValue<Vec3>("Position", &m_vPos);

	UDP_PACK stMsg;
	stMsg.ulFilter = kFILTER_BALL;
	strcpy(stMsg.zName, "Pos");
	stMsg.ulType = MsgType_Reset;
	stMsg.bIsHidden = true;
	stMsg.unValue._vValue.x = v.x;
	stMsg.unValue._vValue.y = v.y;
	stMsg.unValue._vValue.z = v.z;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

void OBall::SetRotation(const Vec3& v)
{
	ODynamicObj::SetOrientation(v);

	m_oWatch.SetValue<Vec3>("Orientation", &m_Orientation);

	UDP_PACK stMsg;
	stMsg.ulFilter = kFILTER_BALL;
	strcpy(stMsg.zName, "Rot");
	stMsg.ulType = MsgType_Reset;
	stMsg.bIsHidden = true;
	stMsg.unValue._vValue.x = v.x;
	stMsg.unValue._vValue.y = v.y;
	stMsg.unValue._vValue.z = v.z;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

void OBall::TurnOfforOnDetails(bool isOn)
{
	UDP_PACK stMsg;
	stMsg.ulFilter = kFILTER_BALL;
	stMsg.ulType = MsgType_DetailsSwitcher;
	stMsg.bIsHidden = true;
	stMsg.unValue._iValue = (int)isOn;

	WSABUF stWSABuf;
	stWSABuf.buf = (char*)&stMsg;
	stWSABuf.len = sizeof(stMsg);

	DWORD cbRet = 0;
	WSASendTo(g_hSSock, &stWSABuf,1, &cbRet, 0, (struct sockaddr*)&g_stDestAddr, sizeof(struct sockaddr), NULL, NULL);
}

/********************************************************************************/
/* OTeam
/********************************************************************************/
OTeam::OTeam(u32 _ul_Team)
{
	for(int i = 0; i < kPLAYER_COUNT; ++i)
	{
		m_PlayerList[i] = new OPlayer(_ul_Team, i);
	}
}

void OTeam::Tick(f32 p_fDeltaTime)
{
	for(int i = 0; i < kPLAYER_COUNT; ++i)
	{
		m_PlayerList[i]->Tick(p_fDeltaTime);
	}
}
void OTeam::Draw()
{
	for(s32 i = 0; i < kPLAYER_COUNT; ++i)
	{
		m_PlayerList[i]->Draw();
	}
}

/********************************************************************************/
/* OWatch
/********************************************************************************/
void OWatch::AddWatch(UDP_PACK* p_poWatch)
{	
	WatchIterator it = m_WatchList.find(p_poWatch->zName);
	if(it == m_WatchList.end())
		m_WatchList.insert(std::pair<std::string, UDP_PACK_ext>(p_poWatch->zName, UDP_PACK_ext(p_poWatch)));
	else
	{
		bool isNewValue = false;
		const UDP_PACK& lastPack = (*it).second.m_Pack;
		switch(p_poWatch->ulType)
		{
		case MsgType_Float:
			isNewValue = !Math::IsEqual(lastPack.unValue._fValue, p_poWatch->unValue._fValue);
			break;
		case MsgType_Vector:
			isNewValue = !Math::IsEqual(lastPack.unValue._vValue.x, p_poWatch->unValue._vValue.x) ||
						 !Math::IsEqual(lastPack.unValue._vValue.y, p_poWatch->unValue._vValue.y) ||
						 !Math::IsEqual(lastPack.unValue._vValue.z, p_poWatch->unValue._vValue.z);
			break;
		case MsgType_Int:
			isNewValue = (lastPack.unValue._iValue != p_poWatch->unValue._iValue);
			break;
		case MsgType_String:
			isNewValue = strcmp(lastPack.unValue._zValue, p_poWatch->unValue._zValue);
			break;
		}
		(*it).second = UDP_PACK_ext(p_poWatch, isNewValue);
	}
}

void OWatch::Tick(f32 p_fDeltaTime)
{
}

void OWatch::Draw()
{
	//g_poSROU->DrawString(kWATCH_START_X, 
	//					 kWATCH_START_Y,
	//					 "Player Watch", D_Color(255,255,255));
	//g_poSROU->DrawString(kWATCH_START_X, 
	//					 kWATCH_START_Y2,
	//					 "User Watch", D_Color(255,255,255));
	//Util::Clear();

	//s32 iLine = 1;
	WatchIterator it;
	for(it = m_WatchList.begin(); it != m_WatchList.end(); it++)
	{
		UDP_PACK_ext* poWatch = &((*it).second);

		Util::DrawValue(E_Tab_Watch, &poWatch->m_Pack, poWatch->m_NewValue);

		/*g_poSROU->DrawString(kWATCH_START_X, 
							 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
							 poWatch->zName, D_Color(255,255,0));
		Char zValue[16];
		switch(poWatch->ulType)
		{
			case MsgType_Float:
				sprintf(zValue, "%f", poWatch->unValue._fValue);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				break;
			case MsgType_Vector:
				sprintf(zValue, "x=%f", poWatch->unValue._vValue.x);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				sprintf(zValue, "y=%f", poWatch->unValue._vValue.y);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				sprintf(zValue, "z=%f", poWatch->unValue._vValue.z);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				break;
			case MsgType_Int:
				sprintf(zValue, "%d", poWatch->unValue._iValue);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				break;
			case MsgType_String:
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y2 + kWATCH_FONTHEIGHT * iLine,
									 poWatch->unValue._zValue, D_Color(255,255,0));
				iLine++;
				break;
		}*/
	}
}
/********************************************************************************/
/* OWatchOthers
/********************************************************************************/
void OWatchOthers::RemoveWatch()
{
	if(g_WatchPlayer != m_wObj)
		m_WatchList.clear();
}
void OWatchOthers::Draw()
{	
	//s32 iLine = 1;

	WatchIterator it;
	for(it = m_WatchList.begin(); it != m_WatchList.end(); it++)
	{
		UDP_PACK_ext* poWatch = &((*it).second);
		if(poWatch->m_Pack.bIsHidden)
			continue;

		//Hacky!
		const char* name = poWatch->m_Pack.zName;
		if(strstr(name, "ATTR_"))
		{
			Util::DrawValue(E_Tab_Attributes, &poWatch->m_Pack, poWatch->m_NewValue);
		}
		else if(strstr(name, "STAT_"))
		{
			Util::DrawValue(E_Tab_Statistics, &poWatch->m_Pack, poWatch->m_NewValue);
		}
		else
		{
			Util::DrawValue(E_Tab_Entity, &poWatch->m_Pack, poWatch->m_NewValue);
		}	

		/*g_poSROU->DrawString(kWATCH_START_X, 
							 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
							 poWatch->zName, D_Color(255,255,0));
		Char zValue[16];
		switch(poWatch->ulType)
		{
			case MsgType_Float:
				sprintf(zValue, "%f", poWatch->unValue._fValue);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				break;
			case MsgType_Vector:
				sprintf(zValue, "x=%f", poWatch->unValue._vValue.x);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				sprintf(zValue, "y=%f", poWatch->unValue._vValue.y);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				sprintf(zValue, "z=%f", poWatch->unValue._vValue.z);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				break;
			case MsgType_Int:
				sprintf(zValue, "%d", poWatch->unValue._iValue);
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
									 zValue, D_Color(255,255,0));
				iLine++;
				break;
			case MsgType_String:
				g_poSROU->DrawString(kWATCH_START_X2, 
									 kWATCH_START_Y + kWATCH_FONTHEIGHT * iLine,
									 poWatch->unValue._zValue, D_Color(255,255,0));
				iLine++;
				break;
		}*/
	}
}