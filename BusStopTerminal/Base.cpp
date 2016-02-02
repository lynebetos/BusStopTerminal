#include "StdAfx.h"
#include "Base.h"
#include <io.h>
#include <mmsystem.h>
#include <Psapi.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "psapi.lib")

string CBase::time2string(time_t t)
{
	tm* time = localtime(&t);
	char sztime[64] = {0};
	sprintf(sztime, "%04d-%02d-%02d %02d:%02d%02d", 
		time->tm_year + 1900, 
		time->tm_mon + 1,
		time->tm_mday,
		time->tm_hour,
		time->tm_min,
		time->tm_sec);

	string strTime = sztime;
	return strTime;
}

string CBase::time2string_date(time_t t)
{
	tm* time = localtime(&t);
	char sztime[64] = {0};
	sprintf(sztime, "%04d-%02d-%02d", 
		time->tm_year + 1900, 
		time->tm_mon + 1,
		time->tm_mday);

	string strTime = sztime;
	return strTime;
}

string CBase::time2string_time(time_t t)
{
	tm* time = localtime(&t);
	char sztime[64] = {0};
	sprintf(sztime, "%02d:%02d:%02d", 
		time->tm_hour,
		time->tm_min,
		time->tm_sec);

	string strTime = sztime;
	return strTime;
}


time_t CBase::string2time(string strtime)
{
	tm time = {0};
	sscanf(strtime.c_str(), "%04d-%02d-%02d %02d:%02d%02d",
		&time.tm_year, 
		&time.tm_mon,
		&time.tm_mday,
		&time.tm_hour,
		&time.tm_min,
		&time.tm_sec);

	time.tm_year -= 1900;
	time.tm_mon--;
	time.tm_isdst = -1;

	time_t t = mktime(&time);
	return t;
}

time_t CBase::string2time_time(string strtime)
{
	tm time = {0};
	sscanf(strtime.c_str(), "%02d:%02d%02d",
		&time.tm_hour,
		&time.tm_min,
		&time.tm_sec);

	time.tm_year = 0;
	time.tm_mon = 0;
	time.tm_mday = 0;
	time.tm_wday = 0;
	time.tm_yday = 0;

	time_t t = mktime(&time);
	return t;
}


time_t CBase::string2time_date(string strtime)
{
	tm time = {0};
	sscanf(strtime.c_str(), "04d-%02d-%02d",
		&time.tm_year, 
		&time.tm_mon,
		&time.tm_mday);

	time.tm_hour = 0;
	time.tm_min = 0;
	time.tm_sec = 0;
	time.tm_year -= 1900;
	time.tm_mon--;
	time.tm_isdst = -1;

	time_t t = mktime(&time);
	return t;
}

std::string AnsiToUtf8(std::string strAnsi)
{
	int WLength = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
	LPWSTR pszW = (LPWSTR) malloc((WLength+1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, pszW, WLength);

	int ALength = WideCharToMultiByte(CP_UTF8, 0, pszW, -1, NULL, 0, NULL, NULL);
	LPSTR pszA = (LPSTR)malloc( ALength + 1);
	WideCharToMultiByte(CP_UTF8, 0, pszW, -1, pszA, ALength, NULL, NULL);  
	pszA[ALength] = 0;

	std::string strUtf8 = pszA;

	free(pszW);
	free(pszA);

	return strUtf8;
}

std::string WcharToUtf8(std::wstring strUnicode)
{
	int ALength = WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	LPSTR pszA = (LPSTR)malloc( ALength + 1);
	WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, pszA, ALength, NULL, NULL);  
	pszA[ALength] = 0;

	std::string strUtf8 = pszA;
	free(pszA);

	return strUtf8;
}

std::string Utf8toAnsi( std::string strutf8 )
{  
	int WLength = MultiByteToWideChar(CP_UTF8, 0, strutf8.c_str(), -1, NULL, NULL );
	LPWSTR pszW = (LPWSTR) malloc( (WLength + 1) *sizeof(WCHAR) ) ;    
	MultiByteToWideChar(CP_UTF8, 0, strutf8.c_str(), -1, pszW, WLength );
	pszW[WLength] = 0;

	int ALength = WideCharToMultiByte(CP_ACP, 0, pszW, -1, NULL, 0, NULL, NULL);
	LPSTR pszA = ( LPSTR ) malloc ( ALength + 1 );
	WideCharToMultiByte(CP_ACP, 0, pszW, -1, pszA, ALength, NULL, NULL);
	pszA[ALength] = 0;

	std::string strAnsi = pszA;

	free(pszW);
	free(pszA);
	return strAnsi;
}

