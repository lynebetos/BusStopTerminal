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
		LogOutInfo("已打开套接字");
	} 
	else
	{
		//进一步绑定套接字
		LogOutError("嵌套字未打开!");
		return;
	}


	BindSocket();

	m_pNet = this;
}

bool CNetManager::BindSocket()
{
	LogObject;

	//需要绑定的参数
	m_sock=socket(AF_INET,SOCK_STREAM,0);//创建了可识别套接字

	SOCKADDR_IN addr;

	addr.sin_family=AF_INET;

	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip地址

	addr.sin_port=htons(m_uPort);//绑定端口

	LogOutInfo("绑定端口");
	bind(m_sock,(SOCKADDR*)&addr,sizeof(SOCKADDR));//绑定完成

// 	unsigned long ul=1;  
// 	int ret=ioctlsocket(m_sock,FIONBIO,(unsigned long *)&ul);//设置成非阻塞模式。  
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

	LogOutInfo("连接服务器");
	int nRes = connect(m_sock,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));//开始连接

	if(0 != nRes)
	{
		std::stringstream sstr;
		sstr<<"与服务器连接失败，SOCKET错误代码:"<<WSAGetLastError()<<";IP="<<m_strIp<<";Port="<<m_uPort;
		LogOutError(sstr.str().c_str());
		sstr.str() = "";
		return false;
	}
	LogOutInfo("服务器连接成功!");

	//建立接收线程
	LogOutInfo("建立接收线程");
	m_bRecv = true;
	unsigned int nthreadid;
	m_hRecv = (HANDLE)_beginthreadex(NULL, 0, threadRecv, (LPVOID)this, 0, &nthreadid);
	if(INVALID_HANDLE_VALUE == m_hRecv)
	{
		LogOutError("建立接收线程失败!");
	}

	m_bConn = true;

	return m_bConn;
}

bool CNetManager::DisConnect()
{
	LogObject;
	m_bRecv = false;
	m_bSend = false;

	LogOutInfo("关闭套接字，断开连接");
	int nRes = closesocket(m_sock);
	if(0 != nRes)
	{
		std::stringstream sstr;
		sstr<<"关闭套接字失败，SOCKET错误代码:"<<WSAGetLastError();
		LogOutError(sstr.str().c_str());
		sstr.str() = "";
	}

	m_bConn = false;

	if(WaitForSingleObject(m_hRecv, 2000) == WAIT_TIMEOUT)
	{
		LogOutError("接收线程退出超时");
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
// 			sstr<<"接收数据失败，SOCKET错误代码:"<<WSAGetLastError();
// 			LogOutError(sstr.str().c_str());
// 			sstr.str() = "";
			Sleep(100);
			continue;
		}

		//log
		std::stringstream sstr;
		sstr<<"接收到的数据长度:"<<nRes;
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
				LogOutError("接收数据解析错误");
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

		//取数据头
		if(strcmp(szStart, START_FLAG) == 0)
		{
			//取数据长度
			pRead+=4;
			nByteRead-=4;
			int nDataLen = 0;
			nDataLen |= pRead[3] & 0x000000ff;
			nDataLen |= (pRead[2] << 8) & 0x0000ff00;
			nDataLen |= (pRead[1] << 16) & 0x00ff0000;
			nDataLen |= (pRead[0] << 24) & 0xff000000;

			pRead+=4;
			nByteRead-=4;

			//如果数据头长度大于缓存数据剩余长度，将剩余数据移动到缓存的起始位置，然后返回可写入的位置
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

			//如果正好够一条数据，存入数组，然后从数据尾部继续解析
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
			sstr<<"接收出一条数据,长度=:"<<nDataLen;
			LogOutInfo(sstr.str().c_str());
			sstr.str() = "";
			sstr.str().clear();

		}
		else
		{
			//没检测到数据头继续向后移动指针解析
			LogOutInfo("数据头解析错误!");
			pRead++;
			nByteRead -- ;
		}

	}

	//跳出循环的情况说明解析完成且缓存中存在的数据不符合规格，或者正好解析成完整的几组json串，做清空缓存操作
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
		sstr<<"发送数据失败，SOCKET错误代码:"<<dwError;
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

				LogOutInfo("连接服务器");
				nRes = connect(m_sock,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));//开始连接

				if(0 != nRes)
				{
					std::stringstream sstr;
					sstr<<"重新连接失败，SOCKET错误代码:"<<WSAGetLastError();
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
		sstr<<"发送数据成功,发送字节数:"<<nRes;
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
		LogOutError("Json 数据解析错误");
		return false;
	}

	Message_Type mtType =Json.GetMessageType();

	if(mtType == MT_Response)
	{
		//LogOutError("接收到应答");
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
		sstr<<"发送登陆失败，SOCKET错误代码:"<<dwError;
		LogOutError(sstr.str().c_str());
		sstr.str() = "";
	}
	delete[] pBuffer;

	return true;
}