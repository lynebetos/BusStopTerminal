#pragma once
#include "ifunction.h"
#include "Message.h"
#include <set>

//�θ��¶���ȫ���б���ɾ��

class CNoticeFunction :
	public IFunction
{
public:
	CNoticeFunction(void);
	virtual ~CNoticeFunction(void);

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	void GetNoticeList(vector<_st_NoticeList>& vecNoticeList);

private:	
	bool WriteListFile();
	bool LoadListFile();

	static unsigned int _stdcall threadUpdate(LPVOID lpvoid);

	void ProcUpdate();

private:
	set<_st_NoticeList> m_setNoticList;
	CMessage m_Message;
	bool m_bUpdateRun;

};
