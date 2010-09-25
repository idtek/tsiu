#ifndef __TCORE_MEMORY__ 
#define __TCORE_MEMORY__

void*	operator new(size_t _uiSize);													
void	operator delete(void* _poMem);			
void*	operator new[](size_t _uiSize);																
void	operator delete[](void* _poMem);

#endif