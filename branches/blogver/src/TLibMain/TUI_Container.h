#ifndef __TUI_CONTAINER_H__
#define __TUI_CONTAINER_H__

#include <string>
#include <vector>

#include "TUtility_UIUtility.h"
#include "TCore_Types.h"
#include "TUI_Factory.h"
#include "TUI_Modifier.h"
#include "TUI_Input.h"


namespace TsiU
{
	class UIModule;
	class UIContainer;
	typedef std::vector<UIContainer *>  UIContainerVector;
	class UIContainer
	{
	public:
		static void SetUIModule(UIModule * ui)
		{
			spUIModule = ui;
		}
		static UIModule * GetUIModule()
		{
			return spUIModule;
		}
	public:
		UIContainer();
		~UIContainer();

	
		virtual void Draw();

		virtual void Tick(float delta);

		void SetName(const char * n){ mName = n; }
		void SetObjectName(const char *n){ mObjectName = n; }

		virtual UIContainer * GetLastSubContainer()
		{
			return mSubContainerList.back();
		}

		virtual UIContainer * GetSubContainer(int i)
		{
			if( i >= mSubContainerList.size())
				return NULL;
			return mSubContainerList.at(i);
		}
		
		virtual UIContainer * GetSubContainer(const char * fn)
		{
			return NULL;
		}

		virtual UIContainer * PushSubContainer(EContainerType ct);

		UIContainer * GetFocusedSubContainer() 
		{
			if( mFocusedSubContainerIndex > -1) 
				return mSubContainerList[ mFocusedSubContainerIndex ] ;

			return NULL;
		}

		void SetPostion(short x, short y){ mPosition.SetX(x), mPosition.SetY(y);}
		void SetSize(short x, short y){ mSize.SetX(x), mSize.SetY(y);}

		const UIPoint & GetPosition() const {return mPosition;} 
		const UIPoint & GetSize() const		{return mSize;}

		void ModifyPoint(const char * name, const UIPoint & p)	{ mModifier.mPointModifiers[name] = p; }
		void ModifyColor(const char * name, unsigned int c)	{ mModifier.mColorModifiers[name] = c; }
		void ModifyTexture(const char * name, UITexture * t)	{ mModifier.mTextureModifiers[name] = t; }
		void ModifyTextureAddress(const char *name, const UITextureUV& a) {mModifier.mTextureAddressModifiers[name] = a;}

		// Input
		virtual bool AcceptInput(const UIInput * );

		bool GetHasFocusable() { return mHasFocusable; }
		bool GetIsFocusable(){ return mFocusable; }
		bool GetIsFocusedBy(short index) { return mFocusedByIndex & index ? true : false; }
		//bool GetIsSubFocusedBy(short index);
		
	protected:
		virtual void BeforeDraw();
		virtual void AfterDraw();

		

		std::string mObjectName;
		std::string mName;
		
		UIPoint			mPosition;
		UIPoint			mSize;

		UIContainerVector			mSubContainerList;			
		int				mFocusedSubContainerIndex;

		UIModifier		mModifier;

		// Input, Focusable
		short			mFocusedByIndex;
		bool			mFocusable			:1;
		bool			mHasFocusable		:1;



		static UIModule *			spUIModule;
	};

	
	

}

#endif