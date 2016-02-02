#include "StdAfx.h"
#include "TemplateFunction.h"
#include "ConfigManager.h"
#include "Message.h"

#define CharToReal(c) 				if(c >= '0' && c <= '9'){c -= '0';}	\
												else if(c >= 'A' && c <= 'Z'){c -= 'A';c += 10;}	\
												else if(c >= 'a' && c <= 'z'){c -= 'a';c += 10;} \


CTemplateFunction::CTemplateFunction(void)
{
	m_ComPannelCount = 0;
	m_nUpdateFinish = 0;
}

CTemplateFunction::~CTemplateFunction(void)
{
	LogObject;
	LogOutInfo("清除模板更新模块");
}

bool CTemplateFunction::InitFunction()
{
	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		string strTyPath = pCM->GetTemplatePath();
		m_strHashCode = pCM->GetTemplateVer();
		return AnalyseTemplate(strTyPath);
	}
	return false;
}

bool CTemplateFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	MessageMap messageType;
	void* pData = pMessage->GetMessageData(messageType);

	if(pData == NULL)
	{
		LogOutError("模板更新数据错误");
		return false;
	}

	tagTemplateUpdate* pTemplate = (tagTemplateUpdate*)pData;

	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pCM)
	{
		m_bUpdate = pTemplate->nUpdate;
		if(m_bUpdate == 1)
		{
			m_strHashCode = pTemplate->strHashCode;
			string strTyPath = pCM->GetTemplatePath();
			pCM->SetTemplateVersion(m_strHashCode);

			string strTValue = "";
			for (int ii = 0; ii < pTemplate->strTemplateValue.size(); ii++)
			{
				char c = pTemplate->strTemplateValue.at(ii);
				CharToReal(c);
				char cReal = 0;
				cReal |= c << 4 & 0xf0;
				ii++;
				c = pTemplate->strTemplateValue.at(ii);
				CharToReal(c);
				cReal |= c  & 0x0f;

				strTValue += cReal;
			}

			int a = 0;
			strTValue;

			ofstream ofs(strTyPath.c_str(), ios::out);

			if(ofs.is_open())
			{
				ofs<<strTValue;
			}

			ofs.close();
		}

	}

	if(m_bUpdate == 1)
	{
		LogOutInfo("模板数据更新成功");
		pCM->SaveConfig();

		m_nUpdateFinish = 1;
		return true;
	}

	LogOutInfo("无新模板数据");
	m_nUpdateFinish = 2;

	return true;
}

bool CTemplateFunction::AnalyseTemplate(string strPath)
{
	TiXmlDocument doc;
	if(!doc.LoadFile(strPath.c_str()))
	{
		return false;
	}

	TiXmlElement* pRoot = doc.RootElement();

	if(pRoot == NULL)
		return false;
	
	_st_template stTL;
	CPanelSize panelSize;
	string strLeft = pRoot->Attribute("left");
	string strRight = pRoot->Attribute("right");
	string strTop = pRoot->Attribute("top");
	string strBottom = pRoot->Attribute("bottom");

	panelSize.left = atoi(strLeft.c_str());
	panelSize.right = atoi(strRight.c_str());
	panelSize.top = atoi(strTop.c_str());
	panelSize.bottom = atoi(strBottom.c_str());

	stTL.size = panelSize;
	stTL.strName = "root";
	stTL.strType ="root";

	m_vecPanel.push_back(stTL);

	return TraverseXMLTree(pRoot);
		
}

bool CTemplateFunction::TraverseXMLTree(TiXmlElement* pEl)
{
	for(TiXmlElement* pChild = pEl->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		TraverseXMLTree(pChild);
		const char* szValue = pChild->Value();
		if(strcmp("range", szValue) == 0)
		{
			_st_template stTL;
			CPanelSize panelSize;
			string strLeft = pChild->Attribute("left");
			string strRight = pChild->Attribute("right");
			string strTop = pChild->Attribute("top");
			string strBottom = pChild->Attribute("bottom");
			string strName = pChild->Attribute("name");
			const char* szFontSize = pChild->Attribute("fontsize");
			if(szFontSize != NULL)
			{
				stTL.nFontsize = atoi(szFontSize);
			}
			const char* szColor = pChild->Attribute("color");
			if(szFontSize != NULL)
			{
				stTL.strColor = szColor;
			}

			panelSize.left = atoi(strLeft.c_str());
			panelSize.right = atoi(strRight.c_str());
			panelSize.top = atoi(strTop.c_str());
			panelSize.bottom = atoi(strBottom.c_str());

			stTL.size = panelSize;
			stTL.strName = strName;
			stTL.strType = pChild->Attribute("type");
			if(stTL.strType == "complicate")
			{
				m_ComPannelCount++;
				string strArrivel = pChild->Attribute("arriveshow");
				if(strArrivel == "stop")
					stTL.nArriveShow = 0;
				else if(strArrivel == "dis")
					stTL.nArriveShow = 1;
			}

			m_vecPanel.push_back(stTL);
		}
	}

	return true;
}

string CTemplateFunction::GetHashCode()
{
	return m_strHashCode;
};
