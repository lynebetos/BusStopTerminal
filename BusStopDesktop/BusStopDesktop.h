
// BusStopDesktop.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBusStopDesktopApp:
// �йش����ʵ�֣������ BusStopDesktop.cpp
//

class CBusStopDesktopApp : public CWinAppEx
{
public:
	CBusStopDesktopApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBusStopDesktopApp theApp;