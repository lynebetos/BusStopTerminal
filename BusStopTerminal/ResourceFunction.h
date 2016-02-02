#pragma once
#include "ifunction.h"

class CResourceFunction :
	public IFunction
{
public:
	CResourceFunction(void);
	virtual ~CResourceFunction(void);

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	static unsigned int _stdcall threadDown(LPVOID lpvoid);
};
