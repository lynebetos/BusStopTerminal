
// BusStopTerminalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "BusStopTerminalDlg.h"
#include "DataManager.h"
#include "ResourceManager.h"
#include "TemplateFunction.h"
#include "PlayListFunction.h"
#include "ConfigManager.h"
#include "NoticeFunction.h"
#include "HartFunction.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CBusStopTerminalDlg 对话框




CBusStopTerminalDlg::CBusStopTerminalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBusStopTerminalDlg::IDD, pParent)
{
	m_pwndNotic = NULL;
	m_pwndTime = NULL;
	m_pwndWeather = NULL;
	m_pwndMediumPanel = NULL;
	m_pwndSimplePanelDlg = NULL;
	m_pwndPictureDlg = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bChange = FALSE;
	m_nChangeCount = 0;
	m_nPannelCount = 0;
}

void CBusStopTerminalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBusStopTerminalDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CBusStopTerminalDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBusStopTerminalDlg::OnBnClickedCancel)
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_MESSAGE(WM_RELOADNOTIC, &CBusStopTerminalDlg::OnReloadNotice)
	ON_MESSAGE(WM_RELOADPLAYLIST, &CBusStopTerminalDlg::OnReloadPlaylist)
	ON_MESSAGE(WM_WEATHER, &CBusStopTerminalDlg::OnWeather)
	ON_MESSAGE(WM_ARRIVAL, &CBusStopTerminalDlg::OnArrival)
	ON_MESSAGE(WM_SETWARNING, &CBusStopTerminalDlg::OnSetWarning)
	ON_MESSAGE(WM_CALLSTATISTIC, &CBusStopTerminalDlg::OnCallStatistic)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBusStopTerminalDlg 消息处理程序

BOOL CBusStopTerminalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码

// 	GetDlgItem(IDOK)->ShowWindow(FALSE);
// 	GetDlgItem(IDCANCEL)->ShowWindow(FALSE);
//  	::SetWindowPos(this->m_hWnd,HWND_TOPMOST ,0,0,0,0,
//  		SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	//GenerateData();
// 	::ShowCursor(FALSE);


	LogObject;

	HideTaskBar(true); 

	//ClientToScreen(rcBase);
	//ScreenToClient(rcBase);
	CTemplateFunction* pFun = (CTemplateFunction*)m_pMain->GetFunction(TemplateFun);
	if(pFun)
	{
		m_nPannelCount = pFun->GetComPannelCount();
	}

	g_hWord = CreateSemaphore(NULL, m_nPannelCount, m_nPannelCount, NULL);
	g_hPage = CreateSemaphore(NULL, m_nPannelCount, m_nPannelCount, NULL);

	m_pMain->StartCallPS();

	LogOutInfo("绘制格面板模块");
	PanelLayout();


	LogOutInfo("绑定数据到面板");
	if(!LinkDatePanel())
	{
		//连接线路数据和面板失败
	}
	LogOutInfo("界面启动完成");

	SetTimer(1, 15000, NULL);
	SetTimer(2, 4000, NULL);
	SetTimer(3, 5000, NULL);
	SetTimer(4, 30000, NULL);

	unsigned int nThreadID = 0;
	HANDLE hConn = (HANDLE)_beginthreadex(NULL, 0, ProcDoConn, (LPVOID)this, 0, &nThreadID);
	if(hConn == INVALID_HANDLE_VALUE)
	{
		LogOutError("启动连接网络失败");
	}

	nThreadID = 0;
	HANDLE hShut = (HANDLE)_beginthreadex(NULL, 0, ProcShutDown, (LPVOID)this, 0, &nThreadID);
	if(hShut == INVALID_HANDLE_VALUE)
	{
		LogOutError("事件监听线程失败");
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

unsigned int _stdcall CBusStopTerminalDlg::ProcShutDown(LPVOID lpvoid)
{
	CBusStopTerminalDlg* pDlg = (CBusStopTerminalDlg*)lpvoid;
	if(pDlg)
	{
		pDlg->CreateEventRestart();
		while (false)
		{
			if(!pDlg->WaitEvent())
				continue;
			else
			{
				pDlg->SendMessage(WM_DESTROY);
				return 0;
			}
		}
	}

	return 0;
}

unsigned int _stdcall CBusStopTerminalDlg::ProcDoConn(LPVOID lpvoid)
{
	CBusStopTerminalDlg* pDlg = (CBusStopTerminalDlg*)lpvoid;
	if(pDlg)
	{
		CMain* pMian = pDlg->GetMain();
		int nRes = pMian->DoConnectServer();

		if(nRes == 2)
		{
			pDlg->SendMessage(WM_DESTROY);
		}
	}

	return 0;
}

void CBusStopTerminalDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBusStopTerminalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		LogObject;
		LogOutInfo("主界面开始重绘");
		CRect rect;
		GetClientRect(rect);
		CDC* pDC = GetDC();
		pDC->FillSolidRect(rect,RGB(255,255,255)); 

		ReleaseDC(pDC);

		if(m_pwndPictureDlg)
			m_pwndPictureDlg->RedrawWindow();
		if(m_pwndSimplePanelDlg)
			m_pwndSimplePanelDlg->RedrawWindow();
		if(m_pwndMediumPanel)
			m_pwndMediumPanel->RedrawWindow();

		for (vector<CBasePanelDlg*>::iterator theiter = m_vecwndLinePanel.begin(); 
			theiter != m_vecwndLinePanel.end(); theiter++)
		{
			CBasePanelDlg* pDlg = *theiter;
			if(pDlg)
			{
				pDlg->RedrawWindow();
			}
		}

		CDialog::OnPaint();
	}
}

