#include "StdAfx.h"
#include "LineStopFunction.h"
#include "JsonManager.h"
#include "ConfigManager.h"
#include "DataManager.h"
#include "NetManager.h"

CLineStopFunction::CLineStopFunction(void)
: m_bUpdate(0)
, m_strHashCode("")
, m_nUpdateFinish(0)
{
}

CLineStopFunction::~CLineStopFunction(void)
{
	LogObject;
	LogOutInfo("清除线站更新模块");
}

bool CLineStopFunction::InitFunction()
{
	string str;
	string strPath = CConfigManager::GetInstance()->GetStaticStopDataPath();
	ifstream ifs(strPath.c_str(), ios::in);
	ostringstream buffer;
	if(ifs.is_open())
	{
		buffer<<ifs.rdbuf();
		str = buffer.str();
	}

	if(str != "")
	{
		AnalyseLineStop(str);
	}

	vector<_st_Linestop> vecLine;
	for (set<_st_Linestop>::iterator setiter= m_listLine.begin(); setiter != m_listLine.end(); setiter++)
	{
		vecLine.push_back(*setiter);
	}

	CDataManager* pDM = CDataManager::GetInstance();
	if(pDM)
	{
		pDM->InitData(vecLine, m_curStopid);
	}

	return true;
}

bool CLineStopFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	MessageMap messageType;
	void* pData = pMessage->GetMessageData(messageType);
	string str = (char*)pData;

	if(str == "")
		return false;

	string strUpDate;
	tagDataHead stHead;
	Json::Reader jreader;
	Json::Value jValue;
	if(jreader.parse(str, jValue))
	{
		stHead.messageType = MT_Dataset;

		stHead.command = Message_Linestop;

		stHead.strServerIP = jValue[J_SERVER].asString();

		stHead.strStopID = jValue[J_STOPID].asString();

		stHead.strCityCode = jValue[J_CITYCODE].asString();

		stHead.strMsgID = jValue[J_MSGID].asString();

		m_bUpdate = jValue["isUpdate"].asUInt();

	}


	if(m_bUpdate == 1)
	{
		string strPath = CConfigManager::GetInstance()->GetStaticStopDataPath();
		ofstream ofs(strPath.c_str(), ios::out);
		if(ofs.is_open())
		{
			ofs<<str.c_str();
		}

		ofs.close();
		LogOutInfo("线站数据更新成功");

		m_nUpdateFinish = 1;
		return true;
	}

	LogOutInfo("无新线站数据");
	m_nUpdateFinish = 2;

// 	tagResponse stResponse;
// 	stResponse.head = stHead;
// 	stResponse.head.messageType = MT_Response;
// 	stResponse.nStatus = 200;
// 	stResponse.strMsg = AnsiToUtf8("正常");
// 	CMessage msg;
// 	msg.SetMessage(Message_Response, &stResponse);
// 	CNetManager* pNM = CNetManager::GetNet();
// 	if(pNM)
// 	{
// 		pNM->SendData(msg);
// 	}

	return true;
}

bool CLineStopFunction::AnalyseLineStop(string& strJson)
{
	Json::Reader jreader;
	Json::Value jValue;
	if(jreader.parse(strJson, jValue))
	{
		m_bUpdate = jValue["isUpdate"].asInt();
		m_strHashCode = jValue["hashCode"].asString();
		m_curStopid = jValue["currStopId"].asString();

		Json::Value vlist = jValue["list"];
		for (int ii=0; ii < (int)vlist.size(); ii++)
		{
			Json::Value vAttr = vlist[ii]["attr"];

			_st_Linestop stLine;
			stLine.strID = vAttr["lineid"].asString();
			stLine.strLineName = vAttr["name"].asString();
			stLine.strAdminCode = vAttr["citycode"].asString();
			stLine.strAdminName = vAttr["cityname"].asString();
			stLine.strLength = vAttr["length"].asString();
			stLine.nLineType = atoi(vAttr["linetype"].asString().c_str());
			stLine.strLineDir = vAttr["linedir"].asString();
			stLine.strCard = vAttr["card"].asString();
			stLine.strTimef = vAttr["timef"].asString();
			stLine.strTimeL = vAttr["timel"].asString();
			stLine.nIntervaln = atoi(vAttr["intervaln"].asString().c_str());
			stLine.nIntervalm = atoi(vAttr["intervalm"].asString().c_str());
			stLine.strTicket = vAttr["ticket"].asString();
			stLine.strCompany = vAttr["company"].asString();
			stLine.strMonTicket = vAttr["monthticket"].asString();

			Json::Value vStop =vAttr["stations"];
			for (int jj=0; jj < (int)vStop.size(); jj++)
			{
				_st_Linestop::_st_Stop stStop;
				stStop.strID = vStop[jj]["stopid"].asString();
				stStop.nNumber = atoi(vStop[jj]["number"].asString().c_str());
				stStop.strName = vStop[jj]["name"].asString();
				stStop.strLng = vStop[jj]["lng"].asString();
				stStop.strLat = vStop[jj]["lat"].asString();

				stLine.listStop.push_back(stStop);
			}
			m_listLine.insert(stLine);
		}

	}

	return true;
}