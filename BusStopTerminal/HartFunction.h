#pragma once
#include "ifunction.h"

class CHartFunction :
	public IFunction
{
public:
	CHartFunction(void);
	virtual ~CHartFunction(void);

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	void SetWarning(unsigned short sWarCode)
	{
		char szWarCode[8] = {0};
		sprintf_s(szWarCode, sizeof(szWarCode), "%d", sWarCode);
		m_vecWarnning.push_back(szWarCode);
	}

private:
	static unsigned int _stdcall threadHart(LPVOID lpvoid);

	void ProcHart();
	bool ReConnect();

private:
	bool m_bHart;
	HANDLE m_hHart;

	//¼à¿Ø×´Ì¬
	vector<tagKeepHart::_st_Monitor> m_vecMonitor;
	vector<string> m_vecWarnning;

};
