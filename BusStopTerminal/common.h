#pragma once
#include <vector>
#include <list>
#include <map>
#include "Base.h"

#ifdef WIN32
#define WM_RELOADPLAYLIST WM_USER + 100
#define WM_RELOADNOTIC WM_RELOADPLAYLIST + 1
#define WM_WEATHER WM_RELOADNOTIC + 1
#define WM_ARRIVAL WM_WEATHER+1
#define WM_CALLSTATISTIC WM_ARRIVAL+1
#define WM_SETWARNING WM_CALLSTATISTIC + 1
#else LINUX
#endif

//for Defence
#define EVENT_PAGE_READ "Global\\Busstop_Mapping_read_event"
#define EVENT_RESTART_APP "Global\\Busstop_Restart_event"
#define MAPPING_NAME "BusStop_Mapping"

#define MAPPING_SIZE 1024

#define RESTART_APP 1		//终端自重启 线站、模板、配置更新
#define RESTART_UPDATE 2 //
#define RUN_NOMAL 3		//终端运行正常

#define WARNING_NOFILE 0101
#define WARNING_FILENOTPLAY 0102
#define WARNING_VALIDADVFORMAT 0103
#define WARNING_NOTICEERROR 0201
#define WARNING_VALIDNOTICFORMAT 0202
#define WARNING_NOLINESTOP 0301
#define WARNING_VALIDLINEORMAT 0302
#define WARNING_NOARRIVE 0401
#define WARNING_NOWEATHER 0501

typedef struct MessageInfo
{
	unsigned int nMessageID;

	void Clear()
	{
		nMessageID = 0;
	}
}MInfo, *PMInfo;
//

#define INSERTE_LIST	"insert"
#define REPLACE_LIST	"replace"


#define Response "response"
#define Dataset "dataset"

#define J_TYPE "type"
#define J_CMD "command"
#define J_SERVER "server"
#define J_STOPID "stopMid"
#define J_CITYCODE "cityCode"
#define J_STATUS "status"
#define J_MSG "msg"
#define J_ARRAY "list"
#define J_MSGID "msgId"

//请求名称
#define login "login"
#define lineStop "lineStop"
#define busArrival "busArrival"
#define keepConn "keepConn"
#define noticeList "noticeList"
#define adPlayList "adPlayList"
#define adPalyTotal "adPalyTotal"
#define monitor "monitor"
#define weatherInfo "weatherInfo"
#define shutDown "shutDown"
#define reboot "reboot"
#define configuer "configure"
#define templateUp "template"
#define resource "resource"
#define volume "volume"

#define HartFun 0
#define StatisticFun HartFun + 1
#define LinestopFun StatisticFun + 1
#define MonitorFun LinestopFun + 1
#define NoticeFun MonitorFun + 1
#define PlaylistFun NoticeFun + 1
#define ArrivalFun PlaylistFun + 1
#define WeatherFun ArrivalFun + 1
#define ShutdownFun WeatherFun + 1
#define RebootFun ShutdownFun + 1
#define TemplateFun RebootFun + 1
#define ConfigFun TemplateFun + 1
#define ResourceFun ConfigFun+1
#define VolumeFun ResourceFun+1

#define ResourceDir "../Resource/weather/"


enum Message_Type
{
	MT_NULL = -1,
	MT_Response = 0,
	MT_Dataset
};

enum MessageMap
{
	Message_NULL = 0,
	Message_Login,
	Message_Application,
	Message_LinestopRequest,
	Message_Linestop,
	Message_ConfigRequest,
	Message_Config,
	Message_TemplateRequest,
	Message_Template,
	Message_Keep,
	Message_Monitor,
	Message_Statistc,
	Message_Notice,
	Message_Playlist,
	Message_Arrival,
	Message_Weather,
	Message_Shutdown,
	Message_Reboot,
	Message_Response,
	Message_Resource,
	Message_Volume
};

class CPanelSize
{
public:
	CPanelSize()
	{
		left = 0;
		right = 0;
		top = 0;
		bottom = 0;	
	}
	~CPanelSize(){}

public:
	int left;
	int right;
	int top;
	int bottom;
};

