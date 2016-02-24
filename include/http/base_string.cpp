//#include "StdAfx.h"

#include "base_string.h"

#include <time.h>
#include <sstream>


void base_string::_Replace(string &src, const char cOld, const char cNew)
{
	size_t nSize = src.size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(src.at(i) == cOld)
			src[i] = cNew;
	}
}

void base_string::_Replace(wstring &src, const wchar_t cOld, const wchar_t cNew)
{
	size_t nSize = src.size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(src.at(i) == cOld)
			src[i] = cNew;
	}
}

void base_string::_Erase(string &src, const char c)
{
	int nPos = string::npos;
	while((nPos = src.find(c)) != string::npos)
		src.erase(nPos, 1);
}

void base_string::_Erase(wstring &src, const wchar_t c)
{
	int nPos = wstring::npos;
	while((nPos = src.find(c)) != wstring::npos)
		src.erase(nPos, 1);
}

std::string base_string::string_To_UTF8(const std::string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, (nwLen + 1)*sizeof(wchar_t));

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete []pwBuf;
	delete []pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

std::string base_string::UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, (nwLen + 1)*sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete []pBuf;
	delete []pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}

std::wstring base_string::_TrimW(const wchar_t *pSrc)
{
	wstring strData = _TrimBackW(pSrc);
	strData = _TrimFrontW(strData.c_str());
	return strData;
}

std::string  base_string::_TrimA(const char *pSrc)
{
	string strData = _TrimBackA(pSrc);
	strData = _TrimFrontA(strData.c_str());
	return strData;
}

std::wstring base_string::_TrimFrontW(const wchar_t *pSrc)
{
	int nLen = ::wcslen(pSrc);
	for(int i=0; i<nLen; ++i)
	{
		if(*pSrc == L' ')
			pSrc++;
		else
			break;
	}

	wstring strTemp(pSrc);
	return strTemp;
}

std::string base_string::_TrimFrontA(const char *pSrc)
{
	int nLen = ::strlen(pSrc);
	for(int i=0; i<nLen; ++i)
	{
		if(*pSrc == ' ')
			pSrc++;
		else
			break;
	}

	string strTemp(pSrc);
	return strTemp;
}

std::wstring base_string::_TrimBackW(const wchar_t *pSrc)
{
	wstring strTemp(pSrc);
	for(int i=strTemp.length()-1; i>=0; --i)
	{
		if(strTemp[i] == L' ')
			strTemp.erase(i, 1);
		else
			break;
	}

	return strTemp;
}

std::string base_string::_TrimBackA(const char *pSrc)
{
	string strTemp(pSrc);
	for(int i=strTemp.length()-1; i>=0; --i)
	{
		if(strTemp[i] == ' ')
			strTemp.erase(i, 1);
		else
			break;
	}

	return strTemp;
}

//add by sherrypan
string base_string::MidStrExA(string strStart, string strEnd, string Buf, bool fgPre/* = true*/)
{//create by sherry
	string result = "";
	size_t nstart ;//= Buf.find(strStart);
	size_t nEnd ;//= Buf.find(strEnd, nstart + strStart.length());
	if (fgPre)
	{
		nstart = Buf.find(strStart);
		nEnd = Buf.find(strEnd, nstart + strStart.length());
	}
	else
	{
		nEnd = Buf.rfind(strEnd);
		nstart = Buf.rfind(strStart, nEnd);
	}

	if (nstart != std::string::npos && nEnd != std::string::npos)
	{
		result = Buf.substr(nstart + strStart.length(), nEnd - nstart - strStart.length());
	}

	return result;
}

string base_string::GetUrlValA(string strUrl, string strName)
{
	size_t nstart = strUrl.find(strName);
	size_t nEnd = strUrl.find("&", nstart + strName.length());
	string strResult = "";
	if (nstart != std::string::npos && nEnd != std::string::npos)
	{
		strResult = strUrl.substr(nstart + strName.length(), nEnd - nstart - strName.length());
	}
	else if (nstart != std::string::npos && nEnd == std::string::npos)
	{
		strResult = strUrl.substr(nstart + strName.length());
	}

	return strResult;
}

