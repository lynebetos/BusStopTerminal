#include "StdAfx.h"
#include "ResourceFunction.h"
#include "ConfigManager.h"
#include "Message.h"
#include "NetManager.h"
#include "../include/http/base_http.h"

using namespace base_http;

CResourceFunction::CResourceFunction(void)
{
}

CResourceFunction::~CResourceFunction(void)
{
	LogObject;
	LogOutInfo("�����Դ����ģ��");
}

bool CResourceFunction::InitFunction()
{
	return true;
}

bool CResourceFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	MessageMap messageType;
	void* pData = pMessage->GetMessageData(messageType);
	if(Message_Resource != messageType)
		return false;

	if(pData == NULL)
	{
		LogOutError("��Դ�������ݴ���");
		return false;
	}


	tagResourceDown* pResourceDown = new tagResourceDown;
	*pResourceDown	= *((tagResourceDown*)pData);

	unsigned int nThread = 0;
	HANDLE hDown = (HANDLE)_beginthreadex(NULL, 0, threadDown, (LPVOID)pResourceDown, 0, &nThread);
	if(hDown == INVALID_HANDLE_VALUE)
	{
		LogOutError("��Դ�����߳�����ʧ��");
		return false;
	}

	return true;
}

unsigned int _stdcall CResourceFunction::threadDown(LPVOID lpvoid)
{
	LogObject;

	bool bDownSucc = true;
	tagResourceDown* pDown = (tagResourceDown*)lpvoid;

	CConfigManager* pCM = CConfigManager::GetInstance();
	if(pDown)
	{
		for (int ii = 0; ii < pDown->vecResource.size(); ii++)
		{
			string strType = pDown->vecResource[ii].strType;
			string strUrl = pDown->vecResource[ii].strUrl;


			if(strType == "1")
			{
				base_http::CBaseHttp baseHttp;
				string strAdvPath = pCM->GetAdverDir();

				strUrl = Utf8toAnsi(strUrl);
				strUrl = AnsiToUtf8(strUrl);
				int nPos = strUrl.find_last_of("/");
				string strSub = strUrl.substr(nPos+1);

				//strAdvPath+= "\\";
				strAdvPath+= strSub;


				baseHttp.AddHttpHeader("Authorization", "Basic YWRtaW46YWRtaW4=");
				bool bDown = baseHttp.HttpGet(strUrl.c_str());
				if(!bDown)
				{
					stringstream sstrLog;
					sstrLog<<"��Դ����ʧ��,URL= "<<strUrl;
					LogOutError(sstrLog.str().c_str());
					sstrLog.str("");
					bDownSucc = false;
					continue;
				}

				bool bWrite = baseHttp.SaveDataToFile(strAdvPath.c_str());
// 				bool bDown = baseHttp.HttpDownload(strUrl.c_str(), strAdvPath.c_str());

				if(!bWrite)
				{
					stringstream sstrLog;
					sstrLog<<"��Դд��ʧ��,����·�� "<<strAdvPath;
					LogOutError(sstrLog.str().c_str());
					sstrLog.str("");
					bDownSucc = false;
					continue;
				}
				else
				{
					stringstream sstrLog;
					sstrLog<<"��Դ���سɹ�,URL= "<<strUrl<<"; ����·��:"<<strAdvPath;
					LogOutError(sstrLog.str().c_str());
					sstrLog.str("");
				}
			}
		}

		//���سɹ���Ӧ��
		tagResponse stResponse;
		stResponse.head = pDown->head;
		stResponse.head.messageType = MT_Response;
		stResponse.nStatus = 200;
		stResponse.strMsg = "����";
		CMessage msg;
		msg.SetMessage(Message_Response, &stResponse);
		CNetManager* pNM = CNetManager::GetNet();
		if(pNM)
		{
			pNM->SendData(msg);
		}

		delete pDown;

	}

	return 0;
}