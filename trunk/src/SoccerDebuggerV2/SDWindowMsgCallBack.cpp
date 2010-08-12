#include "SDWindowMsgCallBack.h"
#include "SDObject.h"
#include "TMath_Quaternion.h"

bool SetSelected(ODynamicObj* p_poObj, f32 p_fRadius, const Vec3& p_vMousePos)
{
	Vec3 vPlayerPos = p_poObj->GetPosition();
	vPlayerPos = AdjustPos(vPlayerPos);
	vPlayerPos *= kPIXEL_PER_METER;
	vPlayerPos.x += kCANVAS_START_X;
	vPlayerPos.y += kCANVAS_START_Y;
	vPlayerPos.z = 0;
	Vec3 vMousePos = p_vMousePos;
	Vec3 vDiff = vMousePos - vPlayerPos;
	if(vDiff.LengthSq() < p_fRadius * kPIXEL_PER_METER * p_fRadius * kPIXEL_PER_METER)	
	{
		if(g_WatchPlayer)
			g_WatchPlayer->SetShowDetailed(false);
		p_poObj->SetShowDetailed(true);
		g_WatchPlayer = p_poObj;
		return true;
	}
	return false;
}

SDWindowMsgCallBack::SDWindowMsgCallBack()
:	m_bIsAltHeld(false)
,	m_bIsControlHeld(false)
{
}

void SDWindowMsgCallBack::OnMouseLDown(s32 x, s32 y)
{
	Vec3 vMousePos(x, y, 0);

	OBall* poBall = g_poEngine->GetSceneMod()->GetSceneObject<OBall>("ZBall");
	if(SetSelected(poBall, kBALL_RADIUS, vMousePos))
	{
		return;
	}
	OTeam* poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>("TeamHome");
	for(s32 i = 0; i < kPLAYER_COUNT; ++i)
	{
		OPlayer* poPlayer = poTeam->GetPlayer(i);
		if(SetSelected(poPlayer, kPLAYER_RADIUS, vMousePos))
		{
			return;
		}
	}
	poTeam = g_poEngine->GetSceneMod()->GetSceneObject<OTeam>("TeamAway");
	for(s32 i = 0; i < kPLAYER_COUNT; ++i)
	{
		OPlayer* poPlayer = poTeam->GetPlayer(i);
		if(SetSelected(poPlayer, kPLAYER_RADIUS, vMousePos))
		{
			return;
		}
	}

	if(g_WatchPlayer)
	{
		g_WatchPlayer->SetShowDetailed(false);

		if (m_bIsAltHeld)
		{
			Vec3 vRealMousePos = Vec3((x - kCANVAS_START_X) / kPIXEL_PER_METER - kRULE_GOALLINE, -(y - kCANVAS_START_Y) / kPIXEL_PER_METER + kRULE_SIDELINE, 0);
			Vec3 newRot = (vRealMousePos - g_WatchPlayer->GetPosition());
			newRot.Normalize();
			g_WatchPlayer->SetOrientation(newRot);
		}
	}
}

void SDWindowMsgCallBack::OnMouseLDrag(s32 x, s32 y, s32 relX, s32 relY)
{
	if (g_WatchPlayer == NULL)
		return;

	// Change position
	if (m_bIsControlHeld)
	{
		Vec3 vRealDisplacement = Vec3(relX / kPIXEL_PER_METER, -relY / kPIXEL_PER_METER, 0);
		g_WatchPlayer->SetPosition(g_WatchPlayer->GetPosition() + vRealDisplacement);
	}
	// Change orientation
	else if (m_bIsAltHeld)
	{
		Vec3 vRealMousePos = Vec3((x - kCANVAS_START_X) / kPIXEL_PER_METER - kRULE_GOALLINE, -(y - kCANVAS_START_Y) / kPIXEL_PER_METER + kRULE_SIDELINE, 0);
		Vec3 newRot = (vRealMousePos - g_WatchPlayer->GetPosition());
		newRot.Normalize();
		g_WatchPlayer->SetOrientation(newRot);
	}
}

void SDWindowMsgCallBack::OnKeyDown(u32 key, s64 info)
{
	m_bIsAltHeld = key == VK_MENU;
	m_bIsControlHeld = key == VK_CONTROL;
}

void SDWindowMsgCallBack::OnKeyUp(u32 key, s64 info)
{
	if (key == VK_MENU)
		m_bIsAltHeld = false;
	else if (key == VK_CONTROL)
		m_bIsControlHeld = false;
}

void SDWindowMsgCallBack::OnWindowResize(s32 width, s32 height)
{
	g_WindowWidth = width;
	g_WindowHeight = height;

	//hack!!
	WinGDIRenderer* pRender = (WinGDIRenderer*)g_poEngine->GetRenderMod()->GetRenderer();
	if(pRender && pRender->GetMainWindow())
		pRender->OnWindowResize(width, height);

	/*Vec3 vNewPos;

	for(s32 i = 1; i < E_GUI_Total; ++i)
	{
		GuiButton *poButton = g_poEngine->GetScene()->GetGuiObject<GuiButton>(i);
		vNewPos = poButton->GetPosition();
		vNewPos.y = kADDITION_START_Y + 30;
		if (i == E_GUI_ButtonOpen || i == E_GUI_ButtonSave || i == E_GUI_ButtonOpenLast)
			vNewPos.y -= 50;
		poButton->SetPosition(vNewPos);
	}*/
}