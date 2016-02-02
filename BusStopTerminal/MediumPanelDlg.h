#pragma once
#include "EsayMemDC.h"
#include "Data.h"

// CMediumPanelDlg �Ի���

class CMediumPanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CMediumPanelDlg)

private:
	void InitPannel();
	void DrawBk();
	void DrawLine();
	void ChangeLine();
	void DrawArrive(Gdiplus::Graphics& gr, vector<BusArrivalInfo>& vecArrival, CRect& rc, CData* pData);
	void MoveUpDown(CDC* pDCSrc, CDC* pDCDst, CRect& rc, CDC* pDCSreen);

private:
	vector<CEsayMemDC*> m_vecMemDC;
	vector<CData*> m_vecData;
	vector<Gdiplus::Image*> m_vecImComfort;		//����ӵ���ȵ�ͼ��
	Gdiplus::Image* m_pImLineBk;							//��·�ű���ͼ
	int m_nIndexShow;											//��¼������ʾ�ڼ�����·;
	int m_nCount;													//��¼�ܹ��ж�����Ļ;
	CLock m_lock;													//������·�л��͵�վ���µ�ͬ��

public:
	CMediumPanelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMediumPanelDlg();

	void SetData(const vector<CData*>& vecData)
	{
		m_vecData = vecData;
		int nCount = m_vecData.size() / 4;
		(m_vecData.size() % 4) == 0 ? nCount : nCount++;
		m_nCount = nCount;
	}
	void UpdateArrival();

// �Ի�������
	enum { IDD = IDD_MEDIUMPANEL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
