#pragma once
#include "AVPlayer.h"

// CPlayerWnd

class CPlayerWnd : public CStatic
{
	DECLARE_DYNAMIC(CPlayerWnd)

public:
	CPlayerWnd();
	virtual ~CPlayerWnd();

	void PlayVideo(CString strFile);
	void SetList(vector<_st_PlayList>& vecList)
	{
		m_lock.Lock();
		m_vecList = vecList;
		m_bUpdate = true;
		m_lock.UnLock();
	}
	void SetCallBack(WPARAM wparam)
	{
		m_lpfSetStatistic = (LPFSETSTATISTIC)wparam;
	}

private:
	void InitPlayThread();
	static unsigned int _stdcall threadPlayCtl(LPVOID lpvoid);

	void ProcPlayCtrl();

	void DrawPic();
	void PlayStat(string strPathName, string strDate, string strStart, string strEnd);
	void Alarm(DWORD dwWarCode)
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_SETWARNING, (WPARAM)dwWarCode);
	}

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

private:
	CAVPlayer m_avp;
	vector<_st_PlayList> m_vecList;
	bool m_bPlayerRun;
	HANDLE m_hPlayer;
	bool m_bUpdate;
	CLock m_lock;
	LPFSETSTATISTIC m_lpfSetStatistic;

public:
	virtual BOOL DestroyWindow();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnPaint();
};


