
// BusStopTerminalDlg.h : ͷ�ļ�
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

// CBusStopTerminalDlg �Ի���
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

	HANDLE m_hRestart; //�����¼����
private:
	bool PanelLayout();			//������������
	bool LayoutVideo(const CRect& rc);			//�����Ƶ
	bool LayoutComplicate(const CRect& rc);	//�������
	bool LayoutNotice(const CRect& rc, DWORD dwCl, int nFontSize);			//����
	bool LayoutWeather(const CRect& rc);		//����
	bool LayoutTime(const CRect& rc);			//ʱ��
	bool LayoutMiddle(const CRect& rc);			//�е����
	bool LayoutSimple(const CRect& rc);			//�����
	bool LayoutPictrue(const CRect& rc);			//ͼƬ�������
	bool InitPlayer();				//��ʼ��������
	bool InitNotice();				//��ʼ������
	bool InitPicture();				//��ʼ��ͼƬ���
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


// ����
public:
	CBusStopTerminalDlg(CWnd* pParent = NULL);	// ��׼���캯��

	void SetMain(CMain* pMain) {m_pMain = pMain;}

// �Ի�������
	enum { IDD = IDD_BUSSTOPTERMINAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
