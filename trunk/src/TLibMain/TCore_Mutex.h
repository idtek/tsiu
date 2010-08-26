#ifndef __TCORE_MUTEX__
#define __TCORE_MUTEX__

#if PLATFORM_TYPE == PLATFORM_WIN32
#include <winbase.h>
#endif

namespace TsiU
{
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		void Lock();
		void UnLock();
		Bool TryLock();

	private:
		Bool _IsInitialized() const;

	private:
#if PLATFORM_TYPE == PLATFORM_WIN32
		CRITICAL_SECTION	m_pMutex;
#endif
	};
}

#endif