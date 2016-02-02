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
	LogOutInfo("站牌终端启动,版本:1.5.1(build20150908)");

	if(!bInitSuc)
	{
		LogOutError("日志模块初始化错误");
		return bInitSuc;
	}

#ifdef DEBUG
	string strConfigPath = "../config/config.xml";
#else
	string strConfigPath = "../config/config.xml";
#endif

	stringstream sstr;
	sstr<<"加载配置文件,路径:"<<strConfigPath;
	LogOutInfo(sstr.str().c_str());
	sstr.str() = "";

	CConfigManager* pConfig = CConfigManager::GetInstance();
	bInitSuc = pConfig->LoadConfig(strConfigPath);

	if(!bInitSuc)
	{
		LogOutError("配置文件加载错误");
		return bInitSuc;
	}

	LogOutInfo("加载资源列表");
	CResourceManager* pRc = CResourceManager::GetInstacne();

	bInitSuc = pRc->LoadResousce();
	if(!bInitSuc)
	{
		LogOutError("加载资源列表失败");
		return bInitSuc;
	}

	//创建消息分发
	LogOutInfo("创建消息分发");
	if(!CreateMessageAllot())
		return 0;

	//调试视频by wxl
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
	LogOutInfo("创建网络连接");
	int ii = 0; 
	while(!m_pNetManager->Connect())
	{
		if(ii > 5)
		{
			m_pNetManager->SetDoConn(true);
			return true;
		}
		LogOutError("网络连接失败");
		ii++;
		Sleep(5000);
	}
	m_pNetManager->SetDoConn(true);

	LogOutInfo("申请登陆");
	m_pNetManager->Authentication();

	//update linestop
	LogOutInfo("申请线站数据更新");
	while(!UpdateLineStop()&&!m_bStop)
	{
		Sleep(100);
	}

	LogOutInfo("申请配置更新");
	while(!m_bStop&&!UpdateConfigFile())
	{
		Sleep(100);
	}

	LogOutInfo("申请模板更新");
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

	LogOutInfo("关闭网络连接");
	if(m_pNetManager!=NULL)
	{
		m_pNetManager->DisConnect();

		delete m_pNetManager;
	}

	LogOutInfo("关闭消息分发线程");
	CMessageManager* pMM = CMessageManager::GetInstance();
	if(pMM)
	{
		pMM->ReleaseInstance();
	}

	LogOutInfo("清除模块");
	for (map<int, IFunction*>::iterator theiter = m_mapFun.begin(); theiter != m_mapFun.end(); theiter++)
	{
		IFunction* pFun = theiter->second;
		delete pFun;
	}

	m_mapFun.clear();
	
	CDataManager::GetInstance()->ReleaseInstance();
	CResourceManager::GetInstacne()->ReleaseInstance();
	CConfigManager::GetInstance()->ReleaseInstance();

	LogOutInfo("退出");

}

