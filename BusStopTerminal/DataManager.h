#pragma once
#include "Data.h"
#include <map>
#include <vector>
#include <set>

class CDataManager
{
public:
	CDataManager(void);
	~CDataManager(void);

	static CDataManager* GetInstance();
	static void ReleaseInstance();
	bool InitData(const vector<_st_Linestop>& refLinestop, string strCurStopId);
	bool GetAllData(vector<CData*>& refDataPoint);  
	bool SetArrivalData(const tagArrival& stArrival);
	void CleanArrivalData();


private:
	static CDataManager* m_pInstance;
	map<string, CData*> m_mapData; //��·ID��Data�Ķ�Ӧ
	map<string, string> m_mapLineID; //��·�ź���·ID�Ķ�Ӧ
	vector<string> m_vecLineNum;	//˳��洢��·����
	set<int> m_setLineNum; //����·��������
};
