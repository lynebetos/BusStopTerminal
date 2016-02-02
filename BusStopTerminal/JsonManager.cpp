#include "StdAfx.h"
#include "JsonManager.h"


CJsonManager::CJsonManager(void)
{

}

CJsonManager::~CJsonManager(void)
{

}

bool CJsonManager::AnalyseJson(string& strValue)
{
	LogObject;
	Json::Reader jreader;
	Json::Value jValue;

	if(jreader.parse(strValue, jValue))
	{
		AnalyseHead(jValue);

		if(m_stHead.messageType == MT_Dataset)
		{
			AnalyseData(jValue);
		}
		else if(m_stHead.messageType == MT_Response)
		{
			AnalyseResponse(jValue);
		}
		if(m_stHead.command == Message_Linestop)
		{ //单独处理线站数据
			m_Message.SetMessage(m_stHead.command, (void*)&strValue);
		}
	}

	return true;
}

bool CJsonManager::AnalyseHead(Json::Value& refValue)
{
	string strType = refValue[J_TYPE].asString();
	if(strType == Response)
	{
		m_stHead.messageType = MT_Response;
	}
	else if(strType == Dataset)
	{
		m_stHead.messageType = MT_Dataset;
	}

	string strCmd = refValue[J_CMD].asString();
	if(strCmd == login)
	{
		m_stHead.command = Message_Login;
	}
	else if(strCmd == lineStop)
	{
		m_stHead.command = Message_Linestop;
	}
	else if(strCmd == busArrival)
	{
		m_stHead.command = Message_Arrival;
	}
	else if(strCmd == keepConn)
	{
		m_stHead.command = Message_Keep;
	}
	else if(strCmd == noticeList)
	{
		m_stHead.command = Message_Notice;
	}
	else if(strCmd == adPlayList)
	{
		m_stHead.command = Message_Playlist;
	}
	else if(strCmd == adPalyTotal)
	{
		m_stHead.command = Message_Statistc;
	}
	else if(strCmd == monitor)
	{
		m_stHead.command = Message_Monitor;
	}
	else if(strCmd == weatherInfo)
	{
		m_stHead.command = Message_Weather;
	}
	else if(strCmd == shutDown)
	{
		m_stHead.command = Message_Shutdown;
	}
	else if(strCmd == reboot)
	{
		m_stHead.command = Message_Reboot;
	}
	else if(strCmd == configuer)
	{
		m_stHead.command = Message_Config;
	}
	else if(strCmd == templateUp)
	{
		m_stHead.command = Message_Template;
	}
	else if(strCmd == resource)
	{
		m_stHead.command = Message_Resource;
	}
	else if(strCmd == volume)
	{
		m_stHead.command = Message_Volume;
	}

	m_stHead.strServerIP = refValue[J_SERVER].asString();

	m_stHead.strStopID = refValue[J_STOPID].asString();

	m_stHead.strCityCode = refValue[J_CITYCODE].asString();

	m_stHead.strMsgID = refValue[J_MSGID].asString();

	return true;
}

bool CJsonManager::AnalyseData(Json::Value& refValue)
{
	switch(m_stHead.command)
	{
// 	case	Message_Login:
// 	case Message_Keep:
// 	case Message_Monitor:
// 	case Message_Statistc:
// 		AnalyseResponse(refValue);
//		break;
	case	Message_Application:
		break;
	case Message_Linestop:
		break;
	case Message_Config:
		break;
	case  Message_Template:
		break;
	case Message_Notice:
		AnalyseNotice(refValue);
		break;
	case Message_Playlist:
		AnalysePlayList(refValue);
		break;
	case Message_Arrival:
		AnalyseBusArrival(refValue);
		break;
	case Message_Weather:
		AnalyseWeather(refValue);
		break;
	case Message_Resource:
		AnalyseResource(refValue);
		break;
	case  Message_Volume:
		AnalyseVolumeConf(refValue);
		break;
	case	Message_Shutdown:
	case	Message_Reboot:
		m_Message.SetCommand(m_stHead.command);
		break;
	default:
		break;

	}
	return true;
}