std::wstring Utf8toWchar( std::string utf8)
{
	int WLength = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, NULL );
	LPWSTR pszW = (LPWSTR) malloc( (WLength + 1) *sizeof(WCHAR) ) ;    
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, pszW, WLength );
	pszW[WLength] = 0;

	std::wstring wstrW = pszW;

	free(pszW);
	return wstrW;
}

std::wstring AnsiToWchar(std::string strAnsi)
{
	int WLength = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
	LPWSTR pszW = (LPWSTR) malloc((WLength+1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, pszW, WLength);
	pszW[WLength] = 0;

	std::wstring wstr = pszW;
	return wstr;
}

void Split(vector<string>& vec, const string& strSource, const string& strDelim)
{
	string::size_type i = 0;
	string::size_type j = strSource.find(strDelim);
	while (j != string::npos)
	{
		vec.push_back(strSource.substr(i, j-i));

		i = j + strDelim.length();
		j = strSource.find(strDelim, i);
	}
	if (j == string::npos)
		vec.push_back(strSource.substr(i, strSource.length()));
}

void GetMsgId(string& strMsgID)
{
	GUID guid;
	::CoCreateGuid(&guid);

	char buf[64] = {0};

	sprintf(buf
		, "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X"
		, guid.Data1
		, guid.Data2
		, guid.Data3
		, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
	);

	strMsgID = buf;
}

/*
	string 转换为 wstring 
*/
std::wstring c2w(const char *pc)
{
	std::wstring val = L"";

	if(NULL == pc)
	{
		return val;
	}
	//size_t size_of_ch = strlen(pc)*sizeof(char);
	//size_t size_of_wc = get_wchar_size(pc);
	size_t size_of_wc;
	size_t destlen = mbstowcs(0,pc,0);
	if (destlen ==(size_t)(-1))
	{
		return val;
	}
	size_of_wc = destlen+1;
	wchar_t * pw  = new wchar_t[size_of_wc];
	mbstowcs(pw,pc,size_of_wc);
	val = pw;
	delete pw;
	return val;
}
/*
	wstring 转换为 string
*/
std::string w2c(const wchar_t * pw)
{
	std::string val = "";
	if(!pw)
	{
   		return val;
	}
	size_t size= wcslen(pw)*sizeof(wchar_t);
	char *pc = NULL;
	if(!(pc = (char*)malloc(size)))
	{
   		return val;
	}
	size_t destlen = wcstombs(pc,pw,size);
	/*转换不为空时，返回值为-1。如果为空，返回值0*/
	if (destlen ==(size_t)(0))
	{
		return val;
	}
	val = pc;
	delete pc;
	return val;
}

void getFiles( string path, vector<string>& files )
{
	//文件句柄
	long   hFile   =   0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if((fileinfo.attrib &  _A_SUBDIR))
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles( p.assign(path).append("\\").append(fileinfo.name), files );
			}
			else
			{
				files.push_back(p.assign(path).append(fileinfo.name) );
			}
		}while(_findnext(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}
}

BOOL SystemOprate(UINT opt)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	// Shut down the system and force all applications to close. 

	if (!ExitWindowsEx(opt | EWX_FORCE, 
		SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED)) 
		return FALSE; 

	return TRUE;
}

