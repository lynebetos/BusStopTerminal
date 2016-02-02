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
	//������Ϣ�ַ�
	bool CreateMessageAllot();

	//���²���
	bool UpdateTemplate();
	bool UpdateLineStop();
	bool UpdateConfigFile();

	//���������̣߳�������ͳ�ƣ�
	bool CreateHartThread();
	bool CreateStatistcThread();

private:
	CNetManager* m_pNetManager;
	map<int, IFunction*> m_mapFun;
	int m_nStatus; //����״̬��0:�������� 1:��վ���� 2:ģ����� 3:���ø���
	bool m_bStop; //�˳�
};
