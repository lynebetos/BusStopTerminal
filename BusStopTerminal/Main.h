#pragma once
#include <map>
#include "NetManager.h"
#include "IFunction.h"


class CMain
{
public:
	CMain(void);
	~CMain(void);

public:
	int Initialize();
	IFunction* const GetFunction(int FunType);
	int DoConnectServer();
	void UnInitialize();
	int GetStatus() {return m_nStatus;}
	void StartCallPS();
	void SetStop() {m_bStop = true;}

private:
	//创建消息分发
	bool CreateMessageAllot();

	//更新操作
	bool UpdateTemplate();
	bool UpdateLineStop();
	bool UpdateConfigFile();

	//创建处理线程（心跳，统计）
	bool CreateHartThread();
	bool CreateStatistcThread();

private:
	CNetManager* m_pNetManager;
	map<int, IFunction*> m_mapFun;
	int m_nStatus; //启动状态：0:正常启动 1:线站更新 2:模板更新 3:配置更新
	bool m_bStop; //退出
};