class CLock 
{
public:
	CLock()
	{
		InitializeCriticalSection(&m_sec);
	}
	~CLock()
	{
		DeleteCriticalSection(&m_sec);
	}

	void Lock()
	{
		EnterCriticalSection(&m_sec);
	}
	void UnLock()
	{
		LeaveCriticalSection(&m_sec);
	}
private:
	CRITICAL_SECTION m_sec;
};

typedef struct _st_DataHead		//包头
{
	Message_Type messageType;
	MessageMap command;
	string strServerIP;
	string strStopID;
	string strCityCode;
	string strMsgID;

	_st_DataHead()
	{
		messageType = MT_NULL;
		command = Message_NULL;
		strServerIP = "";
		strStopID = "";
		strCityCode = "";
		strMsgID = "";
	}
}tagDataHead, *PDateHead;

typedef struct _st_Response		//应答结构
{
	tagDataHead head;
	int nStatus;
	string strMsg;
	_st_Response()
	{
		nStatus = 0;
		strMsg = "";
	}
}tagResponse, *PResponse;

typedef struct _st_authentication	//鉴权
{
	tagDataHead head;
	string strKey;

	_st_authentication()
	{
		strKey = "";
	}
} tagAuthentication, *PAutentication;

typedef struct _st_KeepHart	//心跳
{
	struct _st_Monitor
	{
		string strKey;
		string strValue;
	};

	tagDataHead head;
	vector<_st_Monitor> vecMonitor;
	vector<string> vecWarnning;

} tagKeepHart, *PKeepHart;

typedef struct _st_updateLinestop	//更新线站请求, 配置更新请求, 模板更新请求
{
	tagDataHead head;
	string strHash;
} tagUpdateLinestop, *PUpdateLinestop, tagConfiguer, *PConfigure, tagTemplate, *PTemplate;

typedef struct _st_ConfigUpdate
{
	tagDataHead head;
	int nUpdate;
	string strHashCode;
	string strCurrStopid;
	string strNoticeDir;
	string strTemplateDir;
	string strPictrueDir;
	string strDefaultDir;
	string strAdvDir;
	string strAdvPlaylist;
	string strStopName;
	string strUpdateDown;
	string strServerIP;
	string strLineDataDir;
	string strStatisticCycle;
	string strHartCycle;
} tagConfigUpdate, *PConfigUpdate;

typedef struct _st_TemplateUpdate
{
	tagDataHead head;
	int nUpdate;
	string strHashCode;
	string strCurrStopid;
	string strTemplateValue;
} tagTemplateUpdate, *PTemplateUpdate;

typedef struct _st_Notice
{
	struct _st_list
	{
		string strNoticeID;
		string strText;
	};

	string strID;
	string strStartDate;
	string strEndDate;
	string strStartTime;
	string strEndTime;
	string strInterval;
	string strOptr;
	string strDest;
	vector<_st_list> listNotice;

}tagNotice, *PNotice;		//公告播放列表

typedef struct _st_ListNotic
{
	tagDataHead sthead;
	list<tagNotice> listListNotice;
}tagListNotic, *PListNotic;

//播放列表存储结构，已弃用
typedef struct _st_Play
{
	struct _st_list
	{
		string strID;
		string strName;
		string strNum;

		bool operator < (const _st_list &s) const 
		{
			return strNum < s.strNum;
		}

		bool operator > (const _st_list &s) const
		{
			return strNum > s.strNum;
		}
	};

	string strStartDate;
	string strEndDate;
	string strStartTime;
	string strEndTime;
	vector<_st_list> listPlay;

}tagPlay, *PPlay;

typedef struct _st_ListPlay  //广告播放列表更新用结构
{
	tagDataHead sthead;
	string strUrl;
}tagListPlay, *PListPlay;

struct _st_Linestop		//线路详情（包含线路下的所有站点）
{
	struct _st_Stop
	{
		int nNumber;
		string strName;
		string strID;
		string strLng;
		string strLat;
	};

	string strID;
	string strLineName;
	string strAdminCode;
	string strAdminName;
	string strLength;
	int nLineType;
	string strLineDir;
	string strTimef;
	string strTimeL;
	int nIntervaln;
	int nIntervalm;
	string strCompany;
	string strCard;
	string strTicket;
	string strTicPrice;
	string strMonTicket;

