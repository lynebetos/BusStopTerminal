
// BusStopDefenceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusStopDefence.h"
#include "BusStopDefenceDlg.h"
#include "../include/http/base_http.h"
#include "../include/json/json.h"
#include "../include/zip/unzip.h"
#include "ConfigManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef DEBUG
#pragma comment(lib, "json_vc71_libmtd.lib")
#else
#pragma comment(lib, "json_vc71_libmt.lib")
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CBusStopDefenceDlg �Ի���




CBusStopDefenceDlg::CBusStopDefenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBusStopDefenceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRun = false;
	m_bStopQuit = false;
}

void CBusStopDefenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBusStopDefenceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CBusStopDefenceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBusStopDefenceDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_SHOWTASK, OnShowTask)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SHUTAPP, &CBusStopDefenceDlg::OnShutApp)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEAPP, &CBusStopDefenceDlg::OnBnClickedButtonCloseapp)
END_MESSAGE_MAP()


// CBusStopDefenceDlg ��Ϣ�������

BOOL CBusStopDefenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	LogObject;

	m_strMinShow = "����վ���ػ�����";

	m_nId.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nId.hWnd = this->m_hWnd;
	m_nId.uID = IDR_MAINFRAME;
	m_nId.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nId.uCallbackMessage = WM_SHOWTASK;
	m_nId.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(m_nId.szTip, sizeof(m_nId.szTip), (char*)(LPCTSTR)m_strMinShow);
	Shell_NotifyIcon(NIM_ADD, &m_nId);

	m_menu.CreatePopupMenu();
	m_menu.AppendMenu(MF_STRING, WM_DESTROY, _T("�˳�"));

	SetTimer(1, 1000, NULL);

	LogOutInfo("����վ���ػ���������...");
	InitDefence();

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CBusStopDefenceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBusStopDefenceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CBusStopDefenceDlg::InitDefence()
{
	bool bInitSuc = 1;
	bInitSuc = InitLogs(("../config/"));

	LogObject;

	LogOutInfo("վ���ػ���������������");

#ifdef DEBUG
	string strConfigPath = "../config/config.xml";
#else
	string strConfigPath = "../config/config.xml";
#endif
	CConfigManager* pConfig = CConfigManager::GetInstance();
	 bInitSuc = pConfig->LoadConfig(strConfigPath);
	if(!bInitSuc)
	{
		CString strLog = "���������ļ������ļ�·��:";
		strLog += strConfigPath.c_str();
		LogOutError((char*)(LPCTSTR)strLog);
		::PostMessage(this->m_hWnd, WM_DESTROY, NULL, NULL);
		return;
	}

	CreateDirectory("Update", NULL);

	//��ʼ���ʼ��ͻ���
	m_MailSender.SetMailSender(_T("smtp.chinatransinfo.com"), _T("wangxulin@chinatransinfo.com"), _T("qwer1234"));
	m_MailSender.AddMailReceiver(_T("linlin-ring@163.com"));

	//�����ر��¼�
	m_hEventShut = CreateEvent(NULL, TRUE, FALSE, NULL);;
	//���������ڴ�
	int nRes = CreateShardMemery();

	if(!nRes)
	{
		//���������ڴ�ʧ��
		::PostMessage(this->m_hWnd, WM_DESTROY, NULL, NULL);
		return;
	}

	unsigned int nThreadIDRead = 0;
	m_bRead = true;
	m_hRead= (HANDLE)_beginthreadex(NULL, 0, threadReadMapping, (LPVOID)this, 0, &nThreadIDRead);
	if(m_hRead == INVALID_HANDLE_VALUE)
	{
		//��ȡ�̴߳���ʧ��
		LogOutError("��ȡ�̴߳���ʧ��");
	}

	//����������½���
	unsigned int nThreadIDUpdate = 0;
	m_bUpdate = true;
	m_hUpdate = (HANDLE)_beginthreadex(NULL, 0, threadUpdate, (LPVOID)this, 0, &nThreadIDUpdate);
	if(m_hUpdate == INVALID_HANDLE_VALUE)
	{
		//������´���ʧ��
		LogOutError("������´���ʧ��");
	}

	//�����ն˼�����
	unsigned int nThreadID = 0;
	m_bRun = true;
	m_hRun = (HANDLE)_beginthreadex(NULL, 0, threadKeepBusstop, (LPVOID)this, 0, &nThreadID);
	if(m_hRun == INVALID_HANDLE_VALUE)
	{
		//�ػ��̴߳���ʧ��
		LogOutError("�ػ��̴߳���ʧ��");
	}
	
}