wstring base_string::StrToUrlW(wstring wstrSrc)
{
	wstring wstrResult;
	wchar_t szTemp[10] = {0};
	for (int nIndex = 0; nIndex < wstrSrc.length(); nIndex++)
	{
		if ((wstrSrc[nIndex] >= 'a' && wstrSrc[nIndex] <= 'z')
			|| (wstrSrc[nIndex] >= 'A' && wstrSrc[nIndex] <= 'Z')
			||  (wstrSrc[nIndex] >= '0' && wstrSrc[nIndex] <= '9')
			|| wstrSrc[nIndex] == '.' || wstrSrc[nIndex] == '/' || wstrSrc[nIndex] == ':')
		{
			wstrResult += wstrSrc[nIndex];
		}
		else
		{
			swprintf_s(szTemp, sizeof(szTemp)/ sizeof(wchar_t), L"%02x", wstrSrc[nIndex]);
			if (wcslen(szTemp) <= 2)
			{
				wstrResult += L"%";
				wstrResult += szTemp;
			}
			else
			{
				wchar_t szTempHan[3] = {0};
				for (int nTemp = 0; nTemp < wcslen(szTemp); nTemp += 2)
				{
					szTempHan[0] = szTemp[nTemp];
					szTempHan[1] = szTemp[nTemp + 1];
					wstrResult += L"%";
					wstrResult += szTempHan;
				}
			}
			//wstrResult += L"%";
			//wstrResult += szTemp;
		}
	}

	return wstrResult;
}

string base_string::StrToUrlA(string strSrc)
{
	string strResult;
	char szTemp[10] = {0};
	for (unsigned int nIndex = 0; nIndex < strSrc.length(); nIndex++)
	{
		if ((strSrc[nIndex] >= 'a' && strSrc[nIndex] <= 'z')
			|| (strSrc[nIndex] >= 'A' && strSrc[nIndex] <= 'Z')
			||  (strSrc[nIndex] >= '0' && strSrc[nIndex] <= '9')
			|| strSrc[nIndex] == '.' || strSrc[nIndex] == '/' || strSrc[nIndex] == ':')
		{
			strResult += strSrc[nIndex];
		}
		else
		{
			sprintf_s(szTemp, sizeof(szTemp)/ sizeof(char), "%02x", (unsigned char)strSrc[nIndex]);
			strResult += "%";
			strResult += szTemp;
		}
	}

	return strResult;
}

wstring base_string::UrlEncodeAllW(wstring wstrSrc)
{
	wstring wstrResult;
	wchar_t szTemp[10] = {0};
	for (unsigned int nIndex = 0; nIndex < wstrSrc.length(); nIndex++)
	{
		swprintf_s(szTemp, 10, L"%X", wstrSrc[nIndex]);
		wstrResult += L"%";
		wstrResult += szTemp;
	}

	return wstrResult;
}

BOOL base_string::IsDigitalW(wstring wstrSrc)
{
	int nIndex = 0;
	size_t nPos = wstrSrc.find('.');
	if (wstrSrc.empty())
	{
		return FALSE;
	}
	else if (nPos == wstring::npos)
	{
		for (nIndex = 0; nIndex < wstrSrc.length(); nIndex++)
		{
			if (wstrSrc[nIndex] < '0' || wstrSrc[nIndex] > '9')
			{
				return FALSE;
			}
		}
	}
	else if (nPos == wstrSrc.length() - 1)
	{
		return FALSE;
	}
	else if (nPos != wstring::npos)
	{
		for (nIndex = 0; nIndex < nPos; nIndex++)
		{
			if (wstrSrc[nIndex] < '0' || wstrSrc[nIndex] > '9')
			{
				return FALSE;
			}
		}
		for (nIndex = nPos + 1; nIndex < wstrSrc.length(); nIndex++)
		{
			if (wstrSrc[nIndex] < '0' || wstrSrc[nIndex] > '9')
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL base_string::IsDigitalA(string strSrc)
{
	int nIndex = 0;
	size_t nPos = strSrc.find('.');
	if (strSrc.empty())
	{
		return FALSE;
	}
	else if (nPos == string::npos)
	{
		for (nIndex = 0; nIndex < strSrc.length(); nIndex++)
		{
			if (strSrc[nIndex] < '0' || strSrc[nIndex] > '9')
			{
				return FALSE;
			}
		}
	}
	else if (nPos == strSrc.length() - 1)
	{
		return FALSE;
	}
	else if (nPos != string::npos)
	{
		for (nIndex = 0; nIndex < nPos; nIndex++)
		{
			if (strSrc[nIndex] < '0' || strSrc[nIndex] > '9')
			{
				return FALSE;
			}
		}
		for (nIndex = nPos + 1; nIndex < strSrc.length(); nIndex++)
		{
			if (strSrc[nIndex] < '0' || strSrc[nIndex] > '9')
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


wstring base_string::IntToHexStrW(int nSrc)
{
	wchar_t szTemp[9] = {0};
	swprintf_s(szTemp, 9, L"%08x", nSrc);

	return szTemp;
}

string  base_string::IntToHexStrA(int nSrc)
{
	char szTemp[9] = {0};
	sprintf_s(szTemp, 9, "%08x", nSrc);

	return szTemp;
}


wstring base_string::GetTimeStampW()
{
	time_t t;
	time(&t);
	wstringstream wstrTimes;
	wstrTimes << t;

	return wstrTimes.str();
}

//end add by sherrypan