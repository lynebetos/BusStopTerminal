// TimeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "TimeDlg.h"
#include <GdiPlus.h>

using namespace Gdiplus;
// CTimeDlg �Ի���

IMPLEMENT_DYNAMIC(CTimeDlg, CDialog)

CTimeDlg::CTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeDlg::IDD, pParent)
{
	m_vecWeekDay.push_back("��");
	m_vecWeekDay.push_back("һ");
	m_vecWeekDay.push_back("��");
	m_vecWeekDay.push_back("��");
	m_vecWeekDay.push_back("��");
	m_vecWeekDay.push_back("��");
	m_vecWeekDay.push_back("��");

	m_bTimeRun = false;
	m_hTime = INVALID_HANDLE_VALUE;

}

CTimeDlg::~CTimeDlg()
{
	m_vecWeekDay.clear();
}

void CTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_TIME, m_wndTimeStatic);
}


BEGIN_MESSAGE_MAP(CTimeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTimeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTimeDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTimeDlg ��Ϣ�������

void CTimeDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CTimeDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnCancel();
}

BOOL CTimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	COLORREF maskColor = RGB(255,255,255);
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	HINSTANCE hInst=LoadLibrary(_T("User32.DLL"));
	if(hInst)
	{
// 		typedef BOOL (WINAPI * MYFUNC)(HWND , COLORREF,BYTE,DWORD);
// 		MYFUNC fun=NULL;
// 		fun=(MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes");
		//if(fun)fun(this->GetSafeHwnd(),maskColor,255,1);
		FreeLibrary(hInst);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	m_bTimeRun = true;
	unsigned int nThreadID = 0;
	m_hTime = (HANDLE)_beginthreadex(NULL, 0, theadTime, (LPVOID)this, 0, &nThreadID);
	if(m_hTime == INVALID_HANDLE_VALUE)
	{
		//ʱ���߳�����ʧ��
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

HBRUSH CTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
// 	if(pWnd->GetDlgCtrlID() == IDC_STATIC_TIME)
// 	{
// 		//����static�ı���͸��
// 		pDC->SetBkMode(TRANSPARENT);
// 	}
// 	return    (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CTimeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,RGB(255,255,255)); 
}

void CTimeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(rect);

	// TODO: �ڴ˴������Ϣ����������
}

void CTimeDlg::ProcTime()
{
	LogObject;
	CDC* pDC = GetDC();
	while(m_bTimeRun)
	{

		CRect rc;
		GetClientRect(&rc);

		m_Blend.SourceConstantAlpha = int(255);

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
		strTextDate.Format("%d��%d�� ����%s", systime.wMonth, systime.wDay, m_vecWeekDay[systime.wDayOfWeek].c_str());

		Gdiplus::Color clBusBkWord = Gdiplus::Color::WhiteSmoke;
		Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, 255, 255));
		Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

		//��ʾʱ��
		WCHAR wcTextTime[32] = {0};
		MultiByteToWideChar(CP_ACP, 0, strTextTime, strTextTime.GetLength(), wcTextTime, (strTextTime.GetLength() + 1)*2);

		Gdiplus::PointF pointFBusBkWord((REAL)rc.left, (REAL)rc.top);
		Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width(), (REAL)rc.Height() / 2));
		Gdiplus::Font fontBusBkWord(L"������ͤ�ڼ���", 22 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatBusBkWord(StringFormatFlagsNoWrap);
		stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		//��ʾ����
		WCHAR wcTextDate[32] = {0};
		MultiByteToWideChar(CP_ACP, 0, strTextDate, strTextDate.GetLength(), wcTextDate, (strTextDate.GetLength() + 1)*2);

		Gdiplus::PointF pointFDate((REAL)rc.left, (REAL)rc.top + (REAL)rc.Height() / 2);
		Gdiplus::RectF rectDate(pointFDate, Gdiplus::SizeF((REAL)rc.Width(), (REAL)rc.Height() / 2));
		Gdiplus::Font fontDate(L"������ͤ�ڼ���", 16 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::Pen pen(clBusBkWord);
		gr.SetSmoothingMode(SmoothingModeAntiAlias);
		gr.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		//gr.FillRectangle(&brushBK, rectBusBkWord);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
// 		Gdiplus::GraphicsPath path;
// 		path.AddString(wcTextTime, wcslen(wcTextTime), &fontFamily, Gdiplus::FontStyleRegular, 22, rectBusBkWord, &stringFormatBusBkWord);
// 		path.AddString(wcTextDate, wcslen(wcTextDate), &fontFamily, Gdiplus::FontStyleRegular, 16, rectDate, &stringFormatBusBkWord);
// 		gr.DrawPath(&pen, &path);
// 		gr.FillPath(&brushBusBkWord, &path);

		gr.DrawString(wcTextTime, wcslen(wcTextTime), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);

		//gr.FillRectangle(&brushBK, rectDate);
		gr.DrawString(wcTextDate, wcslen(wcTextDate), &fontDate, rectDate, &stringFormatBusBkWord, &brushBusBkWord);

		//	pDC->DrawText(strText, rc, 0);

		gr.ReleaseHDC(m_memDC);
		//pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
		CPoint point(0, 0);

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

		Sleep(1000);
	}
	ReleaseDC(pDC);

}

unsigned int _stdcall CTimeDlg::theadTime(LPVOID lpvoid)
{
	CTimeDlg* pWnd = (CTimeDlg*)lpvoid;
	if(pWnd)
	{
		pWnd->ProcTime();
	}

	return 0;
}

void CTimeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_bTimeRun = false;
	WaitForSingleObject(m_hTime, 2000);

	// TODO: �ڴ˴������Ϣ����������
}