bool CJsonManager::AnalyseResponse(Json::Value& refValue)
{
	LogObject;
	tagResponse stResp;
	stResp.head = m_stHead;
	stResp.nStatus = refValue[J_STATUS].asInt();
	stResp.strMsg = refValue[J_MSG].asString();
	stResp.strMsg = Utf8toAnsi(stResp.strMsg);

	//m_Message.SetMessage(stResp.head.command, (void*)&stResp);

	string strInfo;

	switch(m_stHead.command)
	{
 	case	Message_Login:
		strInfo = "登陆应答:";
		strInfo += stResp.strMsg;
		LogOutInfo(strInfo.c_str());
		break;
 	case Message_Keep:
		strInfo = "心跳应答:";
		strInfo += stResp.strMsg;
		LogOutInfo(strInfo.c_str());
		break;
	case Message_Monitor:
		strInfo = "监控应答:";
		strInfo += stResp.strMsg;
		LogOutInfo(strInfo.c_str());
		break;
	case Message_Statistc: 		
		strInfo = "统计应答:";
		strInfo += stResp.strMsg;
		LogOutInfo(strInfo.c_str());
		break;
	case  Message_Template:
		AnalyseTemplate(refValue);
		break;
	case Message_Config:
		AnalyseConfig(refValue);
		break;
	}

	return true;
}

bool CJsonManager::AnalyseConfig(Json::Value& refValue)
{
	tagConfigUpdate stConfigUpdate;
	stConfigUpdate.head = m_stHead;
	
	stConfigUpdate.nUpdate = refValue["isUpdate"].asUInt();
	Json::Value jConf = refValue["confValue"];
	stConfigUpdate.strHashCode = jConf["hashCode"].asString();
	stConfigUpdate.strCurrStopid = jConf["currStopid"].asString();
	stConfigUpdate.strNoticeDir = jConf["noticeDir"].asString();
	stConfigUpdate.strTemplateDir = jConf["templateDir"].asString();
	stConfigUpdate.strPictrueDir = jConf["pictrueDir"].asString();
	stConfigUpdate.strDefaultDir = jConf["defaultDir"].asString();
	stConfigUpdate.strAdvDir = jConf["advDir"].asString();
	stConfigUpdate.strAdvPlaylist = jConf["advPlaylist"].asString();
	stConfigUpdate.strStopName = jConf["stopName"].asString();
	stConfigUpdate.strUpdateDown = jConf["updateDown"].asString();
	stConfigUpdate.strServerIP = jConf["serverIP"].asString();
	stConfigUpdate.strLineDataDir = jConf["lineDataDir"].asString();
	stConfigUpdate.strStatisticCycle = jConf["statisticUp"].asString();
	stConfigUpdate.strHartCycle = jConf["hartUp"].asString();

	stConfigUpdate.strNoticeDir = ReplaceAll(stConfigUpdate.strNoticeDir, "$", "\\");
	stConfigUpdate.strTemplateDir = ReplaceAll(stConfigUpdate.strTemplateDir, "$", "\\");
	stConfigUpdate.strPictrueDir = ReplaceAll(stConfigUpdate.strPictrueDir, "$", "\\");
	stConfigUpdate.strDefaultDir = ReplaceAll(stConfigUpdate.strDefaultDir, "$", "\\");
	stConfigUpdate.strAdvDir = ReplaceAll(stConfigUpdate.strAdvDir, "$", "\\");
	stConfigUpdate.strLineDataDir = ReplaceAll(stConfigUpdate.strLineDataDir, "$", "\\");
	stConfigUpdate.strAdvPlaylist = ReplaceAll(stConfigUpdate.strAdvPlaylist, "$", "\\");

	m_Message.SetMessage(stConfigUpdate.head.command, (void*)&stConfigUpdate);

	return true;
}

bool CJsonManager::AnalyseTemplate(Json::Value& refValue)
{
	tagTemplateUpdate stTemplateUpdate;
	stTemplateUpdate.head = m_stHead;

	stTemplateUpdate.nUpdate = refValue["isUpdate"].asUInt();
	stTemplateUpdate.strHashCode = refValue["hashCode"].asString();
	stTemplateUpdate.strCurrStopid = refValue["currStopid"].asString();
	stTemplateUpdate.strTemplateValue = refValue["templateValue"].asString();

	m_Message.SetMessage(stTemplateUpdate.head.command, (void*)&stTemplateUpdate);

	return true;
}

