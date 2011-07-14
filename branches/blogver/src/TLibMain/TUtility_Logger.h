#ifndef __TUTILITY_LOGGER__
#define __TUTILITY_LOGGER__

#include "TCore_Mutex.h"
#include "TUtility_Singleton.h"

namespace TsiU
{
	class File;

	class Logger : public Singleton<Logger>
	{
	public:
		enum LogLevel
		{
			LogLevel_All,
			LogLevel_Debug,
			LogLevel_Info,
			LogLevel_Warn,
			LogLevel_Error,
			LogLevel_Fatal,
			LogLevel_Off
		};

		void WriteLog(const LogLevel& _eLevel, const char* _strFormat, ...);
		void SetOutputFile(StringPtr _strPath = NULL);
		
		D_Inline void SetLevel(const LogLevel& _eLevel)
		{
			m_eMinLevel = _eLevel;
		}

		Logger();
		~Logger();

	private:
		LogLevel	m_eMinLevel;
		Mutex		m_pMutex;
		File*		m_pFile;
	};

	#define LOG(fmt, ...)			Logger::GetPtr()->WriteLog(Logger::LogLevel_All, fmt, __VA_ARGS__)
	#define LOG_DEBUG(fmt, ...)		Logger::GetPtr()->WriteLog(Logger::LogLevel_Debug, fmt, __VA_ARGS__)
	#define LOG_INFO(fmt, ...)		Logger::GetPtr()->WriteLog(Logger::LogLevel_Info, fmt, __VA_ARGS__)
	#define LOG_WARN(fmt, ...)		Logger::GetPtr()->WriteLog(Logger::LogLevel_Warn, fmt, __VA_ARGS__)
	#define LOG_ERROR(fmt, ...)		Logger::GetPtr()->WriteLog(Logger::LogLevel_Error, fmt, __VA_ARGS__)
	#define LOG_FATAL(fmt, ...)		Logger::GetPtr()->WriteLog(Logger::LogLevel_Fatal, fmt, __VA_ARGS__)
}


#endif