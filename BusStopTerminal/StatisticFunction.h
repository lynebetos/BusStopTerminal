#pragma once
#include "ifunction.h"
#include "Message.h"

class CStatisticFunction :
	public IFunction
{
public:
	CStatisticFunction(void);
	virtual ~CStatisticFunction(void);

	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	static void SetStatistic(PPlayStatistic p);

	void StartCall();

	static unsigned int _stdcall threadUpLoad(LPVOID lpvoid);
	void UpLoad();

private:
	void AddStatistic(PPlayStatistic p);
	void ConvertMapToVecter(const map<string, vector<PlayStatistic::_st_StaData>> &mapStatistic, CMessage& msg);

private:
	static CStatisticFunction* m_pInstance;
	map<string, vector<PlayStatistic::_st_StaData>> m_mapStatistic;
	HANDLE m_hThread;
	bool m_bRun;
	CLock m_lock;

};