bool CBusStopTerminalDlg::PanelLayout()
{
	CTemplateFunction* pFun = (CTemplateFunction*)m_pMain->GetFunction(TemplateFun);
	if(pFun)
	{
		const vector<CTemplateFunction::_st_template>& vecTemplate = pFun->GetTemplate();
		for (vector<CTemplateFunction::_st_template>::const_iterator coveciter = vecTemplate.begin();
			coveciter != vecTemplate.end(); coveciter++)
		{
			if(coveciter->strType == "root")
			{
				CRect rc = toRect(coveciter->size);
				this->SetWindowPos((CWnd*)HWND_TOP, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);

// 				CenterWindow();

			}
			else if(coveciter->strType == "video")
			{
				CRect rc = toRect(coveciter->size);
				LayoutVideo(rc);	
			}
			else if(coveciter->strType == "notice")
			{
				CRect rc = toRect(coveciter->size);
				int nFontSize = coveciter->nFontsize;
				DWORD dwCl = strtoul(coveciter->strColor.c_str(), NULL, 16);
				LayoutNotice(rc, dwCl, nFontSize);
			}
			else if(coveciter->strType == "weather")
			{
				CRect rc = toRect(coveciter->size);
				LayoutWeather(rc);
			}
			else if(coveciter->strType == "time")
			{
				CRect rc = toRect(coveciter->size);
				LayoutTime(rc);
			}
			else if(coveciter->strType == "complicate")
			{
				CRect rc = toRect(coveciter->size);
				LayoutComplicate(rc);
			}
			else if(coveciter->strType == "medium")
			{
				CRect rc = toRect(coveciter->size);
				LayoutMiddle(rc);
			}
			else if(coveciter->strType == "simple")
			{
				CRect rc = toRect(coveciter->size);
				LayoutSimple(rc);
			}
			else if(coveciter->strType == "pictrue")
			{
				CRect rc = toRect(coveciter->size);
				LayoutPictrue(rc);
			}
		}

		return true;
	}
	return false;
}

bool CBusStopTerminalDlg::LayoutVideo(const CRect& rc)
{
	CRect rcWnd = rc;
	rcWnd.left-=1;
	rcWnd.top-=1;
	//播放器初始化
	if(!m_wndPlayer.Create(rcWnd, this))
	{
		//创建播放器失败
		return false;
	}

	if(!InitPlayer())
	{
		return false;
	}
	m_wndPlayer.MoveWindow(rcWnd);

	return true;
}

