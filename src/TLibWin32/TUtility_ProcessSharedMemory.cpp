#include "TUtility_ProcessSharedMemory.h"

#define SAFE_CLOSE_HANDLE(h) if (h) CloseHandle(h);

namespace TsiU
{
	ProccessSharedMemory::ProccessSharedMemory(Bool bEnable)
	{
		m_pPointer			= NULL;
		m_pMutexHandle		= NULL;
		m_pMappingHandle	= NULL;
	}
	ProccessSharedMemory::~ProccessSharedMemory(void)
	{
	}

	void* ProccessSharedMemory::Malloc(s32 iSize, StringPtr zName)
	{
		D_CHECK(zName);
		Char zEventName[1024];
		sprintf(zEventName, "%s_MemMutex", zName);
		m_pMutexHandle = CreateMutex(NULL, FALSE, zName);
		if(!m_pMutexHandle)
		{
			D_CHECK(0);
		}
		sprintf(zEventName, "%s_MemMapping", zName);
		m_pMappingHandle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, iSize, zEventName);
		if(m_pMappingHandle)
		{
			m_pPointer = MapViewOfFile(m_pMappingHandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, iSize);
		}
		return m_pPointer;
	}

	void ProccessSharedMemory::Free()
	{
		if(m_pPointer)
		{
			UnmapViewOfFile(m_pPointer);
		}
		SAFE_CLOSE_HANDLE(m_pMutexHandle);
		SAFE_CLOSE_HANDLE(m_pMappingHandle);
	}
	
	void ProccessSharedMemory::Lock()
	{
		WaitForSingleObject(m_pMutexHandle, INFINITE);
	}
	void ProccessSharedMemory::UnLock()
	{
		ReleaseMutex(m_pMutexHandle);
	}
}