bool CJsonManager::AnalyseNotice(Json::Value& refValue)
{
	tagListNotic stListNotice;
	stListNotice.sthead = m_stHead;

	Json::Value Result = refValue[J_ARRAY];
	for (int ii = 0; ii < (int)Result.size(); ii++)
	{
		tagNotice stNotice;
		stNotice.strID = Result[ii]["id"].asString();
		stNotice.strStartDate = Result[ii]["startdate"].asString();
		stNotice.strStartTime = Result[ii]["starttime"].asString();
		stNotice.strEndDate = Result[ii]["enddate"].asString();
		stNotice.strEndTime = Result[ii]["endtime"].asString();
		stNotice.strInterval = Result[ii]["interval"].asString();
		stNotice.strOptr = Result[ii]["oper"].asString();
		stNotice.strDest = Result[ii]["dest"].asString();

		Json::Value vNotice = Result[ii]["notices"];
		for (int jj = 0; jj< (int)vNotice.size(); jj++)
		{
			tagNotice::_st_list stUnit;
			stUnit.strNoticeID = vNotice[jj]["noticeid"].asString();
			stUnit.strText = vNotice[jj]["content"].asString();

			stNotice.listNotice.push_back(stUnit);
		}

		stListNotice.listListNotice.push_back(stNotice);
	}

	m_Message.SetMessage(stListNotice.sthead.command, (void*)&stListNotice);
	return true;
}

bool CJsonManager::AnalysePlayList(Json::Value& refValue)
{
	tagListPlay stListPlay;
	stListPlay.sthead = m_stHead;
	Json::Value jList = refValue[J_ARRAY];

	tagPlay stPlay;
	stPlay.strStartDate = jList["startdate"].asString();
	stPlay.strEndDate = jList["enddate"].asString();

	Json::Value Result = jList["result"];
	for (int ii = 0; ii< (int)Result.size(); ii++)
	{
		stPlay.strStartTime = Result[ii]["starttime"].asString();
		stPlay.strEndTime = Result[ii]["endtime"].asString();

		Json::Value vPlay = Result[ii]["advertisment"];
		for (int jj = 0; jj< (int)vPlay.size(); jj++)
		{
			tagPlay::_st_list stUnit;
			stUnit.strID = vPlay[jj]["id"].asString();
			stUnit.strName = vPlay[jj]["name"].asString();
			stUnit.strNum = vPlay[jj]["num"].asString();

			stPlay.listPlay.push_back(stUnit);
		}

		stListPlay.listListPlay.push_back(stPlay);
		stPlay.listPlay.clear();
	}

	m_Message.SetMessage(stListPlay.sthead.command, (void*)&stListPlay);
	return true;
}

bool CJsonManager::AnalyseWeather(Json::Value& refValue)
{
	tagWeather stWeather;
	stWeather.sthead = m_stHead;

	stWeather.strCity = refValue["city"].asString();
	stWeather.strCityCode = refValue["cityCode"].asString();
	stWeather.strTwd = refValue["dytwd"].asString();
	stWeather.strTtq = refValue["dyttq"].asString();
	stWeather.strTtp = refValue["dyttp"].asString();
	stWeather.strTfx = refValue["dytfx"].asString();
	stWeather.strSkqw = refValue["skqw"].asString();
	stWeather.strSkfx = refValue["skfx"].asString();
	stWeather.strSksd = refValue["sksd"].asString();
	stWeather.strSkqy = refValue["skqy"].asString();

	m_Message.SetMessage(stWeather.sthead.command, (void*)&stWeather);
	return true;
}

bool CJsonManager::AnalyseBusArrival(Json::Value& refValue)
{
	Json::Value jArrival = refValue["list"];
	tagBusArrival stBusArrival;
	stBusArrival.stHead = m_stHead;

	for (int ii = 0; ii < (int)jArrival.size(); ii++)
	{
		tagArrival stArrival;
		stArrival.strLineID = jArrival[ii]["lineid"].asString();

		Json::Value jVehicles = jArrival[ii]["vehicles"];
		for (int jj = 0; jj < (int)jVehicles.size(); jj++)
		{
			BusArrivalInfo stInfo;
			stInfo.iNum = jVehicles[jj]["num"].asInt();
			stInfo.iDistance = jVehicles[jj]["dist"].asInt();
			stInfo.iTime = jVehicles[jj]["time"].asInt();
			stInfo.dLati = jVehicles[jj]["lat"].asDouble();
			stInfo.dLongiti = jVehicles[jj]["lng"].asDouble();

			stArrival.vecArrivalInfo.push_back(stInfo);
		}
		stBusArrival.mapLineArrival.insert(make_pair(stArrival.strLineID, stArrival));
	}

	m_Message.SetMessage(stBusArrival.stHead.command, (void*)&stBusArrival);
	return true;
}

