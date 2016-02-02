#pragma once
#include <string>
#include <algorithm>
#include <assert.h>
#include <windows.h>
using namespace std;

#define _UTF8_TO_STRING(str)	base_string::UTF8_To_string(str).c_str()
#define _STRING_TO_UTF8(str)	base_string::string_To_UTF8(str).c_str()
#define _BWSTRING(str)			base_string::BWString2wstring(str).c_str()
#define _BASTRING(str)			base_string::BAString2string(str).c_str()

namespace base_string
{
	void _Replace(string &src, const char cOld, const char cNew);
	void _Replace(wstring &src, const wchar_t cOld, const wchar_t cNew);
	void _Erase(string &src, const char c);
	void _Erase(wstring &src, const wchar_t c);
	std::string string_To_UTF8(const std::string & str);
	std::string UTF8_To_string(const std::string & str);
	std::wstring _TrimW(const wchar_t *pSrc);
	std::string  _TrimA(const char *pSrc);
	std::wstring _TrimFrontW(const wchar_t *pSrc);
	std::string  _TrimFrontA(const char *pSrc);
	std::wstring _TrimBackW(const wchar_t *pSrc);
	std::string  _TrimBackA(const char *pSrc);

	//add by sherrypan
	string MidStrExA(string strStart, string strEnd, string Buf, bool fgPre = true);
	string GetUrlValA(string strUrl, string strName);
	wstring StrToUrlW(wstring wstrSrc);
	string StrToUrlA(string strSrc);
	wstring UrlEncodeAllW(wstring wstrSrc);
	BOOL IsDigitalW(wstring wstrSrc);
	BOOL IsDigitalA(string strSrc);
	wstring IntToHexStrW(int nSrc);
	string  IntToHexStrA(int nSrc);
	wstring GetTimeStampW();
	//end add by sherrypan

	template<typename T>
	void _EraseString(T &src, const T& t)
	{
		int nPos = typename T::npos;
		while((nPos = src.find(t)) != typename T::npos)
			src.erase(nPos, t.length());
	}

	//////////////////////////////////////////////////////////////////////////
	inline string _ToLowerA(const char *pSrc)
	{
		string strTemp(pSrc);
		std::transform(strTemp.begin(), strTemp.end(), strTemp.begin(), ::tolower);
		return strTemp;
	}

	inline wstring _ToLowerW(const wchar_t *pSrc)
	{
		wstring strTemp(pSrc);
		std::transform(strTemp.begin(), strTemp.end(), strTemp.begin(), ::towlower);
		return strTemp;
	}

	inline string _ToUpperA(const char *pSrc)
	{
		string strTemp(pSrc);
		std::transform(strTemp.begin(), strTemp.end(), strTemp.begin(), ::toupper);
		return strTemp;
	}

	inline wstring _ToUpperW(const wchar_t *pSrc)
	{
		wstring strTemp(pSrc);
		std::transform(strTemp.begin(), strTemp.end(), strTemp.begin(), ::towupper);
		return strTemp;
	}

	inline string IntToStrA(int nNum)
	{
		char szNum[32] = {0};
		::_itoa_s(nNum, szNum, 32, 10);
		return szNum;
	}

	inline wstring IntToStrW(int nNum)
	{
		wchar_t szNum[32] = {0};
		::_itow_s(nNum, szNum, 32, 10);
		return szNum;
	}

	inline wchar_t* _A2W(const char *pSrc, int nSrcLen, int &nDstLen)
	{
		nDstLen = MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLen, NULL, 0);

