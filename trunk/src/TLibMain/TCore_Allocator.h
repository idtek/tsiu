#ifndef __TCORE_ALLOCATOR__ 
#define __TCORE_ALLOCATOR__

#include "TCore_Types.h"
#include "TCore_Creator.h"

namespace TsiU
{
	class Allocator
	{
	public:
		virtual void  Init()								= 0;
		virtual void* Alloc(u32 _uiSize)					= 0;
		virtual void* Realloc(void* _poMem, u32 _uiSize)	= 0;
		virtual void  Free(void* _poMem)					= 0;
	};

	class DefaultAllocator : public Allocator
	{
	public:
		virtual void  Init(){};
		virtual void* Alloc(u32 _uiSize);
		virtual void* Realloc(void* _poMem, u32 p_uiSize);
		virtual void  Free(void* _poMem);
	};

	/************************************************************************/
	/* Global Func                                                          */
	/************************************************************************/
	extern Allocator* g_poAllocator;
}

#endif