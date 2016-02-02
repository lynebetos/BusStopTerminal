// BasePanelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "BasePanelDlg.h"
#include "EsayMemDC.h"


// CBasePanelDlg 对话框

IMPLEMENT_DYNAMIC(CBasePanelDlg, CDialog)

CBasePanelDlg::CBasePanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBasePanelDlg::IDD, pParent)
{
	m_nCur = 0;
	m_hEvent = INVALID_HANDLE_VALUE;
	m_bRun = FALSE;
	m_bDLine = FALSE;
	m_hPage = INVALID_HANDLE_VALUE;	
	m_hWord = INVALID_HANDLE_VALUE;
	//m_bChengePage = FALSE;
}

CBasePanelDlg::~CBasePanelDlg()
{
	m_dataLine.clear();

	vector<Gdiplus::Image*>::iterator iterBusIcon;
	for (iterBusIcon = m_vecimBusIcon.begin(); iterBusIcon != m_vecimBusIcon.end(); iterBusIcon++)
	{
		Gdiplus::Image* pImage = *iterBusIcon;
		if(pImage)
			delete pImage;
	}
	m_vecimBusIcon.clear();

	vector<Gdiplus::Image*>::iterator iterBusStatus;
	for (iterBusStatus = m_vecimBusStatus.begin(); iterBusStatus != m_vecimBusStatus.end(); iterBusStatus++)
	{
		Gdiplus::Image* pImage = *iterBusStatus;
		if(pImage)
			delete pImage;
	}
	m_vecimBusStatus.clear();

}

void CBasePanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasePanelDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CBasePanelDlg::OnBnClickedOk)
 	ON_BN_CLICKED(IDCANCEL, &CBasePanelDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CBasePanelDlg 消息处理程序

BOOL CBasePanelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitImage();
	//this->MoveWindow(CRect(0, 0, 1080, 218));

	SetTimer(1, 5000, NULL);
	SetTimer(2 ,15000, NULL);

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	{//创建切换站点名称的线程
		m_bStation = TRUE;
		unsigned int nThreadID = 0;
		m_hWord = (HANDLE)_beginthreadex(NULL, 0, threadStationWord, (LPVOID)this, 0, &nThreadID);
		if(m_hWord == INVALID_HANDLE_VALUE)
		{
			//excption;
		}
	}


	m_bRun = TRUE;
	unsigned int nThreadID = 0;
	m_hPage = (HANDLE)_beginthreadex(NULL, 0, threadChangePage, (LPVOID)this, 0, &nThreadID);
	if(m_hPage == INVALID_HANDLE_VALUE)
	{
		//excption;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBasePanelDlg::InitImage()
{
	char curDir[160] = {0};
	WCHAR wcurDir[160] = {0};

	GetCurrentDirectory(160, curDir);

	//加载Bus动态图标
	CString strImPath = curDir;
	strImPath += _T("\\Res\\bus.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusIcon.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\bus.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusIcon.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\bus.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusIcon.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\busIn.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusIcon.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\busIn.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusIcon.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\busIn.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusIcon.push_back(Gdiplus::Image(wcurDir).Clone());

	//加载Bus的状态图标
	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\A1.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusStatus.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\A2.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusStatus.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\A3.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusStatus.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\A4.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecimBusStatus.push_back(Gdiplus::Image(wcurDir).Clone());

}

void CBasePanelDlg::SetData(__in CData* pData)
{
	LogObject;
	CDC* pDC = GetDC();

	CRect rc;
	GetClientRect(&rc);

	pData->SetMemDc(pDC, rc.Width(), rc.Height()-1);
	m_dataLine.push_back(pData);

	DrawInMem(pData);
}

void CBasePanelDlg::DrawInMem(CData* pData)
{
	CDC& memDC = pData->GetMemDC();

	Gdiplus::PointF pointFBase(0.0f, 0.0f);
	Gdiplus::Graphics gr(memDC);
	//背景图
	DrawBackground(gr, pointFBase, pData);

	//线路号面板
	DrawLinePenel(gr, pointFBase, memDC, pData);

	//站点
	DrawStation(gr, pointFBase, pData);
}

void CBasePanelDlg::DrawBackground(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CData* pData)
{
	CString strStartTime = pData->GetStartTime();
	CString strEndTime = pData->GetEndTime();
	CString strCartInfo = pData->GetCartInfo();
	CString strMonTicketInfo = pData->GetMonTicketInfo();

	Gdiplus::Color cl = Gdiplus::Color::White;
	Gdiplus::SolidBrush brush(cl);

	CRect rc;
	GetClientRect(&rc);

	RectF rcF(pointFBase.X, pointFBase.Y, (REAL)rc.Width(), (REAL)rc.Height());
	gr.FillRectangle(&brush, rcF);

	//线路背景面板------------------------------------------------------------
	WCHAR wcurDir[160] = {0};
	char curDir[160] = {0};
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\linepannel.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imLine(wcurDir);

	REAL nWidth = (REAL)imLine.GetWidth();
	REAL nHight = (REAL)imLine.GetHeight();
	REAL nLineBkWidth = nWidth;

	gr.DrawImage(&imLine, pointFBase);

	//到站背景面板------------------------------------------------------------
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\dispannel.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imDis(wcurDir);

	nWidth = (REAL)imDis.GetWidth();
	nHight = (REAL)imDis.GetHeight();
	REAL nDisWidth = nWidth;

	Gdiplus::PointF pointFLine(rc.right - nWidth, 0);
	gr.DrawImage(&imDis, pointFLine);

	//到站背景文字------------------------------------------------------------
	Gdiplus::Color clBusBkWord = Gdiplus::Color::DimGray;
	Gdiplus::SolidBrush brushBusBkWord(clBusBkWord/*(RGB(102,102,102))*/);
	WCHAR wcBusBkWord[16] = {L"距离本站"};

	Gdiplus::PointF pointFBusBkWord(rc.right - nDisWidth, 50);
	Gdiplus::RectF rectBusBkWord(pointFBusBkWord, Gdiplus::SizeF(nDisWidth, 18));
	Gdiplus::Font fontBusBkWord(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusBkWord;
	stringFormatBusBkWord.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormatBusBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcBusBkWord, wcslen(wcBusBkWord), &fontBusBkWord, rectBusBkWord, &stringFormatBusBkWord, &brushBusBkWord);

	//华丽分割线------------------------------------------------------------
	Gdiplus::Color clSplit(221, 221, 221);
	Gdiplus::Pen pen(clSplit);

	Gdiplus::PointF pointFSplitStart(nLineBkWidth + 10, (REAL)rc.bottom - 40);
	Gdiplus::PointF pointFSplitEnd(rc.right - nDisWidth - 10, (REAL)rc.bottom - 40);
	gr.DrawLine(&pen, pointFSplitStart, pointFSplitEnd);

	//首车,末车,月票,刷卡框位置------------------------------------------------------------
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\info.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imBusRange(wcurDir);

	nWidth = (REAL)imBusRange.GetWidth();
	nHight = (REAL)imBusRange.GetHeight();

	//首车框------------------------------------------------------------
	Gdiplus::PointF pointFRangeStart(nLineBkWidth + 20, rc.bottom - 20 - nHight/2);
	gr.DrawImage(&imBusRange, pointFRangeStart);

	//首车文字------------------------------------------------------------
	Gdiplus::Color clBusStart(Gdiplus::Color::DodgerBlue);//(RGB(33,142,218));
	Gdiplus::SolidBrush brushBusStart(clBusStart/*(RGB(102,102,102))*/);
	WCHAR wcBusStart[16] = {L"首班"};

	Gdiplus::PointF pointFBusStart(nLineBkWidth + 20, rc.bottom - 20 - nHight/2);
	Gdiplus::RectF rectBusStart(pointFBusStart, Gdiplus::SizeF(nWidth, nHight));
	Gdiplus::Font fontBusStart(L"方正兰亭黑简体", 14 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusStart;
	stringFormatBusStart.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormatBusStart.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcBusStart, wcslen(wcBusStart), &fontBusStart, rectBusStart, &stringFormatBusStart, &brushBusStart);

	//首车信息------------------------------------------------------------
	Gdiplus::Color clBusInfo(51,51,51);/*(Gdiplus::Color::DodgerBlue);*/
	Gdiplus::SolidBrush brushBusInfo(clBusInfo/*(RGB(102,102,102))*/);
	//WCHAR wcBusStartInfo[32] = {L"夏季6:10  冬季6:15"};
	wstring wstrBusStart = Utf8toWchar((char*)(LPCTSTR)strStartTime);

	Gdiplus::PointF pointFBusStartInfo(nLineBkWidth + 30 + nWidth, rc.bottom - 20 - nHight/2);
	Gdiplus::RectF rectBusStartInfo(pointFBusStartInfo, Gdiplus::SizeF(240, nHight));
	Gdiplus::Font fontBusInfo(L"方正兰亭黑简体", 14 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBusInfo;
	stringFormatBusInfo.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatBusInfo.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wstrBusStart.c_str(), wstrBusStart.size(), &fontBusInfo, rectBusStartInfo, &stringFormatBusInfo, &brushBusInfo);

	//末车框------------------------------------------------------------
	Gdiplus::PointF pointFRangeEnd(nLineBkWidth + 20 + 240, rc.bottom - 20 - nHight/2);
	gr.DrawImage(&imBusRange, pointFRangeEnd);

	//末车文字------------------------------------------------------------
	WCHAR wcBusEnd[16] = {L"末班"};

	Gdiplus::RectF rectBusEnd(pointFRangeEnd, Gdiplus::SizeF(nWidth, nHight));

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcBusEnd, wcslen(wcBusEnd), &fontBusStart, rectBusEnd, &stringFormatBusStart, &brushBusStart);

	//末车信息------------------------------------------------------------
	//WCHAR wcBusEndInfo[32] = {L"夏季19:10  冬季18:50"};
	wstring wstrBusEnd = Utf8toWchar((char*)(LPCTSTR)strEndTime);

	Gdiplus::PointF pointFBusEndInfo(pointFRangeEnd.X + 10 + nWidth, pointFRangeEnd.Y);
	Gdiplus::RectF rectBusEndInfo(pointFBusEndInfo, Gdiplus::SizeF(240, nHight));

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wstrBusEnd.c_str(), wstrBusEnd.size(), &fontBusInfo, rectBusEndInfo, &stringFormatBusInfo, &brushBusInfo);

	//刷卡框------------------------------------------------------------
	Gdiplus::PointF pointFRangeCard(rc.right - 160 - nDisWidth - nWidth, rc.bottom - 20 - nHight/2);
	gr.DrawImage(&imBusRange, pointFRangeCard);

	//刷卡文字------------------------------------------------------------
	WCHAR wcBusCard[16] = {L"刷卡"};

	Gdiplus::RectF rectBusCard(pointFRangeCard, Gdiplus::SizeF(nWidth, nHight));

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcBusCard, wcslen(wcBusCard), &fontBusStart, rectBusCard, &stringFormatBusStart, &brushBusStart);

	//刷卡信息------------------------------------------------------------
	//WCHAR wcBusCardInfo[32] = {L"无"};
	wstring wstrBusCardInfo = Utf8toWchar((char*)(LPCTSTR)strCartInfo);
	//wstring wstrBusCardInfo = L"--";

	Gdiplus::PointF pointFBusCardInfo(pointFRangeCard.X + 10 + nWidth, pointFRangeCard.Y);
	Gdiplus::RectF rectBusCardInfo(pointFBusCardInfo, Gdiplus::SizeF(34, nHight));

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wstrBusCardInfo.c_str(), wstrBusCardInfo.size(), &fontBusInfo, rectBusCardInfo, &stringFormatBusInfo, &brushBusInfo);

	//月票框------------------------------------------------------------
	Gdiplus::PointF pointFRangeTicket(pointFRangeCard.X - 60 - nWidth, rc.bottom - 20 - nHight/2);
	gr.DrawImage(&imBusRange, pointFRangeTicket);

	//月票文字------------------------------------------------------------
	WCHAR wcBusTicket[16] = {L"月票"};

	Gdiplus::RectF rectBusTicket(pointFRangeTicket, Gdiplus::SizeF(nWidth, nHight));

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcBusTicket, wcslen(wcBusTicket), &fontBusStart, rectBusTicket, &stringFormatBusStart, &brushBusStart);
	//月票信息------------------------------------------------------------
	//WCHAR wcBusTicketInfo[32] = {L"无"};
	wstring wstrBusTicketInfo = Utf8toWchar((char*)(LPCTSTR)strMonTicketInfo);
	//wstring wstrBusTicketInfo = L"--";

	Gdiplus::PointF pointFBusTicketInfo(pointFRangeTicket.X + 10 + nWidth, pointFRangeTicket.Y);
	Gdiplus::RectF rectBusTicketInfo(pointFBusTicketInfo, Gdiplus::SizeF(34, nHight));

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wstrBusTicketInfo.c_str(), wstrBusTicketInfo.size(), &fontBusInfo, rectBusTicketInfo, &stringFormatBusInfo, &brushBusInfo);


	//----------------------
	m_pointFRangeStart = pointFRangeStart;
	m_pointFRangeEnd = pointFRangeEnd;
	m_pointFRangeCard = pointFRangeCard;
	m_pointFRangeTicket = pointFRangeTicket;
	m_nLineBkWidth = nLineBkWidth;
	m_nDisBkWidth = nDisWidth;
}

