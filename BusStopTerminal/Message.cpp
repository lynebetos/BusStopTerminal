#include "StdAfx.h"
#include "Message.h"

CMessage::CMessage(void)
: m_pVoid(NULL)
, m_nBufLen(0)
, m_Type(Message_NULL)
{
	
}

CMessage::~CMessage(void)
{
	ReleasMemer();
}

void CMessage::ReleasMemer()
{
	if(m_pVoid)
	{
		if(m_Type == Message_Login)
		{
			tagAuthentication* st = (tagAuthentication*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Keep)
		{
			tagKeepHart* st = (tagKeepHart*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Notice)
		{
			tagListNotic* st = (tagListNotic*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Playlist)
		{
			tagListPlay* st = (tagListPlay*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Linestop)
		{
			char* pstr = (char*)m_pVoid;
			free(pstr);
		}
		else if(m_Type == Message_Weather)
		{
			tagWeather* st = (tagWeather*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Arrival)
		{
			tagBusArrival* st = (tagBusArrival*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Response)
		{
			tagResponse* st = (tagResponse*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_LinestopRequest)
		{
			tagUpdateLinestop* st = (tagUpdateLinestop*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Config)
		{
			tagConfigUpdate* st = (tagConfigUpdate*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_ConfigRequest)
		{
			tagConfiguer* st = (tagConfiguer*)m_pVoid;
			delete st;			
		}
		else if(m_Type == Message_Template)
		{
			tagTemplateUpdate* st = (tagTemplateUpdate*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_TemplateRequest)
		{
			tagTemplate* st = (tagTemplate*)m_pVoid;
			delete st;			
		}
		else if(m_Type == Message_Statistc)
		{
			StatisticUp *st = (StatisticUp*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Resource)
		{
			tagResourceDown *st = (tagResourceDown*)m_pVoid;
			delete st;
		}
		else if(m_Type == Message_Volume)
		{
			tagVolumeConf *st = (tagVolumeConf*)m_pVoid;
			delete st;
		}
		m_pVoid = NULL;

	}
}

bool CMessage::SetMessage(const MessageMap Type, void* pData)
{
	m_Type = Type;
	ReleasMemer();
	if(m_Type == Message_Login)
	{
		m_nBufLen = sizeof(tagAuthentication);
		tagAuthentication* st = new tagAuthentication;
		*st = *((tagAuthentication*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Keep)
	{
		m_nBufLen = sizeof(tagKeepHart);
		tagKeepHart* st = new tagKeepHart;
		*st = *((tagKeepHart*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Notice)
	{
		m_nBufLen = sizeof(tagListNotic);
		tagListNotic* st = new tagListNotic;
		*st = *((tagListNotic*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Playlist)
	{
		m_nBufLen = sizeof(tagListPlay);
		tagListPlay* st = new tagListPlay;
		*st = *((tagListPlay*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Linestop)
	{
		string* pstr = (string*)pData;
		m_nBufLen = pstr->size() + 1;
		m_pVoid = malloc(m_nBufLen);
		memset(m_pVoid, 0, m_nBufLen);
		memcpy(m_pVoid, pstr->c_str(), pstr->size());
	}
	else if(m_Type == Message_Weather)
	{
		m_nBufLen = sizeof(tagWeather);
		tagWeather* st = new tagWeather;
		*st = *((tagWeather*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Arrival)
	{
		m_nBufLen = sizeof(tagBusArrival);
		tagBusArrival* st = new tagBusArrival;
		*st = *((tagBusArrival*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Response)
	{
		m_nBufLen = sizeof(tagResponse);
		tagResponse* st = new tagResponse;
		*st = *((tagResponse*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_LinestopRequest)
	{
		m_nBufLen = sizeof(tagUpdateLinestop);
		tagUpdateLinestop* st = new tagUpdateLinestop;
		*st = *((tagUpdateLinestop*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Config)
	{
		m_nBufLen = sizeof(tagConfigUpdate);
		tagConfigUpdate* st = new tagConfigUpdate;
		*st = *((tagConfigUpdate*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_ConfigRequest)
	{
		m_nBufLen = sizeof(tagConfiguer);
		tagConfiguer* st = new tagConfiguer;
		*st = *((tagConfiguer*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Template)
	{
		m_nBufLen = sizeof(tagTemplateUpdate);
		tagTemplateUpdate* st = new tagTemplateUpdate;
		*st = *((tagTemplateUpdate*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_TemplateRequest)
	{
		m_nBufLen = sizeof(tagTemplate);
		tagTemplate* st = new tagTemplate;
		*st = *((tagTemplate*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Statistc)
	{
		m_nBufLen = sizeof(StatisticUp);
		StatisticUp *st = new StatisticUp;
		*st = *((StatisticUp*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Resource)
	{
		m_nBufLen = sizeof(tagResourceDown);
		tagResourceDown *st = new tagResourceDown;
		*st = *((tagResourceDown*)pData);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Volume)
	{
		m_nBufLen = sizeof(tagVolumeConf);
		tagVolumeConf *st = new tagVolumeConf;
		*st = *((tagVolumeConf*)pData);
		m_pVoid = (void*)st;
	}

	return false;
}

void* CMessage::GetMessageData(MessageMap& Type)
{
	Type = m_Type;
	
	return m_pVoid;
}

CMessage& CMessage::operator =(const CMessage& newObj)
{
	ReleasMemer();
	m_nBufLen = newObj.m_nBufLen;
	m_Type = newObj.m_Type;
	if(m_Type == Message_Login)
	{
		tagAuthentication* st = NULL;
		st = new tagAuthentication;
		*st = *((tagAuthentication*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Keep)
	{
		tagKeepHart* st = NULL;
		st = new tagKeepHart;
		*st = *((tagKeepHart*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Notice)
	{
		tagListNotic* st = NULL;
		st = new tagListNotic;
		*st = *((tagListNotic*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Playlist)
	{
		tagListPlay* st = NULL;
		st = new tagListPlay;
		*st = *((tagListPlay*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Linestop)
	{
		char* pstr = (char*)newObj.m_pVoid;
		m_nBufLen = strlen(pstr) + 1;
		m_pVoid = malloc(m_nBufLen);
		memset(m_pVoid, 0, m_nBufLen);
		memcpy(m_pVoid, pstr, strlen(pstr));
	}
	else if(m_Type == Message_Weather)
	{
		tagWeather* st = NULL;
		st = new tagWeather;
		*st = *((tagWeather*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Arrival)
	{
		tagBusArrival* st = NULL;
		st = new tagBusArrival;
		*st = *((tagBusArrival*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Response)
	{
		tagResponse* st = new tagResponse;
		*st = *((tagResponse*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_LinestopRequest)
	{
		tagUpdateLinestop* st = new tagUpdateLinestop;
		*st = *((tagUpdateLinestop*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Config)
	{
		tagConfigUpdate* st = new tagConfigUpdate;
		*st = *((tagConfigUpdate*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_ConfigRequest)
	{
		tagConfiguer* st = new tagConfiguer;
		*st = *((tagConfiguer*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Template)
	{
		tagTemplateUpdate* st = new tagTemplateUpdate;
		*st = *((tagTemplateUpdate*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_TemplateRequest)
	{
		tagTemplate* st = new tagTemplate;
		*st = *((tagTemplate*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Statistc)
	{
		StatisticUp *st = new StatisticUp;
		*st = *((StatisticUp*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Resource)
	{
		tagResourceDown *st = new tagResourceDown;
		*st = *((tagResourceDown*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}
	else if(m_Type == Message_Volume)
	{
		tagVolumeConf *st = new tagVolumeConf;
		*st = *((tagVolumeConf*)newObj.m_pVoid);
		m_pVoid = (void*)st;
	}

	return *this;
}