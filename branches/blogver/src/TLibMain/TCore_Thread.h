#ifndef __TCORE_THREAD__
#define __TCORE_THREAD__


#include <string>

namespace TsiU
{
	class IThreadRunner
	{
	public:
		virtual u32		Run() = 0;
		virtual void	NotifyQuit() = 0;
	};

	class Thread
	{
	public:
		enum EThreadPriority
		{
			EThreadPriority_Low,
			EThreadPriority_Normal,
			EThreadPriority_High,
		};

#if PLATFORM_TYPE == PLATFORM_WIN32
		typedef HANDLE	ThreadID;
#endif
	
	public:
		Thread(IThreadRunner* _pRunner = 0, EThreadPriority _ePriority = EThreadPriority_Normal, StringPtr _strName = "UnnamedThread");
		~Thread();

		Bool Start();
		Bool Stop(s32 _timeOutInMilliSeconds = -1);
		Bool HasStarted() const;

	private:
		u32		_Run();
		void	_Free();

	protected:
		Bool			m_bStarted;
		EThreadPriority m_ePriority;
		IThreadRunner*	m_pRunner;
		ThreadID		m_pThreadID;
		std::string		m_strThreadName;

#if PLATFORM_TYPE == PLATFORM_WIN32
		static unsigned long __stdcall ThreadProc(void* ptr);
#endif
	};

}
#endif