	bool operator < (const _st_Linestop& t) const
	{
		return strID < t.strID;
		//使用ID去重
		int nLeft = 0, nRight = 0;
		nLeft = atoi(strLineName.c_str());
		nRight = atoi(t.strLineName.c_str());
		return (nLeft < nRight);
	}

	bool operator == (const _st_Linestop& t) const
	{
		return strID == t.strID;
	}

	bool operator()(const _st_Linestop& _Left, const _st_Linestop& _Right) const
	{	
		return strID < _Right.strID;
		//使用ID去重
		int nLeft = 0, nRight = 0;
		nLeft = atoi(_Left.strLineName.c_str());
		nRight = atoi(_Right.strLineName.c_str());
		return (nLeft < nRight);
	}


	vector<_st_Stop> listStop;
};

typedef struct _st_weather
{
	tagDataHead sthead;
	string strCity;
	string strCityCode;
	string strTwd;
	string strTtq;
	string strTtp;
	string strTfx;
	string strSkqw;
	string strSkfx;
	string strSksd;
	string strSkqy;
}tagWeather, *PWeather;

typedef struct tagBusArrivalInfo		//到站数据
{
	int iNum;
	int iDistance;
	int iTime;
	double dLongiti;
	double dLati;
	void Init()
	{
		iNum = -1;
		iDistance = -1;
		iTime = -1;
		dLongiti = 0;
		dLati = 0;
	}
	tagBusArrivalInfo()
	{
		Init();
	}
	~tagBusArrivalInfo()
	{
		Init();
	}
}BusArrivalInfo;

typedef struct _st_Arrival
{
	string strLineID;
	vector<tagBusArrivalInfo> vecArrivalInfo;
}tagArrival, *PArrival;

typedef struct _st_BusArrival
{
	tagDataHead stHead;
	map<string, tagArrival> mapLineArrival;
}tagBusArrival, *PBusArrival;

struct _st_PlayList			//程序内部使用的播放列表结构
{
	string strID;
	time_t tStart;
	time_t tEnd;
	int nCycle;
	vector<tagPlay::_st_list> listPlay;

	//暂时不对播放视频排序，留到以后做 by:wxl

	bool operator < (const _st_PlayList& t) const
	{
		return (strID < t.strID);
	}

	bool operator == (const _st_PlayList& t) const
	{
		return strID == t.strID;
	}

	bool operator()(const _st_PlayList& _Left, const _st_PlayList& _Right) const
	{	
		return (_Left < _Right);
	}

};

struct _st_NoticeList		//程序内部使用的公告列表结构
{
	string strID;
	time_t tStart;
	time_t tEnd;
	int nInterval;
	vector<tagNotice::_st_list> listNotice;

	~_st_NoticeList()
	{
		listNotice.clear();
	}

	bool operator < (const _st_NoticeList& t) const
	{
		return (strID < t.strID);
	}

	bool operator == (const _st_NoticeList& t) const
	{
		return strID == t.strID;
	}

	bool operator()(const _st_NoticeList& _Left, const _st_NoticeList& _Right) const
	{	
		return (_Left < _Right);
	}

};

typedef struct _st_PlayStatistic
{
	string strName;
	struct _st_StaData
	{
		string strDate;
		string strStartTime;
		string strEndTime;
	} Data;
} PlayStatistic, *PPlayStatistic;

typedef void (*LPFSETSTATISTIC)(PPlayStatistic p); //播放统计回调函数

typedef struct _st_StatisticUp
{
	typedef struct _st_PlayStatisticUp
	{
		string strName;
		vector<_st_PlayStatistic::_st_StaData> vecData;
	};

	tagDataHead head;
	vector<_st_PlayStatisticUp> vecStat;
} StatisticUp, *PStatisticUp;

typedef struct _st_ResourceDown
{
	struct _st_Resource
	{
		string strUrl;
		string strType;
	};
	tagDataHead head;
	vector<_st_Resource> vecResource;
}tagResourceDown, *PResourceDown;

typedef struct _st_VolumeConf
{
	struct _st_Volume
	{
		string strVolume;
		string strTime;
	};

	tagDataHead head;
	vector<_st_Volume> vecVolume;
}tagVolumeConf, *PVolumeConf;