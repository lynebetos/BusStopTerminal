#pragma once
#include "ifunction.h"
#include "tinyxml/tinyxml.h"
#include <map>

class CTemplateFunction :
	public IFunction
{
public:
	CTemplateFunction(void);
	virtual ~CTemplateFunction(void);

	struct _st_template
	{
		string strName;
		string strType;
		CPanelSize size;
		string strColor;
		int nFontsize;
		int nSpeed;
		int nArriveShow; //0:stop 1:distance

		_st_template()
		{
			strName = "";
			strType = "";
			strColor = "";
			nFontsize = 0;
			nSpeed = 0;
			nArriveShow = 0; 
		}
	};

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);
	const vector<_st_template>& GetTemplate() {return m_vecPanel;};
	int GetComPannelCount() {return m_ComPannelCount;}
	string GetHashCode();
	int IsUpDate() {return m_nUpdateFinish;}

private:
	bool AnalyseTemplate(string strPath);
	bool TraverseXMLTree(TiXmlElement* pEl);

private:
	vector< _st_template> m_vecPanel;
	int m_ComPannelCount;	//完整面板的数量
	int m_nUpdateFinish;	//1:有更新 2:无更新
	string m_strHashCode;
	int m_bUpdate;

};
