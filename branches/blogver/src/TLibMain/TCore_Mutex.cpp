#include "TCore_Mutex.h"

namespace TsiU
{
#if PLATFORM_TYPE == PLATFORM_WIN32
	Mutex::Mutex()
	{
		::InitializeCriticalSection(&m_pMutex);
	}
	Mutex::~Mutex()
	{
		::DeleteCriticalSection(&m_pMutex);
	}
	void Mutex::Lock()
	{
		D_CHECK(_IsInitialized());
		::EnterCriticalSection(&m_pMutex);
	}
	void Mutex::UnLock()
	{
		D_CHECK(_IsInitialized());
		LeaveCriticalSection(&m_pMutex);
	}
	Bool Mutex::TryLock()
	{
		//D_CHECK(_IsInitialized());
		//return TryEnterCriticalSection(&m_pMutex) != 0; // compile problem
		return false;
	}
	Bool Mutex::_IsInitialized() const
	{
		return m_pMutex.DebugInfo != NULL;
	}
#endif
}