void CBasePanelDlg::DrawLinePenel(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CDC& memDC, CData* pData)
{
	CString strTicketInfo = pData->GetTicketInfo();
	//线路号文字
	CRect rc;
	GetClientRect(&rc);
	WCHAR wcFront[] = L"运";
	WCHAR wcLineNumber[32] = {0};
	WCHAR wcAfter[] = L"路";

	WCHAR wcurDir[160] = {0};
	char curDir[160] = {0};
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\linepannel.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imLine(wcurDir);

	REAL nWidth = (REAL)imLine.GetWidth();
	REAL nHight = (REAL)imLine.GetHeight();
	REAL nWidthLine = nWidth;

	CString& strLineNumber = pData->GetLineNumber();
	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strLineNumber, strLineNumber.GetLength(), wcLineNumber, sizeof(wcLineNumber));

	int nWordWidth = 0;
	if(wcslen(wcLineNumber) >= 3)
	{
		nWordWidth = 42;
	}
	else
	{
		nWordWidth = 56;
	}

	Gdiplus::SolidBrush brush(Gdiplus::Color::White);

	BOOL bHalf = TRUE;
	if(bHalf)
	{
		//车号
		Gdiplus::PointF pointFLineNumber(0, nHight/2-nWordWidth/2);
		Gdiplus::RectF rect(pointFLineNumber, Gdiplus::SizeF(100, nWordWidth));
// 		Gdiplus::Font fontLine(memDC.m_hDC, fontBase);
		Gdiplus::Font fontLine(L"Helvetica", nWordWidth , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatLine;
		stringFormatLine.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatLine.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wcLineNumber, wcslen(wcLineNumber), &fontLine, rect, &stringFormatLine, &brush);

		//后缀
		Gdiplus::PointF pointFAfter(90, nHight/2-nWordWidth/2 + 8);
		Gdiplus::RectF rectAfter(pointFAfter, Gdiplus::SizeF(40, nWordWidth));
		Gdiplus::Font fontAfter(L"方正兰亭黑简体", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		stringFormatLine.SetAlignment(Gdiplus::StringAlignmentNear);
		stringFormatLine.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wcAfter, wcslen(wcAfter), &fontAfter, rectAfter, &stringFormatLine, &brush);

	}
	else
	{
		//车号
		/*
		REAL nOffset = 30;
		REAL nLineWidth = nWidth / 3;
		Gdiplus::PointF pointFLineNumber = pointFLine;
		pointFLineNumber.X += nLineWidth - nOffset;
		pointFLineNumber.Y += 10;
		Gdiplus::RectF rect(pointFLineNumber, Gdiplus::SizeF(nLineWidth + nOffset * 2, nHight));
		Gdiplus::Font fontLine(L"微软简粗黑", 120 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatLine;
		stringFormatLine.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatLine.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
		gr.DrawString(wcLineNumber, wcslen(wcLineNumber), &fontLine, rect, &stringFormatLine, &brush);

		//前缀
		Gdiplus::PointF pointFFront = pointFLine;
		Gdiplus::RectF rectFront(pointFFront, Gdiplus::SizeF(nLineWidth - nOffset, nHight));
		Gdiplus::Font fontFront(L"微软简粗黑", 40 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		stringFormatLine.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatLine.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.DrawString(wcFront, wcslen(wcFront), &fontFront, rectFront, &stringFormatLine, &brush);

		//后缀
		Gdiplus::PointF pointFAfter = pointFLine;
		pointFAfter.X += (nLineWidth  + nOffset) * 2 - nOffset;
		Gdiplus::RectF rectAfter(pointFAfter, Gdiplus::SizeF(nLineWidth - nOffset, nHight));
		Gdiplus::Font fontAfter(L"微软简粗黑", 40 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		stringFormatLine.SetAlignment(Gdiplus::StringAlignmentNear);
		stringFormatLine.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.DrawString(wcAfter, wcslen(wcAfter), &fontAfter, rectAfter, &stringFormatLine, &brush);
		*/
	}


	//票价
	std::wstring wstrTicket;
	WCHAR wcEnd[32] = {0};
	vector<CString> vecStation;

	wstrTicket += Utf8toWchar((char*)(LPCTSTR)strTicketInfo);
	unsigned int index = wstrTicket.find(L';');
	if(index != wstring::npos)
		wstrTicket.replace(index, 1, L"\n");


	Gdiplus::Font fontStartEnd(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatStartEnd;
	stringFormatStartEnd.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormatStartEnd.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	Gdiplus::PointF pointFEnd(0, rc.bottom - 80);
	Gdiplus::RectF rectEnd(pointFEnd, Gdiplus::SizeF(nWidthLine, 80));
	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wstrTicket.c_str(), wstrTicket.size(), &fontStartEnd, rectEnd, &stringFormatStartEnd, &brush);

}

