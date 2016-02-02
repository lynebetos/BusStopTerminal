#pragma once
#include "ifunction.h"

class CSystemCommanderFunction :
	public IFunction
{
public:
	CSystemCommanderFunction(void);
	virtual ~CSystemCommanderFunction(void);

	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

};
