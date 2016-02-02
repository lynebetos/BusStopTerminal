#pragma once
#include "ifunction.h"
#include <vector>
#include <set>

class CLineStopFunction :
	public IFunction
{
public:
	CLineStopFunction(void);
	virtual ~CLineStopFunction(void);

	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	string GetHashCode() {return m_strHashCode;};
	int GetLineStopMoniter()
	{
		int nM = -1;
		m_listLine.size() > 0 ? nM = 0 : nM;
		return nM;
	}
	int IsUpDate() {return m_nUpdateFinish;}

private:
	bool AnalyseLineStop(string& strJson);

private:
	int m_bUpdate;
	int m_nUpdateFinish;	//1:有更新 2:无更新
	string m_strHashCode;
	string m_curStopid;
	set<_st_Linestop> m_listLine;

};
