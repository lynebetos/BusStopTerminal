#pragma once
#include "map"

class CResourceManager
{
public:
	CResourceManager(void);
	~CResourceManager(void);
public:
	bool LoadResousce();

	static CResourceManager* GetInstacne();
	static void ReleaseInstance();

	const map<string, string>& GetVideoSrc() {return m_mapVideoSrc;}; 

private:
	bool LoadVideoResource(); //目录暂时写死为Resource文件夹

private:
	static CResourceManager* m_pInstance;
	map<string, string> m_mapVideoSrc;
	string m_strWeather;
};
