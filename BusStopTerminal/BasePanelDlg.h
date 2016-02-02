#pragma once
#include <math.h>
#include <GdiPlus.h>
#include "Data.h"

// CBasePanelDlg 对话框
using namespace Gdiplus;

class CBasePanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CBasePanelDlg)

public:
	CBasePanelDlg(CWnd* pParent = NULL);   // 标准构造函数
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
	int m_nCur;		//当前data在vector中的索引
	HANDLE m_hEvent;	//切换页面通知
	HANDLE m_hPage;	//切换页面线程句柄
	HANDLE m_hWord;	//切换站点文字线程句柄
	//BOOL m_bChengePage; //切换页面量
	BOOL m_bRun; //线程控制
	BOOL m_bStation; //线程控制
	BOOL m_bDLine; //标示两行
	Gdiplus::PointF m_pointFBusBK;			//公车到站背景坐标原点
	Gdiplus::PointF m_pointFRangeStart;		//首车框坐标原点
	Gdiplus::PointF m_pointFRangeEnd;		//末车框坐标原点
	Gdiplus::PointF m_pointFRangeCard;		//刷卡框坐标原点
	Gdiplus::PointF m_pointFRangeTicket;	//月票框坐标原点
	Gdiplus::REAL m_nLineBkWidth; //线路背景宽度 所有面板的位置参照
	Gdiplus::REAL m_nDisBkWidth; //到站背景宽度 所有面板的位置参照
	CLock m_lock;

private:
	void InitImage();
	vector<CString> OutDoubleLine(const CString& strLine);

	//绘制界面
	void DrawInMem(CData* pData);	//将各自站牌绘制到内存中
	void DrawBackground(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CData* pData); //背景
	void DrawLinePenel(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CDC& memDC, CData* pData); //线路名称面板
	void DrawStation(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CData* pData); //站点
	int DrawChangeStationWord(CData* pData, BOOL bUp, Gdiplus::REAL& nChangeY);	//站点名称过长的切换(上下) 0:切换完成， 1:切换未完成 -1:切换失败
	int DrawChangeStationWord(CData* pData, int nChange);//站点名称过长的切换（左右） 0:切换完成， 1:切换未完成 -1:切换失败

	//绘制动态数据
	void DrawDynimacData(CData* pData);
	void DrawBusIcon(CData* pData);
	void DrawBusStatus(CData* pData);

	//界面切换
	static unsigned int _stdcall threadChangePage(LPVOID lpvoid);//切换线程
	void HandlingChangePage();
	static unsigned int _stdcall threadStationWord(LPVOID lpvoid);//切换线程
	void HandlingStationWord();

// 对话框数据
	enum { IDD = IDD_BASEPANEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
