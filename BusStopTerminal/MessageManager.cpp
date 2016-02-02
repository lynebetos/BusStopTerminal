#include "StdAfx.h"
#include "MessageManager.h"

CMessageManager* CMessageManager::m_pInstance = NULL;

CMessageManager::CMessageManager(void)
: m_bMsg(false)
, m_hMsg(INVALID_HANDLE_VALUE)
{
}

CMessageManager::~CMessageManager(void)
{
}

CMessageManager* CMessageManager::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CMessageManager;
		m_pInstance->Inistialize();
	}

	return m_pInstance;
}

void CMessageManager::ReleaseInstance()
{
	if(m_pInstance)
	{
		m_pInstance->m_bMsg = false;
		WaitForSingleObject(m_pInstance->m_hMsg, 2000);

		m_pInstance->m_mapAllot.clear();

		for (	list<CMessage*>::iterator iterlist = m_pInstance->m_queMessage.begin(); iterlist != m_pInstance->m_queMessage.end(); iterlist++)
		{
			CMessage* pMessage = *iterlist;
			if(pMessage)
				delete pMessage;
		}

		m_pInstance->m_queMessage.clear();
		delete m_pInstance;
	}
}

bool CMessageManager::Inistialize()
{
	LogObject;
	unsigned int nThreadID = 0;
	m_bMsg = true;
	m_hMsg = (HANDLE)_beginthreadex(NULL, 0, threadMessage, (LPVOID)this, 0, &nThreadID);
	if(m_hMsg == INVALID_HANDLE_VALUE)
	{
		LogOutError("创建消息分发线程失败");
	}
	return true;
}

bool CMessageManager::AddMessage(const CMessage& message)
{
	if(!m_bMsg)
		return false;

	CMessage* pMessage = new CMessage;
	*pMessage = message;

	m_lock.Lock();
	m_queMessage.push_back(pMessage);
	m_lock.UnLock();
	return true;
}

unsigned int _stdcall CMessageManager::threadMessage(LPVOID lpvoid)
{
	CMessageManager* pManager = (CMessageManager*)lpvoid;
	if(pManager)
	{
		pManager->ProcMessage();
	}

	return 0;
}

void CMessageManager::ProcMessage()
{
	while(m_bMsg)
	{
		if(0 >= m_queMessage.size())
		{
// 			m_lock.UnLock();
			Sleep(100);
			continue;
		}
		m_lock.Lock();
		CMessage* pMessage = m_queMessage.front();
		m_lock.UnLock();

		if(pMessage)
		{
			map<MessageMap, IFunction*>::iterator theiter = m_mapAllot.find(pMessage->GetCommand());
			if(theiter != m_mapAllot.end())
			{
				theiter->second->OnMessage(pMessage);
			}
			
			delete pMessage;
			m_lock.Lock();
			m_queMessage.pop_front();
			m_lock.UnLock();

		}

	}
}