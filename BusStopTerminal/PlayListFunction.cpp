#include "StdAfx.h"
#include "PlayListFunction.h"
#include "ConfigManager.h"
#include "NetManager.h"
#include "TransToWnd.h"
#include "VideoPlayList.h"
#include "../include/http/base_http.h"

CPlayListFunction::CPlayListFunction(void)
: m_bUpDateRun(false)
{
}

CPlayListFunction::~CPlayListFunction(void)
{
	LogObject;
	LogOutInfo("清除广告列表更新模块");
}

bool CPlayListFunction::InitFunction()
{
	LoadListFile();
	return true;
}

bool CPlayListFunction::LoadListFile()
{
	string strPath = CConfigManager::GetInstance()->GetPlaylistPath();

	TiXmlDocument doc;
	if(!doc.LoadFile(strPath.c_str()))
		return false;

	TiXmlElement* pRoot = doc.RootElement();

	CVideoPlayList* pVideoPlayList = CVideoPlayList::GetInstance();
	if(NULL == pVideoPlayList)
	{
		return false;
	}

	for (TiXmlElement* pChild = pRoot->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		const char* szKind = pChild->Value();
		if(strcmp("resources", szKind) == 0)
		{
			for (TiXmlElement* pChildList = pChild->FirstChildElement(); pChildList != NULL; pChildList = pChildList->NextSiblingElement())
			{
				const char* szValue = pChildList->Value();
				if(strcmp("res", szValue) == 0)
				{
					string strName = pChildList->Attribute("name");
					string strUrl = pChildList->Attribute("downUrl");
					pVideoPlayList->AddResource(strName, strUrl);
				}
			}
		}
		else if(strcmp("list", szKind) == 0)
		{
			for (TiXmlElement* pChildList = pChild->FirstChildElement(); pChildList != NULL; pChildList = pChildList->NextSiblingElement())
			{
				//一天段
				CVideoPlayList::_st_dayblock& refDay = pVideoPlayList->AddDay();
				const char* szValue = pChildList->Value();
				if(strcmp("day", szValue) == 0)
				{
					refDay.strStartDate = pChildList->Attribute("start");
					refDay.strEndDate = pChildList->Attribute("end");

					for (TiXmlElement* pChildTime = pChildList->FirstChildElement(); pChildTime != NULL; pChildList = pChildTime->NextSiblingElement())
					{
						//时间段
						const char* szTime = pChildTime->Value();
						if(strcmp("time", szTime) == 0)
						{
							CVideoPlayList::_st_timeblock stTime;
							refDay.vecTimeblock.push_back(stTime);
							CVideoPlayList::_st_timeblock& refTime = refDay.vecTimeblock.back();

							refTime.strStartTime = pChildTime->Attribute("start");
							refTime.strEndTime = pChildTime->Attribute("end");

							for (TiXmlElement* pChildVideo = pChildTime->FirstChildElement(); pChildVideo != NULL; pChildList = pChildVideo->NextSiblingElement())
							{
								//视频控制
								const char* szVideo = pChildVideo->Value();
								if(strcmp("advertisement", szVideo) == 0)
								{
									CVideoPlayList::_st_Video stVideo;
									refTime.vecVideo.push_back(stVideo);
									CVideoPlayList::_st_Video& refVideo = refTime.vecVideo.back();

									refVideo.strName = pChildVideo->Attribute("name");
									refVideo.nPlaysec = atoi(pChildVideo->Attribute("playsec"));

								}
							}
						}
					}
				}
			}

		}
	}

	return true;
}

bool CPlayListFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	while(m_bUpDateRun)
	{
		Sleep(10);
	}

	m_Message = *pMessage;
	MessageMap msgType = m_Message.GetCommand();
	void* pData = m_Message.GetMessageData(msgType);
	tagListPlay* pstListPlay = (tagListPlay*)pData;

	unsigned int nThreadID = 0;
	HANDLE hPlay = (HANDLE)_beginthreadex(NULL, 0, threadUpdate, (LPVOID)this, 0, &nThreadID);
	if(hPlay == INVALID_HANDLE_VALUE)
	{
		LogOutInfo("播放列表更新错误");
		//播放列表更新错误
	}

	tagResponse stResponse;
	stResponse.head = pstListPlay->sthead;
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


	m_bUpDateRun = true;

	return true;
}

