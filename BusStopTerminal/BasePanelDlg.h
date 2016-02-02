#pragma once
#include <math.h>
#include <GdiPlus.h>
#include "Data.h"

// CBasePanelDlg �Ի���
using namespace Gdiplus;

class CBasePanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CBasePanelDlg)

public:
	CBasePanelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasePanelDlg();

public:
	void SetData(__in CData* pData);
	void UpdateArrival();
	void MovePix(int nMovePix, CData* pDataNow, CData* pDataNext);
	CData* GetDataNow();
	CData* GetDataNext();
	void MoveDataToNext() 
	{
		if(2 <= (int)m_dataLine.size())
			m_nCur++;

		if((m_nCur) >= (int)m_dataLine.size())
			m_nCur = 0;
	};

private:
	vector<CData*> m_dataLine;
	vector<Gdiplus::Image*> m_vecimBusIcon;
	vector<Gdiplus::Image*> m_vecimBusStatus;
	int m_nCur;		//��ǰdata��vector�е�����
	HANDLE m_hEvent;	//�л�ҳ��֪ͨ
	HANDLE m_hPage;	//�л�ҳ���߳̾��
	HANDLE m_hWord;	//�л�վ�������߳̾��
	//BOOL m_bChengePage; //�л�ҳ����
	BOOL m_bRun; //�߳̿���
	BOOL m_bStation; //�߳̿���
	BOOL m_bDLine; //��ʾ����
	Gdiplus::PointF m_pointFBusBK;			//������վ��������ԭ��
	Gdiplus::PointF m_pointFRangeStart;		//�׳�������ԭ��
	Gdiplus::PointF m_pointFRangeEnd;		//ĩ��������ԭ��
	Gdiplus::PointF m_pointFRangeCard;		//ˢ��������ԭ��
	Gdiplus::PointF m_pointFRangeTicket;	//��Ʊ������ԭ��
	Gdiplus::REAL m_nLineBkWidth; //��·������� ��������λ�ò���
	Gdiplus::REAL m_nDisBkWidth; //��վ������� ��������λ�ò���
	CLock m_lock;

private:
	void InitImage();
	vector<CString> OutDoubleLine(const CString& strLine);

	//���ƽ���
	void DrawInMem(CData* pData);	//������վ�ƻ��Ƶ��ڴ���
	void DrawBackground(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CData* pData); //����
	void DrawLinePenel(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CDC& memDC, CData* pData); //��·�������
	void DrawStation(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CData* pData); //վ��
	int DrawChangeStationWord(CData* pData, BOOL bUp, Gdiplus::REAL& nChangeY);	//վ�����ƹ������л�(����) 0:�л���ɣ� 1:�л�δ��� -1:�л�ʧ��
	int DrawChangeStationWord(CData* pData, int nChange);//վ�����ƹ������л������ң� 0:�л���ɣ� 1:�л�δ��� -1:�л�ʧ��

	//���ƶ�̬����
	void DrawDynimacData(CData* pData);
	void DrawBusIcon(CData* pData);
	void DrawBusStatus(CData* pData);

	//�����л�
	static unsigned int _stdcall threadChangePage(LPVOID lpvoid);//�л��߳�
	void HandlingChangePage();
	static unsigned int _stdcall threadStationWord(LPVOID lpvoid);//�л��߳�
	void HandlingStationWord();

// �Ի�������
	enum { IDD = IDD_BASEPANEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
};
