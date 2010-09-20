#include "TUtility_Logger.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "TFS_FileManager.h"

namespace TsiU
{
	static const StringPtr g_strLogLevel[] = 
	{
		"[]",
		"[Debug]",
		"[Info]",
		"[Warn]",
		"[Error]",
		"[Fatal]",
		"[]"
	};
	Logger::Logger() 
		: m_eMinLevel(LogLevel_All)
		, m_pFile(NULL)
	{
	}

	Logger::~Logger()
	{
		if(m_pFile)
		{
			FileManager::GetPtr()->CloseFile(m_pFile);
			m_pFile = NULL;
		}
	}

	void Logger::SetOutputFile(StringPtr _strPath)
	{
		if(_strPath)
		{
			m_pFile = FileManager::GetPtr()->OpenFile(_strPath, E_FOM_Write);
		}
	}

	void Logger::WriteLog(const LogLevel& _eLevel, const char* _strFormat, ...)
	{
		m_pMutex.Lock();
		if(_eLevel >= m_eMinLevel)
		{
			Char timeStr[30];
			time_t now = time(NULL);
			struct tm datetime = {0};   
			localtime_s(&datetime, &now);
			sprintf(timeStr, "[%d-%02d-%02d %02d:%02d:%02d]",
				datetime.tm_year+1900, datetime.tm_mon+1, datetime.tm_mday, datetime.tm_hour, datetime.tm_min, datetime.tm_sec);
		
			Char logStr[8];
			sprintf(logStr, "%s", g_strLogLevel[_eLevel]);;

			Char szBuffer[1024];
			const int NUMCHARS = sizeof(szBuffer) / sizeof(Char);
			const int LASTCHAR = NUMCHARS - 1;

			va_list pArgs;
			va_start(pArgs, _strFormat);
			_vsnprintf(szBuffer, NUMCHARS - 1, _strFormat, pArgs);
			va_end(pArgs);
			szBuffer[LASTCHAR] = TEXT('\0');

			if(m_pFile)
			{
				m_pFile->Write(timeStr, strlen(timeStr));
				m_pFile->Write(logStr, strlen(logStr));
				m_pFile->Write(szBuffer, strlen(szBuffer));
			}
			else
			{
				D_Output("%s%s%s", timeStr, logStr, szBuffer);
			}
		}
		m_pMutex.UnLock();
	}
}