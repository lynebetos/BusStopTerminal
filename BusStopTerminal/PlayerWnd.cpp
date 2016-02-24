// PlayerWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "PlayerWnd.h"
#include "ConfigManager.h"
#include "EsayMemDC.h"

// CPlayerWnd

IMPLEMENT_DYNAMIC(CPlayerWnd, CStatic)

CPlayerWnd::CPlayerWnd()
: m_bPlayerRun(false)
, m_bUpdate(false)
, m_lpfSetStatistic(NULL)
{

}

CPlayerWnd::~CPlayerWnd()
{
	m_bPlayerRun = false;
}


BEGIN_MESSAGE_MAP(CPlayerWnd, CStatic)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CPlayerWnd 消息处理程序



BOOL CPlayerWnd::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	CString strText = "CPlayerWnd";
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |  WS_EX_ACCEPTFILES;

	BOOL bCreate = CStatic::Create(strText, dwStyle, rect, pParentWnd, nID);
	if(!bCreate)
		return FALSE;

	DWORD dwExStyle = this->GetExStyle();
	dwExStyle |= WS_EX_ACCEPTFILES;
	this->ModifyStyleEx(0, dwExStyle);
	
// 
// 	if(!bRet)
// 	{
// 		DestroyWindow();
// 	}

	InitPlayThread();

	return TRUE;
}

BOOL CPlayerWnd::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CStatic::DestroyWindow();
}

void CPlayerWnd::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CStatic::OnDropFiles(hDropInfo);
}

void CPlayerWnd::PlayVideo(CString strFile)
{
	//m_avp.Play((char*)(LPCTSTR)strFile);
}

void CPlayerWnd::InitPlayThread()
{
	m_bPlayerRun = true;
	unsigned int nThreadID = 0;
	m_hPlayer = (HANDLE)_beginthreadex(NULL, 0, threadPlayCtl, (LPVOID)this, 0, &nThreadID);
	if(m_hPlayer == INVALID_HANDLE_VALUE)
	{
		//播放控制线程创建失败
	}
}

unsigned int CPlayerWnd::threadPlayCtl(LPVOID lpvoid)
{
	CPlayerWnd* pWnd = (CPlayerWnd*)lpvoid;
	if(pWnd)
	{
		pWnd->ProcPlayCtrl();
	}

	return 0;
}

