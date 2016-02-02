#pragma once
#include "ifunction.h"

class CVolumeFunction :
	public IFunction
{
public:
	CVolumeFunction(void);
	virtual ~CVolumeFunction(void);

public:
	virtual bool InitFunction();
	virtual bool OnMessage(CMessage* pMessage);

private:
	static unsigned int _stdcall threadSetVol(LPVOID lpvoid);
	void SeVolume();
	bool LoadVolumeConfig(string strFile);


	bool m_bRun;
	HANDLE m_hVolume;
	tagVolumeConf m_stVolumeConf;
	string m_strPath;		//音量配置文件路径
	CLock m_lock;

};
