#include "StdAfx.h"
#include "VolumeFunction.h"
#include "VolumeManager.h"
#include "Message.h"
#include "ConfigManager.h"
#include "NetManager.h"

CVolumeFunction::CVolumeFunction(void)
: m_bRun(false)
{
}

CVolumeFunction::~CVolumeFunction(void)
{
	LogObject;
	m_bRun = false;
	WaitForSingleObject(m_hVolume, INFINITE);
	LogOutInfo("清除声音配置模块");
}

bool CVolumeFunction::InitFunction()
{
	LogObject;
	CConfigManager* pCM = CConfigManager::GetInstance();
	string strDir;
	if(pCM)
	{
		strDir = pCM->GetVolumeDir();
		m_strPath = strDir;
		if(!LoadVolumeConfig(m_strPath))
		{
			LogOutError("没有音量配置文件");
		}
	}

	m_bRun = true;
	unsigned int nThreadId = 0;
	m_hVolume = (HANDLE)_beginthreadex(NULL, 0, threadSetVol, (LPVOID)this, 0, &nThreadId);
	if(m_hVolume == INVALID_HANDLE_VALUE)
	{
		//
	}

	return true;
}

bool CVolumeFunction::LoadVolumeConfig(string strFile)
{
	TiXmlDocument doc;
	if(!doc.LoadFile(strFile.c_str()))
		return false;

	TiXmlElement* pRoot = doc.RootElement();

	for (TiXmlElement* pChild = pRoot->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		tagVolumeConf::_st_Volume stVolume;
		stVolume.strTime = pChild->Attribute("time");
		stVolume.strVolume = pChild->Attribute("volume");

		m_stVolumeConf.vecVolume.push_back(stVolume);
	}

	return true;
}

unsigned int _stdcall CVolumeFunction::threadSetVol(LPVOID lpvoid)
{
	CVolumeFunction* pFun = (CVolumeFunction*)lpvoid;
	if(pFun)
	{
		pFun->SeVolume();
	}

	return 0;
}

void CVolumeFunction::SeVolume()
{
	while(m_bRun)
	{
		m_lock.Lock();
		tagVolumeConf stVolumeConf = m_stVolumeConf;
		m_lock.UnLock();
		if(stVolumeConf.vecVolume.size() <= 0)
		{
			Sleep(5*1000);
			continue;;
		}

		for (vector<tagVolumeConf::_st_Volume>::iterator iter = stVolumeConf.vecVolume.begin();
			iter != stVolumeConf.vecVolume.end();
			iter++)	
		{
			string strTime = "";

			time_t tn = time(0);
			string strDateNow = CBase::time2string_date(tn);
			string strTimeNow = CBase::time2string_time(tn);

			strTime = strDateNow;
			strTime += " ";
			strTime += iter->strTime;
			strTime += ":00";

			time_t tc = CBase::string2time(strTime);

			if( _abs64(tn - tc) <= 60)
			{
				CVolumeManager::GetInstance()->SetSpeakerLevel(atoi(iter->strVolume.c_str()));
				break;
			}
		}

		Sleep(5*1000);
	}
}

bool CVolumeFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	MessageMap messageType;
	void* pData = pMessage->GetMessageData(messageType);

	if(pData == NULL)
	{
		LogOutError("音量配置数据错误");
		return false;
	}

	tagVolumeConf* pVolumeConf = (tagVolumeConf*)pData;

	m_lock.Lock();
	m_stVolumeConf = *pVolumeConf;
	m_lock.UnLock();

	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		string strPath = CConfigManager::GetInstance()->GetVolumeDir();

		TiXmlDocument doc;
		TiXmlDeclaration xmlDecl("1.0", "UTF-8", "yes");
		doc.InsertEndChild(xmlDecl);
		TiXmlElement* pRoot = new TiXmlElement("root");
		doc.LinkEndChild(pRoot);

		for (vector<tagVolumeConf::_st_Volume>::iterator iter = pVolumeConf->vecVolume.begin();
			iter != pVolumeConf->vecVolume.end();
			iter++)
		{
			TiXmlElement* pList = new TiXmlElement("list");
			pRoot->LinkEndChild(pList);

			pList->SetAttribute("time", iter->strTime.c_str());
			pList->SetAttribute("volume", iter->strVolume.c_str());
		}

		doc.SaveFile(strPath.c_str());

	}

	LogOutInfo("音量配置更新成功");

	tagResponse stResponse;
	stResponse.head = pVolumeConf->head;
	stResponse.head.messageType = MT_Response;
	stResponse.nStatus = 200;
	stResponse.strMsg = "正常";
	CMessage msg;
	msg.SetMessage(Message_Response, &stResponse);
	CNetManager* pNM = CNetManager::GetNet();
	if(pNM)
	{
		pNM->SendData(msg);
	}

	return true;
}