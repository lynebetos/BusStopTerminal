#pragma once

std::string AnsiToUtf8(std::string strAnsi);
std::string WcharToUtf8(std::wstring strUnicode);
std::string Utf8toAnsi( std::string strutf8 );
std::wstring Utf8toWchar( std::string utf8 );
std::wstring AnsiToWchar(std::string strAnsi);
void Split(vector<string>& vec, const string& strSource, const string& strDelim);
void GetMsgId(string& strMsgID);
std::wstring c2w(const char *pc);
std::string w2c(const wchar_t * pw);
void getFiles( string path, vector<string>& files);
BOOL SystemOprate(UINT opt);
BOOL GetSystemVolum(DWORD &dwVolum);
BOOL SetSystemVolum(const DWORD dwVolum);
string ReplaceAll(string& strSrc, const string& strOld, const string& strNew);
void PrintMemoryInfo( DWORD processID, string& text);
void PrintMemoryInfoCSV(DWORD processID, const string& strFilePath);

class CBase
{
public:
	static string time2string(time_t t);
	static time_t string2time(string strtime);
	static string time2string_date(time_t t);
	static string time2string_time(time_t t);
	static time_t string2time_date(string strtime);
	static time_t string2time_time(string strtime);
};


