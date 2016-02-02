#pragma once
#include "ifunction.h"

class CWeatherFunction :
	public IFunction
{
public:
	CWeatherFunction(void);
	virtual ~CWeatherFunction(void);

	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

	const tagWeather& GetWeather() {return m_weather;};

private:
	tagWeather m_weather;

	static unsigned int __stdcall ThreadWeather(LPVOID lpvoid);

};
