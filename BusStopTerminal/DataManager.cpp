#include "StdAfx.h"
#include "DataManager.h"
#include "ConfigManager.h"

CDataManager* CDataManager::m_pInstance = NULL;

CDataManager::CDataManager(void)
{
}

CDataManager::~CDataManager(void)
{
	m_setLineNum.clear();
	m_vecLineNum.clear();
	m_mapLineID.clear();

	map<string, CData*>::iterator theiter = m_mapData.begin();
	for (theiter; theiter != m_mapData.end(); theiter++)
	{
		CData* pData = theiter->second;
		if(pData)
			delete pData;
	}

	m_mapData.clear();
}

CDataManager* CDataManager::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CDataManager;
	}

	return m_pInstance;
}

void CDataManager::ReleaseInstance()
{
	if(m_pInstance)
		delete m_pInstance;
}

bool CDataManager::InitData(const vector<_st_Linestop>& refLinestop, string strCurStopId)
{
	string strCurStopID = strCurStopId;
	int nCurStopPos = 0;

	if(!m_mapData.size())
	{
		for(vector<_st_Linestop>::const_iterator veciter = refLinestop.begin(); veciter != refLinestop.end(); veciter++)
		{
			
			CData* pData = new CData;
			vector<CString> vecStation;
			for (int ii = 0; ii < (int)veciter->listStop.size(); ii++)
			{
				CString strStationName = veciter->listStop[ii].strName.c_str();
				vecStation.push_back(strStationName);

				if(veciter->listStop[ii].strID == strCurStopID)
				{
					nCurStopPos = vecStation.size() - 1;
				}
			}
			CString strLineNum = veciter->strLineName.c_str();
			pData->SetStationData(vecStation, 
				strLineNum, 
				"", 
				veciter->strTimef.c_str(), 
				veciter->strTimeL.c_str(),
				veciter->strCard.c_str(),
				veciter->strTicket.c_str(),
				veciter->strMonTicket.c_str());
			pData->SetCurStationPos(nCurStopPos);

			m_mapData.insert(make_pair(veciter->strID, pData));
			m_vecLineNum.push_back(veciter->strLineName);
			m_mapLineID.insert(make_pair(veciter->strLineName, veciter->strID));
			int nSet = atoi(veciter->strLineName.c_str());
			m_setLineNum.insert(nSet);
			vecStation.clear();

		}
	}
	else
	{
		return false;
	}

	return true;
}

bool CDataManager::GetAllData(vector<CData*>& refDataPoint)
{

	map<string, string>::iterator mapiter = m_mapLineID.begin();
	map<string, CData*>::iterator mapiterData = m_mapData.begin();
	/*
	for (int ii = 0; ii < (int)m_vecLineNum.size(); ii++)
	{
		string strLineNum = m_vecLineNum[ii];
		mapiter = m_mapLineID.find(strLineNum);
		if(mapiter != m_mapLineID.end())
		{
			mapiterData = m_mapData.find(mapiter->second);
			if(mapiterData != m_mapData.end())
			{
				refDataPoint.push_back(mapiterData->second);
			}
		}
	}

	if(!refDataPoint.size())
		return false;
	*/

	for (set<int>::iterator setIter = m_setLineNum.begin(); setIter != m_setLineNum.end(); setIter++)
	{
		int nSet = *setIter;
		char szLineNum[8] = {0};
		sprintf(szLineNum, "%d", nSet);
		string strLineNum = szLineNum;
		mapiter = m_mapLineID.find(strLineNum);
		if(mapiter != m_mapLineID.end())
		{
			mapiterData = m_mapData.find(mapiter->second);
			if(mapiterData != m_mapData.end())
			{
				refDataPoint.push_back(mapiterData->second);
			}
		}

	}
	return true;
}

bool CDataManager::SetArrivalData(const tagArrival& stArrival)
{
	string strLineID = stArrival.strLineID;
	map<string, CData*>::iterator mapiter = m_mapData.find(strLineID);
	if(mapiter != m_mapData.end())
	{
		CData* pData = mapiter->second;
		pData->UpdateBusInfo(stArrival);
	}

	return true;
}

void CDataManager::CleanArrivalData()
{
	for(map<string, CData*>::iterator mapiter = m_mapData.begin(); mapiter != m_mapData.end(); mapiter++)
	{
		tagArrival stArrival;
		CData* pData = mapiter->second;
		pData->UpdateBusInfo(stArrival);
	}
}