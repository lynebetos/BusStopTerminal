#pragma once

class CData
{
public:
	CData();
	~CData(void);

	//数据保存单元，包括更新界面所需的内存画布
	//暂时考虑到将动态数据也保存在里面
public:
	//set
	void SetStationData(__in const vector<CString>& vecStation, 
		__in const CString& strLineNumber, 
		__in const CString& strLineInfo,
		__in const CString& strStartTime,
		__in const CString& strEndTime,
		__in const CString& strCard,
		__in const CString& strTicket,
		__in const CString& strMonTicket);
	void SetMemDc(CDC* pDC,  int nWidth, int nHeight);
	void SetStationPos(__in const vector<Gdiplus::PointF>& vecPos) {m_vecPos = vecPos;};
 	void UpdateBusInfo(__in const tagArrival& refArrival);
	void SetCurStationPos(__in int nCurStationPos) {m_nCurStationPos = nCurStationPos;}
	void SetChangeY(__in float fChangeY) {m_fChangeY = fChangeY;}
	void SetChange(__in int nChanege) {m_nChange = nChanege;}
	void SetDLineName(__in const vector<BOOL>& vecDLine)
	{
		m_lock.Lock();
		m_vecDLineName = vecDLine;
		m_lock.UnLock();
	}

	//get
	CDC& GetMemDC() {return m_memDC;};
	void GetStation(vector<CString>& vecStation)
	{
		m_lock.Lock();
		vecStation = m_vecStation;
		m_lock.UnLock();
	};
	void GetStationPos(vector<Gdiplus::PointF>& vecPos)
	{
		m_lock.Lock();
		vecPos = m_vecPos;
		m_lock.UnLock();
	}
	void GetDLineName(vector<BOOL>& vecDLine)
	{
		m_lock.Lock();
		vecDLine = m_vecDLineName;
		m_lock.UnLock();
	}

	void GetBusInfo( vector<BusArrivalInfo>& vecBusArrivalInfo)
	{
		m_lockArri.Lock();
		vecBusArrivalInfo = m_vecBusArrivalInfo;
		m_lockArri.UnLock();
	}
	void GetLine(__out CString& strLineName, __out CString& strLineNumber)
	{
		strLineName = m_strLineName;
		strLineNumber = m_strLineNumber;
	}
	CString& GetLineName() {return m_strLineName;};
	CString& GetLineNumber() {return m_strLineNumber;};
	BOOL IsDoubleLine() {return m_bDoubleLine;}
	int GetCurStaionPos() {return m_nCurStationPos;}
	float GetChangeY() {return m_fChangeY;}
	int GetChange() {return m_nChange;}
	CString& GetStartTime() {return m_strStartTime;}
	CString& GetEndTime() {return m_strEndTime;}
	CString& GetCartInfo() {return m_strCard;}
	CString& GetTicketInfo() {return m_strticket;}
	CString& GetMonTicketInfo() {return m_strMonTicket;}

private:
	CDC m_memDC;
	CBitmap m_memBitmap;
	vector<CString> m_vecStation;	//站点名称
	vector<Gdiplus::PointF> m_vecPos;	//和站点名称对应的站点图标的位置
	vector<BOOL> m_vecDLineName;	//站点名称是否超过六个字
	int m_nCurStationPos;			//当前站点名称在站点容器中的位置
	CString m_strStartTime;		//发车时间
	CString m_strEndTime;			//收车时间
	CString m_strCard;				//刷卡信息
	CString m_strticket;				//票价信息
	CString m_strMonTicket;		//月票信息
	CString m_strLineName;		//起始终点站
	CString m_strLineNumber;	//线路号
	BOOL m_bDoubleLine;			//以后不会有折行显示
	BOOL m_bShow;					//是否正在显示
	vector<BusArrivalInfo> m_vecBusArrivalInfo;	//动态车辆信息
	CLock m_lock;						//多线程互斥
	CLock m_lockArri;
	float m_fChangeY; //站点文字切换的记录(上下)
	int m_nChange;//站点文字切换的记录(左右)

};
