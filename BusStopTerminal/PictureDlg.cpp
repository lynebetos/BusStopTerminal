// PictureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "PictureDlg.h"


// CPictureDlg 对话框

IMPLEMENT_DYNAMIC(CPictureDlg, CDialog)

CPictureDlg::CPictureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPictureDlg::IDD, pParent)
{
	m_bRun = false;
	m_hPice = INVALID_HANDLE_VALUE;
}

CPictureDlg::~CPictureDlg()
{
	m_vecPircPath.clear();
}

void CPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPictureDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPictureDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPictureDlg::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPictureDlg 消息处理程序

void CPictureDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CPictureDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

BOOL CPictureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	unsigned int nThread;
	m_bRun = true;
	m_hPice = (HANDLE)_beginthreadex(NULL,0,  threadPic, (LPVOID)this, 0, &nThread);
	if(m_hPice == INVALID_HANDLE_VALUE)
	{
		//创建图片线程失败
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPictureDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

}

unsigned int _stdcall CPictureDlg::threadPic(LPVOID lpvoid)
{
	CPictureDlg* pDlg = (CPictureDlg*)lpvoid;
	if(pDlg)
	{
		pDlg->ProcPic();
	}

	return 0;
}

void CPictureDlg::ProcPic()
{
	LogObject;
	CDC* pDC = GetDC();


	while(m_bRun)
	{
		if(0 >= (int)m_vecPircPath.size() )
		{
			Sleep(1000);
			continue;
		}

		for(int ii = 0; ii < (int)m_vecPircPath.size()&&m_bRun; ii++)
		{
			string strOut = "图片文件路径:";
			strOut += m_vecPircPath[ii];
			LogOutInfo(strOut.c_str());

			CRect rc;
			GetClientRect(&rc);

			CEsayMemDC memDC(*pDC, rc);
			Gdiplus::Graphics gr(memDC.GetDC());

			string& strPath = m_vecPircPath[ii];
			wstring wstrPath = c2w(strPath.c_str());
			Gdiplus::Image im(wstrPath.c_str());

			Gdiplus::RectF rectFPic((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height());
			gr.DrawImage(&im, rectFPic);
			memDC.BltMem(*pDC);

			Sleep(20000);
		}
	}

	ReleaseDC(pDC);

}

void CPictureDlg::OnDestroy()
{
	CDialog::OnDestroy();
	m_bRun = false;
	WaitForSingleObject(m_hPice, INFINITE);
	// TODO: 在此处添加消息处理程序代码
}
