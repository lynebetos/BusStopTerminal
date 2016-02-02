// WeatherStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "WeatherStatic.h"
#include <GdiPlus.h>

using namespace Gdiplus;


// CWeatherStatic

IMPLEMENT_DYNAMIC(CWeatherStatic, CStatic)

CWeatherStatic::CWeatherStatic()
{

}

CWeatherStatic::~CWeatherStatic()
{
}


BEGIN_MESSAGE_MAP(CWeatherStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CWeatherStatic::DrawWeather(_st_weather& stWeather)
{
	m_stWeather = stWeather;
	RedrawWindow();
}

// CWeatherStatic 消息处理程序



void CWeatherStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
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

	Gdiplus::RectF rectFBK((REAL)rc.left, (REAL)rc.top, (REAL)rc.Width(), (REAL)rc.Height());

	Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, 255, 255));
	gr.FillRectangle(&brushBK, rectFBK);

	//图标
	vector<string> vecGidList;
	Split(vecGidList, m_stWeather.strTtp, ",");

	CString strGif = ResourceDir;
	strGif += "b_";
	strGif += vecGidList[0].c_str();
	WCHAR wcurDir[160] = {0};
	

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strGif, strGif.GetLength(), wcurDir, 160);

	Gdiplus::Image im(wcurDir);

	int nHight = im.GetHeight();
	int nWidth = im.GetWidth();

	gr.DrawImage(&im, Gdiplus::PointF((REAL)rc.right - nWidth - 5, (REAL)(rc.Height() - nHight) / 2));

	//文字
	CString strText = "";//m_stWeather.strTtq.c_str();
	//strText += "\n";
	strText += m_stWeather.strTwd.c_str();

	Gdiplus::Color clBusBkWord = Gdiplus::Color::WhiteSmoke;
	Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

	std::wstring wstrText = Utf8toWchar((char*)(LPCTSTR)strText);

	Gdiplus::PointF pointFBusBkWord((REAL)rc.left, (REAL)rc.top);
	Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width() - nWidth - 5, (REAL)rc.Height()));
	Gdiplus::Font fontBusBkWord(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusBkWord(StringFormatFlagsNoWrap);
	stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentFar);
	stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);


	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	gr.DrawString(wstrText.c_str(), wstrText.size(), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);


	//	pDC->DrawText(strText, rc, 0);
	gr.ReleaseHDC(m_memDC);
	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();
	ReleaseDC(pDC);

}
