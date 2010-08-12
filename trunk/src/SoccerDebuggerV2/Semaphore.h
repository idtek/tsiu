#ifndef _EREBUS_SEMAPHORE
#define _EREBUS_SEMAPHORE

#include <windows.h>
#include <assert.h>
class Semaphore
{
private:
	HANDLE hSemaphore;
public:
	Semaphore();
	Semaphore(int count);
	Semaphore(LPCTSTR name,LONG init,LONG max);
	~Semaphore();
	
	DWORD P(DWORD time = INFINITE);
	BOOL  V(PLONG plPreviousCount = NULL);
	BOOL  V(LONG lReleaseCount, PLONG plPreviousCount = NULL);
};

typedef class Semaphore * PSemaphore;

inline Semaphore::Semaphore()
{
	hSemaphore = CreateSemaphore(0,1,1,0);
	assert(hSemaphore!=NULL);
}


//---------------------------------------------------------------------------
inline Semaphore::Semaphore(LPCTSTR name,LONG init,LONG max)
{
	hSemaphore = CreateSemaphore(NULL,init,max,name);
	assert(hSemaphore!=NULL);
}
//---------------------------------------------------------------------------
inline Semaphore::~Semaphore()
{
	CloseHandle(hSemaphore);  //error here: invalid handle ?and hSemaphore!=0
}
//---------------------------------------------------------------------------
inline DWORD Semaphore::P(DWORD time)
{
	return WaitForSingleObject(hSemaphore,time);
}
//---------------------------------------------------------------------------
inline BOOL Semaphore::V(PLONG lpOldVal)
{
	return ReleaseSemaphore(hSemaphore,1,lpOldVal);
}
//---------------------------------------------------------------------------
inline BOOL Semaphore::V(LONG lReleaseCount,PLONG plPreviousCount)
{
	return ReleaseSemaphore(hSemaphore,lReleaseCount,plPreviousCount);
}
//---------------------------------------------------------------------------

#endif
 