// PictureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "PictureDlg.h"


// CPictureDlg �Ի���

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


// CPictureDlg ��Ϣ�������

void CPictureDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CPictureDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

BOOL CPictureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	unsigned int nThread;
	m_bRun = true;
	m_hPice = (HANDLE)_beginthreadex(NULL,0,  threadPic, (LPVOID)this, 0, &nThread);
	if(m_hPice == INVALID_HANDLE_VALUE)
	{
		//����ͼƬ�߳�ʧ��
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPictureDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

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
			string strOut = "ͼƬ�ļ�·��:";
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
	// TODO: �ڴ˴������Ϣ����������
}
