#ifndef __TENGINE__UIOBJECT_H__
#define __TENGINE__UIOBJECT_H__

#include <vector>
#include <map>
#include <string>

#include "TUtility_UIUtility.h"
#include "TCore_Types.h"
#include "TUI_Container.h"
#include "TUI_Factory.h"
#include "TUI_Material.h"

namespace TsiU
{	
	class UIInput;
	class UIModule;
	class UITexture;

	class UIObject
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
		UIObject();
		~UIObject()
		{

		};

		virtual void Draw();
		virtual void DrawSelf();
		virtual void Tick(float delta);
		virtual bool AcceptInput(const UIInput *);

		void SetMaterialTexture(UITexture *t){ mMaterial.SetTexture(t);}
		void SetMaterialSize(short x, short y){ mMaterial.SetSize(x,y);}
		void SetMaterialTopLeftUV(float u, float v){ mMaterial.SetTopLeftUV(u,v);}
		void SetMaterialBottomRightUV(float u, float v){ mMaterial.SetBottomRightUV(u,v);}
		void SetMaterialPosition(short x, short y){ mMaterialPosition.Set(x, y);}
		
		//virtual Bool IsFocused( int InputIndex );
		virtual Bool GetHasFocusable(){return mHasFocusable || mIsFocusable ;}
		virtual Bool GetIsFocusable() { return mIsFocusable; }


		
	protected:
		virtual void BeforeDraw();				
		virtual void AfterDraw();

	protected:
		
		Bool						mHasFocusable;
		Bool						mIsFocusable;
		
		UIRect						mRegion;

		u32							mColor;
		UIMaterial					mMaterial;
		UIPoint						mMaterialPosition;

		static UIModule *			spUIModule;
	};

	
}


#endif