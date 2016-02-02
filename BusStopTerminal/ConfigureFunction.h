#pragma once
#include "ifunction.h"

class CConfigureFunction :
	public IFunction
{
public:
	CConfigureFunction(void);
	virtual ~CConfigureFunction(void);

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);
	string GetHashCode() {return m_strHashCode;};
	int IsUpDate() {return m_nUpdateFinish;}

private:
	int m_nUpdateFinish;	//1:�и��� 2:�޸���
	string m_strHashCode;
	int m_bUpdate;

};