BOOL GetSystemVolum(DWORD &dwVolum)
{
	MIXERCONTROLDETAILS_SIGNED volStruct;
	MIXERCONTROLDETAILS mxcd;
	MIXERLINECONTROLS mxlc;
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	HMIXER hmx;

	UINT nNum = mixerGetNumDevs();

	UINT err = mixerOpen(&hmx, 0, 0, 0, MIXER_OBJECTF_MIXER);
	if(err)
	{
		string str = "1";
	}

	//初始化MIXERLINE结构体  
	ZeroMemory(&mxl,sizeof(mxl));  
	mxl.cbStruct = sizeof(mxl);  
	// 指出需要获取的通道，扬声器用MIXERLINE_COMPONENTTYPE_DST_SPEAKERS  
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;  
	if(mixerGetLineInfo((HMIXEROBJ)hmx,&mxl,  
		MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE))  
	{  
		string str = "1";
		
	}  
	// 取得控制器.  
	ZeroMemory(&mxlc, sizeof(mxlc));  
	mxlc.cbStruct = sizeof(mxlc);  
	mxlc.dwLineID = mxl.dwLineID;  
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;  
	mxlc.cControls =1;  
	mxlc.cbmxctrl = sizeof(mxc);  
	mxlc.pamxctrl = &mxc;  
	ZeroMemory(&mxc, sizeof(mxc));  
	mxc.cbStruct = sizeof(mxc);  
	if(mixerGetLineControls((HMIXEROBJ)hmx,&mxlc,  
		 MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE))  
	{  
		string str = "1";	
	}  
	// 获取控制器中的值的音量范围：mxc.Bounds.lMinimum到mxc.Bounds.lMaximum.  
	  
	// 初始化MIXERCONTROLDETAILS结构体  
	ZeroMemory(&mxcd, sizeof(mxcd));  
	mxcd.cbStruct = sizeof(mxcd);  
	mxcd.cbDetails = sizeof(volStruct);  
	mxcd.dwControlID = mxc.dwControlID;  
	mxcd.paDetails = &volStruct;  
	mxcd.cChannels = 1;  
	  
	// 获得音量值  
	if(mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd,  
		MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))  
	{  
		string str = "1";
	}  

	if(mixerClose(hmx))
	{
		string str = "1";

	}
	
	dwVolum = (DWORD)((double)volStruct.lValue / (double)mxc.Bounds.dwMaximum * 100);
	return TRUE;
}

string ReplaceAll(string& strSrc, const string& strOld, const string& strNew)
{
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=strNew.size())  
	{     
		if((pos=strSrc.find(strOld,pos))!=string::npos)     
			 strSrc.replace(pos, strOld.length(), strNew.c_str());     
		else 
			break;     
	}

	return strSrc;
}

void PrintMemoryInfo( DWORD processID, string& text)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	// Print the process identifier.

	//printf( "\nProcess ID: %u\n", processID );
	stringstream sstrId;
	sstrId<<"Process ID: "<<processID<<endl;
	text = sstrId.str();
	sstrId.str("");

	// Print information about the memory usage of the process.

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID );
	if (NULL == hProcess)
		return;

	if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		stringstream sstr;
		sstr<<"\tPageFaultCount:"<< pmc.PageFaultCount<<endl<<"\tPeakWorkingSetSize:"<<pmc.PeakWorkingSetSize<<endl
			<<"\tWorkingSetSize:"<<pmc.WorkingSetSize<<endl<<"\tQuotaPeakPagedPoolUsage:"<<pmc.QuotaPeakPagedPoolUsage<<endl
			<<"\tQuotaPagedPoolUsage:"<<pmc.QuotaPagedPoolUsage<<endl<<"\tQuotaPeakNonPagedPoolUsage:"<<pmc.QuotaPeakNonPagedPoolUsage<<endl
			<<"\tQuotaNonPagedPoolUsage:"<<pmc.QuotaNonPagedPoolUsage<<endl<<"\tPagefileUsage:"<<pmc.PagefileUsage<<endl
			<<"\tPeakPagefileUsage:"<<pmc.PeakPagefileUsage;

		text += sstr.str();
		sstr.str() = "";
	}

	CloseHandle( hProcess );
}

void PrintMemoryInfoCSV( DWORD processID, const string& strFIlePath)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	// Print the process identifier.

	//printf( "\nProcess ID: %u\n", processID );

	// Print information about the memory usage of the process.

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID );
	if (NULL == hProcess)
		return;

	ofstream ofs(strFIlePath.c_str(), ios::app | ios::out);
	if(!ofs.is_open())
		return;

	ofs.seekp(ios::end);
	int nFileLen = ofs.tellp();

	if(nFileLen <= 5)
	{
		ofs<<"PageFaultCount"<<",PeakWorkingSetSize"<<",WorkingSetSize"<<",QuotaPeakPagedPoolUsage"
			<<",QuotaPagedPoolUsage"<<",QuotaPeakNonPagedPoolUsage"<<",QuotaNonPagedPoolUsage"
			<<",PagefileUsage"<<",PeakPagefileUsage"<<endl;
	}

	if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		ofs<<pmc.PageFaultCount<<","<<pmc.PeakWorkingSetSize<<","<<pmc.WorkingSetSize<<","<<pmc.QuotaPeakPagedPoolUsage<<","
			<<pmc.QuotaPagedPoolUsage<<","<<pmc.QuotaPeakNonPagedPoolUsage<<","<<pmc.QuotaNonPagedPoolUsage<<","
			<<pmc.PagefileUsage<<","<<pmc.PeakPagefileUsage<<endl;
	}

	CloseHandle( hProcess );
}
