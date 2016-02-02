#pragma once
#include "afxwin.h"

// CWeatherDlg 对话框

class CWeatherDlg : public CDialog
{
	DECLARE_DYNAMIC(CWeatherDlg)

public:
	CWeatherDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWeatherDlg();

	void SetWeather(_st_weather& stWeather);
// 对话框数据
	enum { IDD = IDD_WEATHER_DIALOG };

private:
	_st_weather m_stWeather;
	BLENDFUNCTION m_Blend;
	int m_nNoGetWeather;

	void Alarm(DWORD dwWarCode)
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_SETWARNING, (WPARAM)dwWarCode);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
