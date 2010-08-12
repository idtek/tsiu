#ifndef __TCORE_CREATOR__
#define __TCORE_CREATOR__

#include "TCore_Types.h"

namespace TsiU
{
	enum
	{
		E_CreatorType_Allocator,
		E_CreatorType_Panic,
		E_CreatorType_Tick,
		E_CreatorType_Renderer,
		E_CreatorType_RenderWindow,
		E_CreatorType_RenderWindowMsg,
		E_CreatorType_Input,
		E_CreatorType_Socket,
		E_CreatorType_File,

		E_CreatorType_Number
	};

	class Creator
	{
	public:
		virtual void* Create() = 0;
	};

	template<typename T>
	class T_Creator : public Creator
	{
	public:
		virtual void* Create()
		{
			return new T;
		}
	};

	class NullCreator : public Creator
	{
	public:
		virtual void* Create()
		{
			return NULL;
		}
	};
}

#endif