#include "TUI_Module.h"
#include "TUI_Object.h"
#include "TUI_Container.h"
#include "TRender_Renderer.h"
#include "TMath_Matrix4.h"
#include "TRender_Enum.h"
#include "TUI_DisplayStack.h"


namespace TsiU
{
	UIContainer::UIContainer():mHasFocusable(0),mFocusable(0), mFocusedByIndex(0), mFocusedSubContainerIndex(-1)
	{
	}
	UIContainer::~UIContainer()
	{
	}

	void UIContainer::Draw()
	{
		BeforeDraw();
		
		// Draw Object
		// translate
		UIDisplayStack * displayStack = spUIModule->GetDisplayStack();
		displayStack->PushMatrix();
		displayStack->Translate(mPosition.GetX(), mPosition.GetY(), 0.f);
		
		displayStack->PushModifier( & mModifier);
		UIObjectList::iterator iter= spUIModule->FindObject(mObjectName);
		if (iter != spUIModule->EndObject())
		{
			
			iter->second->Draw();
		}

		// Draw SubContainers
		UIContainerVector::iterator iterContainer;
		for(iterContainer = mSubContainerList.begin(); iterContainer!= mSubContainerList.end(); ++iterContainer)
		{
			(*iterContainer)->Draw();
		}

		displayStack->PopModifier();
		displayStack->PopMatrix();


		AfterDraw();
	}

	void UIContainer::Tick(float delta)
	{
	}

	void UIContainer::BeforeDraw()
	{
	}

	void UIContainer::AfterDraw()
	{
	}

	UIContainer * UIContainer::PushSubContainer(EContainerType ct)
	{
		UIContainer * c = NULL;
		c = UIContainer::GetUIModule()->GetFactory()->CreateContainer(ct);
		assert(c);
		if(c)	mSubContainerList.push_back(c);
		
		return c;

	}


	bool UIContainer::AcceptInput(const UIInput * u)
	{
		UIInput uiInput = *u;

		if( mFocusable && (mFocusedByIndex & uiInput.mInputIndex ) )
		{
			// Do things

			return true;
		}
		else if ( mHasFocusable )
		{
			if( uiInput.mInputType == EMouseInput_Mouse )
			{
				uiInput.mValue.pointValue.x -= mPosition.GetX();
				uiInput.mValue.pointValue.y -= mPosition.GetY();
			}

			UIContainerVector::iterator iterContainer = mSubContainerList.begin();
			while( iterContainer != mSubContainerList.end())
				(*iterContainer)->AcceptInput( & uiInput );
		}
	};

	//bool UIContainer::GetIsSubFocusedBy(short index)
	//{
	//}

	UIModule *			UIContainer::spUIModule = NULL;

}
