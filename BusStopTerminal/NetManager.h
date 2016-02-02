#pragma once
#include <list>
#include "JsonManager.h"

#define RECONNET_DILAY 5000
#define START_FLAG "traf"

#define NET_NOERROR 1
#define NET_ERROR_NONCONN NET_NOERROR - 1
#define NET_ERROR_MISSCONN NET_ERROR_NONCONN - 1
#define NET_ERROR_ERRORJSON NET_ERROR_MISSCONN - 1
#define NET_ERROE_OTHER NET_ERROR_ERRORJSON - 1

class CLockNow
{
public:
	CLockNow()
	{
		InitializeCriticalSection(&m_sec);
		EnterCriticalSection(&m_sec);
	}
	~CLockNow()
	{
		LeaveCriticalSection(&m_sec);
		DeleteCriticalSection(&m_sec);
	}
private:
	CRITICAL_SECTION m_sec;

};

class CNetManager
{
public:
	~CNetManager(void);
	CNetManager(string strIp, string strPort);

public:
	bool Connect();
	bool DisConnect();
	bool ReConnect() 
	{
		if(m_bConn)
			return m_bConn;
		return Connect();
	};
	int SendData(CMessage& message);
	bool ReBind() {return BindSocket();};
	bool Authentication();
	static CNetManager* GetNet() {return m_pNet;};
	void SetDoConn(bool bDoConn) {m_bAlreadyDoConn = bDoConn;}
	bool GetDoConn() {return m_bAlreadyDoConn;} 

private:
	static unsigned int _stdcall threadRecv(LPVOID lpvoid);

	void ProcRecv();

	bool RecvDataAnalyse(string& strData);
	bool BindSocket();

	//�������е����ݶ�ȡ������pBuffer����ָ�룬nByte�����е����ݳ��ȣ�vecJsonString�����json�ִ����� ����ֵ�Ƕ�ȡ�󻺴��п�д���λ��
	char* FromBufferGetPacket(char* pBuffer, int& nByte, vector<string>& vecJsonString);

private:
	static CNetManager* m_pNet;

	SOCKET m_sock;
	string m_strIp;
	u_short m_uPort;
	bool m_bConn;	//�Ѿ����ӵı�־
	bool m_bAlreadyDoConn;	//�Ѿ��������Ӷ�����־��

	bool m_bRecv;
	HANDLE m_hRecv;
	bool m_bSend;
	HANDLE m_hSend;
	CLock m_lock;

	//�������ݶ���
	list<string> m_queSend;
};
