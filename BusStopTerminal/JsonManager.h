#pragma once
#include "json/json.h"
#include "Message.h"

#ifdef DEBUG
#pragma comment(lib, "json_vc71_libmtd.lib")
#else
#pragma comment(lib, "json_vc71_libmt.lib")
#endif


class CJsonManager
{
public:
	CJsonManager(void);
	~CJsonManager(void);

private:
	bool AnalyseHead(Json::Value& refValue);
	bool AnalyseData(Json::Value& refValue);

	//toMessage
	//request
	bool AnalyseResponse(Json::Value& refValue);

	//Commander
	bool AnalyseNotice(Json::Value& refValue);
	bool AnalysePlayList(Json::Value& refValue);
	bool AnalyseWeather(Json::Value& refValue);
	bool AnalyseBusArrival(Json::Value& refValue);
	bool AnalyseConfig(Json::Value& refValue);
	bool AnalyseTemplate(Json::Value& refValue);
	bool AnalyseResource(Json::Value& refValue);
	bool AnalyseVolumeConf(Json::Value& refValue);

	bool GenerateHead(const tagDataHead& refHead, Json::Value& refValue);
	//tojson
	//request
	bool GenerateLogin(const tagAuthentication* pAuthentication, string& strJson);
	bool GenarateKeep(const tagKeepHart* pKeepHart, string& strJson);
	bool GenerateLinestop(const tagUpdateLinestop* pUpdateLinestop, string& strJson);
	bool GenerateConfig(const tagConfiguer* pConfiguer, string& strJson);
	bool GenerateTemplate(const tagTemplate* pTemplate, string& strJson);
	bool GenerateStatistic(const StatisticUp* pStatisticUp, string& strJson);
	//Conmmander
	bool GenerateResponse(const tagResponse* refResponse, string& strJson);
	

public:
	bool AnalyseJson(string& strValue);
	Message_Type GetMessageType() {return m_stHead.messageType;};
	const CMessage& GetMessage() {return m_Message;};
	bool ToJson(CMessage& message, string& strJson);

private:
	tagDataHead m_stHead;
	CMessage m_Message;
	string m_strJson;
};
