#ifndef __TCORE_MEMORY__ 
#define __TCORE_MEMORY__


#include "TCore_Allocator.h"

namespace TsiU
{
	#define D_Malloc	g_poAllocator->Alloc;
	#define D_Free		g_poAllocator->Free;
}

/*inline void* operator new(u32 p_uiSize, void* p_poPtr)
{
	return p_poPtr;
}

inline void* operator new(u32 p_uiSize)
{
	if(p_uiSize == 0)
		p_uiSize += 1;

	return TLib::LibState::GetInstance().m_poAllocator->Alloc(p_uiSize);
}													

inline void operator delete(void* p_poMem)
{		
	TLib::LibState::GetInstance().m_poAllocator->Free(p_poMem);
}			

inline void* operator new[](u32 p_uiSize)				
{
	if(p_uiSize == 0)
		p_uiSize += 1;

	return TLib::LibState::GetInstance().m_poAllocator->Alloc(p_uiSize);					
}													

inline void operator delete[](void* p_poMem)			
{							                        
	TLib::LibState::GetInstance().m_poAllocator->Free(p_poMem);                                    
}*/

#endif