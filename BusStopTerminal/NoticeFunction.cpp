#include "StdAfx.h"
#include "NoticeFunction.h"
#include "ConfigManager.h"
#include "tinyxml/tinyxml.h"
#include "TransToWnd.h"
#include "NetManager.h"

CNoticeFunction::CNoticeFunction(void)
: m_bUpdateRun(false)
{
}

CNoticeFunction::~CNoticeFunction(void)
{
	LogObject;
	LogOutInfo("清除公告更新模块");
}

bool CNoticeFunction::InitFunction()
{
	LoadListFile();
	return true;
}

bool CNoticeFunction::LoadListFile()
{
	string strPath = CConfigManager::GetInstance()->GetNoticeDir();

	TiXmlDocument doc;
	if(!doc.LoadFile(strPath.c_str()))
		return false;

	TiXmlElement* pRoot = doc.RootElement();

	for (TiXmlElement* pChild = pRoot->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		_st_NoticeList stNoticeList;
		stNoticeList.strID = pChild->Attribute("id");
		for (TiXmlElement* pChildList = pChild->FirstChildElement(); pChildList != NULL; pChildList = pChildList->NextSiblingElement())
		{
			const char* szValue = pChildList->Value();
			if(strcmp("start", szValue) == 0)
			{
				string strStartDateTime = pChildList->Attribute("date");
				strStartDateTime += " ";
				strStartDateTime += pChildList->Attribute("time");
				stNoticeList.tStart = CBase::string2time(strStartDateTime);
			}
			else if(strcmp("end", szValue) == 0)
			{
				string strEndDateTime = pChildList->Attribute("date");
				strEndDateTime += " ";
				strEndDateTime += pChildList->Attribute("time");
				stNoticeList.tEnd = CBase::string2time(strEndDateTime);
			}
			else if(strcmp("interval", szValue) == 0)
			{
				string strInterval = pChildList->Attribute("time");
				stNoticeList.nInterval = atoi(strInterval.c_str());
			}
			else if(strcmp("notice", szValue) == 0)
			{
				tagNotice::_st_list stList;
				stList.strNoticeID = pChildList->Attribute("id");
				stList.strText = pChildList->Attribute("text");
				stNoticeList.listNotice.push_back(stList);
			}
		}

		m_setNoticList.insert(stNoticeList);
	}

	return true;
}

bool CNoticeFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	while(m_bUpdateRun)
	{
		Sleep(10);
	}

	m_Message = *pMessage;
	MessageMap msgType = m_Message.GetCommand();
	void* pData = m_Message.GetMessageData(msgType);
	tagListNotic* pstListNotic = (tagListNotic*)pData;

	unsigned int nThreadID = 0;
	HANDLE hNotice = (HANDLE)_beginthreadex(NULL, 0, threadUpdate, (LPVOID)this, 0, &nThreadID);
	if(hNotice == INVALID_HANDLE_VALUE)
	{
		LogOutInfo("公告列表更新失败");
		//更新公告列表失败
	}
	LogOutInfo("公告列表更新成功");

	m_bUpdateRun = true;
	return true;
}

unsigned int _stdcall CNoticeFunction::threadUpdate(LPVOID lpvoid)
{
	CNoticeFunction* pFun = (CNoticeFunction*)lpvoid;
	if(pFun)
	{
		pFun->ProcUpdate();
	}
	return 0;
}

void CNoticeFunction::ProcUpdate()
{
	MessageMap msgType = m_Message.GetCommand();
	void* pData = m_Message.GetMessageData(msgType);
	tagListNotic* pstListNotic = (tagListNotic*)pData;
	m_setNoticList.clear();

	for (list<tagNotice>::iterator iterList = pstListNotic->listListNotice.begin();
		iterList != pstListNotic->listListNotice.end();
		iterList++)
	{
		_st_NoticeList stNoticeList;

		string strStartDateTime = iterList->strStartDate;
		strStartDateTime += " ";
		strStartDateTime += iterList->strStartTime;
		
		stNoticeList.tStart = CBase::string2time(strStartDateTime);

		string strEndDateTime = iterList->strEndDate;
		strEndDateTime += " ";
		strEndDateTime += iterList->strEndTime;

		stNoticeList.tEnd = CBase::string2time(strEndDateTime);

		stNoticeList.strID = iterList->strID;
		stNoticeList.nInterval = atoi(iterList->strInterval.c_str());

		stNoticeList.listNotice = iterList->listNotice;

		if(iterList->strOptr == INSERTE_LIST)
		{
			set<_st_NoticeList>::iterator setiter = m_setNoticList.find(stNoticeList);
			if(setiter != m_setNoticList.end())
			{
				*setiter = stNoticeList;
			}
			else
			{
				m_setNoticList.insert(stNoticeList);
			}
		}
		else if(iterList->strOptr == REPLACE_LIST)
		{
			stNoticeList.strID = iterList->strDest;
			set<_st_NoticeList>::iterator setiter = m_setNoticList.find(stNoticeList);
			stNoticeList.strID = iterList->strID;
			if(setiter != m_setNoticList.end())
			{
				*setiter = stNoticeList;
			}
			else
			{
				m_setNoticList.insert(stNoticeList);
			}

		}
	}

	WriteListFile();

	CTransToWnd::SendReloadNotice();

	tagResponse stResponse;
	stResponse.head = pstListNotic->sthead;
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

	m_bUpdateRun = false;

}

bool CNoticeFunction::WriteListFile()
{
	string strPath = CConfigManager::GetInstance()->GetNoticeDir();

	TiXmlDocument doc;
	TiXmlDeclaration xmlDecl("1.0", "UTF-8", "yes");
	doc.InsertEndChild(xmlDecl);
	TiXmlElement* pRoot = new TiXmlElement("root");
	doc.LinkEndChild(pRoot);

	for (set<_st_NoticeList>::iterator iter = m_setNoticList.begin();
		iter != m_setNoticList.end();
		iter++)
	{
		TiXmlElement* pList = new TiXmlElement("list");
		pRoot->LinkEndChild(pList);
		
		pList->SetAttribute("id", iter->strID.c_str());

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

		TiXmlElement* pInterval = new TiXmlElement("interval");
		pList->LinkEndChild(pInterval);

		stringstream sstr;
		sstr<<iter->nInterval;
		pInterval->SetAttribute("time", sstr.str().c_str());
		sstr.str() = "";

		for (int ii = 0; ii < (int)iter->listNotice.size(); ii++)
		{
			tagNotice::_st_list& stNotice = iter->listNotice[ii];
			TiXmlElement* pNotice = new TiXmlElement("notice");
			
			pList->LinkEndChild(pNotice);

			pNotice->SetAttribute("id", stNotice.strNoticeID.c_str());
			pNotice->SetAttribute("text", stNotice.strText.c_str());
		}

	}

	doc.SaveFile(strPath.c_str());
	return true;
}

void CNoticeFunction::GetNoticeList(vector<_st_NoticeList>& vecNoticeList)
{
	for (set<_st_NoticeList>::iterator setiter = m_setNoticList.begin(); setiter != m_setNoticList.end(); setiter++)
	{
		vecNoticeList.push_back(*setiter);
	}
}