//author:cooper

#pragma once

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "Http/RyeolHttpClient.h"
using namespace Ryeol;

namespace base_http{

//add by sherrypan
enum HttpParamAttr  //参照RyeolHttpCllient.h   enum ParamAttr
{
	ParamNormal			= 0x00000000,		
	ParamFile			= 0x00000001,		
	ParamEncodedName	= 0x00000002,	
	ParamEncodedValue	= 0x00000004,		
	ParamEncoded		= ParamEncodedName | ParamEncodedValue	
};

enum HttpType
{
	ht_get,
	ht_post,
};

enum IgnoreFlay
{
	IGNORE_NORMAL = 0x0,
	IGNORE_FORBID = 0x1
};

struct HttpData
{
	BYTE   *pData;
	DWORD  nDataLen;
	DWORD  nSize;
};

struct HttpParam
{
	string strName;
	string strValue;
	DWORD dwParamAttr;  //add by sherrypan
};

struct HttpInfo
{
	string				strUrl;
	vector<HttpParam>	vecParam;

	void AddParam(const char *pName, const char *pValue, DWORD dwParamAttr = ParamNormal);
	void ModifyParam(const char *pName, const char *pValue);
	void Clear();
	string GetParamString();
};

class CHttpCookies
{
public:
	CHttpCookies();
	~CHttpCookies();

	void SetCookies(const char *pCookies);
	bool GetCookies(const char *pName,  string &strData);
	void Clear();
	void AddCookies(const string &strName, const string &strValue);

private:
	void AddCookies(const string & strCookies);

private:
	map<string, string> m_mapCookies;
};

class CBaseHttp
{
public:
	CBaseHttp();
	virtual ~CBaseHttp();

	void SetInternet(bool bUseProxy, const char* pProxyUrl);
	void AddHttpHeader(const char* pName, const char *pValue);
	void RemoveAllHeader(const char *pName);
	bool HttpGet(const char *pUrl, bool bGetCookies = false, int nTryCount = 5);
	bool HttpGet(HttpInfo &info, bool bGetCookies = false, int nTryCount = 5);
	bool HttpPost(HttpInfo &info, bool bGetCookies = false, int nTryCount = 5);
	bool HttpsPost(HttpInfo &info, bool bGetCookies = false, int nTryCount = 5);
	bool HttpsPostA(HttpInfo &info, bool bGetCookies = false, int nTryCount = 5);
	bool HttpDownload(const char *pUrl, const char *pFilePath);
	bool SaveDataToFile(const char *pFilePath);
	int  GetDataLen() { return m_httpData.nDataLen; }

	const BYTE *GetData() { return m_httpData.pData; }

	bool GetCookies(const char *pName, string &strData) { return m_Cookies.GetCookies(pName, strData); }
	void AddCookies(const string &strName, const string &strValue) { m_Cookies.AddCookies(strName, strValue); }

	//add by sherrypan  每次调用HttpGet或者HttpPost时都需要调用，因为Http之后会reset
	void SetIgnoreFlag(DWORD dwFlag)  {m_dwIgnoreFlag = dwFlag;}

protected:
	void HandleCookies(const char *pUrl);
	void HandleParams(HttpInfo *pInfo);
	bool HandleResponse(CHttpResponse *pobjRes, HttpInfo *pInfo, bool bGetCookies);
	void AllocBuf(DWORD nSize);
	bool Http(HttpType type, HttpInfo *pInfo, bool bGetCookies = false);

	virtual bool HandleHttpData();

protected:
	HttpData		m_httpData;
	CHttpCookies	m_Cookies;
	CHttpClient		m_httpObj;
	DWORD			m_dwIgnoreFlag;
};

}