unsigned int _stdcall CPlayListFunction::threadUpdate(LPVOID lpvoid)
{
	CPlayListFunction* pFun = (CPlayListFunction*)lpvoid;
	if(pFun)
	{
		pFun->ProcUpdate();
	}
	return 0;
}

void CPlayListFunction::ProcUpdate()
{
	LogObject;
	MessageMap msgType = m_Message.GetCommand();
	void* pData = m_Message.GetMessageData(msgType);
	tagListPlay* pstListPlay = (tagListPlay*)pData;

	if(pData == NULL)
	{
		LogOutError("播放列表数据错误");
	}

	bool bDownSucc = true;

	CConfigManager* pCM = CConfigManager::GetInstance();

	std::string& strUrl = pstListPlay->strUrl;


	base_http::CBaseHttp baseHttp;
	string strListFile = pCM->GetPlaylistPath();

	strUrl = Utf8toAnsi(strUrl);
	strUrl = AnsiToUtf8(strUrl);

	//baseHttp.AddHttpHeader("Authorization", "Basic YWRtaW46YWRtaW4=");
	bool bDown = baseHttp.HttpGet(strUrl.c_str());
	if(!bDown)
	{
		stringstream sstrLog;
		sstrLog<<"资源下载失败,URL= "<<strUrl;
		LogOutError(sstrLog.str().c_str());
		sstrLog.str("");
		bDownSucc = false;
	}

	bool bWrite = baseHttp.SaveDataToFile(strListFile.c_str());
	// 				bool bDown = baseHttp.HttpDownload(strUrl.c_str(), strAdvPath.c_str());

	if(!bWrite)
	{
		stringstream sstrLog;
		sstrLog<<"资源写入失败,保存路径 "<<strListFile;
		LogOutError(sstrLog.str().c_str());
		sstrLog.str("");
		bDownSucc = false;
	}
	else
	{
		stringstream sstrLog;
		sstrLog<<"资源下载成功,URL= "<<strUrl<<"; 保存路径:"<<strListFile;
		LogOutError(sstrLog.str().c_str());
		sstrLog.str("");
	}

	if(!bDownSucc)
	{
		//报警信息
		return;
	}

	LogOutInfo("播放列表更新成功");

	LoadListFile();

	CTransToWnd::SendReloadPlayList();

}

bool CPlayListFunction::WriteListFile()
{
	string strPath = CConfigManager::GetInstance()->GetPlaylistPath();

	TiXmlDocument doc;
	TiXmlDeclaration xmlDecl("1.0", "UTF-8", "yes");
	doc.InsertEndChild(xmlDecl);
	TiXmlElement* pRoot = new TiXmlElement("root");
	doc.LinkEndChild(pRoot);

	for (vector<_st_PlayList>::iterator iter = m_setPlayList.begin();
		iter != m_setPlayList.end();
		iter++)
	{
		TiXmlElement* pList = new TiXmlElement("list");
		pRoot->LinkEndChild(pList);

		TiXmlElement* pStart = new TiXmlElement("start");
		pList->LinkEndChild(pStart);

		string strDate = CBase::time2string_date(iter->tStart);
		pStart->SetAttribute("date", strDate.c_str());

		string strTime = CBase::time2string_time(iter->tStart);
		pStart->SetAttribute("time", strTime.c_str());

		TiXmlElement* pEnd = new TiXmlElement("end");
		pList->LinkEndChild(pEnd);

		strDate = CBase::time2string_date(iter->tEnd);
		pEnd->SetAttribute("date", strDate.c_str());

		strTime = CBase::time2string_time(iter->tEnd);
		pEnd->SetAttribute("time", strTime.c_str());

		for (int ii = 0; ii < (int)iter->listPlay.size(); ii++)
		{
			tagPlay::_st_list& stPlay = iter->listPlay[ii];
			TiXmlElement* pPlay = new TiXmlElement("advertisment");

			pList->LinkEndChild(pPlay);

			pPlay->SetAttribute("id", stPlay.strID.c_str());
			pPlay->SetAttribute("name", stPlay.strName.c_str());
			pPlay->SetAttribute("num", stPlay.strNum.c_str());
		}

	}

	doc.SaveFile(strPath.c_str());
	return true;
}

void CPlayListFunction::GetPlayList(vector<_st_PlayList>& vecList)
{
	for (vector<_st_PlayList>::iterator setiter = m_setPlayList.begin(); setiter != m_setPlayList.end(); setiter++)
	{
		vecList.push_back(*setiter);
	}
}