void CBasePanelDlg::DrawStation(Gdiplus::Graphics& gr, Gdiplus::PointF& pointFBase, CData* pData)
{
	LogObject;
	vector<CString> vecStation;
	pData->GetStation(vecStation);
	int nStopCount = (int)vecStation.size();
	WCHAR wcurDir[160] = {0};
	char curDir[160] = {0};

	//公车背景面板------------------------------------------------------------
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\buslinebk.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imBusBk(wcurDir);

	REAL nWidth = (REAL)imBusBk.GetWidth();
	REAL nHight = (REAL)imBusBk.GetHeight();
	REAL nBusBKHight = nHight;

	Gdiplus::PointF pointFBusBK(m_nLineBkWidth + 10, 10);
	gr.DrawImage(&imBusBk, pointFBusBK);
	m_pointFBusBK = pointFBusBK;

	//公车站点线------------------------------------------------------------
	Gdiplus::Color clStopLine(4, 171, 0);
	Gdiplus::Pen penStop(clStopLine);

	Gdiplus::PointF pointFSLStart(m_nLineBkWidth + 20, nBusBKHight + 10);
	Gdiplus::PointF pointFSLEnd(m_nLineBkWidth  + nWidth, nBusBKHight + 10);
	gr.DrawLine(&penStop, pointFSLStart, pointFSLEnd);

	//站点图标
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\stop.png");
	vector<Gdiplus::PointF> vecIconPos;

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imStop(wcurDir);

	nWidth = (REAL)imStop.GetWidth();
	nHight = (REAL)imStop.GetHeight();

	//起始站点
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\startstop.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imStartStop(wcurDir);

	REAL nWidthStart = (REAL)imStartStop.GetWidth();
	REAL nHightStart = (REAL)imStartStop.GetHeight();

	//终点站点
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\endstop.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imEndStop(wcurDir);

	REAL nWidthEnd = (REAL)imEndStop.GetWidth();
	REAL nHightEnd = (REAL)imEndStop.GetHeight();

	//当前站点
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\curstop.png");

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imCurStop(wcurDir);

	REAL nWidthCur = (REAL)imCurStop.GetWidth();
	REAL nHightCur = (REAL)imCurStop.GetHeight();

	REAL nStopDis = (pointFSLEnd.X - pointFSLStart.X) / (nStopCount - 1); 

	for (int ii = 0; ii < nStopCount; ii++)
	{
		Gdiplus::PointF pointFStop;
		if(ii == pData->GetCurStaionPos())
		{
			pointFStop.X = pointFSLStart.X - nWidthCur/2 + ii * nStopDis;
			pointFStop.Y= pointFSLStart.Y - nHightCur/2;
			gr.DrawImage(&imCurStop, pointFStop);

			Gdiplus::PointF pointFCurStopLineStart = pointFSLStart;
			Gdiplus::PointF pointFCurStopLineEnd = pointFSLStart;
			pointFCurStopLineStart.Y -= nHightCur / 2;
			pointFCurStopLineEnd.Y -= nBusBKHight;
			pointFCurStopLineStart.X = pointFCurStopLineEnd.X = pointFSLStart.X + ii * nStopDis;
			Gdiplus::Pen penCurStopLine(clStopLine, 2);
			gr.DrawLine(&penCurStopLine, pointFCurStopLineStart, pointFCurStopLineEnd);

		}
		else if(ii == 0 )
		{
			pointFStop.X = pointFSLStart.X - nWidthStart/2 + ii * nStopDis;
			pointFStop.Y= pointFSLStart.Y - nHightStart/2;
			gr.DrawImage(&imStartStop, pointFStop);
		}
		else if(ii == nStopCount - 1)
		{
			pointFStop.X = pointFSLStart.X - nWidthEnd/2 + ii * nStopDis;
			pointFStop.Y= pointFSLStart.Y - nHightEnd/2;
			gr.DrawImage(&imEndStop, pointFStop);
		}
		else
		{
			pointFStop.X = pointFSLStart.X - nWidth/2 + ii * nStopDis;
			pointFStop.Y= pointFSLStart.Y - nHight/2;
			gr.DrawImage(&imStop, pointFStop);
		}

		pointFStop.X = pointFSLStart.X + ii * nStopDis;
		pointFStop.Y= pointFSLStart.Y;

		vecIconPos.push_back(pointFStop);
	}
	pData->SetStationPos(vecIconPos);

	//站点文字
	Gdiplus::SolidBrush brush(Gdiplus::Color(51,51,51));
	vector<Gdiplus::PointF> vecStatonPos;
	pData->GetStationPos(vecStatonPos);
	int nStationCount = (int)vecStation.size();
	int nCurStationPos = pData->GetCurStaionPos();
	vector<BOOL> vecStopDLine;
	CDC* pDC = GetDC();
	Gdiplus::REAL nChangeY = pData->GetChangeY();
	int nChange = pData->GetChange();

	Gdiplus::Font fontStop(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

	Gdiplus::StringFormat stringFormat = Gdiplus::StringFormatFlagsDirectionVertical;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentCenter);

	for (int ii = 0; ii < (int)vecStation.size(); ii++)
	{
		Gdiplus::PointF pointFStopWord = vecIconPos[ii];
		pointFStopWord.Y += 10;
		pointFStopWord.X = pointFStopWord.X - 10;
		RectF rectFStopWord(pointFStopWord,Gdiplus::SizeF(18, 18*7));

		CString strStation = vecStation[ii];
		std::wstring wstrStation = Utf8toWchar((char*)(LPCTSTR)strStation);
		BOOL bChange;
		if(6 < wstrStation.size()) //用来记录切换的站点
		{
			bChange = TRUE;
		}
		else
		{
			bChange = FALSE;
		}


		if(bChange == FALSE)
		{
			if(ii == nCurStationPos)
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(4, 171, 0));
				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstrStation.c_str(), wstrStation.size(), &fontStop, rectFStopWord, &stringFormat, &brushStopWord);
			}
			else
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(51, 51, 51));
				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstrStation.c_str(), wstrStation.size(), &fontStop, rectFStopWord, &stringFormat, &brushStopWord);
			}
		}
		else
		{
			/*
			Gdiplus::PointF pointFStopWord = vecStatonPos[ii];
			pointFStopWord.Y += nHight + 6;
			pointFStopWord.X = pointFStopWord.X + nWidth/2 - 9;
			RectF rectFStopWord(pointFStopWord,Gdiplus::SizeF(18, 18*7-15));
			RectF rectCopy = rectFStopWord;
			CString strStation = vecStation[ii];

			CDC tempDC;
			tempDC.CreateCompatibleDC(pDC);
			CBitmap bitmap;
			bitmap.CreateCompatibleBitmap(pDC, (int)rectCopy.Width, (int)rectCopy.Height);
			tempDC.SelectObject(&bitmap);
			Gdiplus::Graphics gr(tempDC);

			std::wstring wstrStation = Utf8toWchar((char*)(LPCTSTR)strStation);
			Gdiplus::Font fontStop(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormat = Gdiplus::StringFormatFlagsDirectionVertical;
			stringFormat.SetAlignment(StringAlignmentNear);
			stringFormat.SetLineAlignment(StringAlignmentCenter);

			RectF rectFStringSize;
			gr.MeasureString(wstrStation.c_str(), wstrStation.size(), &fontStop, pointFStopWord, &stringFormat, &rectFStringSize);

			RectF rectFtemp(0, 0, 18, 18*7-15);
			rectFtemp.Height = rectFStringSize.Height;

			rectFtemp.Y = -nChangeY;

			REAL dertY = rectFStringSize.Height - rectCopy.Height;

			if(dertY <= nChangeY)
				rectFtemp.Y = -dertY;

			//刷新背景
			RectF rectFBK(0, 0, 18, 18*7);
			Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(255, 255, 255));
			gr.FillRectangle(&brushStopWord, rectFBK);

			if(ii == nCurStationPos)
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(4, 171, 0));
				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstrStation.c_str(), wstrStation.size(), &fontStop, rectFtemp, &stringFormat, &brushStopWord);
			}
			else
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(51, 51, 51));
				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstrStation.c_str(), wstrStation.size(), &fontStop, rectFtemp, &stringFormat, &brushStopWord);
			}

			pData->GetMemDC().BitBlt((int)rectCopy.X, (int)rectCopy.Y, (int)rectCopy.Width, (int)rectCopy.Height, &tempDC, 0, 0, SRCCOPY);
			tempDC.DeleteDC();
			bitmap.DeleteObject();
			*/

			Gdiplus::PointF pointFStopWord = vecStatonPos[ii];
			pointFStopWord.Y += 10;
			pointFStopWord.X = pointFStopWord.X - 10;
			RectF rectFStopWord(pointFStopWord,Gdiplus::SizeF(18, 18*7-15));

			CString strStation = vecStation[ii];
			std::wstring wstrStation = Utf8toWchar((char*)(LPCTSTR)strStation);

			int nPos = wstrStation.find(L'|');

			std::wstring wstrLeft = wstrStation.substr(0, nPos );
			std::wstring wstrRight = wstrStation.substr(nPos+1, wstrStation.size() - nPos);

			std::wstring wstrShow;
			if(nChange)
			{
				wstrShow = wstrRight;
			}
			else
			{
				wstrShow = wstrLeft;
			}

			CEsayMemDC memDC(*pDC, CRect((int)rectFStopWord.GetLeft(), (int)rectFStopWord.GetTop(), (int)rectFStopWord.GetRight(), (int)rectFStopWord.GetBottom()));
			Gdiplus::Graphics grs(memDC.GetDC());

			Gdiplus::Font fontStop(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormat = Gdiplus::StringFormatFlagsDirectionVertical;
			stringFormat.SetAlignment(StringAlignmentNear);
			stringFormat.SetLineAlignment(StringAlignmentCenter);

			//刷新背景
			RectF rectFBK(0, 0, 18, 18*7);
			Gdiplus::SolidBrush brushStopBK(Gdiplus::Color(255, 255, 255));
			grs.FillRectangle(&brushStopBK, rectFBK);

			if(ii == nCurStationPos)
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(4, 171, 0));
				grs.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				grs.DrawString(wstrShow.c_str(), wstrShow.size(), &fontStop, rectFBK, &stringFormat, &brushStopWord);
			}
			else
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(51, 51, 51));
				grs.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				grs.DrawString(wstrShow.c_str(), wstrShow.size(), &fontStop, rectFBK, &stringFormat, &brushStopWord);
			}
			memDC.BltMem(pData->GetMemDC());

		}

		vecStopDLine.push_back(bChange);

	}
	ReleaseDC(pDC);
	pData->SetDLineName(vecStopDLine);

	return;
}

