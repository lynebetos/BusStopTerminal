#pragma once

#include <GdiPlus.h>

using namespace Gdiplus;

// CNoticeStatic

class CNoticeStatic : public CStatic
{
	DECLARE_DYNAMIC(CNoticeStatic)

public:
	CNoticeStatic();
	virtual ~CNoticeStatic();

	void SetList(vector<_st_NoticeList>& vecList)
	{
		m_lock.Lock();
		m_vecList = vecList;
		m_lock.UnLock();
	}
	bool InitNoticePlay();

private:
	REAL m_xOffset;
	vector<_st_NoticeList> m_vecList;
	bool m_bNoticeRun;
	HANDLE m_hNotice;
	CString m_strText; //公告内容
	bool m_bOver;	//单条公告播放完成的标志
	CLock m_lock;

private:
	void DrawNotice();

	static unsigned int _stdcall threadNoticeCtrl(LPVOID lpvoid);

	void ProcNoticeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


