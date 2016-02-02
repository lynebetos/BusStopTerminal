#include "StdAfx.h"
#include "Main.h"
#include "ConfigManager.h"
#include "md5/MD5.h"
#include "MessageManager.h"
#include "DataManager.h"
#include "HartFunction.h"
#include "NoticeFunction.h"
#include "PlayListFunction.h"
#include "LineStopFunction.h"
#include "ResourceManager.h"
#include "WeatherFunction.h"
#include "TemplateFunction.h"
#include "BusArrivalFunction.h"
#include "ConfigureFunction.h"
#include "StatisticFunction.h"
#include "ResourceFunction.h"
#include "VolumeFunction.h"


CMain::CMain(void)
: m_pNetManager(NULL)
,m_nStatus(0)
,m_bStop(false)
{
}

CMain::~CMain(void)
{
}

int CMain::Initialize()
{
	int bInitSuc = 1;
	bInitSuc = InitLogs(("../config/"));

	LogObject;
	LogOutInfo("վ���ն�����,�汾:1.5.1(build20150908)");

	if(!bInitSuc)
	{
		LogOutError("��־ģ���ʼ������");
		return bInitSuc;
	}

#ifdef DEBUG
	string strConfigPath = "../config/config.xml";
#else
	string strConfigPath = "../config/config.xml";
#endif

	stringstream sstr;
	sstr<<"���������ļ�,·��:"<<strConfigPath;
	LogOutInfo(sstr.str().c_str());
	sstr.str() = "";

	CConfigManager* pConfig = CConfigManager::GetInstance();
	bInitSuc = pConfig->LoadConfig(strConfigPath);

	if(!bInitSuc)
	{
		LogOutError("�����ļ����ش���");
		return bInitSuc;
	}

	LogOutInfo("������Դ�б�");
	CResourceManager* pRc = CResourceManager::GetInstacne();

	bInitSuc = pRc->LoadResousce();
	if(!bInitSuc)
	{
		LogOutError("������Դ�б�ʧ��");
		return bInitSuc;
	}

	//������Ϣ�ַ�
	LogOutInfo("������Ϣ�ַ�");
	if(!CreateMessageAllot())
		return 0;

	//������Ƶby wxl
	//return true;

	return 1;
}

int CMain::DoConnectServer()
{
	LogObject;

	CConfigManager* pConfig = CConfigManager::GetInstance();
	if(!pConfig)
		return 0;

	m_pNetManager = new CNetManager(pConfig->GetServerIP(), pConfig->GetBusinessPort());
	LogOutInfo("������������");
	int ii = 0; 
	while(!m_pNetManager->Connect())
	{
		if(ii > 5)
		{
			m_pNetManager->SetDoConn(true);
			return true;
		}
		LogOutError("��������ʧ��");
		ii++;
		Sleep(5000);
	}
	m_pNetManager->SetDoConn(true);

	LogOutInfo("�����½");
	m_pNetManager->Authentication();

	//update linestop
	LogOutInfo("������վ���ݸ���");
	while(!UpdateLineStop()&&!m_bStop)
	{
		Sleep(100);
	}

	LogOutInfo("�������ø���");
	while(!m_bStop&&!UpdateConfigFile())
	{
		Sleep(100);
	}

	LogOutInfo("����ģ�����");
	while(!m_bStop&&!UpdateTemplate())
	{
		Sleep(100);
	}

	if(m_nStatus == 1 || m_nStatus == 2 || m_nStatus == 3 )
	{
		return 2;
	}

	return 1;
}

void CMain::UnInitialize()
{
	LogObject;

	LogOutInfo("�ر���������");
	if(m_pNetManager!=NULL)
	{
		m_pNetManager->DisConnect();

		delete m_pNetManager;
	}

	LogOutInfo("�ر���Ϣ�ַ��߳�");
	CMessageManager* pMM = CMessageManager::GetInstance();
	if(pMM)
	{
		pMM->ReleaseInstance();
	}

	LogOutInfo("���ģ��");
	for (map<int, IFunction*>::iterator theiter = m_mapFun.begin(); theiter != m_mapFun.end(); theiter++)
	{
		IFunction* pFun = theiter->second;
		delete pFun;
	}

	m_mapFun.clear();
	
	CDataManager::GetInstance()->ReleaseInstance();
	CResourceManager::GetInstacne()->ReleaseInstance();
	CConfigManager::GetInstance()->ReleaseInstance();

	LogOutInfo("�˳�");

}

