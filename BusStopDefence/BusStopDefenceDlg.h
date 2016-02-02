
// BusStopDefenceDlg.h : 头文件
//

#pragma once


// CBusStopDefenceDlg 对话框
class CBusStopDefenceDlg : public CDialog
{
// 构造
public:
	CBusStopDefenceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BUSSTOPDEFENCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nId; //最小化托盘程序属性
	CString m_strMinShow;	//最小化托盘时的显示文字
	CMenu m_menu;				//最小化托盘时的右键菜单

	bool m_bRun;					//检测站牌的线程标志
	PROCESS_INFORMATION m_procStruct;	//站牌终端进程属性
	HANDLE m_hRun;		//站牌检测线程句柄
	bool m_bStopQuit;	//站牌是否要退出

	HANDLE m_hMapping; //共享内存句柄
	HANDLE m_hRead; //共享内存读取线程
	bool m_bRead;
	HANDLE m_hEventRead; //共享内存读取事件

	HANDLE m_hUpdate; //更新线程句柄
	bool m_bUpdate;

	HANDLE m_hEventRestart; //重启站牌事件

	HANDLE m_hEventShut; //终端关闭事件

	CMailSend m_MailSender; //邮件发送组件

private:
	static unsigned int _stdcall threadKeepBusstop(LPVOID lpvoid);
	void KeepBusstopRun();

	static unsigned int _stdcall threadReadMapping(LPVOID lpvoid);
	void ReadMapping();

	static unsigned int _stdcall threadUpdate(LPVOID lpvoid);
	void UpdateSoftware();

	void InitDefence();
	bool IsStopProcessRun(); //判断站牌进程是否存在
	bool CreateShardMemery(); //创建共享内存和消息处理


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnShowTask(WPARAM wparam, LPARAM lparam);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnShutApp(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedButtonCloseapp();
};
