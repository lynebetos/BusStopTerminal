#pragma once
#include <list>
#include "Message.h"
#include "IFunction.h"

class CMain;

class CMessageManager
{
public:
	CMessageManager(void);
	~CMessageManager(void);

	friend CMain;

	static CMessageManager* GetInstance();
	static void ReleaseInstance(); //±ØÐërelease
	
	bool AddMessage(const CMessage& message);

private:
	bool Inistialize();

	static unsigned int _stdcall threadMessage(LPVOID lpvoid);

	void ProcMessage();


private:
	static CMessageManager* m_pInstance;

	CLock m_lock;
	bool m_bMsg;
	HANDLE m_hMsg;

private:
	list<CMessage*> m_queMessage;
	map<MessageMap, IFunction*> m_mapAllot;
};
