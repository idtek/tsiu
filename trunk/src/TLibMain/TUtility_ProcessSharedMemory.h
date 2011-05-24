#ifndef __TUTILITY_PROCESSSHAREDMEMORY_H__
#define __TUTILITY_PROCESSSHAREDMEMORY_H__

namespace TsiU
{
#if PLATFORM_TYPE == PLATFORM_WIN32
	class ProccessSharedMemory
	{
	public:
		ProccessSharedMemory(Bool bEnable = true);
		~ProccessSharedMemory(void);

		Udef Malloc(s32 iSize, StringPtr zName);
		void Free();
		void Lock();
		void UnLock();

	private:
		Udef	m_pPointer;
		Udef	m_pMutexHandle;
		Udef	m_pMappingHandle;
	};
#endif
}

#endif