// NoticeStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "NoticeStatic.h"


// CNoticeStatic

IMPLEMENT_DYNAMIC(CNoticeStatic, CStatic)

CNoticeStatic::CNoticeStatic()
: m_xOffset(0)
, m_bNoticeRun(false)
, m_hNotice(INVALID_HANDLE_VALUE)
, m_bOver(false)
, m_strText("")
{

}

CNoticeStatic::~CNoticeStatic()
{
}


BEGIN_MESSAGE_MAP(CNoticeStatic, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CNoticeStatic 消息处理程序



HBRUSH CNoticeStatic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
		//设置static的背景透明
//	pDC->SetBkMode(TRANSPARENT);
	//return    (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CNoticeStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()


//	dc.SetBkMode(TRANSPARENT);
}

void CNoticeStatic::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//DrawNotice();
	CStatic::OnTimer(nIDEvent);
}

int CNoticeStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1, 15, NULL);
	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CNoticeStatic::DrawNotice()
{
	CDC* pDC = GetDC();

	CRect rc;
	GetClientRect(&rc);

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

	Gdiplus::Color clBusBkWord = Gdiplus::Color::WhiteSmoke;
	Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, 255, 255));
	Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

	std::wstring wstrText = Utf8toWchar((char*)(LPCTSTR)strText);

	Gdiplus::PointF pointFBusBkWord((REAL)rc.right + m_xOffset, (REAL)rc.top);
	Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width() * 3, (REAL)rc.Height()));
	Gdiplus::Font fontBusBkWord(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusBkWord(StringFormatFlagsNoWrap);
	stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentNear);

	Gdiplus::RectF rectFBK((REAL)rc.left, (REAL)rc.top, (REAL)rc.Width() * 3, (REAL)rc.Height());

	gr.FillRectangle(&brushBK, rectFBK);
	Gdiplus::RectF rectFStringSize;

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.MeasureString(wstrText.c_str(), wstrText.size(), &fontBusBkWord, rectBusBkWord, &rectFStringSize);

	rectBusBkWord.Width = rectFStringSize.Width;
	gr.FillRectangle(&brushBK, rectBusBkWord);

	gr.DrawString(wstrText.c_str(), wstrText.size(), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);

	//	pDC->DrawText(strText, rc, 0);
	m_xOffset -= 2;

	if(m_xOffset + rc.Width() < (-rectFStringSize.Width))
	{
		m_xOffset = 0;
		m_bOver = true;
	}

	gr.ReleaseHDC(m_memDC);
	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();
	ReleaseDC(pDC);

}

bool CNoticeStatic::InitNoticePlay()
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

unsigned int _stdcall CNoticeStatic::threadNoticeCtrl(LPVOID lpvoid)
{
	CNoticeStatic* pNotice = (CNoticeStatic*)lpvoid;
	if(pNotice)
	{
		pNotice->ProcNoticeCtrl();
	}
	return 0;
}

void CNoticeStatic::ProcNoticeCtrl()
{
	int nPlayListIndex = 0;
	int nNoticeIndex = 0;
	while(m_bNoticeRun)
	{
		m_lock.Lock();
		m_lock.UnLock();
		vector<_st_NoticeList> vecNotic = m_vecList;
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

		if(nNoticeIndex >= stList.listNotice.size())
			nNoticeIndex = 0;

		m_bOver = false;
		m_strText = stNotic.strText.c_str();
		while(!m_bOver)
		{
			DrawNotice();
			Sleep(15);
		}

		Sleep(stList.nInterval*1000);
	}
}