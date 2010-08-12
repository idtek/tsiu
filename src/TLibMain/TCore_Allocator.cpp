#include "TCore_Allocator.h"

namespace TsiU
{
	void* DefaultAllocator::Alloc( u32 _uiSize )
	{
		D_CHECK( _uiSize > 0 );
		void *poMem = malloc( _uiSize );
		D_CHECK( poMem );
		return poMem;
	}
	void* DefaultAllocator::Realloc( void* _poMem, u32 _uiSize )
	{
		D_CHECK( _uiSize >= 0 );
		void *poMem;
		if( _uiSize )
		{
			if( _poMem )
				poMem = realloc( _poMem, _uiSize );
			else
				poMem = malloc( _uiSize );
		}
		else
		{
			if( _poMem )
				free( _poMem );
			poMem = NULL;
		}
		D_CHECK(poMem);
		return poMem;
	}
	void DefaultAllocator::Free(void* _poMem)
	{
		D_CHECK(_poMem);
		free( _poMem );
	}

	Allocator* g_poAllocator = NULL;
}