bool CBusStopDefenceDlg::CreateShardMemery()
{
	LogObject;
	m_hMapping = ::CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL, 
		PAGE_READWRITE,
		0,
		1024,//sizeof(MInfo)*sizeof(TCHAR),
		MAPPING_NAME);

	DWORD dwErr = GetLastError();

	if(dwErr == ERROR_ALREADY_EXISTS || dwErr == 0)
	{
		m_hEventRead = ::CreateEvent(NULL, TRUE, FALSE, EVENT_PAGE_READ);
		m_hEventRestart = ::CreateEvent(NULL, TRUE, FALSE, EVENT_RESTART_APP);

		if(m_hEventRead)
		{
			LogOutError("��ȡ�¼�δ�����ɹ�!");
		}
		return true;
	}
	else
	{
		CString strLog;
		strLog.Format("���������ڴ�ʧ��,�������:%d", dwErr);
		LogOutError((char*)(LPCTSTR)strLog);
		return false;
	}
}

unsigned int _stdcall CBusStopDefenceDlg::threadReadMapping(LPVOID lpvoid)
{
	CBusStopDefenceDlg* pDlg = (CBusStopDefenceDlg*)lpvoid;
	if(pDlg)
	{
		pDlg->ReadMapping();
	}
	
	return 0;
}

void CBusStopDefenceDlg::ReadMapping()
{
	LogObject;
	int nRecord = 0;
	while(m_bRead)
	{
		DWORD dwWait = ::WaitForSingleObject(m_hEventRead, 10000);
		if(dwWait != WAIT_OBJECT_0)
		{
			//if(nRecord >= 3)
			LogOutInfo("�ն�10��δ��Ӧ,�����ն˽���");
			{ //��ʱ�ر������ն�
				nRecord = 0;
				TerminateProcess(m_procStruct.hProcess, 0);
				m_procStruct.dwThreadId =0;
				m_procStruct.dwProcessId = 0;
				CloseHandle(m_procStruct.hProcess);
				CloseHandle(m_procStruct.hThread);
			}
			//else
				//nRecord ++;
			continue;
		}

		LPBYTE* lpRead = (LPBYTE*)::MapViewOfFile(m_hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(MInfo)*sizeof(TCHAR));
		if (lpRead == NULL)
		{
			continue;//
		}

		PMInfo pstPMInfo = reinterpret_cast<PMInfo>(lpRead);

		MInfo info = *pstPMInfo;
		::UnmapViewOfFile(lpRead);
		::ResetEvent(m_hEventRead);

		switch(info.nMessageID)
		{
		case RESTART_APP:
			break;
		case RESTART_UPDATE:
			break;
		case RUN_NOMAL:
		default:
			break;
		}
	}

}

unsigned int _stdcall CBusStopDefenceDlg::threadUpdate(LPVOID lpvoid)
{
	CBusStopDefenceDlg* pDlg = (CBusStopDefenceDlg*)lpvoid;
	if(pDlg)
	{
		pDlg->UpdateSoftware();
	}

	return 0;
}

