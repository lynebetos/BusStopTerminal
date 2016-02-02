#pragma once

class CMessage;

class IFunction
{
public:
	IFunction(void);
	virtual ~IFunction(void);

	virtual bool InitFunction() = 0;
	virtual bool OnMessage(CMessage* pMessage) = 0;
};
