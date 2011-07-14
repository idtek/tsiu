#ifndef __TENGINE_SCENEMODULE__
#define __TENGINE_SCENEMODULE__


#include "TEngine_Module.h"
#include <map>
#include <string>
#include "TUtility_Array.h"
#include "TEngine_Object.h"

namespace TsiU
{

	//#define BEGINE_DEFINE_GUI(poEngine, ulIdx, poGuiObj)	{ \
	//	GuiObject* l_poGuiObj = poGuiObj;	\
	//	poEngine->GetSceneMod()->AddGuiObject(ulIdx, l_poGuiObj);

	//#define REGISTER_GUI_EVENT_HANDLER(ulType, poHandler)  \
	//	l_poGuiObj->RegisterEventHandler(ulType, poHandler);

	//#define END_DEFINE_GUI()	}

}

namespace TsiU
{
	//class Object;
	class Renderer;
	class Camera;
	class Light;

	class SceneModule : public IModule
	{
	public:
		SceneModule();

		virtual void Init();
		virtual void RunOneFrame(float _fDeltaTime);
		virtual void UnInit();

		//virtual Bool AddGuiObject(u8 _ucId, Object* _poObj);
		virtual Bool AddObject(const Char* _poName,Object* _poObj);
		virtual Bool AddCamera(Camera* _poCamera, Bool _bIsDefault);
		virtual Bool AddLight(Light* _poLight);
		
		virtual void Draw();

		inline Camera* GetCamera()
		{
			return m_poDefaultCamera;
		}
		inline void SetDefaultCamera(Camera* _poCamera)
		{
			m_poDefaultCamera = _poCamera;
		}
		inline void SetDefaultCamera(u32 _uiId)
		{
			if(_uiId >= 0 && _uiId < m_poCameraList.Size())
			{
				m_poDefaultCamera = m_poCameraList[_uiId];
			}
		}

		template<typename T>
		T* GetSceneObject(const Char* _poName);

		//template<typename T>
		//T* GetGuiObject(u8 _ucId);

	private:
		typedef std::map<std::string, Object*>::iterator ObjIterator;
		std::map<std::string,  Object*>	m_ObjList;

		Renderer* m_poDefaultRenderer;
		Camera* m_poDefaultCamera;

		Array<Light*> m_poLightList;
		Array<Camera*> m_poCameraList;
	};

	template<typename T>
	T* SceneModule::GetSceneObject(const Char* _poName)
	{
		ObjIterator it = m_ObjList.find(_poName);
		if(it == m_ObjList.end())
			return NULL;
		else
			return (T*)((*it).second);
	}
	//template<typename T>
	//T* SceneModule::GetGuiObject(u8 _ucId)
	//{
	//	Char strBuff[16] = "";
	//	sprintf(strBuff, "__GUIOBJ_%d", _ucId);
	//	return GetSceneObject<T>(strBuff);
	//}
}

#endif