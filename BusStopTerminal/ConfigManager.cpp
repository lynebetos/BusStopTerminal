#include "StdAfx.h"
#include "ConfigManager.h"

#define System "system"
#define Software "software"
#define Configur "configur"
#define Globle "globle"
#define Sever "server"
#define Notice "notice"
#define Adver "advertisement"
#define Template "template"
#define BusStop "busstop"
#define Layout "layout"
#define Down "down"
#define Local "local"
#define Play "play"
#define Static "static"
#define City "citycode"
#define CurStop "curstop"
#define Volume "volume"
#define Attrib "attribute"
#define StaticData "data"
#define Template "template"
#define Ver "version"
#define Height "height"
#define Width "width"
#define AdverCount "advercount"
#define IP "ip"
#define Port "port"
#define Dir "dir"
#define Type "type"
#define Left "left"
#define Top "top"
#define Right "right"
#define Bottom "bottom"
#define ID "id"
#define Name "name"
#define LineCount "linecount"
#define OneBord "onebord"
#define Picture "pictrue"
#define Default "default"
#define Statistic "statistic"
#define Upload "upload"
#define Hart "hart"
//////////////////////////////////////////////////////////////////////////
//CAvdertisementConfig
CAdvertisementConfig::CAdvertisementConfig(void)
{
}

CAdvertisementConfig::~CAdvertisementConfig(void)
{
}

bool CAdvertisementConfig::AnalyseAdver(TiXmlElement* pElement)
{
	for (TiXmlElement* pChiled = pElement->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
		if(strcmp(Layout, szValue) == 0)
		{
			string strLeft = pChiled->Attribute(Left);
			string strRight = pChiled->Attribute(Right);
			string strTop = pChiled->Attribute(Top);
			string strBottom = pChiled->Attribute(Bottom);

			m_size.left = atoi(strLeft.c_str());
			m_size.right = atoi(strRight.c_str());
			m_size.top = atoi(strTop.c_str());
			m_size.bottom = atoi(strBottom.c_str());
		}
		else if(strcmp(Down, szValue) == 0)
		{
			m_strDownPort = pChiled->Attribute(Port);
		}
		else if(strcmp(Local, szValue) == 0)
		{
			m_strSourceDir = pChiled->Attribute(Dir);
		}
		else if(strcmp(Play, szValue) == 0)
		{
			m_strPlaylist = pChiled->Attribute(Dir);
		}
	}

	return true;
}

