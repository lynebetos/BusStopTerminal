#pragma once
#include "tinyxml/tinyxml.h"

class CConfigManager;

class CAdvertisementConfig
{
private:
	CAdvertisementConfig(void);
	~CAdvertisementConfig(void);

	friend class CConfigManager;
	friend class CConfigureFunction;

private:
	bool AnalyseAdver(TiXmlElement* pElement);
	bool SaveAdver(TiXmlElement* pElement);

private:
	CPanelSize m_size;
	int m_nType;
	string m_strDownPort;
	string m_strSourceDir;
	string m_strPlaylist;

};

class CBusStopConfig
{
private:
	CBusStopConfig(void);
	~CBusStopConfig(void);

	friend CConfigManager;
	friend class CConfigureFunction;

private:
	bool AnalyseBusStop(TiXmlElement* pElement);
	bool SaveBusStop(TiXmlElement* pElement);

private:
	CPanelSize m_size;
	string m_strCityCode;
	string m_strStopID;
	string m_strStopName;
	int m_nLineCount ;
	int m_nOneBord;
	string m_strStaticData;
	//string m_strTemplate;
};

class CConfigManager
{
public:
	CConfigManager(void);
	~CConfigManager(void);

	friend class CConfigureFunction;

public:
	bool LoadConfig(string strPath);
	bool SaveConfig();
	static CConfigManager* GetInstance();
	static void ReleaseInstance();

public:   //get
	string GetAppVer() {return m_strAppVer;};
	string GetConfigVer() {return m_strConfigVer;};
	string GetTemplateVer() {return m_strTempVer;};
	string GetHeight() {return m_strHeight;};
	string GetWidth() {return m_strWidth;};
	int GetAdverCount() {return m_nAdverCount;};
	string GetServerIP() {return m_strIp;};
	string GetBusinessPort() {return m_strPort;};
	string GetNoticeDir() {return m_strNoticeDir;};
	const CPanelSize& GetAdverSize() {return m_Adver.m_size;};
	int GetAdverType() {return m_Adver.m_nType;};
	string GetDownPort() {return m_Adver.m_strDownPort;};
	string GetAdverDir() {return m_Adver.m_strSourceDir;};
	string GetPlaylistPath() {return m_Adver.m_strPlaylist;};
	const CPanelSize& GetBusstopSize() {return m_Busstop.m_size;};
	string GetCityCode() {return m_Busstop.m_strCityCode;}
	string GetStopId() {return m_Busstop.m_strStopID;};
	string GetStopName() {return m_Busstop.m_strStopName;};
	int GetStopLineCount() {return m_Busstop.m_nLineCount;};
	int GetStopOneBord() {return m_Busstop.m_nOneBord;};
	string GetStaticStopDataPath() {return m_Busstop.m_strStaticData;}; 
	string GetTemplatePath() {return m_strTemplatePath;};
	string GetPicturePath() {return m_strPictrueDir;}
	string GetDefaultPath() {return m_strDefaultPath;}
	string GetVolumeDir() {return m_strVolumeDir;}
	int GetUploadCycle() {return m_nUpload;};

public:   //set
	void SetTemplateVersion(string strVersion)
	{
		m_strTempVer = strVersion;
	}

private:
	bool AnalyseConfigFile();

private:
	string m_strAppVer;
	string m_strConfigVer;		//配置版本（实际为配置文件的哈希值）
	string m_strTempVer;		//模板版本（实际为模板文件的哈希值）
	string m_strHeight;
	string m_strWidth;
	string m_strTemplatePath;	//模板文件位置
	int m_nAdverCount;
	string m_strIp;
	string m_strPort;
	string m_strNoticeDir;
	string m_strPictrueDir;
	string m_strDefaultPath;		//播放器默认图片的位置
	string m_strSotfUrl;			//软件更新进程需要的连接
	string m_strVolumeDir;		//音量配置文件路径
	int m_nUpload;		//统计上传时间间隔
	int m_nHartUp;		//心跳上传时间间隔
	CAdvertisementConfig m_Adver;
	CBusStopConfig m_Busstop;

	string m_strConfigFile;

	static CConfigManager* m_pInstance;
};
