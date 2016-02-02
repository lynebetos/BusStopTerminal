#pragma once

class CData
{
public:
	CData();
	~CData(void);

	//���ݱ��浥Ԫ���������½���������ڴ滭��
	//��ʱ���ǵ�����̬����Ҳ����������
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
	vector<CString> m_vecStation;	//վ������
	vector<Gdiplus::PointF> m_vecPos;	//��վ�����ƶ�Ӧ��վ��ͼ���λ��
	vector<BOOL> m_vecDLineName;	//վ�������Ƿ񳬹�������
	int m_nCurStationPos;			//��ǰվ��������վ�������е�λ��
	CString m_strStartTime;		//����ʱ��
	CString m_strEndTime;			//�ճ�ʱ��
	CString m_strCard;				//ˢ����Ϣ
	CString m_strticket;				//Ʊ����Ϣ
	CString m_strMonTicket;		//��Ʊ��Ϣ
	CString m_strLineName;		//��ʼ�յ�վ
	CString m_strLineNumber;	//��·��
	BOOL m_bDoubleLine;			//�Ժ󲻻���������ʾ
	BOOL m_bShow;					//�Ƿ�������ʾ
	vector<BusArrivalInfo> m_vecBusArrivalInfo;	//��̬������Ϣ
	CLock m_lock;						//���̻߳���
	CLock m_lockArri;
	float m_fChangeY; //վ�������л��ļ�¼(����)
	int m_nChange;//վ�������л��ļ�¼(����)

};
