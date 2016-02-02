#pragma once
#include "afxwin.h"
#include <GdiPlus.h>

using namespace Gdiplus;


// CNoticeDlg 对话框

class CNoticeDlg : public CDialog
{
	DECLARE_DYNAMIC(CNoticeDlg)

public:
	CNoticeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNoticeDlg();

	void SetNoticeShowAttrib(Gdiplus::Color cl, int nFontPix)
	{
		m_cl = cl;
		m_nFontPix = nFontPix;
	}

	void SetList(vector<_st_NoticeList>& vecList)
	{
		m_lock.Lock();
		m_vecList.assign(vecList.begin(), vecList.end());
		m_bUpdate = true;
		m_lock.UnLock();
	}

	void SetRect(const CRect& rc)
	{
		m_rc = rc;
	}

private:
	REAL m_xOffset;
	vector<_st_NoticeList> m_vecList;
	bool m_bNoticeRun;
	HANDLE m_hNotice;
	CString m_strText; //公告内容
	bool m_bOver;	//单条公告播放完成的标志
	Gdiplus::Color m_cl;
	int m_nFontPix;
	CLock m_lock;
	bool m_bUpdate;
	BLENDFUNCTION m_Blend;
	CRect m_rc;

private:
	bool InitNoticePlay();
	void DrawNotice();

	static unsigned int _stdcall threadNoticeCtrl(LPVOID lpvoid);

	void ProcNoticeCtrl();

// 对话框数据
	enum { IDD = IDD_NOTICE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
