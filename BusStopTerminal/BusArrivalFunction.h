#pragma once
#include "ifunction.h"

class CBusArrivalFunction :
	public IFunction
{
public:
	CBusArrivalFunction(void);
	virtual ~CBusArrivalFunction(void);

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

private:
	


};
