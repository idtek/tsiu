#include "TEngine_SceneModule.h"
#include "TEngine_Object.h"
#include "TRender_Camera.h"
#include "TRender_Renderer.h"
#include "TMath_Matrix4.h"
#include "TRender_Enum.h"
#include "TEngine_Private.h"
#include "TEngine_RenderModule.h"

namespace TsiU
{
	SceneModule::SceneModule()
		:m_poDefaultCamera(NULL), m_poDefaultRenderer(NULL)
	{

	}
		
	void SceneModule::Init()
	{
		m_poDefaultRenderer = poGetEngine()->GetRenderMod()->GetRenderer();

		for(u32 i = 0; i < m_poLightList.Size(); ++i)
			m_poDefaultRenderer->SetLight(i, *m_poLightList[i]);

		ObjIterator it;
		for(it = m_ObjList.begin(); it != m_ObjList.end(); it++)
		{
			Object* pObj = (*it).second;
			pObj->Create();
		}
	}

	void SceneModule::UnInit()
	{
		ObjIterator it;
		for(it = m_ObjList.begin(); it != m_ObjList.end(); it++)
		{
			Object* pObj = (*it).second;
			D_SafeDelete(pObj);
		}
		m_ObjList.clear();

		for(u32 i = 0; i < m_poLightList.Size(); ++i)
			D_SafeDelete(m_poLightList[i]);
		m_poLightList.Clear();

		for(u32 i = 0; i < m_poCameraList.Size(); ++i)
			D_SafeDelete(m_poCameraList[i]);
		m_poCameraList.Clear();
	}

	void SceneModule::RunOneFrame(float p_fDeltaTime)
	{
		if(m_poDefaultCamera)
		{
			m_poDefaultRenderer->SetViewMatrix(m_poDefaultCamera->MakeViewMatrix());
			m_poDefaultRenderer->SetProjectionMatrix(m_poDefaultCamera->MakeProjectionMatrix());
		}
	
		ObjIterator it;
		for(it = m_ObjList.begin(); it != m_ObjList.end(); it++)
		{
			Object* pObj = (*it).second;
			if(pObj->HasControlFlag(E_OCF_Active))
				pObj->Tick(p_fDeltaTime);
		}

	}

	void SceneModule::Draw()
	{		
		ObjIterator it;

		for(it = m_ObjList.begin(); it != m_ObjList.end(); it++)
		{
			Object* pObj = (*it).second;
			pObj->UpdateMatrix();
			if(pObj->HasControlFlag(E_OCF_Drawable))
			{
				DrawableObject *pDObj = (DrawableObject *)pObj;
				pDObj->Draw();
			}
		}

	}
	Bool SceneModule::AddObject(const Char* p_poName, Object* p_poObj)
	{
		Object* l_poObj = GetSceneObject<Object>(p_poName);
		if(l_poObj)
			return false;
		else
			m_ObjList.insert(std::pair<const Char*, Object*>(p_poName, p_poObj));
		return true;
	}
	Bool SceneModule::AddGuiObject(u8 p_ucId, Object* p_poObj)
	{
		Char strBuff[16] = "";
		sprintf(strBuff, "__GUIOBJ_%d", p_ucId);
		return AddObject(strBuff, p_poObj);
	}
	Bool SceneModule::AddCamera(Camera* p_poCamera, Bool _bIsDefault)
	{
		D_CHECK(p_poCamera);
		m_poCameraList.PushBack(p_poCamera);

		if(_bIsDefault)
			m_poDefaultCamera = p_poCamera;
		return true;
	}
	Bool SceneModule::AddLight(Light* p_poLight)
	{
		D_CHECK(p_poLight);
		m_poLightList.PushBack(p_poLight);

		return true;
	}
}