bool CJsonManager::AnalyseResource(Json::Value& refValue)
{
	Json::Value jResource = refValue["list"];
	tagResourceDown stResourceDown;
	stResourceDown.head = m_stHead;

	for (int ii = 0; ii < (int)jResource.size(); ii++)
	{
		tagResourceDown::_st_Resource stResource;
		stResource.strUrl = jResource[ii]["url"].asString();
		stResource.strType = jResource[ii]["type"].asString();

		stResourceDown.vecResource.push_back(stResource);
	}

	m_Message.SetMessage(stResourceDown.head.command, (void*)&stResourceDown);

	return true;
}

bool CJsonManager::AnalyseVolumeConf(Json::Value& refValue)
{
	Json::Value jVolumeConf = refValue["list"];
	tagVolumeConf stVolumeConf;
	stVolumeConf.head = m_stHead;

	for (int ii = 0; ii < (int)jVolumeConf.size(); ii++)
	{
		tagVolumeConf::_st_Volume stVolume;
		stVolume.strTime = jVolumeConf[ii]["time"].asString();
		stVolume.strVolume = jVolumeConf[ii]["volNum"].asString();

		stVolumeConf.vecVolume.push_back(stVolume);
	}

	m_Message.SetMessage(stVolumeConf.head.command, (void*)&stVolumeConf);
	
	return true;
}

bool CJsonManager::ToJson(CMessage& message, string& strJson)
{
	MessageMap messageMap;
	void* pData = message.GetMessageData(messageMap);

	switch(messageMap)
	{
	case	Message_Login:
		GenerateLogin((tagAuthentication*)pData, strJson);
		break;
	case Message_Keep:
		GenarateKeep((tagKeepHart*)pData, strJson);
		break;
	case Message_Monitor:
	case Message_Statistc:
		GenerateStatistic((StatisticUp*)pData, strJson);
		break;
	case	Message_Application:
	case Message_LinestopRequest:
		GenerateLinestop((tagUpdateLinestop*)pData, strJson);
		break;
	case Message_ConfigRequest:
		GenerateConfig((tagConfiguer*)pData, strJson);
		break;
	case  Message_TemplateRequest:
		GenerateTemplate((tagTemplate*)pData, strJson);
		break;
	case Message_Notice:
	case Message_Playlist:
	case Message_Arrival:
	case Message_Weather:
	case Message_Response:
		GenerateResponse((tagResponse*)pData, strJson);
		break;
	case	Message_Shutdown:
	case	Message_Reboot:

	default:
		break;

	}

	if(strJson == "")
		return false;

	return true;
}

bool CJsonManager::GenerateHead(const tagDataHead& refHead, Json::Value& refValue)
{
	string strType = "";

	if(refHead.messageType == MT_Response)
	{
		strType = Response;
	}
	else if(refHead.messageType == MT_Dataset)
	{
		strType = Dataset;
	}

	string strCmd = "";
	if(refHead.command == Message_Login)
	{
		strCmd = login;
	}
	else if(refHead.command == Message_Linestop)
	{
		strCmd = lineStop;
	}
	else if(refHead.command == Message_Config)
	{
		strCmd = configuer;
	}
	else if(refHead.command == Message_Template)
	{
		strCmd = templateUp;
	}
	else if(refHead.command == Message_Arrival)
	{
		strCmd = busArrival;
	}
	else if(refHead.command == Message_Keep)
	{
		strCmd = keepConn;
	}
	else if(refHead.command == Message_Notice)
	{
		strCmd = noticeList;
	}
	else if(refHead.command == Message_Playlist)
	{
		strCmd = adPlayList;
	}
	else if(refHead.command == Message_Statistc)
	{
		strCmd = adPalyTotal;
	}
	else if(refHead.command == Message_Monitor)
	{
		strCmd = monitor;
	}
	else if(refHead.command == Message_Weather)
	{
		strCmd = weatherInfo;
	}
	else if(refHead.command == Message_Shutdown)
	{
		strCmd = shutDown;
	}
	else if(refHead.command == Message_Reboot)
	{
		strCmd = reboot;
	}
	else if(refHead.command == Message_Resource)
	{
		strCmd = resource;
	}
	else if(refHead.command == Message_Volume)
	{
		strCmd = volume;
	}

	refValue[J_TYPE] = strType;
	refValue[J_CMD] = strCmd;
	refValue[J_SERVER] = refHead.strServerIP;
	refValue[J_CITYCODE] = refHead.strCityCode;
	refValue[J_STOPID] = refHead.strStopID;
	refValue[J_MSGID] = refHead.strMsgID;
	return true;
}

