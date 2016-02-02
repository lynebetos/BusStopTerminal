// TimeStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "TimeStatic.h"
#include <GdiPlus.h>

using namespace Gdiplus;

// CTimeStatic

IMPLEMENT_DYNAMIC(CTimeStatic, CStatic)

CTimeStatic::CTimeStatic()
{
	m_vecWeekDay.push_back("日");
	m_vecWeekDay.push_back("一");
	m_vecWeekDay.push_back("二");
	m_vecWeekDay.push_back("三");
	m_vecWeekDay.push_back("四");
	m_vecWeekDay.push_back("五");
	m_vecWeekDay.push_back("六");

	m_bTimeRun = false;
	m_hTime = INVALID_HANDLE_VALUE;
}

CTimeStatic::~CTimeStatic()
{
	m_bTimeRun = false;
}


BEGIN_MESSAGE_MAP(CTimeStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CTimeStatic 消息处理程序

void CTimeStatic::Init()
{
	m_bTimeRun = true;
	unsigned int nThreadID = 0;
	m_hTime = (HANDLE)_beginthreadex(NULL, 0, theadTime, (LPVOID)this, 0, &nThreadID);
	if(m_hTime == INVALID_HANDLE_VALUE)
	{
		//时间线程启动失败
	}
}

unsigned int _stdcall CTimeStatic::theadTime(LPVOID lpvoid)
{
	CTimeStatic* pWnd = (CTimeStatic*)lpvoid;
	if(pWnd)
	{
		pWnd->ProcTime();
	}

	return 0;
}

void CTimeStatic::ProcTime()
{
	while(m_bTimeRun)
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

		SYSTEMTIME systime;
		GetLocalTime(&systime);
		CString strTextTime = ("");
		CString strTextDate = ("");
		strTextTime.Format("%d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);
		strTextDate.Format("%d月%d日 星期%s", systime.wMonth, systime.wDay, m_vecWeekDay[systime.wDayOfWeek].c_str());

		Gdiplus::Color clBusBkWord = Gdiplus::Color::WhiteSmoke;
		Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, 255, 255));
		Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

		//显示时间
		WCHAR wcTextTime[32] = {0};
		MultiByteToWideChar(CP_ACP, 0, strTextTime, strTextTime.GetLength(), wcTextTime, (strTextTime.GetLength() + 1)*2);

		Gdiplus::PointF pointFBusBkWord((REAL)rc.left, (REAL)rc.top);
		Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width(), (REAL)rc.Height() / 2));
		Gdiplus::Font fontBusBkWord(L"方正兰亭黑简体", 22 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatBusBkWord(StringFormatFlagsNoWrap);
		stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		//显示日期
		WCHAR wcTextDate[32] = {0};
		MultiByteToWideChar(CP_ACP, 0, strTextDate, strTextDate.GetLength(), wcTextDate, (strTextDate.GetLength() + 1)*2);

		Gdiplus::PointF pointFDate((REAL)rc.left, (REAL)rc.top + (REAL)rc.Height() / 2);
		Gdiplus::RectF rectDate(pointFDate, Gdiplus::SizeF((REAL)rc.Width(), (REAL)rc.Height() / 2));
		Gdiplus::Font fontDate(L"方正兰亭黑简体", 16 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);


		gr.FillRectangle(&brushBK, rectBusBkWord);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

		gr.DrawString(wcTextTime, wcslen(wcTextTime), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);

		gr.FillRectangle(&brushBK, rectDate);
		gr.DrawString(wcTextDate, wcslen(wcTextDate), &fontDate, rectDate, &stringFormatBusBkWord, &brushBusBkWord);

		//	pDC->DrawText(strText, rc, 0);

		gr.ReleaseHDC(m_memDC);
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
		m_memDC.DeleteDC();
		m_memBitmap.DeleteObject();
		ReleaseDC(pDC);

		Sleep(1000);
	}
}

void CTimeStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()


}

void CTimeStatic::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	CStatic::OnTimer(nIDEvent);
}

void CTimeStatic::OnDestroy()
{
	CStatic::OnDestroy();

	m_bTimeRun = false;
	WaitForSingleObject(m_hTime, 2000);
	// TODO: 在此处添加消息处理程序代码
}
