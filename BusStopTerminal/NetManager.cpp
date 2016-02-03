#include "StdAfx.h"
#include "NetManager.h"
#include "ConfigManager.h"
#include "MessageManager.h"
#include <WinSock2.h>
#include "md5/MD5.h"

CNetManager* CNetManager::m_pNet = NULL;

#define CleanMemery(mem, memlen) memset(mem, 0, memlen)

CNetManager::~CNetManager(void)
{
	WSACleanup();
	m_pNet = NULL;
}

CNetManager::CNetManager(string strIp, string strPort)
: m_bRecv(false)
, m_bSend(false)
, m_hRecv(INVALID_HANDLE_VALUE)
, m_hSend(INVALID_HANDLE_VALUE)
, m_bAlreadyDoConn(false)
{
	LogObject;

	if(m_pNet)
		return;

	u_short nPort = (u_short)atoi(strPort.c_str());

	m_strIp = strIp;
	m_uPort = nPort;

	WORD VersionRequest = MAKEWORD(1,1);
	WSADATA wsadata;
	int err;

	err=WSAStartup(VersionRequest,&wsadata);

	if (!err)
	{
		LogOutInfo("�Ѵ��׽���");
	} 
	else
	{
		//��һ�����׽���
		LogOutError("Ƕ����δ��!");
		return;
	}


	BindSocket();

	m_pNet = this;
}

bool CNetManager::BindSocket()
{
	LogObject;

	//��Ҫ�󶨵Ĳ���
	m_sock=socket(AF_INET,SOCK_STREAM,0);//�����˿�ʶ���׽���

	SOCKADDR_IN addr;

	addr.sin_family=AF_INET;

	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip��ַ

	addr.sin_port=htons(m_uPort);//�󶨶˿�

	LogOutInfo("�󶨶˿�");
	bind(m_sock,(SOCKADDR*)&addr,sizeof(SOCKADDR));//�����

// 	unsigned long ul=1;  
// 	int ret=ioctlsocket(m_sock,FIONBIO,(unsigned long *)&ul);//���óɷ�����ģʽ��  
	return true;
};

bool CNetManager::Connect()
{
	if(m_bConn)
		return true;

	LogObject;
	SOCKADDR_IN clientsock_in;

	HOSTENT* phostentry = gethostbyname(m_strIp.c_str());

	if(!phostentry)
		return false;

	clientsock_in.sin_addr.S_un.S_addr= *(ULONG*)phostentry->h_addr_list[0];

	clientsock_in.sin_family=AF_INET;

	clientsock_in.sin_port=htons(m_uPort);

	LogOutInfo("���ӷ�����");
	int nRes = connect(m_sock,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));//��ʼ����

	if(0 != nRes)
	{
		std::stringstream sstr;
		sstr<<"�����������ʧ�ܣ�SOCKET�������:"<<WSAGetLastError()<<";IP="<<m_strIp<<";Port="<<m_uPort;
		LogOutError(sstr.str().c_str());
		sstr.str() = "";
		return false;
	}
	LogOutInfo("���������ӳɹ�!");

	//���������߳�
	LogOutInfo("���������߳�");
	m_bRecv = true;
	unsigned int nthreadid;
	m_hRecv = (HANDLE)_beginthreadex(NULL, 0, threadRecv, (LPVOID)this, 0, &nthreadid);
	if(INVALID_HANDLE_VALUE == m_hRecv)
	{
		LogOutError("���������߳�ʧ��!");
	}

	m_bConn = true;

	return m_bConn;
}

bool CNetManager::DisConnect()
{
	LogObject;
	m_bRecv = false;
	m_bSend = false;

	LogOutInfo("�ر��׽��֣��Ͽ�����");
	int nRes = closesocket(m_sock);
	if(0 != nRes)
	{
		std::stringstream sstr;
		sstr<<"�ر��׽���ʧ�ܣ�SOCKET�������:"<<WSAGetLastError();
		LogOutError(sstr.str().c_str());
		sstr.str() = "";
	}

	m_bConn = false;

	if(WaitForSingleObject(m_hRecv, 2000) == WAIT_TIMEOUT)
	{
		LogOutError("�����߳��˳���ʱ");
	}

	//CloseHandle(m_hRecv);

	return true;
}

unsigned int __stdcall CNetManager::threadRecv(LPVOID lpvoid)
{
	CNetManager* pNet = (CNetManager*)lpvoid;
	if(pNet)
	{
		pNet->ProcRecv();
	}
	return 0;
}

