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

	//将缓存中的数据读取出来，pBuffer缓存指针，nByte缓存中的数据长度，vecJsonString输出的json字串数组 返回值是读取后缓存中可写入的位置
	char* FromBufferGetPacket(char* pBuffer, int& nByte, vector<string>& vecJsonString);

private:
	static CNetManager* m_pNet;

	SOCKET m_sock;
	string m_strIp;
	u_short m_uPort;
	bool m_bConn;	//已经连接的标志
	bool m_bAlreadyDoConn;	//已经做过连接动作标志。

	bool m_bRecv;
	HANDLE m_hRecv;
	bool m_bSend;
	HANDLE m_hSend;
	CLock m_lock;

	//发送数据队列
	list<string> m_queSend;
};