		wchar_t *pDst = new wchar_t[nDstLen+1];
		memset(pDst, 0, (nDstLen+1)*2);
		MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLen, pDst, nDstLen);
		nDstLen += 1;
		return pDst;
	}

	inline char *_W2A(const wchar_t *pSrc, int nSrcLen, int &nDstLen)
	{
		nDstLen = WideCharToMultiByte(CP_ACP, 0, pSrc, nSrcLen, NULL, 0, NULL, NULL);

		char *pDst = new char[nDstLen+1];
		memset(pDst, 0, (nDstLen+1));
		WideCharToMultiByte(CP_ACP, 0, pSrc, nSrcLen, pDst, nDstLen, NULL, NULL);
		nDstLen += 1;
		return pDst;
	}

	inline TCHAR* _A2T(const char *pSrc, int nSrcLen, int &nDstLen)
	{
#ifdef _UNICODE
		return _A2W(pSrc, nSrcLen, nDstLen);
#else
		TCHAR *pDst = new TCHAR[nSrcLen+1];
		memset(pDst, 0, (nSrcLen+1));
		memcpy(pDst, pSrc, nSrcLen);
		nDstLen = nSrcLen + 1;
		return pDst;
#endif

		return NULL;
	}

	inline char *_T2A(const TCHAR *pSrc, int nSrcLen, int &nDstLen)
	{
#ifdef _UNICODE
		return _W2A(pSrc, nSrcLen, nDstLen);
#else
		char *pDst = new char[nSrcLen+1];
		memset(pDst, 0, (nSrcLen+1));
		memcpy(pDst, pSrc, nSrcLen);
		nDstLen = nSrcLen+1;
		return pDst;
#endif

		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//转换字符串的基类，T代表源字符串类型，U代表目标字符串类型
	template<typename T, typename U, class Cy>
	class CBaseString
	{
	public:
		typedef int size_type ;

		explicit CBaseString(const T* pString)
			:m_pString(NULL)
			,m_uSize(0)
		{
			reset(pString);
		}

		virtual ~CBaseString()
		{
			release();
		}

		const U*	c_str()		   { return m_pString; }
		size_type	size()   const { return m_uSize; }
		size_type	length() const { return m_uSize; }
		bool		empty()  const { return m_uSize == 0; }

		const U& at (size_type index) const
		{
			assert(index < size());
			return m_pString[ index ];
		}

		CBaseString<T, U, Cy>& assign(const T* pString)
		{
			reset(pString);
		}

		U& operator [] (size_type index) const
		{
			assert(index < size());
			return m_pString[index];
		}

		void operator=(CBaseString<T, U, Cy>& string)
		{
			if((*this == string) == 0 || m_pString == string.c_str())
				return;

			release();
			m_pString = new U[string.size()];
			memcpy(m_pString, string.c_str(), string.size()*sizeof(U));
		}

		void operator=(const T* pString)
		{
			reset(pString);
		}

	protected:
		void reset(const T *pString)
		{
			assert(pString);

			release();

			Cy *pCy = static_cast<Cy*>(this);
			assert(pCy);
			m_pString = pCy->conver(pString);
		}

		void release()
		{
			if(m_pString != NULL)
			{
				delete[] m_pString;
				m_pString = NULL;

				m_uSize = 0;
			}
		}

		int compare(const U* string) 
		{ 
			Cy *pCy = static_cast<Cy*>(this);
			assert(pCy);
			return pCy->_cmp(string); 
		}

		//_cmp需要重载
		int	_cmp(const U* string) { return NULL;}

		//conver需要重载
		U*  conver(const T* string) { return NULL; }

		friend inline bool operator == (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b);
		friend inline bool operator == (const CBaseString<T, U, Cy>& a, const U* b);
		friend inline bool operator <  (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b);

	protected:
		U*			m_pString;
		size_type	m_uSize;
	};	

	template<typename T, typename U, class Cy>
	inline bool operator == (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b)
	{
		return (a.size() == b.size()) && (a.compare(b.c_str()) == 0);
	}

	template<typename T, typename U, class Cy>
	inline bool operator < (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b)
	{
		return a.compare(b.c_str()) < 0;
	}

	template<typename T, typename U, class Cy>
	inline bool operator != (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b) { return !(a == b); }

	template<typename T, typename U, class Cy>
	inline bool operator >  (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b) { return b < a; }

	template<typename T, typename U, class Cy>
	inline bool operator <= (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b) { return !(b < a); }

	template<typename T, typename U, class Cy>
	inline bool operator >= (const CBaseString<T, U, Cy>& a, const CBaseString<T, U, Cy>& b) { return !(a < b); }

	template<typename T, typename U, class Cy>
	inline bool operator == (const CBaseString<T, U, Cy>& a, const U* b) { return a.compare(b); }

	template<typename T, typename U, class Cy>
	inline bool operator == (const U* a, const CBaseString<T, U, Cy>& b) { return b == a; }

	template<typename T, typename U, class Cy>
	inline bool operator != (const CBaseString<T, U, Cy>& a, const U* b) { return !(a == b); }

	template<typename T, typename U, class Cy>
	inline bool operator != (const U* a, const CBaseString<T, U, Cy>& b) { return !(b == a); }

	//////////////////////////////////////////////////////////////////////////
	//BWString用于将char转换为wchar
	class BWString : public CBaseString<char, wchar_t, BWString>
	{
		friend class CBaseString<char, wchar_t, BWString>;
	public:
		explicit BWString(const char *pString)
			:CBaseString<char, wchar_t, BWString>(pString)
		{
		}

	protected:
		int _cmp(const wchar_t* string)
		{
			return wcscmp(m_pString, string);
		}

		wchar_t* conver(const char* string)
		{
			return _A2W(string, strlen(string), m_uSize);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//BAString用于将wchar转换为char
	class BAString : public CBaseString<wchar_t, char, BAString>
	{
		friend class CBaseString<wchar_t, char, BAString>;
	public:
		explicit BAString(const wchar_t *pString)
			:CBaseString<wchar_t, char, BAString>(pString)
		{
		}

	protected:
		int _cmp(const char* string)
		{
			return strcmp(m_pString, string);
		}

		char* conver(const wchar_t* string)
		{
			return _W2A(string, wcslen(string), m_uSize);
		}
	};

	inline wstring BWString2wstring(const char *pString)
	{
		BWString strConver(pString);
		wstring strRet(strConver.c_str());
		return strRet;
	}

	inline string BAString2string(const wchar_t *pString)
	{
		BAString strConver(pString);
		string strRet(strConver.c_str());
		return strRet;
	}
};