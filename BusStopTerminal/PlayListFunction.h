#pragma once
#include "ifunction.h"
#include "Message.h"
#include <set>

class CPlayListFunction :
	public IFunction
{
public:
	CPlayListFunction(void);
	virtual ~CPlayListFunction(void);

	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	void GetPlayList(vector<_st_PlayList>& vecList);

private:

	bool WriteListFile();
	bool LoadListFile();

	static unsigned int _stdcall threadUpdate(LPVOID lpvoid);

	void ProcUpdate();

private:
	vector<_st_PlayList> m_setPlayList;
	CMessage m_Message;
	bool  m_bUpDateRun;

};
