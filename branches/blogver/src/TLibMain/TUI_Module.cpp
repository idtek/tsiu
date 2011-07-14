#include "TUI_Module.h"
#include "TUI_Object.h"
#include "TUI_Container.h"
#include "TRender_UITexture.h"
#include "TRender_Renderer.h"
#include "TMath_Matrix4.h"
#include "TRender_Enum.h"

namespace TsiU
{
	UIModule::UIModule(Renderer* p_poRenderer, UIFactory * p_UIFactory)
		:IModule()//, m_poDefaultCamera(NULL)
		,m_poDefaultRenderer(p_poRenderer)
		,m_UIFactory(p_UIFactory)
	{
		UIObject::SetUIModule(this);
		UIContainer::SetUIModule(this);

		UITexture::SetRenderer(p_poRenderer);

		mDisplayStack = p_UIFactory->CreateDisplayStack();
		
	}

	void UIModule::RunOneFrame(float p_fDeltaTime)
	{
		UIContainerNameList::iterator iter;
		UIContainerList::iterator iterPage;
		for(iter = mDrawStack.begin(); iter != mDrawStack.end(); ++iter)
		{
			iterPage = mPageList.find(*iter);
			if(iterPage != mPageList.end())
			{
				iterPage->second->Tick(p_fDeltaTime);
			}
		}
	}

	void UIModule::Draw()
	{
		float  l, r, t, b,zf, zn;
		l=0;
		r=1280;		
		t=0;
		b=960;
		zf=1.0f;
		zn=0.f;

		Mat4 proj;
		proj.MakeProjectionOrthoMatrix(l, r, t, b, zn, zf);

		m_poDefaultRenderer->Clear(0, E_ClearFlagZBuffer);
		m_poDefaultRenderer->SetProjectionMatrix( proj);	

		float xOffset=0, yOffset=0, zOffset=0.f;
		Mat4 view(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			xOffset, yOffset, zOffset, 1
			);
		m_poDefaultRenderer->SetViewMatrix(view);	

		UIContainerNameList::iterator iter;
		UIContainerList::iterator iterPage;
		for(iter = mDrawStack.begin(); iter != mDrawStack.end(); ++iter)
		{
			iterPage = mPageList.find(*iter);
			if(iterPage != mPageList.end())
			{
				iterPage->second->Draw();
			}
		}
	}

	UITexture* UIModule::CreateTexture(std::string & name, const std::string & filename)
	{
		UITexture * texture = m_poDefaultRenderer->CreateUITexture(filename.c_str());
		if(mTextureList.find(name) == mTextureList.end())
		{
			mTextureList[name] = texture;
			return texture;
		}
		else
		{
			char buffer[12] = {};
			std::string newname = name;
			int i =1;

			do 
			{
				_itoa_s(i++, buffer,12, 10);
				newname = name + "_" + buffer;
			}
			while (mTextureList.find(newname) == mTextureList.end());

			mTextureList[newname] =texture;
			name = newname;
			return texture;
		}
	}

	UIObject * UIModule::CreateObject(std::string & name, EObjectType ot)
	{
		UIObject * obj = m_UIFactory->CreateObject(ot);
		if(! obj)
			return NULL;

		if(mObjectList.find(name) == mObjectList.end() )			
		{			
			mObjectList[name] = obj;
			return obj;
		}
		else
		{
			char buffer[12] = {};
			std::string newname = name;
			int i =1;
			do 
			{
				_itoa_s(i++, buffer,12, 10);
				newname = name + "_" + buffer;
			}
			while (mObjectList.find(newname) == mObjectList.end());

			mObjectList[newname] = obj;
			
			name = newname;

			return obj;
		}
		
	}

	UIContainer * UIModule::CreatePage(std::string & name)
	{
		UIContainer * page = m_UIFactory->CreateContainer(EContainerType_Basic);
		if(! page)
			return NULL;

		if(mPageList.find(name) == mPageList.end() )			
		{			
			mPageList[name] = page;
			return page;
		}
		else
		{
			char buffer[12] = {};
			std::string newname = name;
			int i =1;
			do 
			{
				_itoa_s(i++, buffer,12, 10);
				newname = name + "_" + buffer;
			}
			while (mPageList.find(newname) == mPageList.end());

			mPageList[newname] = page;

			name= newname;

			return page;
		}
		
	}
}
