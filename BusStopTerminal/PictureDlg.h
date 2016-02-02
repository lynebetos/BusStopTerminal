#pragma once
#include "EsayMemDC.h"

// CPictureDlg �Ի���

class CPictureDlg : public CDialog
{
	DECLARE_DYNAMIC(CPictureDlg)

private:
	vector<string> m_vecPircPath;
	HANDLE m_hPice;
	bool m_bRun;

private:
	static unsigned int _stdcall threadPic(LPVOID lpvoid);

	void ProcPic();

public:
	CPictureDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPictureDlg();

	void SetPicPath(vector<string>& vecPath)
	{
		m_vecPircPath = vecPath;
	}
// �Ի�������
	enum { IDD = IDD_PICTRUE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
};
