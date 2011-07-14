#include "TCore_Thread.h"

namespace TsiU
{
#if PLATFORM_TYPE == PLATFORM_WIN32
	static int sThreadPriorities[] = 
	{
		THREAD_PRIORITY_LOWEST,		// EThreadPriority_Low		= 0
		THREAD_PRIORITY_NORMAL,		// EThreadPriority_Normal	= 1
		THREAD_PRIORITY_HIGHEST,	// EThreadPriority_High		= 2
	};
#endif

	unsigned long __stdcall Thread::ThreadProc(void* ptr)
	{
		D_CHECK(ptr);
		Thread* pThread = static_cast<Thread*>(ptr);
		return pThread->_Run();
	}

	Thread::Thread(IThreadRunner* _pRunner, EThreadPriority _ePriority, StringPtr _strName)
		: m_pRunner(_pRunner)
		, m_ePriority(_ePriority)
		, m_strThreadName(_strName)
		, m_bStarted(false)
		, m_pThreadID(NULL)
	{

	}
	Thread::~Thread()
	{
		_Free();
	}

	Bool Thread::HasStarted() const
	{
		return m_bStarted;
	}

	Bool Thread::Start()
	{
		if(HasStarted())
		{
			D_Output("Thread has started\n");
			return false;
		}
		else
		{
			_Free();

#if PLATFORM_TYPE == PLATFORM_WIN32			
			DWORD threadId;
			m_pThreadID = ::CreateThread(0, 0, ThreadProc, this, 0, &threadId);
			if(m_pThreadID == NULL)
			{
				D_CHECK(0);
				return false;
			}
			::SetThreadPriority(m_pThreadID, sThreadPriorities[m_ePriority]);
#endif
		}
		return true;
	}

	Bool Thread::Stop(s32 _timeOutInMilliSeconds)
	{
		if(HasStarted())
		{
			m_pRunner->NotifyQuit();

#if PLATFORM_TYPE == PLATFORM_WIN32
			DWORD timeOut = INFINITE;
			if(_timeOutInMilliSeconds >= 0)
				timeOut = _timeOutInMilliSeconds;
			s32 nRet = WaitForSingleObject(m_pThreadID, timeOut);
			if(nRet == WAIT_TIMEOUT)
			{
				TerminateThread(m_pThreadID, 0);
			}
#endif
		}
		return true;
	}

	u32 Thread::_Run()
	{
		m_bStarted = true;
		u32 threadRet = m_pRunner->Run();
		m_bStarted = false;

		return threadRet;
	}

	void Thread::_Free()
	{
#if PLATFORM_TYPE == PLATFORM_WIN32	
		if(m_pThreadID)
		{
			::CloseHandle(m_pThreadID);
		}
#endif
	}
}