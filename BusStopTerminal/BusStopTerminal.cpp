
// BusStopTerminal.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include <GdiPlus.h>
#include "BusStopTerminal.h"
#include "BusStopTerminalDlg.h"
#include "Main.h"
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBusStopTerminalApp

BEGIN_MESSAGE_MAP(CBusStopTerminalApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

//ʹ���а汾�����Բ����쳣 
void DisableSetUnhandledExceptionFilter() 
{ 
	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter"); 

	if (addr)  
	{ 
		unsigned char code[16]; 
		int size = 0; 
		code[size++] = 0x33; 
		code[size++] = 0xC0; 
		code[size++] = 0xC2; 
		code[size++] = 0x04; 
		code[size++] = 0x00; 

		DWORD dwOldFlag, dwTempFlag; 
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag); 
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL); 
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag); 
	}
}

LONG WINAPI ExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo) 
{ 
	//::AfxMessageBox("ExceptionFilter"); 
	LogObject;

	HANDLE hFile = ::CreateFile( _T("C:\\dumpfile11_20.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
	if( hFile != INVALID_HANDLE_VALUE) 
	{ 
		MINIDUMP_EXCEPTION_INFORMATION einfo; 
		einfo.ThreadId = ::GetCurrentThreadId(); 
		einfo.ExceptionPointers = ExceptionInfo; 
		einfo.ClientPointers = FALSE; 

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &einfo, NULL, NULL); 
		::CloseHandle(hFile); 

		DWORD dwErr = ::GetLastError();

		stringstream sstr;
		sstr<<"Dump error code="<<dwErr;
		LogOutInfo(sstr.str().c_str());
		sstr.str() = "";

	} 

	return 0; 
} 

//�ѵ�ǰʱ�̵��߳�ջ��¼��DUMP�ļ��� 
int RecordCurStack() 
{ 
	HANDLE hFile = ::CreateFile( _T("C:\\dumpfile.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
	if( hFile != INVALID_HANDLE_VALUE) 
	{ 
		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpWithFullMemory  ,NULL, NULL, NULL); 

		::CloseHandle(hFile); 
		return 1; 
	} 

	return 0; 
} 

bool bCreateDumpThrd = true; 
//ѭ������߳� 
//�鿴����ADTV2_TEMP.TXT�ļ������¼�µ�ǰʱ�̵Ķ�ջ 
void CreateDumpThrd(void* pv) 
{ 
	//HANDLE hFile;  
	//string strPath = FileAssist::GetExePath() + "\\ADTV2_TEMP.TXT"; 
	while(bCreateDumpThrd) 
	{ 
		//ÿ5����һ�� 
		Sleep(5000); 
		//hFile = CreateFileA(strPath.c_str(),    // file to open 
		//	GENERIC_READ,          // open for reading 
		//	FILE_SHARE_READ,       // share for reading 
		//	NULL,                  // default security 
		//	OPEN_EXISTING,         // existing file only 
		//	FILE_ATTRIBUTE_NORMAL, // normal file 
		//	NULL);                 // no attr. template 

		//if (hFile != INVALID_HANDLE_VALUE)  
		//{  
		//	//��ֹ��μ�¼��ǰ��ջ��Ϣ��ɾ���ļ� 
		//	::CloseHandle(hFile); 
		//	::DeleteFile(strPath.c_str()); 
		if(RecordCurStack() == 1)
			exit(-1);
		//} 
	} 
}

// CBusStopTerminalApp ����

CBusStopTerminalApp::CBusStopTerminalApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CBusStopTerminalApp ����

CBusStopTerminalApp theApp;


// CBusStopTerminalApp ��ʼ��

BOOL CBusStopTerminalApp::InitInstance()
{
	//������Ϣ 
	::SetUnhandledExceptionFilter(ExceptionFilter); //�����쳣������ 
	DisableSetUnhandledExceptionFilter();           //��ȡδ������쳣

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	::CoInitialize(NULL);

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));



	GdiplusStartupInput gdiSI;
	ULONG_PTR gdiToken;

	//gdiSI.SuppressBackgroundThread = TRUE;
	Status st = GdiplusStartup(&gdiToken, &gdiSI, NULL);
	
	{
		LPARAM lParam;
		lParam = ABS_AUTOHIDE | ABS_ALWAYSONTOP;//�Զ�����
		APPBARDATA apBar; 
		memset(&apBar,0,sizeof(apBar)); 
		apBar.cbSize = sizeof(apBar); 
		apBar.hWnd = (HWND)FindWindow("Shell_TrayWnd",NULL);
		if(apBar.hWnd != NULL) 
		{ 
			apBar.lParam   =   lParam; 
			SHAppBarMessage(ABM_SETSTATE,&apBar); //�����������Զ�����
		}   
	}


#ifdef DEBUG
	AllocConsole();
#endif

	CMain main;
	int nRes = main.Initialize();
	if( 0 == nRes)
	{
		return FALSE;
	}
	LogObject;

	//for Defence
	m_hMessage = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_PAGE_READ);
	if(!m_hMessage)
	{
		LogOutError("�����ڴ���Ϣ֪ͨ�¼���ʧ��");
	}

	m_hMapping = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAPPING_NAME);
	if(!m_hMapping)
	{
		LogOutError("�����ڴ�û�д���");
	}


// 	LogOutInfo("������������");
// 	int nRes = m_pMain->DoConnectServer();
// 
// 	if(nRes == 2)
// 	{
// 		this->PostMessage(WM_DESTROY);
// 		return FALSE;
// 	}


	//FreeConsole();

	g_hExit = CreateEvent(NULL, FALSE, FALSE, NULL);

	CBusStopTerminalDlg dlg;
	dlg.SetMain(&main);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�


	main.SetStop();
	main.UnInitialize();

	::CoUninitialize();

	if(main.GetStatus() != 0)
	{
		/*
		TCHAR szAppName[MAX_PATH];  
		:: GetModuleFileName(theApp.m_hInstance, szAppName, MAX_PATH);  
		CString strAppFullName;  
		strAppFullName.Format(_T("%s"),szAppName);  
		//��������  
		STARTUPINFO StartInfo;  
		PROCESS_INFORMATION procStruct;  
		memset(&StartInfo, 0, sizeof(STARTUPINFO));  
		StartInfo.cb = sizeof(STARTUPINFO);  
		::CreateProcess( 
			(LPCTSTR)strAppFullName,  
			NULL,  
			NULL,  
			NULL,  
			FALSE,  
			NORMAL_PRIORITY_CLASS,  
			NULL,  
			NULL,  
			&StartInfo,  
			&procStruct); 
			*/
		return FALSE;
	}



	return TRUE;
}
