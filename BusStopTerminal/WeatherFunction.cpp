#include "StdAfx.h"
#include "WeatherFunction.h"
#include "Message.h"
#include "NetManager.h"
#include "TransToWnd.h"

CWeatherFunction::CWeatherFunction(void)
{
}

CWeatherFunction::~CWeatherFunction(void)
{
	LogObject;
	LogOutInfo("�����������ģ��");
}

bool CWeatherFunction::InitFunction()
{
	return true;
}

bool CWeatherFunction::OnMessage(CMessage* pMessage)
{
	LogObject;
	MessageMap Type;
	void* pData = pMessage->GetMessageData(Type);
	if(Message_Weather != Type)
		return false;

	tagWeather* pWeather = (tagWeather*)pData;
	m_weather = *((tagWeather*)pData);

	unsigned int nThread = 0;
	HANDLE hWeather = (HANDLE)_beginthreadex(NULL, 0, ThreadWeather, (LPVOID)&m_weather, 0, &nThread);
	if(hWeather == INVALID_HANDLE_VALUE)
	{
		LogOutError("���������߳�����ʧ��");
		return false;
	}

	return true;
}

unsigned int __stdcall CWeatherFunction::ThreadWeather(LPVOID lpvoid)
{
	LogObject;
	tagWeather* pWeather = (tagWeather*)lpvoid;
	if(pWeather)
	{
		
		while(!CTransToWnd::SendWeather(pWeather))
		{
			Sleep(100);
		}

		LogOutInfo("�������³ɹ�!");

		tagResponse stResponse;
		stResponse.head = pWeather->sthead;
		stResponse.head.messageType = MT_Response;
		stResponse.nStatus = 200;
		stResponse.strMsg = "����";//AnsiToUtf8("����");
		CMessage msg;
		msg.SetMessage(Message_Response, &stResponse);
		CNetManager* pNM = CNetManager::GetNet();
		if(pNM)
		{
			pNM->SendData(msg);
		}

	}

	return 0;

}