int CBasePanelDlg::DrawChangeStationWord(CData* pData, int nChange)
{
	LogObject;
	vector<BOOL> vecStopChange;
	pData->GetDLineName(vecStopChange);
	vector<CString> vecStation;
	pData->GetStation(vecStation);
	vector<Gdiplus::PointF> vecStatonPos;
	pData->GetStationPos(vecStatonPos);
	int nCurStationPos = pData->GetCurStaionPos();

	CDC* pDC = GetDC();
	CRect rc;
	GetClientRect(&rc);

	//站点图标用来确定位置
	CDC& memDC = pData->GetMemDC();
	WCHAR wcurDir[160] = {0};
	char curDir[160] = {0};
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\stop.png");
	vector<Gdiplus::PointF> vecIconPos;

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imStop(wcurDir);

	REAL nWidth = (REAL)imStop.GetWidth();
	REAL nHight = (REAL)imStop.GetHeight();

	int nMove = 18;
	int nRet = 0;

	while(nMove> 0)
	{

		for (int ii = 0; ii < (int)vecStopChange.size()&&ii<(int)vecStation.size(); ii++)
		{
			BOOL bDraw = vecStopChange[ii];
			if(bDraw)
			{
				//设定移动的区域
				Gdiplus::PointF pointFStopWord = vecStatonPos[ii];
				pointFStopWord.Y += 10;
				pointFStopWord.X = pointFStopWord.X - 10;
				RectF rectFStopWord(pointFStopWord,Gdiplus::SizeF(18, 18*7-15));

				CEsayMemDC memDC(*pDC, CRect((int)rectFStopWord.GetLeft(), (int)rectFStopWord.GetTop(), (int)rectFStopWord.GetRight(), (int)rectFStopWord.GetBottom()));
				Gdiplus::Graphics gr(memDC.GetDC());

				CString strStation = vecStation[ii];
				std::wstring wstrStation = Utf8toWchar((char*)(LPCTSTR)strStation);

				int nPos = wstrStation.find(L'|');

				std::wstring wstrLeft = wstrStation.substr(0, nPos);
				std::wstring wstrRight = wstrStation.substr(nPos+1, wstrStation.size() - nPos);


				RectF rectFBK(0, 0, 18, 18*7);
				RectF rectFWordLeft = rectFBK;
				RectF rectFWordRight = rectFBK;

				if(nChange)
				{
					rectFWordRight.X = rectFWordLeft.X + rectFWordLeft.Width - 1;
				}
				else
				{
					rectFWordLeft.X = rectFWordRight.X - rectFWordRight.Width;
				}

				Gdiplus::Font fontStop(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormat = Gdiplus::StringFormatFlagsDirectionVertical;
				stringFormat.SetAlignment(StringAlignmentNear);
				stringFormat.SetLineAlignment(StringAlignmentCenter);

				Gdiplus::SolidBrush brushStopBK(Gdiplus::Color(255, 255, 255));
				gr.FillRectangle(&brushStopBK, rectFBK);

				if(nChange)	//如果nChange是1，则从右向左移
				{
					rectFWordLeft.X -= 19 -nMove ;
					rectFWordRight.X -= 19- nMove ;

				}
				else //如果nChange是0，则从左向右移
				{
					rectFWordLeft.X+= 19-nMove;
					rectFWordRight.X+= 19-nMove;
				}

				if(ii == nCurStationPos)
				{
					Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(4, 171, 0));
					gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
					gr.DrawString(wstrLeft.c_str(), wstrLeft.size(), &fontStop, rectFWordLeft, &stringFormat, &brushStopWord);

					gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
					gr.DrawString(wstrRight.c_str(), wstrRight.size(), &fontStop, rectFWordRight, &stringFormat, &brushStopWord);
				}
				else
				{
					Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(51, 51, 51));
					gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
					gr.DrawString(wstrLeft.c_str(), wstrLeft.size(), &fontStop, rectFWordLeft, &stringFormat, &brushStopWord);

					gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
					gr.DrawString(wstrRight.c_str(), wstrRight.size(), &fontStop, rectFWordRight, &stringFormat, &brushStopWord);
				}
				memDC.BltMem(*pDC);
			}
		}

		nMove--;
		Sleep(20);

	}

	ReleaseDC(pDC);

	return nRet;
}

