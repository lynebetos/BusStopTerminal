#include "StdAfx.h"
#include "TransToWnd.h"
#include "BusStopTerminal.h"

CTransToWnd::CTransToWnd(void)
{
}

CTransToWnd::~CTransToWnd(void)
{
}

void CTransToWnd::SendReloadNotice()
{
	if(theApp.GetMainWnd()->GetSafeHwnd())
		::SendMessage(theApp.GetMainWnd()->GetSafeHwnd(), WM_RELOADNOTIC, NULL, NULL);
}

void CTransToWnd::SendReloadPlayList()
{
	if(theApp.GetMainWnd()->GetSafeHwnd())
		::SendMessage(theApp.GetMainWnd()->GetSafeHwnd(), WM_RELOADPLAYLIST, NULL, NULL);
}

bool CTransToWnd::SendWeather(tagWeather* pParam)
{
	if(theApp.GetMainWnd()->GetSafeHwnd())
		::SendMessage(theApp.GetMainWnd()->GetSafeHwnd(), WM_WEATHER, (WPARAM)pParam, NULL);
	else
		return false;

	return true;
}

void CTransToWnd::SendArrival()
{
	if(theApp.GetMainWnd()->GetSafeHwnd())
		::SendMessage(theApp.GetMainWnd()->GetSafeHwnd(), WM_ARRIVAL, NULL, NULL);
}

void CTransToWnd::SendStatisticCallback(LPFSETSTATISTIC lpFun)
{
	if(theApp.GetMainWnd()->GetSafeHwnd())
		::SendMessage(theApp.GetMainWnd()->GetSafeHwnd(), WM_CALLSTATISTIC, (WPARAM)lpFun, NULL);
}
