#include "TUI_Factory.h"
#include "TUI_Object.h"
#include "TUI_Container.h"
#include "TUI_DisplayStack.h"

namespace TsiU
{


	UIContainer * UIContainerFactory::CreateContainer(EContainerType ct) const
	{
		switch (ct)
		{
		case EContainerType_Basic:
			return new UIContainer;
		default:
			assert(0);
			return NULL;
		}
	}

	UIObject * UIObjectFactory::CreateObject(EObjectType ot) const
	{
		switch (ot)
		{
		case EObjectType_Basic:
			return new UIObject;
		default:
			assert(0);
			return NULL;
		}
	}


	
	UIFactory::UIFactory()
	{
		mObjectFactory = new UIObjectFactory;
		mContainerFactory = new UIContainerFactory;

	}
	
	
	UIObject * UIFactory::CreateObject(EObjectType ot) const
	{
		return mObjectFactory->CreateObject(ot);
	}

	UIContainer * UIFactory::CreateContainer(EContainerType ct) const
	{
		return mContainerFactory->CreateContainer(ct);
	}

	UIDisplayStack * UIFactory::CreateDisplayStack() const
	{
		return new UIDisplayStack();
	}




}