int CBasePanelDlg::DrawChangeStationWord(CData* pData, BOOL bUp, Gdiplus::REAL& nChangeY)
{
	LogObject;
	CDC& memDC = pData->GetMemDC();
	WCHAR wcurDir[160] = {0};
	char curDir[160] = {0};

	vector<CString> vecStation;
	pData->GetStation(vecStation);
	vector<Gdiplus::PointF> vecStatonPos;
	pData->GetStationPos(vecStatonPos);
	vector<BOOL> vecStopChange;
	pData->GetDLineName(vecStopChange);
	int nCurStationPos = pData->GetCurStaionPos();
	CString strLineID = pData->GetLineNumber();


	//站点图标用来确定位置
	memset(wcurDir, 0, sizeof(wcurDir));
	memset(curDir, 0, sizeof(curDir));
	GetCurrentDirectory(160, curDir);
	strcat_s(curDir, "\\Res\\stop.png");
	vector<Gdiplus::PointF> vecIconPos;

	MultiByteToWideChar(CP_ACP, 0, curDir, strlen(curDir), wcurDir, 160);
	Image imStop(wcurDir);

	REAL nWidth = (REAL)imStop.GetWidth();
	REAL nHight = (REAL)imStop.GetHeight();

	if(bUp)
		nChangeY ++;
	else
		nChangeY--;

	if(vecStation.size() != vecStopChange.size())
		return -1;
	
	CDC* pDC = GetDC();

	int nRet = 0;
	for (int ii = 0; ii < (int)vecStation.size(); ii++)
	{
		BOOL bDraw = vecStopChange[ii];
		if(bDraw)
		{
			Gdiplus::PointF pointFStopWord = vecStatonPos[ii];
			pointFStopWord.Y += nHight + 6;
			pointFStopWord.X = pointFStopWord.X + nWidth/2 - 9;
			RectF rectFStopWord(pointFStopWord,Gdiplus::SizeF(18, 18*7-15));
			RectF rectCopy = rectFStopWord;
			CString strStation = vecStation[ii];

			CDC tempDC;
			tempDC.CreateCompatibleDC(pDC);
			CBitmap bitmap;
			bitmap.CreateCompatibleBitmap(pDC, (int)rectCopy.Width, (int)rectCopy.Height);
			tempDC.SelectObject(&bitmap);
			Gdiplus::Graphics gr(tempDC);

			std::wstring wstrStation = Utf8toWchar((char*)(LPCTSTR)strStation);
			Gdiplus::Font fontStop(L"方正兰亭黑简体", 18 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormat = Gdiplus::StringFormatFlagsDirectionVertical;
			stringFormat.SetAlignment(StringAlignmentNear);
			stringFormat.SetLineAlignment(StringAlignmentCenter);

			RectF rectFStringSize;
			gr.MeasureString(wstrStation.c_str(), wstrStation.size(), &fontStop, pointFStopWord, &stringFormat, &rectFStringSize);
			
			RectF rectFtemp(0, 0, 18, 18*7-15);
			rectFtemp.Height = rectFStringSize.Height;
			rectFtemp.Y = -nChangeY;

			REAL dertY = rectFStringSize.Height - rectCopy.Height;
			if((bUp && dertY < nChangeY) || (!bUp && nChangeY < 0.0 ) || (!bUp && nChangeY > dertY))
			{
				tempDC.DeleteDC();
				bitmap.DeleteObject();
				continue;
			}
			else
			{
				nRet = 1;
			}


			//刷新背景
			RectF rectFBK(0, 0, 18, 18*7);
			Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(255, 255, 255));
			gr.FillRectangle(&brushStopWord, rectFBK);
			rectFStopWord.Y+=10;
			PointF distPoint = pointFStopWord;
			distPoint.Y+=10;
			
			if(ii == nCurStationPos)
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(4, 171, 0));
				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstrStation.c_str(), wstrStation.size(), &fontStop, rectFtemp, &stringFormat, &brushStopWord);
			}
			else
			{
				Gdiplus::SolidBrush brushStopWord(Gdiplus::Color(51, 51, 51));
				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstrStation.c_str(), wstrStation.size(), &fontStop, rectFtemp, &stringFormat, &brushStopWord);
			}
			
			pDC->BitBlt((int)rectCopy.X, (int)rectCopy.Y, (int)rectCopy.Width, (int)rectCopy.Height, &tempDC, 0, 0, SRCCOPY);
			tempDC.DeleteDC();
			bitmap.DeleteObject();

		}
	}

	ReleaseDC(pDC);

	return nRet;
}

void CBasePanelDlg::OnPaint()
{
	LogObject;
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CDC* pDC = GetDC();

	if((m_nCur) >= (int)m_dataLine.size())
		m_nCur = 0;

	CRect rc;
	GetClientRect(&rc);

	CData* pData = m_dataLine[m_nCur];
	CDC& memDC = pData->GetMemDC();

	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	//底边分割线------------------------------------------------------------
	Gdiplus::Graphics gr(*pDC);
	Gdiplus::Color clSplitDi(215, 215, 215);
	Gdiplus::Pen penDi(clSplitDi);

	Gdiplus::PointF pointFSplitStartDi((REAL)rc.left, (REAL)rc.bottom-1);
	Gdiplus::PointF pointFSplitEndDi((REAL)rc.right, (REAL)rc.bottom - 1);
	gr.DrawLine(&penDi, pointFSplitStartDi, pointFSplitEndDi);

	ReleaseDC(pDC);
	CDialog::OnPaint();
}

void CBasePanelDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
// 	OnOK();
}

void CBasePanelDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetParent()->SendMessage(WM_CLOSE, NULL, NULL);
// 	OnCancel();
}

vector<CString> CBasePanelDlg::OutDoubleLine(const CString& strLine)
{
	vector<CString> vec;
	CString strTmp(strLine);
	int nIndex = -1;
	while(0 <(nIndex = strTmp.Find("\r\n")))
	{
		CString str(strTmp.Left(nIndex));
		strTmp.Delete(0, nIndex + 2);
		vec.push_back(str);
	}

	vec.push_back(strTmp);
	return vec;
}

unsigned int _stdcall CBasePanelDlg::threadStationWord(LPVOID lpvoid)
{
	CBasePanelDlg* pDlg = (CBasePanelDlg*)lpvoid;
	if(pDlg)
	{
		pDlg->HandlingStationWord();
	}

	return 0;
}