bool CBusStopTerminalDlg::LayoutNotice(const CRect& rc, DWORD dwCl, int nFontSize)
{
	CRect rcParent;
	this->GetWindowRect(&rcParent);
	CRect rcClient = rc;
	rcClient.left += rcParent.left;
	rcClient.right += rcParent.left;

	m_pwndNotic = new CNoticeDlg;
	if(!m_pwndNotic->Create(IDD_NOTICE_DIALOG, this))
	{
		//创建公告面板失败；
	}
	m_pwndNotic->MoveWindow(rcClient);
	m_pwndNotic->ShowWindow(TRUE);
	m_pwndNotic->SetRect(rcClient);
	if(!InitNotice())
	{
		//初始化公告失败
	}
	m_pwndNotic->SetNoticeShowAttrib(dwCl, nFontSize);

	return true;
}

bool CBusStopTerminalDlg::LayoutWeather(const CRect& rc)
{
	CRect rcParent;
	this->GetWindowRect(&rcParent);
	CRect rcClient = rc;
	rcClient.left += rcParent.left;
	rcClient.right += rcParent.left;

	m_pwndWeather = new CWeatherDlg;
	if(!m_pwndWeather->Create(IDD_WEATHER_DIALOG, this))
	{
		//创建天气面板失败；
	}
	m_pwndWeather->MoveWindow(rcClient);
	m_pwndWeather->ShowWindow(TRUE);
	return true;
}

bool CBusStopTerminalDlg::LayoutTime(const CRect& rc)
{
	CRect rcParent;
	this->GetWindowRect(&rcParent);
	CRect rcClient = rc;
	rcClient.left += rcParent.left;
	rcClient.right += rcParent.left;

	m_pwndTime = new CTimeDlg;
	if(!m_pwndTime->Create(IDD_TIME_DIALOG, this))
	{
		//创建时间面板失败；
	}
	m_pwndTime->MoveWindow(rcClient);
	m_pwndTime->ShowWindow(TRUE);
	return true;
}

bool CBusStopTerminalDlg::LayoutComplicate(const CRect& rc)
{
	CBasePanelDlg* pwndLinePanel = new CBasePanelDlg;
	if(!pwndLinePanel->Create(IDD_BASEPANEL_DIALOG, this))
	{
		//创建线路面板失败
		delete pwndLinePanel;
		return false;
	}
	pwndLinePanel->MoveWindow(rc);
	m_vecwndLinePanel.push_back(pwndLinePanel);
	return true;
}

bool CBusStopTerminalDlg::LayoutMiddle(const CRect& rc)
{
	m_pwndMediumPanel = new CMediumPanelDlg;
	if(!m_pwndMediumPanel->Create(IDD_MEDIUMPANEL_DLG, this))
	{
		//创建线路面板失败
		delete m_pwndMediumPanel;
		return false;
	}
	m_pwndMediumPanel->MoveWindow(rc);
	m_pwndMediumPanel->ShowWindow(TRUE);
	m_pwndMediumPanel->RedrawWindow();
	return true;
}

bool CBusStopTerminalDlg::LayoutSimple(const CRect& rc)
{
	m_pwndSimplePanelDlg = new CSimplePanelDlg;
	if(!m_pwndSimplePanelDlg->Create(IDD_SIMPLEPANEL_DIALOG, this))
	{
		//创建线路面板失败
		delete m_pwndSimplePanelDlg;
		return false;
	}
	m_pwndSimplePanelDlg->MoveWindow(rc);
	m_pwndSimplePanelDlg->ShowWindow(TRUE);
	m_pwndSimplePanelDlg->RedrawWindow();
	return true;
}

bool CBusStopTerminalDlg::LayoutPictrue(const CRect& rc)
{
	m_pwndPictureDlg = new CPictureDlg;
	if(!m_pwndPictureDlg->Create(IDD_PICTRUE_DIALOG, this))
	{
		//创建图片播放面板失败
		delete m_pwndPictureDlg;
		return false;
	}
	m_pwndPictureDlg->MoveWindow(rc);
	m_pwndPictureDlg->ShowWindow(TRUE);
	m_pwndPictureDlg->RedrawWindow();
	InitPicture();
	return true;
}

