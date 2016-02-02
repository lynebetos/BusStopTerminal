#include "StdAfx.h"
#include "PlayListFunction.h"
#include "ConfigManager.h"
#include "NetManager.h"
#include "TransToWnd.h"

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

	for (TiXmlElement* pChild = pRoot->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		_st_PlayList stPlayList;
		for (TiXmlElement* pChildList = pChild->FirstChildElement(); pChildList != NULL; pChildList = pChildList->NextSiblingElement())
		{
			const char* szValue = pChildList->Value();
			if(strcmp("start", szValue) == 0)
			{
				string strStartDateTime = pChildList->Attribute("date");
				strStartDateTime += " ";
				strStartDateTime += pChildList->Attribute("time");
				stPlayList.tStart = CBase::string2time(strStartDateTime);
			}
			else if(strcmp("end", szValue) == 0)
			{
				string strEndDateTime = pChildList->Attribute("date");
				strEndDateTime += " ";
				strEndDateTime += pChildList->Attribute("time");
				stPlayList.tEnd = CBase::string2time(strEndDateTime);
			}
			else if(strcmp("advertisment", szValue) == 0)
			{
				tagPlay::_st_list stList;
				stList.strName = pChildList->Attribute("name");
				stList.strNum = pChildList->Attribute("num");
				stPlayList.listPlay.push_back(stList);
			}
		}

		m_setPlayList.push_back(stPlayList);
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
	m_setPlayList.clear();

	for (list<tagPlay>::iterator iterList = pstListPlay->listListPlay.begin();
		iterList != pstListPlay->listListPlay.end();
		iterList++)
	{
		_st_PlayList stPlayList;

		string strStartDateTime = iterList->strStartDate;
		strStartDateTime += " ";
		strStartDateTime += iterList->strStartTime;

		stPlayList.tStart = CBase::string2time(strStartDateTime);

		string strEndDateTime = iterList->strEndDate;
		strEndDateTime += " ";
		strEndDateTime += iterList->strEndTime;

		stPlayList.tEnd = CBase::string2time(strEndDateTime);

		stPlayList.listPlay = iterList->listPlay;

		std::sort(stPlayList.listPlay.begin(), stPlayList.listPlay.end(), less<tagPlay::_st_list>());
		m_setPlayList.push_back(stPlayList);

	}

	WriteListFile();
	LogOutInfo("播放列表更新成功");

	CTransToWnd::SendReloadPlayList();

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

	m_bUpDateRun = false;

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