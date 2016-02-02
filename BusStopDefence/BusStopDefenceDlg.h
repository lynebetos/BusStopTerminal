
// BusStopDefenceDlg.h : ͷ�ļ�
//

#pragma once


// CBusStopDefenceDlg �Ի���
class CBusStopDefenceDlg : public CDialog
{
// ����
public:
	CBusStopDefenceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BUSSTOPDEFENCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nId; //��С�����̳�������
	CString m_strMinShow;	//��С������ʱ����ʾ����
	CMenu m_menu;				//��С������ʱ���Ҽ��˵�

	bool m_bRun;					//���վ�Ƶ��̱߳�־
	PROCESS_INFORMATION m_procStruct;	//վ���ն˽�������
	HANDLE m_hRun;		//վ�Ƽ���߳̾��
	bool m_bStopQuit;	//վ���Ƿ�Ҫ�˳�

	HANDLE m_hMapping; //�����ڴ���
	HANDLE m_hRead; //�����ڴ��ȡ�߳�
	bool m_bRead;
	HANDLE m_hEventRead; //�����ڴ��ȡ�¼�

	HANDLE m_hUpdate; //�����߳̾��
	bool m_bUpdate;

	HANDLE m_hEventRestart; //����վ���¼�

	HANDLE m_hEventShut; //�ն˹ر��¼�

	CMailSend m_MailSender; //�ʼ��������

private:
	static unsigned int _stdcall threadKeepBusstop(LPVOID lpvoid);
	void KeepBusstopRun();

	static unsigned int _stdcall threadReadMapping(LPVOID lpvoid);
	void ReadMapping();

	static unsigned int _stdcall threadUpdate(LPVOID lpvoid);
	void UpdateSoftware();

	void InitDefence();
	bool IsStopProcessRun(); //�ж�վ�ƽ����Ƿ����
	bool CreateShardMemery(); //���������ڴ����Ϣ����


	// ���ɵ���Ϣӳ�亯��
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