bool CBusStopTerminalDlg::InitPlayer()
{
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(!pCM)
		return false;

	CPlayListFunction* pFun = (CPlayListFunction*)m_pMain->GetFunction(PlaylistFun);
	if(pFun)
	{
		vector<_st_PlayList> vecList;
		pFun->GetPlayList(vecList);

		for (int ii = 0; ii < (int)vecList.size(); ii++)
		{
			_st_PlayList& stPL = vecList[ii];
			for (int jj = 0; jj < (int)stPL.listPlay.size(); jj++)
			{
				tagPlay::_st_list& stVL = stPL.listPlay[jj];
				string strName = pCM->GetAdverDir() + stVL.strName;
				stVL.strName = strName;
			}
			
		}
		m_wndPlayer.SetList(vecList);
		return true;
	}
	return false;
}

bool CBusStopTerminalDlg::InitNotice()
{
	CNoticeFunction* pFun = (CNoticeFunction*)m_pMain->GetFunction(NoticeFun);
	if(pFun)
	{
		vector<_st_NoticeList> vecList;
		pFun->GetNoticeList(vecList);

		if(m_pwndNotic)
			m_pwndNotic->SetList(vecList);
		return true;
	}

	return false;
}

bool CBusStopTerminalDlg::InitPicture()
{
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		string strPath = pCM->GetPicturePath();
		vector<string> vecPice;
		getFiles(strPath, vecPice);
		if(m_pwndPictureDlg)
			m_pwndPictureDlg->SetPicPath(vecPice);
	}

	return false;
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBusStopTerminalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBusStopTerminalDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CBusStopTerminalDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
// 	ShowWindow(FALSE);
// 	SetEvent(g_hExit);
}

LRESULT CBusStopTerminalDlg::OnReloadPlaylist(WPARAM wparam, LPARAM lparam)
{
	InitPlayer();
	return 0;
}

LRESULT CBusStopTerminalDlg::OnReloadNotice(WPARAM wparam, LPARAM lparam)
{
	InitNotice();
	return 0;
}

LRESULT CBusStopTerminalDlg::OnCallStatistic(WPARAM wparam, LPARAM lparam)
{
	m_wndPlayer.SetCallBack(wparam);
	return 0;
}

LRESULT CBusStopTerminalDlg::OnArrival(WPARAM wparam, LPARAM lparam)
{
	if(m_pwndMediumPanel)
		m_pwndMediumPanel->UpdateArrival();
	if(m_pwndSimplePanelDlg)
		m_pwndSimplePanelDlg->UpdateArrival();

	for(vector<CBasePanelDlg*>::iterator theiter = m_vecwndLinePanel.begin(); 
		theiter != m_vecwndLinePanel.end(); theiter++)
	{
		CBasePanelDlg* pDlg = *theiter;
		if(pDlg && pDlg->GetSafeHwnd())
			pDlg->UpdateArrival();
	}

	return 0;
}

LRESULT CBusStopTerminalDlg::OnWeather(WPARAM wparam, LPARAM lparam)
{
	_st_weather* pWeather = (_st_weather*)wparam;
	if(m_pwndWeather)
		m_pwndWeather->SetWeather(*pWeather);
	return 0;
}

LRESULT CBusStopTerminalDlg::OnSetWarning(WPARAM wparam, LPARAM lparam)
{
	CHartFunction* pFun = (CHartFunction*)m_pMain->GetFunction(HartFun);
	if(pFun)
	{
		pFun->SetWarning((unsigned short)wparam);
	}

	return 0;
}

void CBusStopTerminalDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UINT nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);   // 获取拖入的文件数

	char szFileName[_MAX_PATH] = _T("");

	::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);

	::DragFinish(hDropInfo);    // 释放内存  

	m_wndPlayer.PlayVideo(szFileName);

	CDialog::OnDropFiles(hDropInfo);
}

