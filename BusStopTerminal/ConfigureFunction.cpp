#include "StdAfx.h"
#include "ConfigureFunction.h"
#include "ConfigManager.h"
#include "Message.h"

CConfigureFunction::CConfigureFunction(void)
: m_nUpdateFinish(0)
{
}

CConfigureFunction::~CConfigureFunction(void)
{
	LogObject;
	LogOutInfo("清除配置更新模块");
}

bool CConfigureFunction::InitFunction()
{
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		m_strHashCode = pCM->m_strConfigVer;
	}
	return true;
}

bool CConfigureFunction::OnMessage(CMessage* pMessage)
{

	LogObject;
	MessageMap messageType;
	void* pData = pMessage->GetMessageData(messageType);

	if(pData == NULL)
	{
		LogOutError("配置更新数据错误");
		return false;
	}

	tagConfigUpdate* pConfigUpdate = (tagConfigUpdate*)pData;

	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		m_bUpdate = pConfigUpdate->nUpdate;
		if(m_bUpdate == 1)
		{
			m_strHashCode = pConfigUpdate->strHashCode;
			pCM->m_strConfigVer = m_strHashCode;
			pCM->m_strNoticeDir = pConfigUpdate->strNoticeDir;
			pCM->m_strTemplatePath = pConfigUpdate->strTemplateDir;
			pCM->m_strPictrueDir = pConfigUpdate->strPictrueDir;
			pCM->m_strDefaultPath = pConfigUpdate->strDefaultDir;
			pCM->m_Adver.m_strSourceDir = pConfigUpdate->strAdvDir;
			pCM->m_Adver.m_strPlaylist = pConfigUpdate->strAdvPlaylist;
			pCM->m_Busstop.m_strStopName = pConfigUpdate->strStopName;
			pCM->m_strSotfUrl = pConfigUpdate->strUpdateDown;
			pCM->m_strIp = pConfigUpdate->strServerIP;
			pCM->m_Busstop.m_strStaticData = pConfigUpdate->strLineDataDir;
			pCM->m_nUpload = atoi(pConfigUpdate->strStatisticCycle.c_str());
			pCM->m_nHartUp = atoi(pConfigUpdate->strHartCycle.c_str());
		}
	}

	if(m_bUpdate == 1)
	{
		pCM->SaveConfig();
		LogOutInfo("配置数据更新成功");

		m_nUpdateFinish = 1;
		return true;
	}

	LogOutInfo("无新配置数据");
	m_nUpdateFinish = 2;

	return true;
}
