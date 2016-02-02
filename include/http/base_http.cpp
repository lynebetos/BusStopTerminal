#include "StdAfx.h"
#include "base_http.h"
#include "base_string.h"
//#include "base_log.h"

#include <memory>
using namespace std;

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#define MAX_BUF_SIZE 256*1024

#define STATUS_CODE_FORBIDDEN  403

namespace base_http{
//////////////////////////////////////////////////////////////////////////
//HttpInfo
void HttpInfo::AddParam(const char *pName, const char *pValue, DWORD dwParamAttr)
{
	HttpParam param;
	param.strName = pName;
	param.strValue = pValue;
	param.dwParamAttr = dwParamAttr;
	vecParam.push_back(param);
}

void HttpInfo::ModifyParam(const char *pName, const char *pValue)
{
	vector<HttpParam>::iterator iter = vecParam.begin();
	while(iter != vecParam.end())
	{
		if(iter->strName == pName)
		{
			iter->strValue = pValue;
			break;
		}

		++iter;
	}
}

void HttpInfo::Clear()
{
	strUrl.empty();
	vecParam.clear();
}

string HttpInfo::GetParamString()
{
	string strParam;
	int nSize = vecParam.size();
	for(int i=0; i<nSize; ++i)
	{
		strParam += vecParam[i].strName + "=" + vecParam[i].strValue;
		if(i < nSize -1)
			strParam += "&";
	}

	return strParam;
}

//////////////////////////////////////////////////////////////////////////
//CNWCookies
CHttpCookies::CHttpCookies()
{

}

CHttpCookies::~CHttpCookies()
{

}

void CHttpCookies::SetCookies(const char *pCookies)
{
	assert(pCookies);
	string strCookies = pCookies;
	string strTemp;

	int nPos = string::npos;
	while((nPos = strCookies.find(';')) != string::npos)
	{
		strTemp = strCookies.substr(0, nPos);
		strCookies.erase(0, nPos+1);
		AddCookies(strTemp);
	}

	if(strCookies.length() > 0)
		AddCookies(strCookies);
}

void CHttpCookies::AddCookies(const string & strCookies)
{
	int nPos = strCookies.find('=');

	if(nPos != string::npos)
	{
		string strName, strValue;
		strName = strCookies.substr(0, nPos);
		strName = base_string::_TrimA(strName.c_str());
		strValue = strCookies.substr(nPos+1);
		strValue = base_string::_TrimA(strValue.c_str());
		m_mapCookies[strName] = strValue;
	}
}

void CHttpCookies::AddCookies(const string &strName, const string &strValue)
{
	m_mapCookies[strName] = strValue;
}

bool CHttpCookies::GetCookies(const char *pName,  string &strData)
{
	map<string, string>::iterator iter = m_mapCookies.find(pName);
	if(iter != m_mapCookies.end())
	{
		strData = iter->second;
		return true;
	}

	return false;
}

void CHttpCookies::Clear()
{
	m_mapCookies.clear();
}

//////////////////////////////////////////////////////////////////////////
//CBaseHttp
CBaseHttp::CBaseHttp() 
	: m_dwIgnoreFlag(IGNORE_NORMAL)
{
	memset(&m_httpData, 0, sizeof(m_httpData));
}

CBaseHttp::~CBaseHttp()
{
	if(m_httpData.pData)
	{
		delete[] m_httpData.pData;
		m_httpData.pData = NULL;
	}
}

void CBaseHttp::SetInternet(bool bUseProxy, const char* pProxyUrl)
{
	if(bUseProxy)
	{
		assert(pProxyUrl);
		m_httpObj.SetInternet("NetWorld User Agent v1.1", INTERNET_OPEN_TYPE_PROXY, pProxyUrl, NULL, 0);
	}
	else
	{
		m_httpObj.SetInternet("NetWorld User Agent v1.1");
	}
}

void CBaseHttp::AddHttpHeader(const char* pName, const char *pValue)
{
	m_httpObj.AddHeader(pName, pValue);
}

void CBaseHttp::RemoveAllHeader(const char *pName)
{
	m_httpObj.RemoveAllHeader(pName);
}

bool CBaseHttp::HttpGet(HttpInfo &info, bool bGetCookies/* = false*/, int nTryCount/* = 5*/)
{
	for(int i=0; i<nTryCount; ++i)
	{
		if(Http(ht_get, &info, bGetCookies))
			return true;
	}

	return false;
}

bool CBaseHttp::HttpGet(const char *pUrl, bool bGetCookies/* = false*/, int nTryCount/* = 5*/)
{
	assert(pUrl);
	HttpInfo info;
	info.strUrl = pUrl;
	return HttpGet(info, bGetCookies, nTryCount);
}

bool CBaseHttp::HttpPost(HttpInfo &info, bool bGetCookies/* = false*/, int nTryCount/* = 5*/)
{
	for(int i=0; i<nTryCount; ++i)
	{
		if(Http(ht_post, &info, bGetCookies))
			return true;
	}

	return false;
}

bool CBaseHttp::HttpsPost(HttpInfo &info, bool bGetCookies/* = false*/, int nTryCount/* = 5*/)
{
	URL_COMPONENTS crackedURL;

	char szHostName[128] = {0};
	char szUrlPath[256] = {0};

	string strUrl = info.strUrl;
	bool nRet = false;

	string postData = "";
	vector<HttpParam>::iterator iter = info.vecParam.begin();
	while(iter != info.vecParam.end())
	{
		postData += iter->strName;
		postData += "=";
		postData += iter->strValue;
		postData += "&";
		iter++;
	}

	ZeroMemory(&crackedURL, sizeof (crackedURL));
	crackedURL.dwStructSize = sizeof (crackedURL);
	crackedURL.lpszHostName = szHostName;
	crackedURL.dwHostNameLength = (sizeof(szHostName)/sizeof(char))-1;
	crackedURL.lpszUrlPath = szUrlPath; 
	crackedURL.dwUrlPathLength = (sizeof(szUrlPath)/sizeof(char))-1;

	::InternetCrackUrl(strUrl.c_str(), (DWORD)strUrl.length(), 0, &crackedURL);

	DWORD dwFlags = 0;
	::InternetGetConnectedState(&dwFlags, 0);

	const char *pstrAgent = "NetWorld User Agent v1.0";
	HINTERNET hHttpOpen = NULL;
	HINTERNET hHttpConn = NULL;

	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
	{
		hHttpOpen = ::InternetOpen(pstrAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	}
	else
	{
		hHttpOpen = ::InternetOpen(pstrAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	}

	if(hHttpOpen == NULL)
	{
		//SetLastError("HTTP初始化失败");
		return false;
	}

	//建立HTTP连接
	hHttpConn = ::InternetConnect(hHttpOpen, crackedURL.lpszHostName, crackedURL.nPort, 
		NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	if(hHttpConn == NULL)
	{
		//SetLastError("HTTP连接失败，请检查网络连接");
		::InternetCloseHandle(hHttpOpen);
		return false;
	}

	//创建一个URL请求
	HINTERNET hHttpReq = ::HttpOpenRequest(hHttpConn, "POST", crackedURL.lpszUrlPath, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);

	if(hHttpReq == NULL)
	{
		//SetLastError("HTTP创建URL请求失败，请检查网络连接");
		::InternetCloseHandle(hHttpOpen);
		::InternetCloseHandle(hHttpConn);
		return false;
	}

	string strHeader = "Content-type: application/x-www-form-urlencoded\r\n";

	if (!::HttpAddRequestHeaders(hHttpReq, strHeader.c_str(), strHeader.length(), HTTP_ADDREQ_FLAG_ADD))
	{
		goto _exit;
	}

	DWORD dwContLen = 0;
	DWORD dwLen = 0;
	BOOL  bResult = FALSE;
	DWORD nBytesGet = 0;
	DWORD dwRetCode = 0;
	DWORD dwSizeOfRq = sizeof(DWORD);
	char szLen[64] = {0};
	dwSizeOfRq = sizeof(szLen)/sizeof(char);
	const char * strPostData = postData.c_str();

	//const char *pHeader = L"GET / HTTP/1.1 \nAccept: */* \nAccept-Language: zh-cn \nAccept-Encoding: gzip, deflate \nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0) \n";

	if (::HttpSendRequest(hHttpReq, 0, 0, (void *)strPostData, postData.length()) && 
		::HttpQueryInfo(hHttpReq, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL) && 
		dwRetCode < 400)
	{
		::HttpQueryInfoW(hHttpReq, HTTP_QUERY_CONTENT_LENGTH, &szLen, &dwSizeOfRq, NULL);
		DWORD dwRet = ::GetLastError();
		if(strlen(szLen) < 1)
			dwContLen = MAX_BUF_SIZE;
		else
			dwContLen = atol(szLen);

		if(dwContLen < 1)
			goto _exit;

		AllocBuf(dwContLen+1);

		m_httpData.nDataLen = 0;
		while(TRUE)
		{
			if (!::InternetReadFile(hHttpReq, m_httpData.pData, dwContLen, &dwLen))
			{
				break;
			}

			m_httpData.nDataLen += dwLen;
			if(m_httpData.nDataLen >= dwContLen || dwLen == 0)
			{
				nRet = true;
				break;
			}
		}

		if(nRet && bGetCookies)
		{
			//base_string::BAString strCookie(strUrl.c_str());
			HandleCookies(strUrl.c_str());
		}
	}

_exit:
	::InternetCloseHandle(hHttpReq);
	::InternetCloseHandle(hHttpConn);
	::InternetCloseHandle(hHttpOpen);

	return true;
}

bool CBaseHttp::Http(HttpType type, HttpInfo *pInfo, bool bGetCookies/* = false*/)
{
	LogObject;
	assert(pInfo);
	bool bRet = false;

	try
	{
		CHttpResponse* pobjRes = NULL;
		HandleParams(pInfo);

		switch(type)
		{
		case ht_post:
			{
				m_httpObj.BeginPost(pInfo->strUrl.c_str());
				const DWORD cbProceed = 1024;
				do 
				{
					// Place codes to report progress information to user.
				} while (!(pobjRes = m_httpObj.Proceed (cbProceed)));
			}
			break;
		case ht_get:
			pobjRes = m_httpObj.RequestGetEx(pInfo->strUrl.c_str());
			break;
		default:
			break;
		}

		std::auto_ptr<CHttpResponse> pAutoObj(pobjRes);
		bRet = HandleResponse(pAutoObj.get(), pInfo, bGetCookies);
		if(bRet)
			HandleHttpData();
		else
		{
			string str = "http没有应答";
			//LogOutError(str.c_str());
		}
	}
	catch (CHttpClient::Exception & e) 
	{
		//LOG_ERROR("Exception error = %S", e.errmsg());
		//LogOutError(e.errmsg());
		m_httpObj.Cancel() ;
		return false;
	}

	return bRet;
}

bool CBaseHttp::HandleHttpData()
{
	return true;
}

void CBaseHttp::HandleParams(HttpInfo *pInfo)
{
	m_httpObj.ClearParam();  //pxx add
	vector<HttpParam>::iterator iter = pInfo->vecParam.begin();
	while(iter != pInfo->vecParam.end())
	{
		m_httpObj.AddParam(iter->strName.c_str(), iter->strValue.c_str(), iter->dwParamAttr);
		++iter;
	}
}

bool CBaseHttp::HandleResponse(CHttpResponse *pobjRes, HttpInfo *pInfo, bool bGetCookies)
{
	LogObject;
	bool bRet = false;
	if(pobjRes)
	{
		DWORD dwStatus = pobjRes->GetStatus();
		if(dwStatus >= 400)
		{
			char cl[8] = {0};
			sprintf(cl, "%d", dwStatus);
			string str = "Recv error =";
			str += cl;

			//LogOutError(str);
			if (dwStatus == STATUS_CODE_FORBIDDEN && (m_dwIgnoreFlag & IGNORE_FORBID))
			{  
				m_dwIgnoreFlag = IGNORE_NORMAL;  //reset
			}
			else
			{
				return bRet;
			}
		}

		DWORD dwLen = 0;
		pobjRes->GetContentLength(dwLen);
		if(dwLen < 1)
			dwLen = MAX_BUF_SIZE;
		else
			dwLen += 1;

		AllocBuf(dwLen);
		DWORD dwRead = 0;
		while(dwRead = pobjRes->ReadContent(m_httpData.pData + m_httpData.nDataLen, dwLen - m_httpData.nDataLen))  //sherrypan modify  
		{
			m_httpData.nDataLen += dwRead;
			if(m_httpData.nDataLen >= dwLen)
				break;
		}

		bRet = m_httpData.nDataLen > 0;

		if(bRet && bGetCookies)
		{
			//base_string::BAString strAUrl(pInfo->strUrl.c_str());
			HandleCookies(pInfo->strUrl.c_str());
		}
	}

	return bRet;
}

void CBaseHttp::HandleCookies(const char *pUrl)
{
	assert(pUrl);
	if(pUrl)
	{
		DWORD dwCookieSize = 0;
		::InternetGetCookieA(pUrl, "", NULL, &dwCookieSize);
		if(dwCookieSize > 0)
		{
			char *pCookies = new char[dwCookieSize+1];
			memset(pCookies, 0, (dwCookieSize+1)*sizeof(char));
			::InternetGetCookieA(pUrl, "", pCookies, &dwCookieSize);
			m_Cookies.SetCookies(pCookies);
			delete[] pCookies;
		}
	}
}

bool CBaseHttp::HttpDownload(const char *pUrl, const char *pFilePath)
{
	bool bRet = HttpGet(pUrl, false);

	if(pFilePath && bRet)
	{
		return SaveDataToFile(pFilePath);
	}

	return bRet;
}

void CBaseHttp::AllocBuf(DWORD nSize)
{
	if(nSize > m_httpData.nSize)
	{
		m_httpData.nSize = nSize;
		if(m_httpData.pData)
		{
			delete[] m_httpData.pData;
			m_httpData.pData = NULL;
		}

		m_httpData.pData = new BYTE[nSize];
	}

	if(m_httpData.pData)
		memset(m_httpData.pData, 0, m_httpData.nSize);

	m_httpData.nDataLen = 0;
}

bool CBaseHttp::SaveDataToFile(const char *pFilePath)
{
	assert(pFilePath && m_httpData.pData);

	FILE *file = NULL;
	fopen_s(&file, pFilePath, "wb+");

	bool bRet = false;
	if(file)
	{
		bRet = fwrite(m_httpData.pData, 1, m_httpData.nDataLen, file) > 0 ? true : false;
		fclose(file);
	}

	return bRet;
}

bool CBaseHttp::HttpsPostA(HttpInfo &info, bool bGetCookies/* = false*/, int nTryCount/* = 5)*/)
{
	URL_COMPONENTSA crackedURL;

	char szHostName[128] = {0};
	char szUrlPath[256] = {0};

	string strUrl = info.strUrl.c_str();
	bool nRet = false;

	string postData = "";
	vector<HttpParam>::iterator iter = info.vecParam.begin();
	while(iter != info.vecParam.end())
	{
		postData += iter->strName;
		postData += "=";
		postData += iter->strValue;
		postData += "&";
		iter++;
	}
	string strPostData = postData.c_str();

	ZeroMemory(&crackedURL, sizeof (crackedURL));
	crackedURL.dwStructSize = sizeof (crackedURL);
	crackedURL.lpszHostName = szHostName;
	crackedURL.dwHostNameLength = (sizeof(szHostName)/sizeof(char))-1;
	crackedURL.lpszUrlPath = szUrlPath; 
	crackedURL.dwUrlPathLength = (sizeof(szUrlPath)/sizeof(char))-1;

	::InternetCrackUrlA(strUrl.c_str(), (DWORD)strUrl.length(), 0, &crackedURL);

	DWORD dwFlags = 0;
	::InternetGetConnectedState(&dwFlags, 0);

	const char *pstrAgent = "NetWorld User Agent v1.0";
	HINTERNET hHttpOpen = NULL;
	HINTERNET hHttpConn = NULL;

	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
	{
		hHttpOpen = ::InternetOpenA(pstrAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	}
	else
	{
		hHttpOpen = ::InternetOpenA(pstrAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	}

	if(hHttpOpen == NULL)
	{
		//SetLastError("HTTP初始化失败");
		return false;
	}

	//建立HTTP连接
	hHttpConn = ::InternetConnectA(hHttpOpen, crackedURL.lpszHostName, crackedURL.nPort, 
		NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	if(hHttpConn == NULL)
	{
		//SetLastError("HTTP连接失败，请检查网络连接");
		::InternetCloseHandle(hHttpOpen);
		return false;
	}

	//创建一个URL请求
	HINTERNET hHttpReq = ::HttpOpenRequestA(hHttpConn, "POST", crackedURL.lpszUrlPath, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);

	if(hHttpReq == NULL)
	{
		//SetLastError("HTTP创建URL请求失败，请检查网络连接");
		::InternetCloseHandle(hHttpOpen);
		::InternetCloseHandle(hHttpConn);
		return false;
	}

	string strHeader = "Content-type: application/x-www-form-urlencoded\r\n";

	if (!::HttpAddRequestHeadersA(hHttpReq, strHeader.c_str(), strHeader.length(), HTTP_ADDREQ_FLAG_ADD))
	{
		goto _exit;
	}

	DWORD dwContLen = 0;
	DWORD dwLen = 0;
	BOOL  bResult = FALSE;
	DWORD nBytesGet = 0;
	DWORD dwRetCode = 0;
	DWORD dwSizeOfRq = sizeof(DWORD);
	char szLen[64] = {0};
	dwSizeOfRq = sizeof(szLen)/sizeof(char);
	//const char * strPostData = postData.c_str();

	//const char *pHeader = L"GET / HTTP/1.1 \nAccept: */* \nAccept-Language: zh-cn \nAccept-Encoding: gzip, deflate \nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0) \n";

	if (::HttpSendRequestA(hHttpReq, 0, 0, (void *)strPostData.c_str(), strPostData.length()) && 
		::HttpQueryInfoA(hHttpReq, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL) && 
		dwRetCode < 400)
	{
		::HttpQueryInfoA(hHttpReq, HTTP_QUERY_CONTENT_LENGTH, &szLen, &dwSizeOfRq, NULL);
		DWORD dwRet = ::GetLastError();
		if(strlen(szLen) < 1)
			dwContLen = MAX_BUF_SIZE;
		else
			dwContLen = atoi(szLen);

		if(dwContLen < 1)
			goto _exit;

		AllocBuf(dwContLen+1);

		m_httpData.nDataLen = 0;
		while(TRUE)
		{
			if (!::InternetReadFile(hHttpReq, m_httpData.pData, dwContLen, &dwLen))
			{
				break;
			}

			m_httpData.nDataLen += dwLen;
			if(m_httpData.nDataLen >= dwContLen || dwLen == 0)
			{
				nRet = true;
				break;
			}
		}

		/*if(bGetCookies)
		{
		DWORD dwCookieSize = 0;
		base_help::BAString strCookie(strUrl.c_str());
		::InternetGetCookieA(strCookie.c_str(), "", NULL, &dwCookieSize);
		if(dwCookieSize > 0)
		{
		char *pCookies = new char[dwCookieSize+1];
		memset(pCookies, 0, dwCookieSize+1);
		::InternetGetCookieA(strCookie.c_str(), "", pCookies, &dwCookieSize);
		m_Cookies.SetCookies(_Bstring(pCookies));
		delete[] pCookies;
		}
		}*/
	}

_exit:
	::InternetCloseHandle(hHttpReq);
	::InternetCloseHandle(hHttpConn);
	::InternetCloseHandle(hHttpOpen);

	return true;
}
}