bool CBusStopTerminalDlg::LinkDatePanel()
{
	bool bRet = false;
	vector<CData*> vecDataPoint;
	CDataManager* pDM = CDataManager::GetInstance();
	if(pDM)
	{
		bRet = pDM->GetAllData(vecDataPoint);
	}
	else
		return false;

	if(!vecDataPoint.size())
		return false;

	if(NULL != m_pwndMediumPanel)
		m_pwndMediumPanel->SetData(vecDataPoint);

	if(NULL != m_pwndSimplePanelDlg)
		m_pwndSimplePanelDlg->SetData(vecDataPoint);

	vector<CBasePanelDlg*>::iterator iterPanel = m_vecwndLinePanel.begin();
	for (int ii = 0; ii < (int)vecDataPoint.size(); ii++)
	{
		if(iterPanel != m_vecwndLinePanel.end())
		{
			CBasePanelDlg* pDlg = *iterPanel;
			pDlg->SetData(vecDataPoint[ii]);
			pDlg->ShowWindow(TRUE);
			pDlg->RedrawWindow();
			iterPanel ++;
			if(iterPanel == m_vecwndLinePanel.end())
			{
				iterPanel = m_vecwndLinePanel.begin();
			}
		}
	}


	return bRet;
}

void CBusStopTerminalDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	return;

	if(!this->GetSafeHwnd())
	{
		return;
	}

	PanelLayout();

	// TODO: 在此处添加消息处理程序代码
}

void CBusStopTerminalDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LogObject;
	if(nIDEvent == 1)
	{
		if(this->GetSafeHwnd())
			this->SetActiveWindow();
	}
	else if(nIDEvent == 2)
	{
		
		vector<CData*> vecData;
		CDataManager* pDM = CDataManager::GetInstance();
		if(pDM)
		{
			pDM->GetAllData(vecData);
		}
		else
		{
			return;
		}

		if(m_nChangeCount == 4)
		{
			m_nChangeCount = 0;

			if(vecData.size() > m_vecwndLinePanel.size())
			{
				long nPageCount = 0;
				ReleaseSemaphore(g_hPage, m_nPannelCount, &nPageCount);

				//同步刷机制
				for (int nMovePix = 1; nMovePix <= 218; nMovePix+=5)
				{
					vector<CBasePanelDlg*>::iterator theiter = m_vecwndLinePanel.begin();
					for (theiter; theiter != m_vecwndLinePanel.end(); theiter++)
					{
						CBasePanelDlg* pDlg = *theiter;
						CData* pDataNow = pDlg->GetDataNow();
						CData* pDataNext = pDlg->GetDataNext();
						pDlg->MovePix(nMovePix, pDataNow, pDataNext);

					}
				}

				//将当前页标识指向下一页
				vector<CBasePanelDlg*>::iterator theiter = m_vecwndLinePanel.begin();
				for (theiter; theiter != m_vecwndLinePanel.end(); theiter++)
				{
					CBasePanelDlg* pDlg = *theiter;
					pDlg->MoveDataToNext();
				}

				//

			}
		}
		else
		{
			CDataManager* pDM = CDataManager::GetInstance();
			if(pDM)
			{
				vector<CData*> vecData;
				pDM->GetAllData(vecData);
				for (vector<CData*>::iterator iter = vecData.begin(); iter != vecData.end(); iter++)
				{
					CData* pData = *iter;
					pData->SetChange(m_bChange);
				}
			}

			long nCount = 0;
			ReleaseSemaphore(g_hWord, m_nPannelCount, &nCount);
			m_nChangeCount++;
			if(m_bChange)
			{
				m_bChange = FALSE;
			}
			else
			{
				m_bChange = TRUE;
			}
		}



// 		SetEvent(g_hWord);
// 		Sleep(200);
// 		ResetEvent(g_hWord);
// 		g_bWord = true;
// 		Sleep(200);
// 		g_bWord = false;


	}
	else if(nIDEvent == 3)
	{
		MInfo info;
		info.nMessageID = RUN_NOMAL;
		((CBusStopTerminalApp*)AfxGetApp())->WriteFileMapping(&info);
	}
	else if(nIDEvent == 4)
	{

	}

	CDialog::OnTimer(nIDEvent);
}

void CBusStopTerminalDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LogObject;

	CDialog::OnClose();
}

