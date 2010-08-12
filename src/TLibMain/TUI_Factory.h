#ifndef __TUI_FACTORY_H__
#define __TUI_FACTORY_H__

namespace TsiU
{
	class UIObject;
	class UIContainer;
	class UIDisplayStack;

	enum EObjectType
	{
		EObjectType_Basic = 0,
		EObjectType_Max

	};

	enum EContainerType
	{
		EContainerType_Basic = 0,
		EContainerType_Max
	};

	class UIObjectFactory
	{
	public:
		virtual UIObject * CreateObject(EObjectType ) const ;

	};

	class UIContainerFactory
	{
	public:
		virtual UIContainer * CreateContainer(EContainerType ) const;
	};


	class UIFactory
	{
	public:
		UIFactory();
		virtual UIObject * CreateObject(EObjectType ot) const ;
		virtual UIContainer * CreateContainer(EContainerType ct) const ;
		
		virtual UIDisplayStack * CreateDisplayStack() const;

		UIObjectFactory *	mObjectFactory;
		UIContainerFactory * mContainerFactory;
	};
}

#endif