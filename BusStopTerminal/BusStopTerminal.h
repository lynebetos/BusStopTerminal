
// BusStopTerminal.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CBusStopTerminalApp:
// 有关此类的实现，请参阅 BusStopTerminal.cpp
//

class CBusStopTerminalApp : public CWinAppEx
{
public:
	CBusStopTerminalApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

public:
	bool WriteFileMapping(PMInfo pInfo)
	{
		LogObject;
		if(!m_hMapping)
			return false;

		LPBYTE* lpWrite = (LPBYTE*)::MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
		if(lpWrite == NULL)
		{
			DWORD dwErr = GetLastError();
			stringstream sstr;
			sstr<<"共享内存操作地址获取失败,错误代码"<<dwErr;
			LogOutError(sstr.str().c_str());
			sstr.str("");
			return false;
		}

		PMInfo pInfoWrite = reinterpret_cast<PMInfo>(lpWrite);
		pInfoWrite->Clear();
		*pInfoWrite = *pInfo;
		UnmapViewOfFile(lpWrite);

		if(!::SetEvent(m_hMessage))
		{
			DWORD dwErr = GetLastError();
			stringstream sstr;
			sstr<<"共享内存事件触发失败,错误代码"<<dwErr;
			LogOutError(sstr.str().c_str());
			sstr.str("");
		}

		return true;
	}

private:
	HANDLE m_hMessage; //共享内存写入事件
	HANDLE m_hMapping; //共享内存句柄

	

};

extern CBusStopTerminalApp theApp;