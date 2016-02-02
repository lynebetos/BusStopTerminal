#include "StdAfx.h"
#include "BusArrivalFunction.h"
#include "DataManager.h"
#include "Message.h"
#include "NetManager.h"
#include "TransToWnd.h"

CBusArrivalFunction::CBusArrivalFunction(void)
{
}

CBusArrivalFunction::~CBusArrivalFunction(void)
{
	LogObject;
	LogOutInfo("�����վģ��");
}

bool CBusArrivalFunction::InitFunction()
{

	return true;
}

bool CBusArrivalFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	MessageMap Type;
	void* pData = pMessage->GetMessageData(Type);

	if(Type != Message_Arrival)
		return false;

	tagBusArrival* pArrival = (tagBusArrival*)pData;

	//�Ե�վ��Ϣ���� by:wxl
	if(pArrival)
	{
		CDataManager* pDM = CDataManager::GetInstance();
		if(!pDM)
			return false;

		pDM->CleanArrivalData();

		for (map<string, tagArrival>::iterator mapiter = pArrival->mapLineArrival.begin();
			mapiter != pArrival->mapLineArrival.end(); mapiter++)
		{
			pDM->SetArrivalData(mapiter->second);
		}
		LogOutInfo("��վ���ݸ��³ɹ�!");
		
		tagResponse stResponse;
		stResponse.head = pArrival->stHead;
		stResponse.head.messageType = MT_Response;
		stResponse.nStatus = 200;
		stResponse.strMsg = "����"; //AnsiToUtf8("����");
		CMessage msg;
		msg.SetMessage(Message_Response, &stResponse);
		CNetManager* pNM = CNetManager::GetNet();
		if(pNM)
		{
			pNM->SendData(msg);
		}

		CTransToWnd::SendArrival();

	}

	return true;
}
