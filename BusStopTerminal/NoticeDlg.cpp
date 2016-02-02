// NoticeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "NoticeDlg.h"


// CNoticeDlg 对话框

IMPLEMENT_DYNAMIC(CNoticeDlg, CDialog)

CNoticeDlg::CNoticeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNoticeDlg::IDD, pParent)
{
	m_xOffset = 0; 
	m_bNoticeRun = (false);
	m_hNotice = (INVALID_HANDLE_VALUE);
	m_bOver = (false);
	m_strText = ("");
	m_cl = Gdiplus::Color::WhiteSmoke;
	m_nFontPix = 24;
	m_bUpdate = false;

}

CNoticeDlg::~CNoticeDlg()
{
	m_vecList.clear();
}

void CNoticeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNoticeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNoticeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CNoticeDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CNoticeDlg 消息处理程序

BOOL CNoticeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//是窗体透明的代码
	////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	COLORREF maskColor = RGB(255,255,255);
// 	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
// 	HINSTANCE hInst=LoadLibrary(_T("User32.DLL"));
// 	if(hInst)
// 	{
// 		typedef BOOL (WINAPI * MYFUNC)(HWND , COLORREF,BYTE,DWORD);
// 		MYFUNC fun=NULL;
// 		fun=(MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes");
// 		if(fun)fun(this->GetSafeHwnd(),maskColor,255,1);
// 		FreeLibrary(hInst);
// 	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	InitNoticePlay();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNoticeDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CNoticeDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

HBRUSH CNoticeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
// 	if(pWnd->GetDlgCtrlID() == IDC_STATIC_NOTICE)
// 	{
// 		//设置static的背景透明
// 		pDC->SetBkMode(TRANSPARENT);
// 	}
	return    (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CNoticeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
// 	CRect rect;
// 	GetClientRect(rect);
// 	dc.FillSolidRect(rect,RGB(255,255,255)); 

	DrawNotice();
}

void CNoticeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
}

int CNoticeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CNoticeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	/*
	if(!m_wndShowNotic.GetSafeHwnd())
		return;

	CRect rect;
	GetClientRect(rect);

	m_wndShowNotic.MoveWindow(rect);
	*/
	// TODO: 在此处添加消息处理程序代码
}

void CNoticeDlg::DrawNotice()
{
	LogObject;
	CDC* pDC = GetDC();

	CRect rc;
	GetClientRect(&rc);

	m_Blend.SourceConstantAlpha = 255;

	CDC m_memDC;
	CBitmap m_memBitmap;

	m_memDC.CreateCompatibleDC(pDC);
	if(!m_memDC.GetSafeHdc())
		return;

	m_memBitmap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	m_memDC.SelectObject(m_memBitmap);

	Gdiplus::Graphics gr(m_memDC.m_hDC);
	CString strText = m_strText;
	//CString strText = _T("秦皇岛火车站道路临时通车，为方便市民出行，我公司计划于2013年12月5日，恢复调整6路、32路、34路公交线路走向，同时取消四道桥汽车站至北戴河火车站快线线路");

	Gdiplus::Color clBusBkWord = m_cl;
	Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, 255, 255));
	Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

	std::wstring wstrText = Utf8toWchar((char*)(LPCTSTR)strText);

	Gdiplus::PointF pointFBusBkWord((REAL)rc.right + m_xOffset, (REAL)rc.top);
	Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width() * 3, (REAL)rc.Height()));
	Gdiplus::Font fontBusBkWord(L"方正兰亭黑简体", (REAL)m_nFontPix , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusBkWord(StringFormatFlagsNoWrap);
	stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentNear);

	Gdiplus::RectF rectFBK((REAL)rc.left, (REAL)rc.top, (REAL)rc.Width() * 3, (REAL)rc.Height());

	//gr.FillRectangle(&brushBK, rectFBK);
	Gdiplus::RectF rectFStringSize;

	gr.MeasureString(wstrText.c_str(), wstrText.size(), &fontBusBkWord, rectBusBkWord, &rectFStringSize);

	rectBusBkWord.Width = rectFStringSize.Width;
	//gr.FillRectangle(&brushBK, rectBusBkWord);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	gr.DrawString(wstrText.c_str(), wstrText.size(), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);

	//	pDC->DrawText(strText, rc, 0);
	m_xOffset -= 2;

	if(m_xOffset + rc.Width() < (-rectFStringSize.Width))
	{
		m_xOffset = 0;
		m_bOver = true;
	}

	gr.ReleaseHDC(m_memDC);
	//pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);	//Don't forget to set 
	//the dialog's extent style.
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	//CRect rcWin;
	//GetWindowRect(&rcWin);
	POINT pointWin = {m_rc.left, m_rc.top};
	CPoint point(0, 0);
	GetClientRect(&rc);
	SIZE winSize = {rc.Width(), rc.Height()};
	::UpdateLayeredWindow(this->m_hWnd, pDC->m_hDC, &pointWin, &winSize, m_memDC.m_hDC, &point, 0, &m_Blend, 2);
	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();
	ReleaseDC(pDC);

}