void CBasePanelDlg::HandlingStationWord()
{
	LogObject;
	while(m_bStation)
	{
		/*if(m_bChengePage)
		{
			Sleep(1000);
			continue;
		}*/

		WaitForSingleObject(g_hWord, INFINITE);
		if(!m_bStation)
			return;


		if(0 >= m_dataLine.size())
		{
			Sleep(1000);
			continue;;
		}

		CData* pData = m_dataLine[m_nCur];
		if(pData == NULL)
		{
			Sleep(1000);
			continue;;
		}

		m_lock.Lock();

		Gdiplus::REAL nChangeY = pData->GetChangeY();
		int nChange = pData->GetChange();

		BOOL bUp = FALSE;
		if(nChangeY <= 0)
		{
			bUp = TRUE;
		}

		if(nChange)
			nChange = 0;
		else
			nChange = 1;

		DrawChangeStationWord(pData, nChange);

// 		while(!g_bWord)
// 		{
// 			Sleep(100);
// 		}

		/*
		while(int nRet = DrawChangeStationWord(pData, bUp, nChangeY))
		{
			if(-1 == nRet)
				break;

			Sleep(10);
		}
		*/

		pData->SetChange(nChange);
		pData->SetChangeY(nChangeY);
		m_lock.UnLock();


		//Sleep(2000);
	}

}

void CBasePanelDlg::UpdateArrival()
{
	if(0 >= m_dataLine.size())
	{
		return;
	}

	CData* pData = m_dataLine[m_nCur];
	if(pData == NULL)
	{
		return;
	}

	m_lock.Lock();
	DrawDynimacData(pData);
	m_lock.UnLock();

}

unsigned int _stdcall CBasePanelDlg::threadChangePage(LPVOID lpvoid)
{
	CBasePanelDlg* pDlg = (CBasePanelDlg*)lpvoid;
	if(pDlg)
	{
		pDlg->HandlingChangePage();
	}

	return 0;
}

void CBasePanelDlg::MovePix(int nMovePix, CData* pDataNow, CData* pDataNext)
{
	if(2 <= (int)m_dataLine.size() )
	{
		CRect rc;
		GetClientRect(&rc);

		int nMove = rc.Height() - nMovePix;
		int nHeight = rc.Height() - 1;

		CDC& memDC = pDataNow->GetMemDC();
		CDC& memDCNext = pDataNext->GetMemDC();

		CDC* pDC = this->GetDC();

		pDC->BitBlt(0, nMove - nHeight, 1080, nHeight, &memDC, 0, 0, SRCCOPY);
		pDC->BitBlt(0, nMove, 1080, nHeight - nMove, &memDCNext, 0, 0, SRCCOPY);

		ReleaseDC(pDC);

	}

}

CData* CBasePanelDlg::GetDataNow()
{
	return m_dataLine[m_nCur];
}

CData* CBasePanelDlg::GetDataNext()
{
	if(2 <= (int)m_dataLine.size() )
	{
		if((m_nCur+1) >= (int)m_dataLine.size() )
			return m_dataLine[0];
		return m_dataLine[m_nCur + 1];
	}

	return NULL;
}

void CBasePanelDlg::HandlingChangePage()
{
	LogObject;
	while(0/*m_bRun*/)
	{
		//ResetEvent(m_hEvent);
		//m_bChengePage = TRUE;
		WaitForSingleObject(g_hPage, INFINITE);
		if(!m_bRun)
			return;

		m_lock.Lock();

		if(2 <= (int)m_dataLine.size() )
		{
			if((m_nCur) >= (int)m_dataLine.size())
				m_nCur = 0;
			CRect rc;
			GetClientRect(&rc);

			CDC* pDC = this->GetDC();
			int nLeftOffset = rc.Height() - 1;
			int nHeight = rc.Height() - 1;
			CData* pData = m_dataLine[m_nCur];
			m_nCur++;
			if((m_nCur) >= (int)m_dataLine.size())
				m_nCur = 0;
			CData* pDataNew = m_dataLine[m_nCur];
			CDC& memDC = pData->GetMemDC();
			CDC& memDC2 = pDataNew->GetMemDC();

			float w , i;
			w = i = 0;
			while(0 <= nLeftOffset)
			{
				pDC->BitBlt(0, nLeftOffset - nHeight, 1080, nHeight, &memDC, 0, 0, SRCCOPY);
				pDC->BitBlt(0, nLeftOffset, 1080, nHeight - nLeftOffset, &memDC2, 0, 0, SRCCOPY);

				nLeftOffset -= 1;
				//Sleep(5);

				//i += static_cast<float>(180.0/584.0);
//				w = (float)sin(static_cast<int>(i ) / 180 * PI);
//				s_util.MicroSecondSleep((int)w*1024);
 				//Sleep(1);

			}
			ReleaseDC(pDC);
//			DrawInMem(pDataNew);
			UpdateArrival();

		}
		m_lock.UnLock();
		//m_bChengePage = FALSE;

		//SetEvent(m_hEvent);
		//Sleep(10000);
	}

}

void CBasePanelDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(1 == nIDEvent)
	{
		UpdateArrival();
		KillTimer(1);
	}
	else  if (2 == nIDEvent)
	{

	}
	CDialog::OnTimer(nIDEvent);
}

void CBasePanelDlg::DrawDynimacData(CData* pData)
{
	LogObject;
	DrawInMem(pData);

	DrawBusIcon(pData);

	DrawBusStatus(pData);

	CDC& memDC = pData->GetMemDC();

	CDC* pDC = GetDC();
	CRect rc;
	GetClientRect(&rc);

	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	ReleaseDC(pDC);
}