void CPlayerWnd::ProcPlayCtrl()
{
	LogObject;

	Sleep(1000);
	CRect rc;
	//CRect rc(0, 0, 1080, 608);

	this->GetClientRect(&rc);
 	RedrawWindow();

	int nPlayListIndex = 0;
	int nVideoListIndex = 0;
	int nLastPlayList = 0;
	vector<_st_PlayList> vecPlayList;
	while (m_bPlayerRun)
	{
		m_lock.Lock();
		if(m_bUpdate)
		{
			vecPlayList  = m_vecList;
			nPlayListIndex = 0;
			nVideoListIndex = 0;
			m_bUpdate = false;
			nLastPlayList = 0;
		}
		m_lock.UnLock();
		if(0 >= vecPlayList.size())
		{
			DrawPic();
			Sleep(1000);
			continue;
		}
		
		time_t t = time(0);
		string strDateNow = CBase::time2string_date(t);
		string strTimeNow = CBase::time2string_time(t);
		bool bFindList = false;

		for (nPlayListIndex = 0; nPlayListIndex < (int)vecPlayList.size(); nPlayListIndex++)	//多个列表中的寻找符合当前时间的列表 整点时间属于下一个列表的
		{
			string strDateStart = CBase::time2string_date(vecPlayList[nPlayListIndex].tStart);
			string strDateEnd = CBase::time2string_date(vecPlayList[nPlayListIndex].tEnd);
			string strTimeStart = CBase::time2string_time(vecPlayList[nPlayListIndex].tStart);
			string strTimeEnd = CBase::time2string_time(vecPlayList[nPlayListIndex].tEnd);
			if((strDateStart <= strDateNow && strDateEnd >= strDateNow) && (strTimeStart <= strTimeNow && strTimeEnd > strTimeNow))
			{
				bFindList = true;
				if(nPlayListIndex != nLastPlayList)
				{
					nVideoListIndex = 0;
				}
				nLastPlayList = nPlayListIndex;
				break;
			}
		}

		if(!bFindList)
		{
			//处理没有列表的情况
			//如果没有列表，显示一张图片
			g_nPlaylist = 1;
			DrawPic();
			Sleep(5000);
			continue;
		}

		_st_PlayList& stList = vecPlayList[nPlayListIndex];
		tagPlay::_st_list& stVideo = stList.listPlay[nVideoListIndex];

// 		if(m_avp.IsPlayOver())		//单个列表里面的视频循环
// 		{
// 			m_avp.Close();
// 			nVideoListIndex++;
// 			if(nVideoListIndex >= (int)stList.listPlay.size())
// 			{
// 				nVideoListIndex = 0;
// 			}
// 			Sleep(1000);
// 			continue;
// 		}

		m_avp.SetHwnd((unsigned long)this->m_hWnd);
		bool bRet = m_avp.Init(rc.Width(), rc.Height());

		//log
		string strlog = "播放视频ID是:";
		strlog += stVideo.strName;

		strlog += "播放起始时间:";
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		CString strDate, strTimeStart;
		strDate.Format("%04d-%02d-%02d", systime.wYear, systime.wMonth,systime.wDay);
		strTimeStart.Format("%02d:%02d:%02d", systime.wHour,systime.wMinute,systime.wSecond);
		CString strStartTime;
		strStartTime.Format("%04d-%02d-%02d %02d:%02d:%02d", systime.wYear, systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
		strlog += (char*)(LPCTSTR)strStartTime;
		LogOutInfo(strlog);		//

		
		if(!m_avp.Play(stVideo.strName.c_str()))
		{
			Alarm(WARNING_NOFILE);
			strlog = "视频播放失败,ID=";
			strlog += stVideo.strName;

			LogOutInfo(strlog);
			Sleep(1000);
			continue;
		}
		{
			//m_avp.Stop();
			m_avp.Close();
			nVideoListIndex++;
			if(nVideoListIndex >= (int)stList.listPlay.size())
			{
				nVideoListIndex = 0;
			}
			g_nPlaylist = 0;
			Sleep(1000);
		}
		//log
		strlog = "播放视频序号是:";
		strlog += stVideo.strNum;

		strlog = "播放结束时间:";
		GetLocalTime(&systime);
		CString strTimeEnd;
		strTimeEnd.Format("%02d:%02d:%02d", systime.wHour,systime.wMinute,systime.wSecond);
		CString strEndTime;
		strEndTime.Format("%04d-%02d-%02d %02d:%02d:%02d", systime.wYear, systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
		strlog += (char*)(LPCTSTR)strEndTime;
		LogOutInfo(strlog);
		//

		//播放统计
		PlayStat(stVideo.strName, (char*)(LPCTSTR)strDate, (char*)(LPCTSTR)strTimeStart, (char*)(LPCTSTR)strTimeEnd);

		CDC* pDC = GetDC();
		pDC->FillSolidRect(rc,RGB(0,0,0)); 

		ReleaseDC(pDC);

		//string strInfo;
		DWORD dwId = GetCurrentProcessId();
		char szBuff[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, szBuff);
		string strFileCSV = szBuff;
		strFileCSV += "\\memeryBus.csv";
		PrintMemoryInfoCSV(dwId, strFileCSV);
		//PrintMemoryInfo(dwId, strInfo);
		//LogOutInfo(strInfo.c_str());


	}
}

void CPlayerWnd::PlayStat(string strPathName, string strDate, string strStart, string strEnd)
{
	PlayStatistic stPlayStatistic;
	int nPos = strPathName.find_last_of('\\');
	stPlayStatistic.strName = strPathName.substr(nPos+1);
	stPlayStatistic.Data.strDate = strDate;
	stPlayStatistic.Data.strStartTime = strStart;
	stPlayStatistic.Data.strEndTime = strEnd;

	if(m_lpfSetStatistic)
		m_lpfSetStatistic(&stPlayStatistic);
}

void CPlayerWnd::DrawPic()
{
	LogObject;
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(!pCM)
	{
		return;
	}
	
	string strPath = pCM->GetDefaultPath();

	string strLog = "默认图片的路径:";
	strLog += strPath;
	LogOutInfo(strLog.c_str());

	CDC* pDC = GetDC();
	CRect rc;
	GetClientRect(&rc);

	CEsayMemDC memDC(*pDC, rc);

	Gdiplus::Graphics gr(memDC.GetDC());

	wstring wstrPath = c2w(strPath.c_str());

	Gdiplus::Image im(wstrPath.c_str());

	Gdiplus::REAL Height = (Gdiplus::REAL)im.GetHeight();
	Gdiplus::REAL Width = (Gdiplus::REAL)im.GetWidth();

	Gdiplus::RectF rcF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height());
	
	gr.DrawImage(&im, rcF);

	memDC.BltMem(*pDC);

	ReleaseDC(pDC);
}

void CPlayerWnd::OnDestroy()
{
	CStatic::OnDestroy();
	m_bPlayerRun = false;
	m_avp.Close();

	WaitForSingleObject(m_hPlayer, 2000);

	// TODO: 在此处添加消息处理程序代码
}

void CPlayerWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CStatic::OnClose();
}

void CPlayerWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CRect rect;
	GetClientRect(rect);

	Gdiplus::Graphics gr(dc.m_hDC);
	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));

	gr.FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());

// 	dc.SetBkColor(RGB(0, 0, 0));
}
