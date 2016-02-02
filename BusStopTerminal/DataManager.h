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
	map<string, CData*> m_mapData; //线路ID和Data的对应
	map<string, string> m_mapLineID; //线路号和线路ID的对应
	vector<string> m_vecLineNum;	//顺序存储线路号码
	set<int> m_setLineNum; //对线路进行排序
};