bool CJsonManager::GenerateLogin(const tagAuthentication* pAuthentication, string& strJson)
{
	Json::Value root;

	GenerateHead(pAuthentication->head, root);
	root["cipher"] = pAuthentication->strKey;

	strJson = root.toStyledString();
	return true;
}

bool CJsonManager::GenerateResponse(const tagResponse* pResponse, string& strJson)
{
	Json::Value root;

	GenerateHead(pResponse->head, root);
	root["status"] = pResponse->nStatus;
	root["msg"] = pResponse->strMsg;

	strJson = root.toStyledString();

	return true;
}

bool CJsonManager::GenarateKeep(const tagKeepHart* pKeepHart, string& strJson)
{
	Json::Value root;

	GenerateHead(pKeepHart->head, root);


	Json::Value jmonitor;
	for (int ii = 0; ii < pKeepHart->vecMonitor.size(); ii++)
	{
		jmonitor[ii]["monkey"]=pKeepHart->vecMonitor[ii].strKey;
		jmonitor[ii]["monValue"]=pKeepHart->vecMonitor[ii].strValue;
	}

	root["monitor"] = jmonitor;

	Json::Value jWarnning;
	for (int ii = 0; ii < pKeepHart->vecWarnning.size(); ii++)
	{
		jWarnning[ii]["warcode"]=pKeepHart->vecWarnning[ii];
	}

	root["warnning"] = jWarnning;

	strJson = root.toStyledString();

	return true;
}

bool CJsonManager::GenerateLinestop(const tagUpdateLinestop* pUpdateLinestop, string& strJson)
{
	Json::Value root;

	GenerateHead(pUpdateLinestop->head, root);
	root["hashCode"] = pUpdateLinestop->strHash;

	strJson = root.toStyledString();

	return true;

}

bool CJsonManager::GenerateConfig(const tagConfiguer* pConfiguer, string& strJson)
{
	Json::Value root;

	GenerateHead(pConfiguer->head, root);
	root["hashCode"] = pConfiguer->strHash;

	strJson = root.toStyledString();

	return true;
}

bool CJsonManager::GenerateTemplate(const tagTemplate* pTemplate, string& strJson)
{
	Json::Value root;

	GenerateHead(pTemplate->head, root);
	root["hashCode"] = pTemplate->strHash;

	strJson = root.toStyledString();

	return true;
}

bool CJsonManager::GenerateStatistic(const StatisticUp* pStatisticUp, string& strJson)
{
	Json::Value root;

	GenerateHead(pStatisticUp->head, root);

	Json::Value jStatistic;
	for (int ii = 0; ii < pStatisticUp->vecStat.size(); ii++)
	{
		jStatistic[ii]["name"]=pStatisticUp->vecStat[ii].strName;
		Json::Value jRecorde;
		for (int jj = 0; jj < pStatisticUp->vecStat[ii].vecData.size(); jj++)
		{
			jRecorde[jj]["date"] = pStatisticUp->vecStat[ii].vecData[jj].strDate;
			jRecorde[jj]["startTime"] = pStatisticUp->vecStat[ii].vecData[jj].strStartTime;
			jRecorde[jj]["endTime"] = pStatisticUp->vecStat[ii].vecData[jj].strEndTime;			
		}
		jStatistic[ii]["recode"] = jRecorde;
	}

	if(pStatisticUp->vecStat.size() == 0)
	{
		int i = 0;
		jStatistic[i]["name"]="";
		Json::Value jRecorde;
		jRecorde[i]["date"] = "";
		jRecorde[i]["startTime"] = "";
		jRecorde[i]["endTime"] = "";			
		jStatistic[i]["recode"] = jRecorde;
	}

	root["statirstic"] = jStatistic;
	strJson = root.toStyledString();

	return true;
}