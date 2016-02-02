#pragma once
#include "afxwin.h"


// CTimeDlg �Ի���

class CTimeDlg : public CDialog
{
	DECLARE_DYNAMIC(CTimeDlg)

public:
	CTimeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTimeDlg();

// �Ի�������
	enum { IDD = IDD_TIME_DIALOG };
	//CTimeStatic m_wndTimeStatic;

private:
	vector<string> m_vecWeekDay;
	bool m_bTimeRun;
	HANDLE m_hTime;
	BLENDFUNCTION m_Blend;

private:
	static unsigned int _stdcall theadTime(LPVOID lpvoid);
	void ProcTime();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
