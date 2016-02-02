#include "StdAfx.h"
#include "StatisticFunction.h"
#include "TransToWnd.h"
#include "NetManager.h"
#include "ConfigManager.h"

CStatisticFunction* CStatisticFunction::m_pInstance = NULL;

CStatisticFunction::CStatisticFunction(void)
: m_bRun(false)
{
}

CStatisticFunction::~CStatisticFunction(void)
{
	LogObject;
	m_bRun = false;
	if(WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0)
	{
		TerminateThread(m_hThread, 0);
	}
	LogOutInfo("清除统计模块");
}

bool CStatisticFunction::InitFunction()
{
	m_pInstance = this;

	m_bRun = true;
	unsigned int nThread = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, threadUpLoad, (LPVOID)this, 0, &nThread);
	if(m_hThread == INVALID_HANDLE_VALUE)
	{
		//
	}
	return true;
}

bool CStatisticFunction::OnMessage(CMessage* pMessage)
{
	return true;
}

void CStatisticFunction::StartCall()
{
	CTransToWnd::SendStatisticCallback(&CStatisticFunction::SetStatistic);
}

void CStatisticFunction::SetStatistic(PPlayStatistic p)
{
	m_pInstance->AddStatistic(p);
}

void CStatisticFunction::AddStatistic(PPlayStatistic p)
{
	m_lock.Lock();
	map<string, vector<PlayStatistic::_st_StaData>>::iterator iter = m_mapStatistic.find(p->strName);
	if(iter == m_mapStatistic.end())
	{
		vector<PlayStatistic::_st_StaData> vecData;
		vecData.push_back(p->Data);
		m_mapStatistic.insert(make_pair<string, vector<PlayStatistic::_st_StaData>>(p->strName, vecData));
	}
	else
	{
		iter->second.push_back(p->Data);
	}
	m_lock.UnLock();

}

unsigned int _stdcall CStatisticFunction::threadUpLoad(LPVOID lpvoid)
{
	CStatisticFunction* pFun = (CStatisticFunction*)lpvoid;
	if(pFun)
	{
		pFun->UpLoad();
	}

	return 0;
}

void CStatisticFunction::UpLoad()
{
	int nCycle = 30;
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		nCycle = pCM->GetUploadCycle();
		if (nCycle == 0)
		{
			nCycle = 30;
		}
	}
	while(m_bRun)
	{
		CNetManager* pNM = CNetManager::GetNet();
		if(pNM)
		{
			CMessage msg;
			m_lock.Lock();
			map<string, vector<PlayStatistic::_st_StaData>> mapStatistic = m_mapStatistic;
			m_mapStatistic.clear();
			m_lock.UnLock();
			ConvertMapToVecter(mapStatistic, msg);

			CNetManager* pNet = CNetManager::GetNet();
			if(pNet && mapStatistic.size())
			{
				int nRes = pNet->SendData(msg);
			}

		}
		Sleep(nCycle*1000);
	}

}

void CStatisticFunction::ConvertMapToVecter(const map<string, vector<PlayStatistic::_st_StaData>> &mapStatistic, CMessage& msg)
{
	StatisticUp stStatUp;
	LogObject;
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(!pCM)
	{
		LogOutError("获取配置失败");
	}
	stStatUp.head.command = Message_Statistc;
	stStatUp.head.messageType = MT_Dataset;
	stStatUp.head.strServerIP = pCM->GetServerIP();
	stStatUp.head.strCityCode = pCM->GetCityCode();
	stStatUp.head.strStopID = pCM->GetStopId();

	for(map<string, vector<PlayStatistic::_st_StaData>>::const_iterator coiter = mapStatistic.begin();
		coiter != mapStatistic.end(); coiter++)
	{
		StatisticUp::_st_PlayStatisticUp stPSU;
		stPSU.strName = coiter->first;
		stPSU.vecData = coiter->second;

		stStatUp.vecStat.push_back(stPSU);
	}

	msg.SetMessage(Message_Statistc, (void*)&stStatUp);


}