
// BusStopTerminal.cpp : 定义应用程序的类行为。
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

//使所有版本都可以捕获到异常 
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

//把当前时刻的线程栈记录到DUMP文件中 
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
//循环检测线程 
//查看到有ADTV2_TEMP.TXT文件，则记录下当前时刻的堆栈 
void CreateDumpThrd(void* pv) 
{ 
	//HANDLE hFile;  
	//string strPath = FileAssist::GetExePath() + "\\ADTV2_TEMP.TXT"; 
	while(bCreateDumpThrd) 
	{ 
		//每5秒检测一次 
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
		//	//防止多次记录当前堆栈信息，删除文件 
		//	::CloseHandle(hFile); 
		//	::DeleteFile(strPath.c_str()); 
		if(RecordCurStack() == 1)
			exit(-1);
		//} 
	} 
}

// CBusStopTerminalApp 构造

CBusStopTerminalApp::CBusStopTerminalApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBusStopTerminalApp 对象

CBusStopTerminalApp theApp;


// CBusStopTerminalApp 初始化

BOOL CBusStopTerminalApp::InitInstance()
{
	//调试信息 
	::SetUnhandledExceptionFilter(ExceptionFilter); //设置异常处理函数 
	DisableSetUnhandledExceptionFilter();           //获取未处理的异常

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	::CoInitialize(NULL);

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));



	GdiplusStartupInput gdiSI;
	ULONG_PTR gdiToken;

	//gdiSI.SuppressBackgroundThread = TRUE;
	Status st = GdiplusStartup(&gdiToken, &gdiSI, NULL);
	
	{
		LPARAM lParam;
		lParam = ABS_AUTOHIDE | ABS_ALWAYSONTOP;//自动隐藏
		APPBARDATA apBar; 
		memset(&apBar,0,sizeof(apBar)); 
		apBar.cbSize = sizeof(apBar); 
		apBar.hWnd = (HWND)FindWindow("Shell_TrayWnd",NULL);
		if(apBar.hWnd != NULL) 
		{ 
			apBar.lParam   =   lParam; 
			SHAppBarMessage(ABM_SETSTATE,&apBar); //设置任务栏自动隐藏
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
		LogOutError("共享内存消息通知事件打开失败");
	}

	m_hMapping = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAPPING_NAME);
	if(!m_hMapping)
	{
		LogOutError("共享内存没有创建");
	}


// 	LogOutInfo("启动网络连接");
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
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。


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
		//重启程序  
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
