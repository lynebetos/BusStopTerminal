#include "StdAfx.h"
#include "LogOut.h"

static CLockConsole g_Lock;


bool InitLogs( const string& strConfigPath)
{
	// 系统日志
	log4cplus::Logger logger;
	string strLog4cplusConfig(strConfigPath + "BusStopTerminal.properties");
	helpers::LogLog::getLogLog()->setInternalDebugging(true);
	Logger root = Logger::getRoot();
	try 
	{
		PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(strLog4cplusConfig));
		logger = Logger::getInstance(LOG4CPLUS_TEXT("BusStopTerminal"));
	}
	catch(...) 
	{
		cout << "Exception..." << endl;
		LOG4CPLUS_FATAL(root, "Exception occured...");
		return false;
	}

	return true;
}

void OutConsole(const string& strOutput)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int nNum = 0;

	DWORD dwThreadid = GetCurrentThreadId();

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	CString strTime;
	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d [%d] ",
		systime.wYear, 
		systime.wMonth, 
		systime.wDay, 
		systime.wHour, 
		systime.wMinute, 
		systime.wSecond,
		systime.wMilliseconds,
		dwThreadid);

	g_Lock.Lock();
	WriteConsole(hConsole, (char*)(LPCTSTR)strTime, strTime.GetLength(), (LPDWORD)&nNum, NULL);
	WriteConsole(hConsole, strOutput.c_str(), strOutput.size(), (LPDWORD)&nNum, NULL);
	WriteConsole(hConsole, "\n", strlen("\n"), (LPDWORD)&nNum, NULL);
	g_Lock.UnLock();

}