bool CMain::CreateMessageAllot()
{
	LogObject;
	CMessageManager* pMsgManager = CMessageManager::GetInstance();

	LogOutInfo("���������߳�");
	CHartFunction* pHart = new CHartFunction;
	m_mapFun.insert(make_pair(HartFun, (IFunction*)pHart));
	if(!pHart->InitFunction())
	{
		LogOutError("�����̴߳���ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Keep, pHart));

	LogOutInfo("������վ����");
	CLineStopFunction* pLine = new CLineStopFunction;
	m_mapFun.insert(make_pair(LinestopFun, (IFunction*)pLine));
	if(!pLine->InitFunction())
	{
		LogOutError("������վ����ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Linestop, pLine));

	LogOutInfo("���������б����ģ��");
	CPlayListFunction* pPlay = new CPlayListFunction;
	m_mapFun.insert(make_pair(PlaylistFun, (IFunction*)pPlay));
	if(!pPlay->InitFunction())
	{
		LogOutError("�����б����ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Playlist, pPlay));

	LogOutInfo("�����������ģ��");
	CNoticeFunction* pNotice = new CNoticeFunction;
	m_mapFun.insert(make_pair(NoticeFun, (IFunction*)pNotice));
	if(!pNotice->InitFunction())
	{
		LogOutError("�����б����ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Notice, pNotice));

	LogOutInfo("������������ģ��");
	CWeatherFunction* pWeather = new CWeatherFunction;
	m_mapFun.insert(make_pair(WeatherFun, (IFunction*)pWeather));
	if(!pWeather->InitFunction())
	{
		LogOutError("��������ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Weather, pWeather));

	LogOutInfo("�������ø���ģ��");
	CConfigureFunction* pConfigure = new CConfigureFunction;
	m_mapFun.insert(make_pair(ConfigFun, (IFunction*)pConfigure));
	if(!pConfigure->InitFunction())
	{
		LogOutError("���ø���ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Config, pConfigure));

	LogOutInfo("����ģ�����ģ��");
	CTemplateFunction* pTemplate = new CTemplateFunction;
	m_mapFun.insert(make_pair(TemplateFun, (IFunction*)pTemplate));
	if(!pTemplate->InitFunction())
	{
		LogOutError("ģ�����ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Template, pTemplate));

	LogOutInfo("��������ͳ��ģ��");
	CStatisticFunction* pStatistic = new CStatisticFunction;
	m_mapFun.insert(make_pair(StatisticFun, (IFunction*)pStatistic));
	if(!pStatistic->InitFunction())
	{
		LogOutError("����ͳ��ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Statistc, pStatistic));

	LogOutInfo("������վ����ģ��");
	CBusArrivalFunction* pArrival = new CBusArrivalFunction;
	m_mapFun.insert(make_pair(ArrivalFun, (IFunction*)pArrival));
	if(!pArrival->InitFunction())
	{
		LogOutError("��վ����ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Arrival, pArrival));

	LogOutInfo("������Դ����ģ��");
	CResourceFunction* pResource = new CResourceFunction;
	m_mapFun.insert(make_pair(ResourceFun, (IFunction*)pResource));
	if(!pResource->InitFunction())
	{
		LogOutError("��Դ����ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Resource, pResource));

	LogOutInfo("������������ģ��");
	CVolumeFunction* pVolume = new CVolumeFunction;
	m_mapFun.insert(make_pair(VolumeFun, (IFunction*)pVolume));
	if(!pVolume->InitFunction())
	{
		LogOutError("��������ģ�鴴��ʧ��");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Volume, pVolume));

	return true;
}

IFunction* const CMain::GetFunction(int FunType)
{
	map<int, IFunction*>::iterator iter = m_mapFun.find(FunType);

	if(iter != m_mapFun.end())
	{
		IFunction* pFun = iter->second;
		return pFun;
	}

	return NULL;
}

bool CMain::UpdateLineStop()
{
	LogObject;
	if(!m_pNetManager)
	{
		LogOutError("δ������������");
		return false;
	}

	CMessage msg;
	tagUpdateLinestop stUDLs;
	CConfigManager* pConfig = CConfigManager::GetInstance();

	if(pConfig)
	{
		//�����ṹ��
		stUDLs.head.command = Message_Linestop;
		stUDLs.head.messageType = MT_Dataset;
		stUDLs.head.strCityCode = pConfig->GetCityCode();
		stUDLs.head.strServerIP = pConfig->GetServerIP();
		stUDLs.head.strStopID = pConfig->GetStopId();
		GetMsgId(stUDLs.head.strMsgID);

		CLineStopFunction* pFun = (CLineStopFunction*)GetFunction(LinestopFun);
		if(pFun)
		{
			stUDLs.strHash = pFun->GetHashCode();
		}
		else
		{
			stUDLs.strHash = "";
		}
	}
	else
	{
		LogOutError("��ȡ��վ����ʧ��");
		return false;
	}

	msg.SetMessage(Message_LinestopRequest, &stUDLs);

	int nRes = m_pNetManager->SendData(msg);
	if(nRes != NET_NOERROR)
	{
		LogOutError("������վ����ʧ��");
		return false;
	}

	map<int, IFunction*>::iterator theiter = m_mapFun.find(LinestopFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("��վģ�����ʧ��");
		return false;
	}

	CLineStopFunction* pLine = (CLineStopFunction*)theiter->second;
	int nTimeOut = 0;
	while(150 > nTimeOut)
	{
		int nRes = pLine->IsUpDate();
		if(nRes != 0)
		{
			if(nRes == 1)
				m_nStatus = 1;
			return true;
		}
		Sleep(100);
		nTimeOut++;
	}

	LogOutError("�ȴ���վ���ݳ�ʱ");
	return false;
}

bool CMain::UpdateConfigFile()
{
	LogObject;
	if(!m_pNetManager)
	{
		LogOutError("δ������������");
		return false;
	}

	CMessage msg;
	tagConfiguer stUConfig;
	CConfigManager* pConfig = CConfigManager::GetInstance();

	if(pConfig)
	{
		//�����ṹ��
		stUConfig.head.command = Message_Config;
		stUConfig.head.messageType = MT_Dataset;
		stUConfig.head.strCityCode = pConfig->GetCityCode();
		stUConfig.head.strServerIP = pConfig->GetServerIP();
		stUConfig.head.strStopID = pConfig->GetStopId();
		GetMsgId(stUConfig.head.strMsgID);

		CConfigureFunction* pFun = (CConfigureFunction*)GetFunction(ConfigFun);
		if(pFun)
		{
			stUConfig.strHash = pFun->GetHashCode();
		}
		else
		{
			stUConfig.strHash = "";
		}
	}
	else
	{
		LogOutError("��ȡ����ʧ��");
		return false;
	}

	msg.SetMessage(Message_ConfigRequest, &stUConfig);

	int nRes = m_pNetManager->SendData(msg);
	if(nRes != NET_NOERROR)
	{
		LogOutError("������������ʧ��");
		return false;
	}

	map<int, IFunction*>::iterator theiter = m_mapFun.find(ConfigFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("����ģ�����ʧ��");
		return false;
	}

	CConfigureFunction* pConfigUpdate = (CConfigureFunction*)theiter->second;
	int nTimeOut = 0;
	while(150 > nTimeOut)
	{
		int nRes = pConfigUpdate->IsUpDate();
		if(nRes != 0)
		{
			if(nRes == 1)
				m_nStatus = 2;
			return true;
		}
		Sleep(100);
		nTimeOut++;
	}

	LogOutError("�ȴ��������ݳ�ʱ");
	return false;
}

bool CMain::UpdateTemplate()
{
	LogObject;
	if(!m_pNetManager)
	{
		LogOutError("δ������������");
		return false;
	}

	CMessage msg;
	tagTemplate stUTemplate;
	CConfigManager* pConfig = CConfigManager::GetInstance();

	if(pConfig)
	{
		//�����ṹ��
		stUTemplate.head.command = Message_Template;
		stUTemplate.head.messageType = MT_Dataset;
		stUTemplate.head.strCityCode = pConfig->GetCityCode();
		stUTemplate.head.strServerIP = pConfig->GetServerIP();
		stUTemplate.head.strStopID = pConfig->GetStopId();
		GetMsgId(stUTemplate.head.strMsgID);

		CTemplateFunction* pFun = (CTemplateFunction*)GetFunction(TemplateFun);
		if(pFun)
		{
			stUTemplate.strHash = pFun->GetHashCode();
		}
		else
		{
			stUTemplate.strHash = "";
		}
	}
	else
	{
		LogOutError("��ȡģ��ʧ��");
		return false;
	}

	msg.SetMessage(Message_ConfigRequest, &stUTemplate);

	int nRes = m_pNetManager->SendData(msg);
	if(nRes != NET_NOERROR)
	{
		LogOutError("����ģ���������ʧ��");
		return false;
	}

	map<int, IFunction*>::iterator theiter = m_mapFun.find(TemplateFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("ģ��ģ�����ʧ��");
		return false;
	}

	CTemplateFunction* pTemplate = (CTemplateFunction*)theiter->second;
	int nTimeOut = 0;
	while(150 > nTimeOut)
	{
		int nRes = pTemplate->IsUpDate();
		if(nRes != 0)
		{
			if(nRes == 1)
				m_nStatus = 3;
			return true;
		}
		Sleep(100);
		nTimeOut++;
	}

	LogOutError("�ȴ�ģ�����ݳ�ʱ");
	return false;
}

void CMain::StartCallPS()
{
	LogObject;
	map<int, IFunction*>::iterator theiter = m_mapFun.find(StatisticFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("����ͳ�Ƽ���ʧ��");
	}

	CStatisticFunction* PlayStatistic = (CStatisticFunction*)theiter->second;
	if(PlayStatistic)
	{
		PlayStatistic->StartCall();
	}

}