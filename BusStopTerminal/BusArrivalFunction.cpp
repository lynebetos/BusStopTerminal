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
	LogOutInfo("清除到站模块");
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

	//对到站信息排序 by:wxl
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
		LogOutInfo("到站数据更新成功!");
		
		tagResponse stResponse;
		stResponse.head = pArrival->stHead;
		stResponse.head.messageType = MT_Response;
		stResponse.nStatus = 200;
		stResponse.strMsg = "正常"; //AnsiToUtf8("正常");
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
