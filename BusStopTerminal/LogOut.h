#pragma once

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>

#ifdef _DEBUG
#pragma comment(lib, "log4cplusSD.lib")
#else
#pragma comment(lib, "log4cplusS.lib")
#endif

using namespace log4cplus;

bool InitLogs( const string& strConfigPath);
void OutConsole(const string& strOutput);


#define LogObject 	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("BusStopTerminal"))
#define LogOutInfo(text) LOG4CPLUS_INFO(logger, text); OutConsole(text);
#define LogOutError(text) LOG4CPLUS_ERROR(logger, text); OutConsole(text);
#define LogOutDebug(text) LOG4CPLUS_DEBUG(logger, text); OutConsole(text);

class CLockConsole
{
public:
	CLockConsole()
	{
		InitializeCriticalSection(&m_secConSole);
	}
	~CLockConsole()
	{
		DeleteCriticalSection(&m_secConSole);
	}

	void Lock()
	{
		EnterCriticalSection(&m_secConSole);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_secConSole);
	}


private:
	CRITICAL_SECTION m_secConSole;

};