void CNetManager::ProcRecv()
{
	LogObject;

	char* pBuff = new char[800*1024];
	char* pBuffRecv = new char[8 * 1024];
	memset(pBuff, 0, 800*1024);
	memset(pBuffRecv, 0, 8*1024);
	char* pBuffIndex = pBuff;
	int nAllByte = 0;

	while (m_bRecv)
	{
		int nRes = recv(m_sock, pBuffRecv, 8*1024, 0);
		if(0 == nRes)
		{
//			m_bRecv = false;
			Sleep(1);
			continue;
		}
		else if(0 > nRes)
		{
// 			std::stringstream sstr;
// 			sstr<<"��������ʧ�ܣ�SOCKET�������:"<<WSAGetLastError();
// 			LogOutError(sstr.str().c_str());
// 			sstr.str() = "";
			Sleep(100);
			continue;
		}

		//log
		std::stringstream sstr;
		sstr<<"���յ������ݳ���:"<<nRes;
		LogOutInfo(sstr.str().c_str());
		sstr.str() = "";
		sstr.str().clear();

		memcpy(pBuffIndex, pBuffRecv, nRes);
		memset(pBuffRecv, 0, 8*1024);
		nAllByte += nRes;

		vector<string> vecData;
		pBuffIndex = FromBufferGetPacket(pBuff, nAllByte, vecData);

		for (int ii = 0; ii < (int)vecData.size(); ii++)
		{
			string strData = vecData[ii];
			if(!RecvDataAnalyse(strData))
			{
				LogOutError("�������ݽ�������");
			}
			else
			{
				LogOutInfo(strData.c_str());
			}
		}
	}

	delete[] pBuff ;
	delete[] pBuffRecv ;

	return;
}

char* CNetManager::FromBufferGetPacket(char* pBuffer, int& nByte, vector<string>& vecJsonString)
{
	LogObject;

	char* pRead = pBuffer;
	char szStart[5] = {0};
	int nByteRead = nByte;

	while(nByteRead > 0)
	{
		memcpy(szStart, pRead, 4 * sizeof(char));

		//ȡ����ͷ
		if(strcmp(szStart, START_FLAG) == 0)
		{
			//ȡ���ݳ���
			pRead+=4;
			nByteRead-=4;
			int nDataLen = 0;
			nDataLen |= pRead[3] & 0x000000ff;
			nDataLen |= (pRead[2] << 8) & 0x0000ff00;
			nDataLen |= (pRead[1] << 16) & 0x00ff0000;
			nDataLen |= (pRead[0] << 24) & 0xff000000;

			pRead+=4;
			nByteRead-=4;

			//�������ͷ���ȴ��ڻ�������ʣ�೤�ȣ���ʣ�������ƶ����������ʼλ�ã�Ȼ�󷵻ؿ�д���λ��
			if(nDataLen > nByteRead)
			{
				nByteRead+=8;
				pRead-=8;
				char* pTempBuffer = new char[nByteRead+1];
				CleanMemery(pTempBuffer, nByteRead+1);
				memcpy(pTempBuffer, pRead, nByteRead);
				CleanMemery(pBuffer, nByte);
				memcpy(pBuffer, pTempBuffer, nByteRead);
				pRead = pBuffer;
				delete[] pTempBuffer;
				pTempBuffer = NULL;
				nByte = nByteRead;
				return pRead += nByteRead;
			}

			//������ù�һ�����ݣ��������飬Ȼ�������β����������
			char* pTempBuffer = new char[nDataLen+1];
			CleanMemery(pTempBuffer, nDataLen+1);
			memcpy(pTempBuffer, pRead, nDataLen);
			string strData = pTempBuffer;
			delete[] pTempBuffer;
			vecJsonString.push_back(strData);
			pRead+=nDataLen;
			nByteRead-=nDataLen;
			//log
			std::stringstream sstr;
			sstr<<"���ճ�һ������,����=:"<<nDataLen;
			LogOutInfo(sstr.str().c_str());
			sstr.str() = "";
			sstr.str().clear();

		}
		else
		{
			//û��⵽����ͷ��������ƶ�ָ�����
			LogOutInfo("����ͷ��������!");
			pRead++;
			nByteRead -- ;
		}

	}

	//����ѭ�������˵����������һ����д��ڵ����ݲ����Ϲ�񣬻������ý����������ļ���json��������ջ������
	CleanMemery(pBuffer, nByte);
	pRead = pBuffer;
	nByte = 0;

	return pRead;

}

