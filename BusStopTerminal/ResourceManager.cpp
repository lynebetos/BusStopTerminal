#include "StdAfx.h"
#include "ResourceManager.h"
#include "tinyxml/tinyxml.h"

CResourceManager* CResourceManager::m_pInstance = NULL;

CResourceManager::CResourceManager(void)
: m_strWeather("../Resource/weather/")
{
}

CResourceManager::~CResourceManager(void)
{
}

CResourceManager* CResourceManager::GetInstacne()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CResourceManager;
	}

	return m_pInstance;
}

void CResourceManager::ReleaseInstance()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

bool CResourceManager::LoadResousce()
{
	//if(!LoadVideoResource())
		//return false;

	return true;
}

bool CResourceManager::LoadVideoResource()
{
	string strPath = "../Resource/video.xml";

	TiXmlDocument doc;

	if(!doc.LoadFile(strPath.c_str()))
		return false;

	TiXmlElement* pRoot = doc.RootElement();

	for (TiXmlElement* pChiled = pRoot->FirstChildElement(); pChiled != NULL; pChiled = pChiled->NextSiblingElement())
	{
		const char* szValue = pChiled->Value();
		if(strcmp("rc", szValue) == 0)
		{
			string strId = pChiled->Attribute("id");
			string strName = pChiled->Attribute("name");

			m_mapVideoSrc.insert(make_pair(strId, strName));
		}
	}
	return true;

}