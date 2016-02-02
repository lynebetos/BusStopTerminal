// WeatherDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "WeatherDlg.h"

using namespace Gdiplus;

// CWeatherDlg �Ի���

IMPLEMENT_DYNAMIC(CWeatherDlg, CDialog)

CWeatherDlg::CWeatherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWeatherDlg::IDD, pParent)
{

}

CWeatherDlg::~CWeatherDlg()
{
}

void CWeatherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWeatherDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CWeatherDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWeatherDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWeatherDlg ��Ϣ�������

void CWeatherDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CWeatherDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnCancel();
}

BOOL CWeatherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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

	//m_wndWeather.Init();
	m_nNoGetWeather = 120;

	SetTimer(1, 60*1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

HBRUSH CWeatherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
// 	if(pWnd->GetDlgCtrlID() == IDC_STATIC_WEATHER)
// 	{
// 		//����static�ı���͸��
// 		pDC->SetBkMode(TRANSPARENT);
// 	}
	return    (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CWeatherDlg::SetWeather(_st_weather& stWeather)
{
	m_stWeather = stWeather;
	m_nNoGetWeather = 0;
	RedrawWindow();
}

void CWeatherDlg::OnPaint()
{
	LogObject;
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	//CRect rect;
	//GetClientRect(rect);
	//dc.FillSolidRect(rect,RGB(255,255,255)); 

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
	//gr.FillRectangle(&brushBK, rectFBK);

	//ͼ��
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

	//����
	CString strText = "";//m_stWeather.strTtq.c_str();
	//strText += "\n";
	strText += m_stWeather.strTwd.c_str();

	Gdiplus::Color clBusBkWord = Gdiplus::Color::WhiteSmoke;
	Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

	std::wstring wstrText = Utf8toWchar((char*)(LPCTSTR)strText);

	Gdiplus::PointF pointFBusBkWord((REAL)rc.left, (REAL)rc.top);
	Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width() - nWidth - 5, (REAL)rc.Height()));
	Gdiplus::Font fontBusBkWord(L"������ͤ�ڼ���", 20 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusBkWord(StringFormatFlagsNoWrap);
	stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentFar);
	stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);


	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	gr.DrawString(wstrText.c_str(), wstrText.size(), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);


	CPoint point(0, 0);
	//	pDC->DrawText(strText, rc, 0);
	gr.ReleaseHDC(m_memDC);
	//pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);	//Don't forget to set 
	//the dialog's extent style.
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	CRect rcWin;
	GetWindowRect(&rcWin);
	POINT pointWin = {rcWin.left, rcWin.top};
	GetClientRect(&rc);
	SIZE winSize = {rc.Width(), rc.Height()};
	::UpdateLayeredWindow(this->m_hWnd, pDC->m_hDC, &pointWin, &winSize, m_memDC.m_hDC, &point, 0, &m_Blend, 2);

	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();
	ReleaseDC(pDC);

}

void CWeatherDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(rect);

	// TODO: �ڴ˴������Ϣ����������
}

void CWeatherDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent == 1)
	{
		if(m_nNoGetWeather > 120)
		{
			Alarm(WARNING_NOWEATHER);
		}
		else if(m_nNoGetWeather <= 120)
		{
			m_nNoGetWeather ++;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
