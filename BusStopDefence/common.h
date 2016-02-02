#pragma once
#include <Tlhelp32.h.>

#define  WM_SHOWTASK WM_USER+1
#define  WM_SHUTAPP WM_SHOWTASK+1

#define EVENT_PAGE_READ "Global\\Busstop_Mapping_read_event"
#define EVENT_RESTART_APP "Global\\Busstop_Restart_event"
#define MAPPING_NAME "BusStop_Mapping"

#define MAPPING_SIZE 1024

#define RESTART_APP 1		//终端自重启 线站、模板、配置更新
#define RESTART_UPDATE 2 //
#define RUN_NOMAL 3		//终端运行正常

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

typedef struct MessageInfo
{
	unsigned int nMessageID;
}MInfo, *PMInfo;

inline DWORD GetProcessidFromName(LPCTSTR name)
{
	PROCESSENTRY32 pe;
	DWORD id=0;
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
		return 0;
	while(1)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==FALSE)
			break;
		if(strcmp(pe.szExeFile,name)==0)
		{
			id=pe.th32ProcessID;

			break;
		}


	}
	CloseHandle(hSnapshot);
	return id;
}

inline string GetFileName(string strPathName)
{
	int nPos = strPathName.find_last_of("\\");
	string strSub = strPathName.substr(nPos);
	return strSub;
}
