#pragma once

class CTransToWnd
{
public:
	CTransToWnd(void);
	~CTransToWnd(void);

	static void SendReloadPlayList();
	static void SendReloadNotice();
	static bool SendWeather(tagWeather* pParam);
	static void SendArrival();
	static void SendStatisticCallback(LPFSETSTATISTIC lpFun);
};
