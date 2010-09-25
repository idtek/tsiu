#include "TCore_LibSettings.h"
#include "TCore_Allocator.h"

static TsiU::DefaultAllocator defAlloc;

void* operator new(size_t _uiSize)
{
	if(TsiU::HasInited())
		return TsiU::GetLibSettings()->GetAllocator()->Alloc(_uiSize);
	else
		return defAlloc.Alloc(_uiSize);
}													

void operator delete(void* _poMem)
{	
	if(TsiU::HasInited())
		TsiU::GetLibSettings()->GetAllocator()->Free(_poMem);
	else
		defAlloc.Free(_poMem);
}			

void* operator new[](size_t _uiSize)				
{
	if(TsiU::HasInited())
		return TsiU::GetLibSettings()->GetAllocator()->Alloc(_uiSize);
	else
		return defAlloc.Alloc(_uiSize);					
}													

void operator delete[](void* _poMem)			
{							                        
	if(TsiU::HasInited())
		TsiU::GetLibSettings()->GetAllocator()->Free(_poMem);
	else
		defAlloc.Free(_poMem);                                    
}