void CBusStopDefenceDlg::UpdateSoftware()
{
	LogObject;
	base_http::CBaseHttp baseHttp;
	base_http::HttpInfo info;

	CConfigManager* pCM = CConfigManager::GetInstance();
	if(!pCM)
	{
		LogOutError("δ��ʼ������");
		return;
	}




	while(m_bUpdate)
	{
		info.Clear();

		info.strUrl = pCM->GetSoftUpdateUrl();//"http://192.168.3.102:8080/TriffCityBusVersion/HttpGetVersion?";
		info.AddParam("username", "admin");
		info.AddParam("password", "admin");
		info.AddParam("citycode", pCM->GetCityCode().c_str());
		info.AddParam("stopmid", pCM->GetStopId().c_str());
		info.AddParam("type", "get");

		info.AddParam("version", pCM->GetAppVer().c_str());
		baseHttp.HttpGet(info);

		const BYTE* buff = baseHttp.GetData();
		if(buff == NULL)
		{
			CString strLog;
			strLog.Format("�޷����Ӹ��·���!URL = %s", info.strUrl.c_str());
			LogOutError((char*)(LPCTSTR)strLog);
			Sleep(5000);
			continue;
		}

		string strData = (char*)buff;

		//����Ӧ��
		Json::Reader jreader;
		Json::Value jValue;

		if(jreader.parse(strData.c_str(), jValue))
		{
			int nUpdate = jValue["updatecode"].asInt();
			string strUrl = jValue["httpaddress"].asString();
			int nStatus = jValue["status"].asInt();

			if(nUpdate == 1)
			{
				char szPath[MAX_PATH] = {0};
				GetCurrentDirectory(MAX_PATH, szPath);

				string strZipFilePath = szPath;
				strZipFilePath += "\\";
				strZipFilePath += "Update";
				string::size_type pos = strUrl.find_last_of('/');
				string strFileName = strUrl.substr(pos + 1, strUrl.length() - pos );
				strZipFilePath += "\\";
				strZipFilePath += strFileName;
				baseHttp.AddHttpHeader("Authorization", "Basic YWRtaW46YWRtaW4=");
				bool bDown = baseHttp.HttpDownload(strUrl.c_str(), strZipFilePath.c_str());

				if(bDown)
				{
					//֪ͨ����̸߳���
					this->SendMessage(WM_SHUTAPP, 1);
					//wait
					WaitForSingleObject(m_hEventShut, INFINITE);

					Sleep(1000);

					//��ѹ����
					HZIP hz = OpenZip(strZipFilePath.c_str(),0);
					ZIPENTRY ze; GetZipItem(hz,-1,&ze); int numitems=ze.index;

					SetUnzipBaseDir(hz, szPath);
					for (int i=0; i<numitems; i++)
					{ 
						GetZipItem(hz,i,&ze);
						ZRESULT res = UnzipItem(hz,i,ze.name);

						stringstream sstrLog;
						sstrLog<<"��ѹ�ļ�:"<<ze.name<<"��ѹ·��:"<<szPath<<"��ѹ���:"<<res;
						LogOutInfo(sstrLog.str().c_str());
						sstrLog.str("");
					}
					CloseZip(hz);

					pos = strFileName.find('(');
					string strVer = strFileName.substr(1, pos - 1);
			

					//���¼���һ�������ļ�����ֹ�ͻ����޸�
#ifdef DEBUG
					string strConfigPath = "../config/config.xml";
#else
					string strConfigPath = "../config/config.xml";
#endif

					pCM->LoadConfig(strConfigPath);
					pCM->SetAppVer(strVer);
					pCM->SaveConfig();

					//pCM->LoadConfig(strConfigPath);

					this->SendMessage(WM_SHUTAPP, 0);

					base_http::HttpInfo infoEnd;
					infoEnd.strUrl = pCM->GetSoftUpdateUrl();
					infoEnd.AddParam("username", "admin");
					infoEnd.AddParam("password", "admin");
					infoEnd.AddParam("citycode", pCM->GetCityCode().c_str());
					infoEnd.AddParam("stopmid", pCM->GetStopId().c_str());
					infoEnd.AddParam("updatecode", "1");
					infoEnd.AddParam("type", "result");

					baseHttp.HttpGet(infoEnd);

				}
				else
				{
					LogOutError("���󲻵��°汾����!");
				}
			}
		}

		Sleep(5000);
	}
}

unsigned int _stdcall CBusStopDefenceDlg::threadKeepBusstop(LPVOID lpvoid)
{
	CBusStopDefenceDlg* pDlg = (CBusStopDefenceDlg*)lpvoid;
	if(pDlg)
		pDlg->KeepBusstopRun();

	return 0;
}