int CNetManager::SendData(CMessage& message)
{
	LogObject;

	if(!m_bConn)
		return NET_ERROR_NONCONN;

	string strData = "";
	CJsonManager json;
	if(!json.ToJson(message, strData))
		return NET_ERROR_ERRORJSON;

	LogOutInfo(strData.c_str());

// 	string strTmp = AnsiToUtf8(strData);
// 	strData = strTmp;

	string strSend;
	strSend = START_FLAG;

	int nLen = strData.size();
	char *pBuffer = new char[strData.size() + 8];
	memcpy(pBuffer, START_FLAG, 4); 
	pBuffer[4] = nLen >> 24;
	pBuffer[5] = nLen >> 16;
	pBuffer[6] = nLen >> 8;
	pBuffer[7] = nLen;

	for (int ii =8; ii < (int)strData.size() + 8; ii++)
	{
		pBuffer[ii] = strData.at(ii - 8);
	}

	int nRes = send(m_sock, pBuffer, strData.size() + 8, 0);
	if(nRes == SOCKET_ERROR)
	{
		DWORD dwError = WSAGetLastError();
		std::stringstream sstr;
		sstr<<"��������ʧ�ܣ�SOCKET�������:"<<dwError;
		LogOutError(sstr.str().c_str());
		sstr.str() = "";

		if(dwError == WSAENOTCONN || dwError==WSAECONNABORTED || dwError == WSAECONNRESET)
		{
			delete[] pBuffer;
			return NET_ERROR_MISSCONN;
			/*
			closesocket(m_sock);
			m_bConn = false;
			int nRes = 0;
			do 
			{
				BindSocket();

				SOCKADDR_IN clientsock_in;

				clientsock_in.sin_addr.S_un.S_addr=inet_addr(m_strIp.c_str());

				clientsock_in.sin_family=AF_INET;

				clientsock_in.sin_port=htons(m_uPort);

				LogOutInfo("���ӷ�����");
				nRes = connect(m_sock,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));//��ʼ����

				if(0 != nRes)
				{
					std::stringstream sstr;
					sstr<<"��������ʧ�ܣ�SOCKET�������:"<<WSAGetLastError();
					LogOutError(sstr.str().c_str());

					Sleep(RECONNET_DILAY);
				}
			} while (0 != nRes);

			m_bConn = true;
			bool bRet = Authentication();
			*/
		}
		else
		{
			delete[] pBuffer;
			return NET_ERROE_OTHER;
		}
	}
	else
	{
		std::stringstream sstr;
		sstr<<"�������ݳɹ�,�����ֽ���:"<<nRes;
		LogOutInfo(sstr.str().c_str());
		sstr.str() = "";
	}

	delete[] pBuffer;

	return NET_NOERROR;
}

bool CNetManager::RecvDataAnalyse(string& strData)
{
	LogObject;
	CJsonManager Json;
	//LogOutInfo(strData.c_str());
	if(!Json.AnalyseJson(strData))
	{
		LogOutError("Json ���ݽ�������");
		return false;
	}

	Message_Type mtType =Json.GetMessageType();

	if(mtType == MT_Response)
	{
		//LogOutError("���յ�Ӧ��");
	}
	else if(mtType == MT_Dataset)
	{
		//pJson->GetMessage(&pMessage);
	}

	const CMessage& message = Json.GetMessage();

	CMessageManager* pManager = CMessageManager::GetInstance();

	pManager->AddMessage(message);

	return true;
}

bool CNetManager::Authentication()
{
	LogObject;

	CConfigManager* pConfig = CConfigManager::GetInstance();

	MD5 md5(pConfig->GetStopId().c_str());
	string strKey = md5.hexdigest();
	transform(strKey.begin(), strKey.end(), strKey.begin(), toupper);

	tagAuthentication stAuthen;
	stAuthen.head.command = Message_Login;
	stAuthen.head.messageType = MT_Dataset;
	stAuthen.head.strCityCode = pConfig->GetCityCode();
	stAuthen.head.strServerIP = pConfig->GetServerIP();
	stAuthen.head.strStopID = pConfig->GetStopId();
	GetMsgId(stAuthen.head.strMsgID);
	stAuthen.strKey = strKey;

	CMessage message;
	message.SetMessage(Message_Login, (void*)&stAuthen);

	string strData;
	CJsonManager json;
	json.ToJson(message, strData);

	string strSend;
	strSend = START_FLAG;

	int nLen = strData.size();

	strSend += nLen;
	strSend += strData;

	char *pBuffer = new char[strData.size() + 8];
	memcpy(pBuffer, START_FLAG, 4); 
	pBuffer[4] = nLen >> 24;
	pBuffer[5] = nLen >> 16;
	pBuffer[6] = nLen >> 8;
	pBuffer[7] = nLen;

	for (int ii =8; ii < (int)strData.size() + 8; ii++)
	{
		pBuffer[ii] = strData.at(ii - 8);
	}

	int nRes = send(m_sock, pBuffer, strSend.size() + 8, 0);
	if(nRes == SOCKET_ERROR)
	{
		DWORD dwError = WSAGetLastError();
		std::stringstream sstr;
		sstr<<"���͵�½ʧ�ܣ�SOCKET�������:"<<dwError;
		LogOutError(sstr.str().c_str());
		sstr.str() = "";
	}
	delete[] pBuffer;

	return true;
}