bool CNoticeDlg::InitNoticePlay()
{
	m_bNoticeRun = true;
	unsigned int nThreadID = 0;
	m_hNotice = (HANDLE)_beginthreadex(NULL, 0, threadNoticeCtrl, (LPVOID)this, 0, &nThreadID);
	if(m_hNotice == INVALID_HANDLE_VALUE)
	{
		//创建公告控制线程失败
	}
	return true;
}

unsigned int _stdcall CNoticeDlg::threadNoticeCtrl(LPVOID lpvoid)
{
	CNoticeDlg* pNotice = (CNoticeDlg*)lpvoid;
	if(pNotice)
	{
		pNotice->ProcNoticeCtrl();
	}
	return 0;
}

void CNoticeDlg::ProcNoticeCtrl()
{
	int nPlayListIndex = 0;
	int nNoticeIndex = 0;
	vector<_st_NoticeList> vecNotic;
	while(m_bNoticeRun)
	{
		m_lock.Lock();
		if(m_bUpdate)
		{
			vecNotic = m_vecList;
			nPlayListIndex = 0;
			nNoticeIndex = 0;
			m_bUpdate = false;
		}
		m_lock.UnLock();

		if(0 >= vecNotic.size())
		{
			Sleep(1000);
			continue;
		}

		time_t t = time(0);
		string strDateNow = CBase::time2string_date(t);
		string strTimeNow = CBase::time2string_time(t);
		bool bFindList = false;

		for (nPlayListIndex = 0; nPlayListIndex < (int)vecNotic.size(); nPlayListIndex++)	//多个列表中的寻找符合当前时间的列表 整点时间属于下一个列表的
		{
			string strDateStart = CBase::time2string_date(vecNotic[nPlayListIndex].tStart);
			string strDateEnd = CBase::time2string_date(vecNotic[nPlayListIndex].tEnd);
			string strTimeStart = CBase::time2string_time(vecNotic[nPlayListIndex].tStart);
			string strTimeEnd = CBase::time2string_time(vecNotic[nPlayListIndex].tEnd);
			if(strDateStart <= strDateNow && strDateEnd >= strDateNow)
			{
				if(strTimeStart <= strTimeNow && strTimeEnd > strTimeNow)
				{
					bFindList = true;
					break;
				}
			}
		}

		if(!bFindList)
		{
			//处理没有列表的情况
			Sleep(5000);
			continue;
		}

		_st_NoticeList& stList = vecNotic[nPlayListIndex];
		tagNotice::_st_list& stNotic = stList.listNotice[nNoticeIndex++];

		if(nNoticeIndex >= (int)stList.listNotice.size())
			nNoticeIndex = 0;

		m_bOver = false;
		m_strText = stNotic.strText.c_str();
		while(!m_bOver && m_bNoticeRun)
		{
			DrawNotice();
			Sleep(25);
		}

		Sleep(stList.nInterval*1000);
	}
}
void CNoticeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_bNoticeRun = false;
	WaitForSingleObject(m_hNotice, 2000);
}
