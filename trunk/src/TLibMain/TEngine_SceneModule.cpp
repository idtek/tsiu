#include "TEngine_SceneModule.h"
#include "TRender_Camera.h"
#include "TRender_Renderer.h"
#include "TMath_Matrix4.h"
#include "TRender_Enum.h"
#include "TEngine_Private.h"
#include "TEngine_RenderModule.h"
#include "TCore_LibSettings.h"
#include "TRender_Light.h"

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

	void SceneModule::RunOneFrame(float _fDeltaTime)
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
				pObj->Tick(_fDeltaTime);
		}

	}

	void SceneModule::Draw()
	{		
		if(GetLibSettings()->IsDefined(E_LS_Has_GDI))
		{
			Array<Object*> objectArrayWithZOrder[EZOrder_Max];

			ObjIterator it;
			for(it = m_ObjList.begin(); it != m_ObjList.end(); it++)
			{
				Object* pObj = (*it).second;
				pObj->UpdateMatrix();
				if(pObj->HasControlFlag(E_OCF_Drawable))
				{
					objectArrayWithZOrder[pObj->GetZOrder()].PushBack(pObj);
				}
			}
			for(s32 i = EZOrder_Bottom; i >= EZOrder_Top; --i)
			{
				Array<Object*>& pCurObjectArray = objectArrayWithZOrder[i];
				for(s32 j = 0; j < pCurObjectArray.Size(); ++j)
				{
					DrawableObject* pDObj = (DrawableObject*)pCurObjectArray[j];
					pDObj->Draw();
				}
			}
		}
		else
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
	}
	Bool SceneModule::AddObject(const Char* _poName, Object* _poObj)
	{
		Object* poObj = GetSceneObject<Object>(_poName);
		if(poObj)
			return false;
		else
		{
			m_ObjList.insert(std::pair<const Char*, Object*>(_poName, _poObj));
		}
		return true;
	}
	//Bool SceneModule::AddGuiObject(u8 _ucId, Object* _poObj)
	//{
	//	Char strBuff[16] = "";
	//	sprintf(strBuff, "__GUIOBJ_%d", _ucId);
	//	return AddObject(strBuff, _poObj);
	//}
	Bool SceneModule::AddCamera(Camera* _poCamera, Bool _bIsDefault)
	{
		D_CHECK(_poCamera);
		m_poCameraList.PushBack(_poCamera);

		if(_bIsDefault)
			m_poDefaultCamera = _poCamera;
		return true;
	}
	Bool SceneModule::AddLight(Light* _poLight)
	{
		D_CHECK(_poLight);
		m_poLightList.PushBack(_poLight);

		return true;
	}
}
