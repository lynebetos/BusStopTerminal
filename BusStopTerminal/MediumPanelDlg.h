#pragma once
#include "EsayMemDC.h"
#include "Data.h"

// CMediumPanelDlg 对话框

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
	vector<Gdiplus::Image*> m_vecImComfort;		//保存拥挤度的图标
	Gdiplus::Image* m_pImLineBk;							//线路号背景图
	int m_nIndexShow;											//记录现在显示第几屏线路;
	int m_nCount;													//记录总共有多少屏幕;
	CLock m_lock;													//保持线路切换和到站更新的同步

public:
	CMediumPanelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMediumPanelDlg();

	void SetData(const vector<CData*>& vecData)
	{
		m_vecData = vecData;
		int nCount = m_vecData.size() / 4;
		(m_vecData.size() % 4) == 0 ? nCount : nCount++;
		m_nCount = nCount;
	}
	void UpdateArrival();

// 对话框数据
	enum { IDD = IDD_MEDIUMPANEL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
