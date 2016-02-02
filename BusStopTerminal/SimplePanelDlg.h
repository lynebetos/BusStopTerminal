#pragma once
#include "EsayMemDC.h"
#include "Data.h"

// CSimplePanelDlg �Ի���

class CSimplePanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CSimplePanelDlg)

private:
	void InitPannel();
	void DrawBk();
	void DrawLine();
	void DrawArrive(Gdiplus::Graphics& gr, vector<BusArrivalInfo>& vecArrival, CRect& rc, CData* pData);
	void DrawEmpty(Gdiplus::Graphics& gr, CRect& rc);
	void MoveUpDown(CDC* pDCSrc, CDC* pDCDst, CRect& rc, CDC* pDCSreen);
	void ChangeLine();

private:
	vector<CEsayMemDC*> m_vecMemDC;
	vector<CData*> m_vecData;
	int m_nIndexShow;											//��¼������ʾ�ڼ�����·;
	int m_nCount;													//��¼�ܹ��ж�����Ļ;
	Gdiplus::Image* m_pImLineBK;							//��·�ű���
	CLock m_lock;

public:
	CSimplePanelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSimplePanelDlg();

	void SetData(const vector<CData*>& vecData)
	{
		m_vecData = vecData;
		int nCount = m_vecData.size() / 6;
		(m_vecData.size() % 6) == 0 ? nCount : nCount++;
		m_nCount = nCount;
	}
	void UpdateArrival();

// �Ի�������
	enum { IDD = IDD_SIMPLEPANEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