bool CAdvertisementConfig::SaveAdver(TiXmlElement* pElement)
{
	for (TiXmlElement* pChiled = pElement->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
		if(strcmp(Down, szValue) == 0)
		{
			pChiled->SetAttribute(Port, m_strDownPort.c_str());
		}
		else if(strcmp(Local, szValue) == 0)
		{
			pChiled->SetAttribute(Dir, m_strSourceDir.c_str());
		}
		else if(strcmp(Play, szValue) == 0)
		{
			pChiled->SetAttribute(Dir, m_strPlaylist.c_str());
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//CBusStopConfig
CBusStopConfig::CBusStopConfig(void)
{
}

CBusStopConfig::~CBusStopConfig(void)
{
}

bool CBusStopConfig::AnalyseBusStop(TiXmlElement* pElement)
{
	for (TiXmlElement* pChiled = pElement->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
		if(strcmp(Layout, szValue) == 0)
		{
			string strLeft = pChiled->Attribute(Left);
			string strRight = pChiled->Attribute(Right);
			string strTop = pChiled->Attribute(Top);
			string strBottom = pChiled->Attribute(Bottom);

			m_size.left = atoi(strLeft.c_str());
			m_size.right = atoi(strRight.c_str());
			m_size.top = atoi(strTop.c_str());
			m_size.bottom = atoi(strBottom.c_str());
		}
		else if(strcmp(Static, szValue) == 0)
		{
			for (TiXmlElement* pStatic = pChiled->FirstChildElement(); pStatic != NULL; pStatic = pStatic->NextSiblingElement())
			{
				const char* szStaticValue = pStatic->Value();
				if(strcmp(City, szStaticValue) == 0)
				{
					m_strCityCode = pStatic->Attribute(ID);
				}
				else if(strcmp(CurStop, szStaticValue) == 0)
				{
					m_strStopID = pStatic->Attribute(ID);
					m_strStopName = pStatic->Attribute(Name);
				}
				else if(strcmp(Attrib, szStaticValue) == 0)
				{
					string strLineCount = pStatic->Attribute(LineCount);
					string strOneBord = pStatic->Attribute(OneBord);
					m_nLineCount = atoi(strLineCount.c_str());
					m_nOneBord = atoi(strOneBord.c_str());
				}
				else if(strcmp(StaticData, szStaticValue) == 0)
				{
					m_strStaticData = pStatic->Attribute(Dir);
				}
			}
		}
	}

	return true;
}

bool CBusStopConfig::SaveBusStop(TiXmlElement* pElement)
{
	for (TiXmlElement* pChiled = pElement->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
// 		if(strcmp(Layout, szValue) == 0)
// 		{
// 			string strLeft = pChiled->Attribute(Left);
// 			string strRight = pChiled->Attribute(Right);
// 			string strTop = pChiled->Attribute(Top);
// 			string strBottom = pChiled->Attribute(Bottom);
// 
// 			m_size.left = atoi(strLeft.c_str());
// 			m_size.right = atoi(strRight.c_str());
// 			m_size.top = atoi(strTop.c_str());
// 			m_size.bottom = atoi(strBottom.c_str());
// 		}
		if(strcmp(Static, szValue) == 0)
		{
			for (TiXmlElement* pStatic = pChiled->FirstChildElement(); pStatic != NULL; pStatic = pStatic->NextSiblingElement())
			{
				const char* szStaticValue = pStatic->Value();
				if(strcmp(City, szStaticValue) == 0)
				{
					pStatic->SetAttribute(ID, m_strCityCode.c_str());
				}
				else if(strcmp(CurStop, szStaticValue) == 0)
				{
					pStatic->SetAttribute(ID, m_strStopID.c_str());
					pStatic->SetAttribute(Name, m_strStopName.c_str());
				}
// 				else if(strcmp(Attrib, szStaticValue) == 0)
// 				{
// 					string strLineCount = pStatic->Attribute(LineCount);
// 					string strOneBord = pStatic->Attribute(OneBord);
// 					m_nLineCount = atoi(strLineCount.c_str());
// 					m_nOneBord = atoi(strOneBord.c_str());
// 				}
				else if(strcmp(StaticData, szStaticValue) == 0)
				{
					pStatic->SetAttribute(Dir, m_strStaticData.c_str());
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//CConfigManager

CConfigManager* CConfigManager::m_pInstance = NULL;


CConfigManager::CConfigManager(void)
: m_strAppVer("")
,m_strConfigVer("")
,m_strHeight("")
,m_strWidth("")
,m_nAdverCount(0)
,m_strIp("")
,m_strPort("")
,m_strNoticeDir("")
{
}

CConfigManager::~CConfigManager(void)
{

}

CConfigManager* CConfigManager::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CConfigManager;
	}
	
	return m_pInstance;
}

void CConfigManager::ReleaseInstance()
{
	if(m_pInstance)
		delete m_pInstance;
}

bool CConfigManager::LoadConfig(string strPath)
{
	m_strConfigFile = strPath;

	return AnalyseConfigFile();
}

bool CConfigManager::SaveConfig()
{
	TiXmlDocument doc;
	doc.LoadFile(m_strConfigFile.c_str());

	TiXmlElement* pRoot = doc.RootElement();
	const char* szRoot = pRoot->Value();
	if(strcmp("system", szRoot) != 0)
		return false;

	for (TiXmlElement* pChiled = pRoot->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
		if(strcmp(Software, szValue) == 0)
		{
			pChiled->SetAttribute(Ver, m_strAppVer.c_str());
		}
		else if(strcmp(Configur, szValue) == 0)
		{
			pChiled->SetAttribute(Ver, m_strConfigVer.c_str());
		}
		else if(strcmp(Sever, szValue) == 0)
		{
			pChiled->SetAttribute(IP, m_strIp.c_str());
			pChiled->SetAttribute(Port, m_strPort.c_str());
		}
		else if(strcmp(Notice, szValue) == 0)
		{
			pChiled->SetAttribute(Dir, m_strNoticeDir.c_str());
		}
		else if(strcmp(Template, szValue) == 0)
		{
			pChiled->SetAttribute(Ver, m_strTempVer.c_str());
			pChiled->SetAttribute(Dir, m_strTemplatePath.c_str());
		}
		else if(strcmp(Adver, szValue) == 0)
		{
			m_Adver.SaveAdver(pChiled);
		}
		else if(strcmp(BusStop, szValue) == 0)
		{
			m_Busstop.SaveBusStop(pChiled);
		}
		else if(strcmp(Picture, szValue) == 0)
		{
			pChiled->SetAttribute(Dir, m_strPictrueDir.c_str());
		}
		else if(strcmp(Default, szValue) == 0)
		{
			pChiled->SetAttribute(Dir, m_strDefaultPath.c_str());
		}
		else if(strcmp(Statistic, szValue) == 0)
		{
			stringstream sstr;
			sstr<<m_nUpload;
			pChiled->SetAttribute(Upload, sstr.str().c_str());
			sstr.str("");
		}
		else if(strcmp(Hart, szValue) == 0)
		{
			stringstream sstr;
			sstr<<m_nHartUp;
			pChiled->SetAttribute(Upload, sstr.str().c_str());
			sstr.str("");
		}
	}

	doc.SaveFile();
	return true;
}

bool CConfigManager::AnalyseConfigFile()
{
	TiXmlDocument doc;
	doc.LoadFile(m_strConfigFile.c_str());
	
	TiXmlElement* pRoot = doc.RootElement();
	const char* szRoot = pRoot->Value();
	if(strcmp("system", szRoot) != 0)
		return false;


	for (TiXmlElement* pChiled = pRoot->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
		if(strcmp(Software, szValue) == 0)
		{
			m_strAppVer = pChiled->Attribute(Ver);
		}
		else if(strcmp(Configur, szValue) == 0)
		{
			m_strConfigVer = pChiled->Attribute(Ver);
		}
		else if(strcmp(Globle, szValue) == 0)
		{
			m_strHeight = pChiled->Attribute(Height);
			m_strWidth = pChiled->Attribute(Width);
			string strAdverCount = pChiled->Attribute(AdverCount);
			m_nAdverCount = atoi(strAdverCount.c_str());
		}
		else if(strcmp(Sever, szValue) == 0)
		{
			m_strIp = pChiled->Attribute(IP);
			m_strPort = pChiled->Attribute(Port);
		}
		else if(strcmp(Notice, szValue) == 0)
		{
			m_strNoticeDir = pChiled->Attribute(Dir);
		}
		else if(strcmp(Template, szValue) == 0)
		{
			m_strTempVer = pChiled->Attribute(Ver);
			m_strTemplatePath = pChiled->Attribute(Dir);
		}
		else if(strcmp(Adver, szValue) == 0)
		{
			m_Adver.AnalyseAdver(pChiled);
		}
		else if(strcmp(BusStop, szValue) == 0)
		{
			m_Busstop.AnalyseBusStop(pChiled);
		}
		else if(strcmp(Picture, szValue) == 0)
		{
			m_strPictrueDir = pChiled->Attribute(Dir);
		}
		else if(strcmp(Default, szValue) == 0)
		{
			m_strDefaultPath = pChiled->Attribute(Dir);
		}
		else if(strcmp(Volume, szValue) == 0)
		{
			m_strVolumeDir = pChiled->Attribute(Dir);
		}
		else if(strcmp(Statistic, szValue) == 0)
		{
			string strUploade = pChiled->Attribute(Upload);
			m_nUpload = atoi(strUploade.c_str());
		}
		else if(strcmp(Hart, szValue) == 0)
		{
			string strHartUp = pChiled->Attribute(Upload);
			m_nHartUp = atoi(strHartUp.c_str());
		}
	}

	return true;
}