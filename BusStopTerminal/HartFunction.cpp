#include "StdAfx.h"
#include "HartFunction.h"
#include "Message.h"
#include "NetManager.h"
#include "ConfigManager.h"

#define HartSleep 15000


CHartFunction::CHartFunction(void)
: m_bHart(false)
, m_hHart(INVALID_HANDLE_VALUE)
{
}

CHartFunction::~CHartFunction(void)
{
	LogObject;
	m_bHart = false;
	WaitForSingleObject(m_hHart, INFINITE);
	LogOutInfo("清除心跳上传模块");
}

bool CHartFunction::InitFunction()
{
//	return true;
	LogObject;
	m_bHart = true;
	unsigned int nThreadID = 0;
	m_hHart = (HANDLE)_beginthreadex(NULL, 0, threadHart, (LPVOID)this, 0, &nThreadID);
	if(m_hHart == INVALID_HANDLE_VALUE)
	{
		LogOutError("心跳线程创建失败");
		return false;
	}
	return true;
}

bool CHartFunction::OnMessage(CMessage* pMessage)
{
	return true;
}

unsigned int _stdcall CHartFunction::threadHart(LPVOID lpvoid)
{
	CHartFunction* pFun = (CHartFunction*)lpvoid;
	if(pFun)
	{
		pFun->ProcHart();
	}

	return 0;
}

void CHartFunction::ProcHart()
{
	LogObject;
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(!pCM)
	{
		LogOutError("获取配置失败");
	}
	tagKeepHart stHart;
	stHart.head.command = Message_Keep;
	stHart.head.messageType = MT_Dataset;
	stHart.head.strServerIP = pCM->GetServerIP();
	stHart.head.strCityCode = pCM->GetCityCode();
	stHart.head.strStopID = pCM->GetStopId();
	while(m_bHart)
	{
		GetMsgId(stHart.head.strMsgID );
		tagKeepHart::_st_Monitor stMonitor;

		string strVer = pCM->GetAppVer();
		stMonitor.strKey = "01";
		stMonitor.strValue = strVer;
		stHart.vecMonitor.push_back(stMonitor);


		ULARGE_INTEGER nFreeBytesAvailableToCaller, nTotalNumberOfBytes, nTotalNumberOfFreeBytes;
		GetDiskFreeSpaceEx("c:\\", &nFreeBytesAvailableToCaller, &nTotalNumberOfBytes, &nTotalNumberOfFreeBytes);

		//double dbAll = (double)nTotalNumberOfBytes.QuadPart / 1024 / 1024 / 1024;
		double dbFree = (double)nTotalNumberOfFreeBytes.QuadPart / 1024 / 1024;

		stringstream sstrFree;
		sstrFree<<dbFree;

		stMonitor.strKey = "02";
		stMonitor.strValue = sstrFree.str();
		sstrFree.str("");
		stHart.vecMonitor.push_back(stMonitor);

		DWORD dwVolume = 0, dwVoMax;
		GetSystemVolum(dwVolume);
		CVolumeManager* pVM = CVolumeManager::GetInstance();
		if(pVM)
			pVM->GetSpeakerLevel(&dwVolume, &dwVoMax);

		sstrFree<<dwVolume + 1;
		stMonitor.strKey = "03";
		stMonitor.strValue = sstrFree.str();
		stHart.vecMonitor.push_back(stMonitor);

		stHart.vecWarnning = m_vecWarnning;

		CMessage message;
		message.SetMessage(Message_Keep, (void*)&stHart);

		CNetManager* pNet = CNetManager::GetNet();
		if(pNet)
		{
			if(!pNet->GetDoConn())
			{
				Sleep(HartSleep);
				continue;
			}
			
			int nRes = pNet->SendData(message);
			if(NET_ERROR_NONCONN == nRes || NET_ERROR_MISSCONN == nRes)
			{
				ReConnect();
				continue;
			}
		}

		stHart.vecMonitor.clear();
		stHart.vecWarnning.clear();
		m_vecWarnning.clear();
		Sleep(HartSleep);
	}
}

bool CHartFunction::ReConnect()
{
	LogObject;
	bool bRet = false;
	CNetManager* pNM = CNetManager::GetNet();
	if(pNM)
	{
		if(!pNM->GetDoConn())
			return false;
		pNM->DisConnect();
		pNM->ReBind();

		int nRes = 0;
		do 
		{
			Sleep(RECONNET_DILAY);
			LogOutInfo("重新连接服务器");
			bRet = pNM->ReConnect();
		} while (!bRet&m_bHart);

		LogOutInfo("登陆服务器");
		bRet = pNM->Authentication();
	}

	return bRet;
}