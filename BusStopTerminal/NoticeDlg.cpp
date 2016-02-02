// NoticeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "NoticeDlg.h"


// CNoticeDlg �Ի���

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


// CNoticeDlg ��Ϣ�������

BOOL CNoticeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//�Ǵ���͸���Ĵ���
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CNoticeDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CNoticeDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnCancel();
}

HBRUSH CNoticeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
// 	if(pWnd->GetDlgCtrlID() == IDC_STATIC_NOTICE)
// 	{
// 		//����static�ı���͸��
// 		pDC->SetBkMode(TRANSPARENT);
// 	}
	return    (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CNoticeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
// 	CRect rect;
// 	GetClientRect(rect);
// 	dc.FillSolidRect(rect,RGB(255,255,255)); 

	DrawNotice();
}

void CNoticeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnTimer(nIDEvent);
}

int CNoticeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

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
	// TODO: �ڴ˴������Ϣ����������
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
	//CString strText = _T("�ػʵ���վ��·��ʱͨ����Ϊ����������У��ҹ�˾�ƻ���2013��12��5�գ��ָ�����6·��32·��34·������·����ͬʱȡ���ĵ�������վ�������ӻ�վ������·");

	Gdiplus::Color clBusBkWord = m_cl;
	Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, 255, 255));
	Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);

	std::wstring wstrText = Utf8toWchar((char*)(LPCTSTR)strText);

	Gdiplus::PointF pointFBusBkWord((REAL)rc.right + m_xOffset, (REAL)rc.top);
	Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF((REAL)rc.Width() * 3, (REAL)rc.Height()));
	Gdiplus::Font fontBusBkWord(L"������ͤ�ڼ���", (REAL)m_nFontPix , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
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
		//������������߳�ʧ��
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

		for (nPlayListIndex = 0; nPlayListIndex < (int)vecNotic.size(); nPlayListIndex++)	//����б��е�Ѱ�ҷ��ϵ�ǰʱ����б� ����ʱ��������һ���б��
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
			//����û���б�����
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

	// TODO: �ڴ˴������Ϣ����������
	m_bNoticeRun = false;
	WaitForSingleObject(m_hNotice, 2000);
}
