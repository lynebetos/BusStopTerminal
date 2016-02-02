
// BusStopTerminal.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBusStopTerminalApp:
// �йش����ʵ�֣������ BusStopTerminal.cpp
//

class CBusStopTerminalApp : public CWinAppEx
{
public:
	CBusStopTerminalApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

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
			sstr<<"�����ڴ������ַ��ȡʧ��,�������"<<dwErr;
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
			sstr<<"�����ڴ��¼�����ʧ��,�������"<<dwErr;
			LogOutError(sstr.str().c_str());
			sstr.str("");
		}

		return true;
	}

private:
	HANDLE m_hMessage; //�����ڴ�д���¼�
	HANDLE m_hMapping; //�����ڴ���

	

};

extern CBusStopTerminalApp theApp;