void CBasePanelDlg::DrawBusIcon(CData* pData)
{
	CDC& memDC = pData->GetMemDC();
	Gdiplus::Graphics gr(memDC);
	vector<BusArrivalInfo> vecBusInfo;
	pData->GetBusInfo(vecBusInfo);

	int nDoubleBus = 0, nBus1stPos = 0, nBus2ndPos = 0, ii;

	vector<BusArrivalInfo>::const_iterator iter;
	for (ii = 0, iter = vecBusInfo.begin(); iter != vecBusInfo.end(); iter++, ii++)
	{
		int nStationNum = pData->GetCurStaionPos() - iter->iNum + 1;
		vector<Gdiplus::PointF> vecPos;
		pData->GetStationPos(vecPos);
		Gdiplus::PointF pointF = vecPos[nStationNum - 1];
		Gdiplus::PointF pointFlast = vecPos[nStationNum - 2];
		Gdiplus::PointF pointFHalf(pointFlast.X + (pointF.X - pointFlast.X )/2, pointF.Y);

		if(0 == ii)
		{
			nBus1stPos = iter->iNum;
		}
		else if(1 == ii)
		{
			nBus2ndPos = iter->iNum;
			if(nBus2ndPos == nBus1stPos)
			{
				nDoubleBus ++;
			}
		}
		else if(2 == ii)
		{
			if(nBus2ndPos == iter->iNum)
			{
				nDoubleBus++;
			}
			else if(nDoubleBus != 0)
			{
				nDoubleBus -- ;
			}
		}

		Gdiplus::Image* pIm = NULL;

		//当前站使用红色图标
		if(0 == iter->iNum && iter->iDistance <= 40)
		{
			pIm = m_vecimBusIcon[nDoubleBus+3];		
		}
		else
		{
			pIm = m_vecimBusIcon[nDoubleBus];
		}

		REAL iHeight = (REAL)pIm->GetHeight();
		REAL iWidth = (REAL)pIm->GetWidth();

		//if((0 == iter->iNum && iter->iDistance <= 40) || 0 < iter->iNum)
		{
			gr.DrawImage(pIm, pointF.X - iWidth/2, pointF.Y - iHeight - 5, iWidth, iHeight);
		}
		//else
		{
			//gr.DrawImage(pIm, pointFHalf.X - 1, pointFHalf.Y - 18, iWidth, iHeight);
		}
	}
}

void CBasePanelDlg::DrawBusStatus(CData* pData)
{
	CDC& memDC = pData->GetMemDC();
	Gdiplus::Graphics gr(memDC);
	vector<BusArrivalInfo> vecBusInfo ;
	pData->GetBusInfo(vecBusInfo);

	CRect rc;
	GetClientRect(&rc);


	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 126, 254));
	Gdiplus::StringFormat stringformat;
	stringformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	stringformat.SetAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	//第一辆车
	CString strStationNum;
	if(0 < vecBusInfo.size())
	{
		const BusArrivalInfo& refBus1Info = vecBusInfo[0];

		int nStationNum = refBus1Info.iNum;
		int nDis = refBus1Info.iDistance;

		if(nStationNum > 0 && nDis > 100)
		{
			strStationNum.Format(_T("%d"), nStationNum);

			Gdiplus::PointF pointFirst(rc.right - m_nDisBkWidth/2 - 56, 81);
			Gdiplus::RectF rectFLine1Num(pointFirst, Gdiplus::SizeF(100, 56));
			Gdiplus::Font font(L"方正兰亭黑简体", 56 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
//  			Gdiplus::PointF pointFirst(rc.right - m_nDisBkWidth/2 - 28, 81);
//  			Gdiplus::RectF rectFLine1Num(pointFirst, Gdiplus::SizeF(56, 56));
//  			Gdiplus::Font font(L"方正兰亭黑简体", 15 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			stringformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			stringformat.SetAlignment(Gdiplus::StringAlignmentCenter);

			WCHAR wcStationNum[32] = {0};
			MultiByteToWideChar(CP_ACP, 0, strStationNum, strStationNum.GetLength(), wcStationNum, sizeof(wcStationNum));
			gr.DrawString(wcStationNum, wcslen(wcStationNum), &font, rectFLine1Num, &stringformat, &brush);

			//到站文字
			Gdiplus::SolidBrush brushBusBkDis(Gdiplus::Color(0,126,254));
			WCHAR wcBusBkDis[16] = {L"站"};
			//WCHAR wcBusBkDis[16] = {L"米"};
			Gdiplus::PointF pointFBusBkDis((REAL)rc.right - 34, 109);
			Gdiplus::RectF rectBusBkDis(pointFBusBkDis, Gdiplus::SizeF(24, 24));
			Gdiplus::Font fontBusBkDis(L"方正兰亭黑简体", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			stringformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			stringformat.SetAlignment(Gdiplus::StringAlignmentCenter);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wcBusBkDis, wcslen(wcBusBkDis), &fontBusBkDis, rectBusBkDis, &stringformat, &brushBusBkDis);

		}
		else if(nStationNum == 0)
		{
			if(nDis > 50)
				strStationNum.Format(_T("即将到站"));
			else
				strStationNum.Format(_T("已经进站"));

			WCHAR wcStationNum[8] = {0};
			MultiByteToWideChar(CP_ACP, 0, strStationNum, strStationNum.GetLength(), wcStationNum, sizeof(wcStationNum));
			Gdiplus::Font font(L"方正兰亭黑简体", 26 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

			Gdiplus::PointF pointFirst(rc.right - m_nDisBkWidth, 81);
			Gdiplus::RectF rectFLine1Num(pointFirst, Gdiplus::SizeF(m_nDisBkWidth, 56));
			gr.DrawString(wcStationNum, wcslen(wcStationNum), &font, rectFLine1Num, &stringformat, &brush);
		}
	}
	else 
	{
		strStationNum.Format(_T("待发"));

		Gdiplus::PointF pointFirst(rc.right - m_nDisBkWidth, 81);
		Gdiplus::RectF rectFLine1Num(pointFirst, Gdiplus::SizeF(m_nDisBkWidth, 56));
		Gdiplus::Font font(L"方正兰亭黑简体", 26 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		WCHAR wcStationNum[8] = {0};
		MultiByteToWideChar(CP_ACP, 0, strStationNum, strStationNum.GetLength(), wcStationNum, sizeof(wcStationNum));
		gr.DrawString(wcStationNum, wcslen(wcStationNum), &font, rectFLine1Num, &stringformat, &brush);
	}
}

void CBasePanelDlg::OnDestroy()
{
	CDialog::OnDestroy();
	m_bRun = FALSE;
	m_bStation = FALSE;
	CloseHandle(m_hEvent);

	TerminateProcess(m_hPage, 0);
	TerminateProcess(m_hWord, 0);
// 	WaitForSingleObject(m_hPage, INFINITE);
// 	WaitForSingleObject(m_hWord, INFINITE);

	vector<Gdiplus::Image*>::iterator iter;
	for (iter = m_vecimBusIcon.begin(); iter != m_vecimBusIcon.end(); iter++)
	{
		Gdiplus::Image* pIm = *iter;
		if(pIm)
		{
			delete pIm;
			pIm = NULL;
		}
	}

	m_vecimBusIcon.clear();

	for (iter = m_vecimBusStatus.begin(); iter != m_vecimBusStatus.end(); iter++)
	{
		Gdiplus::Image* pIm = *iter;
		if(pIm)
		{
			delete pIm;
			pIm = NULL;
		}
	}

	m_vecimBusStatus.clear();
	// TODO: 在此处添加消息处理程序代码
}

void CBasePanelDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}
