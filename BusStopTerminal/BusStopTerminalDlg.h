
// BusStopTerminalDlg.h : 头文件
//

#pragma once

#include "Main.h"
#include "PlayerWnd.h"
#include "BasePanelDlg.h"
#include "NoticeDlg.h"
#include "TimeDlg.h"
#include "WeatherDlg.h"
#include "MediumPanelDlg.h"
#include "SimplePanelDlg.h"
#include "PictureDlg.h"

// CBusStopTerminalDlg 对话框
class CBusStopTerminalDlg : public CDialog
{
private:
	CMain* m_pMain;
	CPlayerWnd m_wndPlayer;
	vector<CBasePanelDlg*> m_vecwndLinePanel;
	CNoticeDlg* m_pwndNotic;
	CTimeDlg* m_pwndTime;
	CWeatherDlg* m_pwndWeather;
	CMediumPanelDlg* m_pwndMediumPanel;
	CSimplePanelDlg* m_pwndSimplePanelDlg;
	CPictureDlg* m_pwndPictureDlg;
	BOOL m_bChange;
	int m_nChangeCount;
	int m_nPannelCount;

	HANDLE m_hRestart; //重启事件句柄
private:
	bool PanelLayout();			//布局整个界面
	bool LayoutVideo(const CRect& rc);			//广告视频
	bool LayoutComplicate(const CRect& rc);	//完整面板
	bool LayoutNotice(const CRect& rc, DWORD dwCl, int nFontSize);			//公告
	bool LayoutWeather(const CRect& rc);		//天气
	bool LayoutTime(const CRect& rc);			//时间
	bool LayoutMiddle(const CRect& rc);			//中等面板
	bool LayoutSimple(const CRect& rc);			//简单面板
	bool LayoutPictrue(const CRect& rc);			//图片播放面板
	bool InitPlayer();				//初始化播放器
	bool InitNotice();				//初始化公告
	bool InitPicture();				//初始化图片广告
	bool LinkDatePanel();
	void HideTaskBar(bool bHide);

	inline CRect toRect(const CPanelSize& size)
	{
		CRect rc;
		rc.top = size.top;
		rc.bottom = size.bottom;
		rc.left = size.left;
		rc.right = size.right;

		return rc;
	}

	CMain* GetMain() {return m_pMain;}
	void CreateEventRestart()
	{
		m_hRestart = ::OpenEvent(NULL, FALSE, EVENT_RESTART_APP);
	}

	bool WaitEvent()
	{
		DWORD dwWait = ::WaitForSingleObject(m_hRestart, 2000);
		if(dwWait == WAIT_OBJECT_0)
		{
			::ResetEvent(m_hRestart);
			return true;
		}

		return false;
	}

	static unsigned int _stdcall ProcDoConn(LPVOID lpvoid);
	static unsigned int _stdcall ProcShutDown(LPVOID lpvoid);


// 构造
public:
	CBusStopTerminalDlg(CWnd* pParent = NULL);	// 标准构造函数

	void SetMain(CMain* pMain) {m_pMain = pMain;}

// 对话框数据
	enum { IDD = IDD_BUSSTOPTERMINAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnReloadPlaylist(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnReloadNotice(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnWeather(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnArrival(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnCallStatistic(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnSetWarning(WPARAM wparam, LPARAM lparam);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};
