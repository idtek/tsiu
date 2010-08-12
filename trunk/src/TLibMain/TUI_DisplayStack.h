#ifndef __TUI_DISPLAYSTACK_H__
#define __TUI_DISPLAYSTACK_H__

#include "TMath_Matrix4.h"
#include "TUI_Module.h"
#include "TUI_Modifier.h"

namespace TsiU
{
#define UIDISPLAY_MAX_STACKSIZE 16
	class UIDisplayStack
	{
	public:		
		static void SetUIModule(UIModule * ui)
		{
			spUIModule = ui;
		}

	public:
		UIDisplayStack():mMatrixStackSize(0), mModifierStackSize(0)
		{
			mMatrixStack[0]= Mat4::IDENTITY;
			mModifierStack[0]= new UIModifier;		//prepare first one, others will be Pushed
		}
		virtual ~UIDisplayStack()
		{
			delete mModifierStack[0];				//delete first one
		};

		// matrix stack
		void PushMatrix()
		{
			mMatrixStackSize++; 
			mMatrixStack[mMatrixStackSize]= mMatrixStack[mMatrixStackSize-1];
			assert(mMatrixStackSize < UIDISPLAY_MAX_STACKSIZE);
		}
		void PopMatrix()
		{
			mMatrixStackSize--; 
			assert(mMatrixStackSize>=0);
		}
		const Mat4& GetCurrentMatrix()
		{
			return mMatrixStack[mMatrixStackSize];
		}

		void Translate(float x, float y, float z)
		{ 
			mMatrixStack[mMatrixStackSize] *= Mat4(x,y,z);
		}

		void Scale(float x, float y, float z)
		{

		}

		// Modifier stack
		void PushModifier(UIModifier * mod)
		{
			mModifierStackSize++; 
			mModifierStack[mMatrixStackSize]= mod;
			assert(mModifierStackSize < UIDISPLAY_MAX_STACKSIZE);
		}
		void PopModifier()
		{
			mModifierStack[mModifierStackSize--] = NULL; 
			assert(mModifierStackSize>=0);
		}

		UIModifier * GetCurrentModifier() const
		{
			return mModifierStack[mModifierStackSize];
		}



	protected:

		// matrix stack
		Mat4	mMatrixStack[UIDISPLAY_MAX_STACKSIZE];
		int		mMatrixStackSize;

		// modifier stack
		UIModifier*	mModifierStack[UIDISPLAY_MAX_STACKSIZE];
		int		mModifierStackSize;

	private:
		static UIModule * spUIModule;
	};
}

#endif