void CBusStopTerminalDlg::OnDestroy()
{
#ifdef DEBUG
	AllocConsole();
#endif

	LogObject;

	LogOutInfo("关闭定时器");
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);

	LogOutInfo("关闭播放界面");
	if(m_wndPlayer.GetSafeHwnd())
		m_wndPlayer.OnDestroy();

	LogOutInfo("关闭天气界面");
	if(m_pwndWeather && m_pwndWeather->GetSafeHwnd())
	{
		m_pwndWeather->SendMessage(WM_CLOSE);
		delete m_pwndWeather;
		m_pwndWeather = NULL;
	}

	LogOutInfo("关闭时间界面");
	if(m_pwndTime && m_pwndTime->GetSafeHwnd())
	{
		m_pwndTime->SendMessage(WM_CLOSE);
		delete m_pwndTime;
		m_pwndTime = NULL;
	}

	LogOutInfo("关闭公告界面");
	if(m_pwndNotic && m_pwndNotic->GetSafeHwnd())
	{
		m_pwndNotic->SendMessage(WM_CLOSE);
		delete m_pwndNotic;
		m_pwndNotic = NULL;
	}
	
	LogOutInfo("关闭站牌界面");
	if(m_pwndPictureDlg && m_pwndPictureDlg->GetSafeHwnd())
	{
		m_pwndPictureDlg->SendMessage(WM_CLOSE);
		delete m_pwndPictureDlg;
		m_pwndPictureDlg = NULL;
	}

	if(m_pwndMediumPanel && m_pwndMediumPanel->GetSafeHwnd())
	{
		m_pwndMediumPanel->SendMessage(WM_CLOSE);
		delete m_pwndMediumPanel;
		m_pwndMediumPanel = NULL;
	}

	if(m_pwndSimplePanelDlg && m_pwndSimplePanelDlg->GetSafeHwnd())
	{
		m_pwndSimplePanelDlg->SendMessage(WM_CLOSE);
		delete m_pwndSimplePanelDlg;
		m_pwndSimplePanelDlg = NULL;
	}

	for(vector<CBasePanelDlg*>::iterator theiter = m_vecwndLinePanel.begin(); 
		theiter != m_vecwndLinePanel.end(); theiter++)
	{
		CBasePanelDlg* pDlg = *theiter;
		if(pDlg && pDlg->GetSafeHwnd())
			pDlg->SendMessage(WM_DESTROY);

	}

	m_vecwndLinePanel.clear();

	long nPageCount = 0;
	ReleaseSemaphore(g_hPage, m_nPannelCount, &nPageCount);

	long nCount = 0;
	ReleaseSemaphore(g_hWord, m_nPannelCount, &nCount);

	LogOutInfo("关闭信号量句柄");
	CloseHandle(g_hPage);
	CloseHandle(g_hWord);
	g_hPage = INVALID_HANDLE_VALUE;
	g_hWord = INVALID_HANDLE_VALUE;
	
	HideTaskBar(false); 

	for(vector<CBasePanelDlg*>::iterator theiter = m_vecwndLinePanel.begin(); 
		theiter != m_vecwndLinePanel.end(); theiter++)
	{
		CBasePanelDlg* pDlg = *theiter;
		if(pDlg)
		{
			delete pDlg;
			pDlg = NULL;
		}

	}


	CDialog::OnDestroy();


	// TODO: 在此处添加消息处理程序代码
}

void CBusStopTerminalDlg::HideTaskBar(bool bHide)
{
	HWND hWnd;
	hWnd = (HWND)FindWindow("Shell_TrayWnd",NULL);
	if(bHide)
 		::ShowWindow(hWnd,SW_HIDE);
	else
		::ShowWindow(hWnd,SW_SHOW);

	LPARAM lParam;
	if(bHide)
	{
		lParam = ABS_AUTOHIDE;//自动隐藏
	}
	else
	{
		lParam = ABS_ALWAYSONTOP;//取消自动隐藏
	}

	APPBARDATA apBar; 
	memset(&apBar,0,sizeof(apBar)); 
	apBar.cbSize = sizeof(apBar); 
	apBar.hWnd = hWnd;
	if(apBar.hWnd != NULL) 
	{ 
		apBar.lParam   =   lParam; 
		SHAppBarMessage(ABM_SETSTATE,&apBar); //设置任务栏自动隐藏
	}   

}

