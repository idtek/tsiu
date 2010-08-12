#ifndef __TUI_MODULE__
#define __TUI_MODULE__

#include "TCore_Types.h"
#include "TEngine_Module.h"
#include "TUI_Object.h"
#include "TUI_Factory.h"


#include <stdlib.h>
#include <vector>
#include <map>

namespace TsiU
{
	typedef std::vector<std::string> UIContainerNameList;
	typedef std::map<std::string, UIObject*>	UIObjectList;
	typedef std::map<std::string, UITexture*>	UITextureList;
	typedef std::map<std::string, UIContainer*> UIContainerList;
	class Renderer;
	class UIDisplayStack;
	

	class UIModule:	public IModule
	{
	public:
		UIModule(Renderer* p_poRenderer, UIFactory * p_UIFactory);
		virtual ~UIModule(){};

		virtual void Init(){};
		virtual void RunOneFrame(float p_fDeltaTime);
		virtual void UnInit(){};

		virtual void Draw();

		UIFactory * GetFactory()  { return m_UIFactory;}
		Renderer *	GetRenderer() { return m_poDefaultRenderer;}

		UITexture * CreateTexture(std::string & name, const std::string & filename);

		UIObject * CreateObject(std::string & name, EObjectType ot= EObjectType_Basic);

		UIContainer * CreatePage(std::string & name);


		UITexture*	GetTexture(const std::string & name)
		{
			if(mTextureList.find(name) == mTextureList.end())
				return NULL;
			else
				return mTextureList.find(name)->second;
		}
		UITextureList::iterator	FindTexture(const std::string & name){return mTextureList.find(name); }
		UITextureList::iterator	EndTexture() {return mTextureList.end(); }

		UIObject*				GetUIObject(const std::string & name)
		{
			if(mObjectList.find(name) == mObjectList.end() )
				return NULL;
			else
				return mObjectList.find(name)->second;
		}
		UIObjectList::iterator	FindObject(const std::string & name){return mObjectList.find(name); }
		UIObjectList::iterator	EndObject() {return mObjectList.end(); }

		UIContainer*				GetUIPage(const std::string & name)
		{
			if(mPageList.find(name) == mPageList.end() )
				return NULL;
			else
				return mPageList.find(name)->second;
		}
		UIContainerList::iterator	FindPage(const std::string & name){return mPageList.find(name); }
		UIContainerList::iterator	EndPage() {return mPageList.end(); }

		void	PushDrawStack(const std::string & name){mDrawStack.push_back(name); }
		void	PopDrawStack(const std::string & name){mDrawStack.pop_back();}

		UIDisplayStack *		GetDisplayStack(){ return mDisplayStack;}
	protected:
		UIContainerNameList		mDrawStack;
		UIContainerNameList		mInputStack;

		UIContainerList			mPageList;
		UIObjectList			mObjectList;
		UITextureList			mTextureList;

		Renderer* m_poDefaultRenderer;
		UIFactory *		m_UIFactory;

		UIDisplayStack *		mDisplayStack;

	public:
		// APIs

	};
}

#endif