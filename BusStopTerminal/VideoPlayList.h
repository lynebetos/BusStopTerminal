#pragma once

class CVideoPlayList
{
public:
	CVideoPlayList(void);
	~CVideoPlayList(void);

public:
	struct _st_Video 
	{
		string strName;
		int nPlaysec;
	};

	struct _st_timeblock
	{
		string strStartTime;
		string strEndTime;
		std::vector<_st_Video> vecVideo;
	};

	struct _st_dayblock
	{
		string strStartDate;
		string strEndDate;
		std::vector<_st_timeblock> vecTimeblock;
	};

public:
	static CVideoPlayList* GetInstance();
	static void ReleaseInstance();

	void AddResource(const string& strName, const string& strUrl)
	{
		m_mapResource.insert(make_pair(strName, strUrl));
	}
	_st_dayblock& AddDay()
	{
		_st_dayblock stDay;
		m_vecDayblock.push_back(stDay);
		return m_vecDayblock.back();
	}

private:
	std::vector<_st_dayblock> m_vecDayblock;
	std::map<std::string, std::string> m_mapResource;

	static CVideoPlayList* m_pInstance; 

};
