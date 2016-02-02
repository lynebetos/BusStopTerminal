#include "StdAfx.h"
#include "Data.h"

//CDataLock
CData::CData()
{
	m_strLineName = "";
	m_strLineNumber = "";
	m_bDoubleLine = FALSE;
	m_fChangeY = 0.0;
	m_nChange = 0;
}

CData::~CData(void)
{
	m_vecDLineName.clear();
	m_vecPos.clear();
	m_vecStation.clear();
	m_vecBusArrivalInfo.clear();
	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();
}

void CData::SetStationData(__in const vector<CString>& vecStation, 
					__in const CString& strLineNumber, 
					__in const CString& strLineInfo,
					__in const CString& strStartTime,
					__in const CString& strEndTime,
					__in const CString& strCard,
					__in const CString& strTicket,
					__in const CString& strMonTicket)
{
	m_vecStation = vecStation;
	m_strLineNumber = strLineNumber;
	m_strStartTime = strStartTime;
	m_strEndTime = strEndTime;
	m_strCard = strCard;
	m_strticket = strTicket;
	m_strMonTicket = strMonTicket;

}

void CData::SetMemDc(CDC* pDC, int nWidth, int nHeight)
{
	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();

	m_memDC.CreateCompatibleDC(pDC);
	m_memBitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	m_memDC.SelectObject(m_memBitmap);

}

void CData::UpdateBusInfo(__in const tagArrival& refArrival)
{
	m_lockArri.Lock();
	m_vecBusArrivalInfo.clear();
	m_vecBusArrivalInfo = refArrival.vecArrivalInfo;
	m_lockArri.UnLock();
}