bool CMain::CreateMessageAllot()
{
	LogObject;
	CMessageManager* pMsgManager = CMessageManager::GetInstance();

	LogOutInfo("创建心跳线程");
	CHartFunction* pHart = new CHartFunction;
	m_mapFun.insert(make_pair(HartFun, (IFunction*)pHart));
	if(!pHart->InitFunction())
	{
		LogOutError("心跳线程创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Keep, pHart));

	LogOutInfo("加载线站数据");
	CLineStopFunction* pLine = new CLineStopFunction;
	m_mapFun.insert(make_pair(LinestopFun, (IFunction*)pLine));
	if(!pLine->InitFunction())
	{
		LogOutError("加载线站数据失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Linestop, pLine));

	LogOutInfo("创建播放列表更新模块");
	CPlayListFunction* pPlay = new CPlayListFunction;
	m_mapFun.insert(make_pair(PlaylistFun, (IFunction*)pPlay));
	if(!pPlay->InitFunction())
	{
		LogOutError("播放列表加载失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Playlist, pPlay));

	LogOutInfo("创建公告更新模块");
	CNoticeFunction* pNotice = new CNoticeFunction;
	m_mapFun.insert(make_pair(NoticeFun, (IFunction*)pNotice));
	if(!pNotice->InitFunction())
	{
		LogOutError("公告列表加载失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Notice, pNotice));

	LogOutInfo("创建天气更新模块");
	CWeatherFunction* pWeather = new CWeatherFunction;
	m_mapFun.insert(make_pair(WeatherFun, (IFunction*)pWeather));
	if(!pWeather->InitFunction())
	{
		LogOutError("天气更新模块创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Weather, pWeather));

	LogOutInfo("创建配置更新模块");
	CConfigureFunction* pConfigure = new CConfigureFunction;
	m_mapFun.insert(make_pair(ConfigFun, (IFunction*)pConfigure));
	if(!pConfigure->InitFunction())
	{
		LogOutError("配置更新模块创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Config, pConfigure));

	LogOutInfo("创建模板更新模块");
	CTemplateFunction* pTemplate = new CTemplateFunction;
	m_mapFun.insert(make_pair(TemplateFun, (IFunction*)pTemplate));
	if(!pTemplate->InitFunction())
	{
		LogOutError("模板更新模块创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Template, pTemplate));

	LogOutInfo("创建播放统计模块");
	CStatisticFunction* pStatistic = new CStatisticFunction;
	m_mapFun.insert(make_pair(StatisticFun, (IFunction*)pStatistic));
	if(!pStatistic->InitFunction())
	{
		LogOutError("播放统计模块创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Statistc, pStatistic));

	LogOutInfo("创建到站接收模块");
	CBusArrivalFunction* pArrival = new CBusArrivalFunction;
	m_mapFun.insert(make_pair(ArrivalFun, (IFunction*)pArrival));
	if(!pArrival->InitFunction())
	{
		LogOutError("到站接收模块创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Arrival, pArrival));

	LogOutInfo("创建资源下载模块");
	CResourceFunction* pResource = new CResourceFunction;
	m_mapFun.insert(make_pair(ResourceFun, (IFunction*)pResource));
	if(!pResource->InitFunction())
	{
		LogOutError("资源下载模块创建失败");
		return false;
	}
	pMsgManager->m_mapAllot.insert(make_pair(Message_Resource, pResource));

	LogOutInfo("创建音量配置模块");
	CVolumeFunction* pVolume = new CVolumeFunction;
	m_mapFun.insert(make_pair(VolumeFun, (IFunction*)pVolume));
	if(!pVolume->InitFunction())
	{
		LogOutError("音量配置模块创建失败");
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
		LogOutError("未创建网络连接");
		return false;
	}

	CMessage msg;
	tagUpdateLinestop stUDLs;
	CConfigManager* pConfig = CConfigManager::GetInstance();

	if(pConfig)
	{
		//构建结构体
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
		LogOutError("获取线站配置失败");
		return false;
	}

	msg.SetMessage(Message_LinestopRequest, &stUDLs);

	int nRes = m_pNetManager->SendData(msg);
	if(nRes != NET_NOERROR)
	{
		LogOutError("发送线站请求失败");
		return false;
	}

	map<int, IFunction*>::iterator theiter = m_mapFun.find(LinestopFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("线站模块加载失败");
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

	LogOutError("等待线站数据超时");
	return false;
}

bool CMain::UpdateConfigFile()
{
	LogObject;
	if(!m_pNetManager)
	{
		LogOutError("未创建网络连接");
		return false;
	}

	CMessage msg;
	tagConfiguer stUConfig;
	CConfigManager* pConfig = CConfigManager::GetInstance();

	if(pConfig)
	{
		//构建结构体
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
		LogOutError("获取配置失败");
		return false;
	}

	msg.SetMessage(Message_ConfigRequest, &stUConfig);

	int nRes = m_pNetManager->SendData(msg);
	if(nRes != NET_NOERROR)
	{
		LogOutError("发送配置请求失败");
		return false;
	}

	map<int, IFunction*>::iterator theiter = m_mapFun.find(ConfigFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("配置模块加载失败");
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

	LogOutError("等待配置数据超时");
	return false;
}

bool CMain::UpdateTemplate()
{
	LogObject;
	if(!m_pNetManager)
	{
		LogOutError("未创建网络连接");
		return false;
	}

	CMessage msg;
	tagTemplate stUTemplate;
	CConfigManager* pConfig = CConfigManager::GetInstance();

	if(pConfig)
	{
		//构建结构体
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
		LogOutError("获取模板失败");
		return false;
	}

	msg.SetMessage(Message_ConfigRequest, &stUTemplate);

	int nRes = m_pNetManager->SendData(msg);
	if(nRes != NET_NOERROR)
	{
		LogOutError("发送模板更新请求失败");
		return false;
	}

	map<int, IFunction*>::iterator theiter = m_mapFun.find(TemplateFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("模板模块加载失败");
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

	LogOutError("等待模板数据超时");
	return false;
}

void CMain::StartCallPS()
{
	LogObject;
	map<int, IFunction*>::iterator theiter = m_mapFun.find(StatisticFun);
	if(theiter == m_mapFun.end())
	{
		LogOutError("播发统计加载失败");
	}

	CStatisticFunction* PlayStatistic = (CStatisticFunction*)theiter->second;
	if(PlayStatistic)
	{
		PlayStatistic->StartCall();
	}

}