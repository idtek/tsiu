#include "TCore_LibSettings.h"
#include "TCore_Allocator.h"
#include <map>
#include <limits>

static TsiU::DefaultAllocator defAlloc;
static int gMemUsed = 0;

#ifdef max
#undef max
#endif

#pragma warning(disable:4311)

int GetMemUsed()
{
	return gMemUsed;
}
template<typename T>
class userAllocator 
{
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;

	pointer address( reference r ) const { 
		return &r; 
	}
	const_pointer address( const_reference r ) const {
		return &r;
	}
	userAllocator() throw() {}
	template<class U> 
	userAllocator( const userAllocator<U>& ) throw() {}
	~userAllocator() throw() {}

	pointer allocate( size_type n, std::allocator<void>::const_pointer hint = 0 ) 
	{
		return static_cast<T*>( malloc( n * sizeof( T ) ) );
	}
	void deallocate( pointer p, size_type n ) {
		free( p );
	}
	void construct(pointer p, const T& val) {
		new ( p ) T( val );
	}
	void destroy(pointer p) { 
		p->~T(); 
	}
	size_type max_size() const throw() {
		 return std::numeric_limits<size_type>::max() / sizeof(T);
	}
	template<class U>
	struct rebind {
		typedef userAllocator<U> other;
	};
};
typedef std::map< int, int, std::less<int>, userAllocator<int> >			MemMap;
typedef std::map< int, int, std::less<int>, userAllocator<int> >::iterator	MemMapIterator;

static MemMap gMemMap;

void* operator new(size_t _uiSize)
{
	void* pMem = NULL;

	if(TsiU::HasInited())
		pMem = TsiU::GetLibSettings()->GetAllocator()->Alloc(_uiSize);
	else
		pMem = defAlloc.Alloc(_uiSize);
	D_CHECK(pMem);
	MemMapIterator it = gMemMap.find((int)pMem);
	if(it == gMemMap.end())
	{
		gMemMap.insert(std::pair<int,int>((int)pMem, _uiSize));
		gMemUsed += _uiSize;
	}
	else
	{
		D_CHECK(0);
	}
	return pMem;
}													

void operator delete(void* _poMem)
{	
	if(TsiU::HasInited())
		TsiU::GetLibSettings()->GetAllocator()->Free(_poMem);
	else
		defAlloc.Free(_poMem);
	MemMapIterator it = gMemMap.find((int)_poMem);
	if(it != gMemMap.end())
	{
		gMemUsed -= (*it).second;
		gMemMap.erase(it);
	}
	else
	{
		D_CHECK(0);
	}
}			

void* operator new[](size_t _uiSize)				
{
	void* pMem = NULL;

	if(TsiU::HasInited())
		pMem = TsiU::GetLibSettings()->GetAllocator()->Alloc(_uiSize);
	else
		pMem = defAlloc.Alloc(_uiSize);
	D_CHECK(pMem);
	MemMapIterator it = gMemMap.find((int)pMem);
	if(it == gMemMap.end())
	{
		gMemMap.insert(std::pair<int,int>((int)pMem, _uiSize));
		gMemUsed += _uiSize;
	}
	else
	{
		D_CHECK(0);
	}
	return pMem;			
}													

void operator delete[](void* _poMem)			
{				
	if(TsiU::HasInited())
		TsiU::GetLibSettings()->GetAllocator()->Free(_poMem);
	else
		defAlloc.Free(_poMem);
	MemMapIterator it = gMemMap.find((int)_poMem);
	if(it != gMemMap.end())
	{
		gMemUsed -= (*it).second;
		gMemMap.erase(it);
	}
	else
	{
		D_CHECK(0);
	}                                  
}