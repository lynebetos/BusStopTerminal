#include "StdAfx.h"
#include "SystemCommanderFunction.h"
#include "Message.h"

CSystemCommanderFunction::CSystemCommanderFunction(void)
{
}

CSystemCommanderFunction::~CSystemCommanderFunction(void)
{
	LogObject;
	LogOutInfo("清除系统指令模块");
}

bool CSystemCommanderFunction::InitFunction()
{
	return true;
}
bool CSystemCommanderFunction::OnMessage(CMessage* pMessage)
{
	MessageMap messageType;
	pMessage->GetMessageData(messageType);
	if(messageType != Message_Reboot && messageType != Message_Shutdown)
		return false;

	if(messageType == Message_Reboot)
		SystemOprate(EWX_REBOOT);

	if(messageType == Message_Shutdown)
		SystemOprate(EWX_SHUTDOWN);

	return true;
}
