#pragma once
#include <vector>

// CTimeStatic

class CTimeStatic : public CStatic
{
	DECLARE_DYNAMIC(CTimeStatic)

public:
	CTimeStatic();
	virtual ~CTimeStatic();

	void Init();

private:
	vector<string> m_vecWeekDay;
	bool m_bTimeRun;
	HANDLE m_hTime;

private:
 	static unsigned int _stdcall theadTime(LPVOID lpvoid);
	void ProcTime();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};