void CBusStopDefenceDlg::KeepBusstopRun()
{
	m_procStruct.dwProcessId = 0;
	m_procStruct.dwThreadId = 0;
	m_procStruct.hProcess = INVALID_HANDLE_VALUE;
	m_procStruct.hThread = INVALID_HANDLE_VALUE;

	while(m_bRun)
	{
		if(!m_bStopQuit && !IsStopProcessRun())
		{
			TCHAR szAppName[MAX_PATH] = {""};
			GetCurrentDirectory(MAX_PATH, szAppName);

			CString strAppFullName;  
			strAppFullName.Format(_T("%s"),szAppName);

#ifdef DEBUG
			int nDefCount = strlen("\\BusStopDefence");
			strAppFullName.Delete(strAppFullName.GetLength() - nDefCount, nDefCount);
			strAppFullName += "\\Debug\\BusStopTerminal.exe";
#else
			strAppFullName += "\\BusStopTerminal.exe";
#endif
			//��������  
			STARTUPINFO StartInfo;  
			memset(&StartInfo, 0, sizeof(STARTUPINFO));  
			StartInfo.cb = sizeof(STARTUPINFO);  
			BOOL bCreate = ::CreateProcess( 
				(LPCTSTR)strAppFullName,  
				NULL,  
				NULL,  
				NULL,  
				FALSE,  
				NORMAL_PRIORITY_CLASS,  
				NULL,  
				NULL,  
				&StartInfo,  
				&m_procStruct);

			if(!bCreate)
			{
				DWORD dwErr = GetLastError();

				CString strErr;
				strErr.Format("error code = %d", dwErr);
				//MessageBox(strErr);
			}

		}


		if(m_bStopQuit)
		{
			if(m_procStruct.dwThreadId != 0)
			{
				TerminateProcess(m_procStruct.hProcess, 0);
				m_procStruct.dwThreadId =0;
				m_procStruct.dwProcessId = 0;
				CloseHandle(m_procStruct.hProcess);
				CloseHandle(m_procStruct.hThread);
				::SetEvent(m_hEventShut);
			}
		}

		Sleep(1000);
	}

}

bool CBusStopDefenceDlg::IsStopProcessRun()
{
	LogObject;

	if(m_procStruct.dwProcessId == 0)
		return false;

	DWORD dwCode = 0;
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_procStruct.dwProcessId);
	if(::GetExitCodeProcess(hProcess, &dwCode))
	{
		if(dwCode != STILL_ACTIVE && dwCode != 0)
		{
			//�������˳������ʹ���
			CString strLog;
			strLog.Format("�����ն�վ������,�˳�����=%d", dwCode);
			LogOutInfo((char*)(LPCTSTR)strLog);

			if(dwCode != 1)
			{
				m_MailSender.SetInfo((char*)(LPCTSTR)strLog, "");
				m_MailSender.SendMail();
			}
			CloseHandle(hProcess);
			return false;
		}
	}

	CloseHandle(hProcess);
	return true;

}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBusStopDefenceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBusStopDefenceDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CBusStopDefenceDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

LRESULT CBusStopDefenceDlg::OnShowTask(WPARAM wparam, LPARAM lparam)
{
	if(wparam != IDR_MAINFRAME)
	{
		return 1;
	}
	SetForegroundWindow();

	switch(lparam)
	{
	case WM_RBUTTONUP:
		{
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);
			m_menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
			delete lpoint;
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			this->ShowWindow(SW_SHOWDEFAULT);
			this->SetActiveWindow();
		}
		break;
	}
	return 0;
}

void CBusStopDefenceDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	m_bRun = false;
	
	WaitForSingleObject(m_hRun, INFINITE);

	CloseHandle(m_hMapping);
	
	Shell_NotifyIcon(NIM_DELETE, &m_nId);
	m_menu.DestroyMenu();

}

void CBusStopDefenceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if(nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);
	}
}

void CBusStopDefenceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_HIDE);

	KillTimer(1);

	CDialog::OnTimer(nIDEvent);
}

LRESULT CBusStopDefenceDlg::OnShutApp(WPARAM wparam, LPARAM lparam)
{
	int nShut = (int)wparam;
	if(nShut == 1)
		m_bStopQuit = true;
	else if(nShut == 0)
		m_bStopQuit = false;
	return 0;
}

void CBusStopDefenceDlg::OnBnClickedButtonCloseapp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
