// MediumPanelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "MediumPanelDlg.h"


// CMediumPanelDlg 对话框

IMPLEMENT_DYNAMIC(CMediumPanelDlg, CDialog)

CMediumPanelDlg::CMediumPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMediumPanelDlg::IDD, pParent)
{
	m_vecData.clear();
	m_nIndexShow = 0;
	m_nCount = 0;
}

CMediumPanelDlg::~CMediumPanelDlg()
{
}

void CMediumPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMediumPanelDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CMediumPanelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMediumPanelDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMediumPanelDlg 消息处理程序

BOOL CMediumPanelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitPannel();

	SetTimer(1, 10000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMediumPanelDlg::InitPannel()
{
	LogObject;

	char curDir[160] = {0};
	WCHAR wcurDir[160] = {0};

	GetCurrentDirectory(160, curDir);

	CString strImPath = curDir;
	strImPath += _T("\\Res\\comfort.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecImComfort.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\normal.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecImComfort.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\congestion.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_vecImComfort.push_back(Gdiplus::Image(wcurDir).Clone());

	ZeroMemory(wcurDir, sizeof(wcurDir));

	strImPath = curDir;
	strImPath += _T("\\Res\\linepannel_mid.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_pImLineBk = new Gdiplus::Image(wcurDir);
	int nHeight = m_pImLineBk->GetHeight();

	CDC* pDC = GetDC();
	CRect rc(0, 33, 1080, nHeight+33);

	for(int ii = 0; ii < 4; ii++)
	{
		CEsayMemDC* pMemDC =  new CEsayMemDC(*pDC, rc);
		m_vecMemDC.push_back(pMemDC);
		rc.top += nHeight+5;
		rc.bottom += nHeight+5;
	}

	ReleaseDC(pDC);


}

void CMediumPanelDlg::DrawBk()
{
	LogObject;
	CDC* pDC = GetDC();

	CRect rc;
	GetClientRect(&rc);

	CEsayMemDC* pmemDC = new CEsayMemDC(*pDC, rc);
	Gdiplus::Color cl = Gdiplus::Color::White;
	Gdiplus::SolidBrush brush(cl);

	Gdiplus::Graphics gr(pmemDC->GetDC());
	gr.FillRectangle(&brush, rc.left, rc.top, rc.Width(), rc.Height());

	Gdiplus::SolidBrush brushBkWord(Gdiplus::Color(149,158,168));
	std::wstring str = L"第一辆车";

	Gdiplus::PointF pointFBkWord(280, 0);
	Gdiplus::RectF rectBkWord(pointFBkWord, Gdiplus::SizeF(420-280, 33));
	Gdiplus::Font fontBkWord(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBkWord;
	stringFormatBkWord.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(str.c_str(), str.size(), &fontBkWord, rectBkWord, &stringFormatBkWord, &brushBkWord);

	str = L"第二辆车";
	pointFBkWord.X = 750;
	rectBkWord.X = pointFBkWord.X;
	gr.DrawString(str.c_str(), str.size(), &fontBkWord, rectBkWord, &stringFormatBkWord, &brushBkWord);

	Gdiplus::SolidBrush brushLine(Gdiplus::Color(153, 153, 153));
	Gdiplus::Pen penLine(&brushLine, 1);

	gr.DrawLine(&penLine, Gdiplus::Point(rc.left, rc.bottom-1), Gdiplus::Point(rc.right, rc.bottom-1));

	pmemDC->BltMem(*pDC);

	delete pmemDC;

	ReleaseDC(pDC);
}

void CMediumPanelDlg::DrawArrive(Gdiplus::Graphics& gr, vector<BusArrivalInfo>& vecArrival, CRect& rc, CData* pData)
{
	//每次先刷新背景
	Gdiplus::Color cl = Gdiplus::Color::White;
	Gdiplus::SolidBrush brush(cl);
	gr.FillRectangle(&brush, 0, 0, rc.Width(), rc.Height());

	//显示灰背景
	brush.SetColor(Gdiplus::Color(242, 242, 242));
	gr.FillRectangle(&brush, 70, 0, rc.Width() - 130, rc.Height());

	//显示线路号背景
	Gdiplus::PointF pointLineBK(70, 0);
	gr.DrawImage(m_pImLineBk, pointLineBK);

	int nWidth = m_pImLineBk->GetWidth();
	int nHeight = m_pImLineBk->GetHeight();

	//显示线路号
	CString strLineNum = pData->GetLineNumber();
	Gdiplus::SolidBrush brushLineNum(Gdiplus::Color::White);

	WCHAR wcLineNum[6] = {0};
	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strLineNum, strLineNum.GetLength(), wcLineNum, sizeof(wcLineNum));

	Gdiplus::PointF pointLineNum(70, 0);
	Gdiplus::RectF rectLineNum(pointLineNum, Gdiplus::SizeF(nWidth, (Gdiplus::REAL)rc.Height()));
	Gdiplus::Font fontLineNum(L"helvetica", 36 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatLineNum;
	stringFormatLineNum.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormatLineNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcLineNum, wcslen(wcLineNum), &fontLineNum, rectLineNum, &stringFormatLineNum, &brushLineNum);



	//显示路
	std::wstring str = L"路";

	Gdiplus::PointF pointWord(160, 0);
	Gdiplus::RectF rectWord(pointWord, Gdiplus::SizeF(40, (Gdiplus::REAL)rc.Height() - 5));
	Gdiplus::Font fontLu(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatWord;
	stringFormatWord.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatWord.SetLineAlignment(Gdiplus::StringAlignmentFar);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(str.c_str(), str.size(), &fontLu, rectWord, &stringFormatWord, &brushLineNum);

	//两条分割线
	Gdiplus::SolidBrush brushLine(Gdiplus::Color(215, 215, 215));
	Gdiplus::Pen penLine(&brushLine, 1);

	//gr.DrawLine(&penLine, Gdiplus::Point(370, 0), Gdiplus::Point(370, 30));
	//gr.DrawLine(&penLine, Gdiplus::Point(510, 0), Gdiplus::Point(510, 30));
	gr.DrawLine(&penLine, Gdiplus::Point(500+nWidth, 0), Gdiplus::Point(500+nWidth, nHeight));
	//gr.DrawLine(&penLine, Gdiplus::Point(830, 0), Gdiplus::Point(830, 30));
	//gr.DrawLine(&penLine, Gdiplus::Point(940, 0), Gdiplus::Point(940, 30));

	if(0 < vecArrival.size())
	{
		//显示第一辆车到站
		BusArrivalInfo& stArrive = vecArrival[0];
		if(stArrive.iNum > 0)
		{
			//距离站
			CString strNum;
			strNum.Format("%d", stArrive.iNum);
			Gdiplus::SolidBrush brushNum(Gdiplus::Color(23,117,231));

			WCHAR wcNum[6] = {0};
			MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strNum, strNum.GetLength(), wcNum, sizeof(wcNum));

			Gdiplus::PointF pointNum(70+nWidth, 0);
			Gdiplus::RectF rectNum(pointNum, Gdiplus::SizeF(90, (Gdiplus::REAL)rc.Height()));
			Gdiplus::Font fontNum(L"helvetica", 36 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatNum;
			stringFormatNum.SetAlignment(Gdiplus::StringAlignmentFar);
			stringFormatNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wcNum, wcslen(wcNum), &fontNum, rectNum, &stringFormatNum, &brushNum);

			std::wstring wstr = L"站";
			Gdiplus::SolidBrush brushZhan(Gdiplus::Color(149,158,168));
			Gdiplus::PointF pointZhan(160+nWidth, 0);
			Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(40, (Gdiplus::REAL)rc.Height() - 5));
			Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatZhan;
			stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
			stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);


		}
		else if(stArrive.iNum == 0)
		{
			if(stArrive.iDistance <= 50)
			{
				std::wstring wstr = L"已经进站";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(70+nWidth, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(130, (Gdiplus::REAL)rc.Height() - 5));
				Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);
			}
			else
			{
				std::wstring wstr = L"即将到站";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(70+nWidth, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(130, (Gdiplus::REAL)rc.Height() - 5));
				Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);
			}

		}

		//距离分钟
		int nTime = stArrive.iTime / 60 + 1;
		CString strTime;
		strTime.Format("%d", nTime);

		WCHAR wcTime[6] = {0};
		MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strTime, strTime.GetLength(), wcTime, sizeof(wcTime));

		Gdiplus::SolidBrush brushTime(Gdiplus::Color(23,117,231));
		Gdiplus::PointF pointTime(200+nWidth, 0);
		Gdiplus::RectF rectTime(pointTime, Gdiplus::SizeF(100, (Gdiplus::REAL)rc.Height()));
		Gdiplus::Font fontTime(L"helvetica", 36 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatTime;
		stringFormatTime.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatTime.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wcTime, wcslen(wcTime), &fontTime, rectTime, &stringFormatTime, &brushTime);

		wstring wstr = L"分钟";
		Gdiplus::SolidBrush brushFen(Gdiplus::Color(149,158,168));
		Gdiplus::PointF pointFen(270+nWidth, 0);
		Gdiplus::RectF rectFen(pointFen, Gdiplus::SizeF(80, (Gdiplus::REAL)rc.Height() - 5));
		Gdiplus::Font fontFen(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatFen;
		stringFormatFen.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatFen.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontFen, rectFen, &stringFormatFen, &brushFen);

		//显示第一辆车拥挤度
		Gdiplus::Image* pim = m_vecImComfort[0];

		Gdiplus::REAL Height = (Gdiplus::REAL)pim->GetHeight();
		Gdiplus::REAL Width = (Gdiplus::REAL)pim->GetWidth();

		Gdiplus::PointF pointComf(405+nWidth, 5);

		gr.DrawImage(pim, pointComf);


	}
	else
	{
		std::wstring wstr = L"待发";
		Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
		Gdiplus::PointF pointZhan(70+nWidth, 0);
		Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(130, (Gdiplus::REAL)rc.Height() - 5));
		Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatZhan;
		stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

		Gdiplus::PointF pointFen(200+nWidth, 0);
		Gdiplus::RectF rectFen(pointFen, Gdiplus::SizeF(150, (Gdiplus::REAL)rc.Height() - 5));
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectFen, &stringFormatZhan, &brushZhan);

		//显示第一辆车拥挤度
		Gdiplus::Image* pim = m_vecImComfort[0];

		Gdiplus::REAL Height = (Gdiplus::REAL)pim->GetHeight();
		Gdiplus::REAL Width = (Gdiplus::REAL)pim->GetWidth();

		wstring wstrComf = L"--";
		Gdiplus::SolidBrush brushComf(Gdiplus::Color(149,158,168));

		Gdiplus::PointF pointComf(400+nWidth, 5);
		Gdiplus::RectF rectComf(pointComf, Gdiplus::SizeF(50, (Gdiplus::REAL)rc.Height()));
		Gdiplus::Font fontComf(L"方正兰亭黑简体", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

		Gdiplus::StringFormat stringFormatComf;
		stringFormatComf.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatComf.SetLineAlignment(Gdiplus::StringAlignmentNear);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstrComf.c_str(), wstrComf.size(), &fontComf, rectComf, &stringFormatComf, &brushComf);

	}

	if(1 < vecArrival.size())
	{
		//显示第二辆车到站
		BusArrivalInfo& stArrive = vecArrival[1];
		if(stArrive.iNum > 0)
		{
			//距离站
			CString strNum;
			strNum.Format("%d", stArrive.iNum);
			Gdiplus::SolidBrush brushNum(Gdiplus::Color(23,117,231));

			WCHAR wcNum[6] = {0};
			MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strNum, strNum.GetLength(), wcNum, sizeof(wcNum));

			Gdiplus::PointF pointNum(500+nWidth, 0);
			Gdiplus::RectF rectNum(pointNum, Gdiplus::SizeF(90, (Gdiplus::REAL)rc.Height()));
			Gdiplus::Font fontNum(L"helvetica", 36 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatNum;
			stringFormatNum.SetAlignment(Gdiplus::StringAlignmentFar);
			stringFormatNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wcNum, wcslen(wcNum), &fontNum, rectNum, &stringFormatNum, &brushNum);

			std::wstring wstr = L"站";
			Gdiplus::SolidBrush brushZhan(Gdiplus::Color(149,158,168));
			Gdiplus::PointF pointZhan(590+nWidth, 0);
			Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(40, (Gdiplus::REAL)rc.Height() - 5));
			Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatZhan;
			stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
			stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);


		}
		else if(stArrive.iNum == 0)
		{
			if(stArrive.iDistance <= 50)
			{
				std::wstring wstr = L"已经进站";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(500+nWidth, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(130, (Gdiplus::REAL)rc.Height() - 5));
				Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);
			}
			else
			{
				std::wstring wstr = L"即将到站";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(500+nWidth, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(130, (Gdiplus::REAL)rc.Height() - 5));
				Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

			}
		}

		//显示第二辆车拥挤度
		Gdiplus::Image* pim = m_vecImComfort[1];

		Gdiplus::REAL Height = (Gdiplus::REAL)pim->GetHeight();
		Gdiplus::REAL Width = (Gdiplus::REAL)pim->GetWidth();

		Gdiplus::PointF pointComf(835+nWidth, 5);

		gr.DrawImage(pim, pointComf);

		//距离分钟
		int nTime = stArrive.iTime / 60 + 1;
		CString strTime;
		strTime.Format("%d", nTime);

		WCHAR wcTime[6] = {0};
		MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strTime, strTime.GetLength(), wcTime, sizeof(wcTime));

		Gdiplus::SolidBrush brushTime(Gdiplus::Color(23,117,231));
		Gdiplus::PointF pointTime(630+nWidth, 0);
		Gdiplus::RectF rectTime(pointTime, Gdiplus::SizeF(100, (Gdiplus::REAL)rc.Height()));
		Gdiplus::Font fontTime(L"helvetica", 36 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatTime;
		stringFormatTime.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatTime.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wcTime, wcslen(wcTime), &fontTime, rectTime, &stringFormatTime, &brushTime);

		wstring wstr = L"分钟";
		Gdiplus::SolidBrush brushFen(Gdiplus::Color(149,158,168));
		Gdiplus::PointF pointFen(700+nWidth, 0);
		Gdiplus::RectF rectFen(pointFen, Gdiplus::SizeF(80, (Gdiplus::REAL)rc.Height() - 5));
		Gdiplus::Font fontFen(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatFen;
		stringFormatFen.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatFen.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontFen, rectFen, &stringFormatFen, &brushFen);
	}
	else
	{
		std::wstring wstr = L"待发";
		Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
		Gdiplus::PointF pointZhan(500+nWidth, 0);
		Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(130, (Gdiplus::REAL)rc.Height() - 5));
		Gdiplus::Font fontZhan(L"方正兰亭黑简体", 20 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatZhan;
		stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

		Gdiplus::PointF pointFen(630+nWidth, 0);
		Gdiplus::RectF rectFen(pointFen, Gdiplus::SizeF(150, (Gdiplus::REAL)rc.Height() - 5));
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectFen, &stringFormatZhan, &brushZhan);

		//显示第二辆车拥挤度
		Gdiplus::Image* pim = m_vecImComfort[0];

		Gdiplus::REAL Height = (Gdiplus::REAL)pim->GetHeight();
		Gdiplus::REAL Width = (Gdiplus::REAL)pim->GetWidth();

		wstring wstrComf = L"--";
		Gdiplus::SolidBrush brushComf(Gdiplus::Color(149,158,168));

		Gdiplus::PointF pointComf(830+nWidth, 5);
		Gdiplus::RectF rectComf(pointComf, Gdiplus::SizeF(50, (Gdiplus::REAL)rc.Height()));
		Gdiplus::Font fontComf(L"方正兰亭黑简体", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

		Gdiplus::StringFormat stringFormatComf;
		stringFormatComf.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatComf.SetLineAlignment(Gdiplus::StringAlignmentNear);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstrComf.c_str(), wstrComf.size(), &fontComf, rectComf, &stringFormatComf, &brushComf);		

	}
}

void CMediumPanelDlg::DrawLine()
{
	LogObject;
	if(0 >= m_vecData.size())
		return;

	CDC* pDC = GetDC();

	int nSize = 0;
	m_vecData.size() < m_vecMemDC.size() ? nSize = (int)m_vecData.size() : nSize = (int)m_vecMemDC.size();

	for (int ii = 0; ii < nSize; ii++)
	{
		int nIndexData = ii;

		//显示下一屏的数据，如果不够显示当前屏的
		(ii + m_nIndexShow * 4) >= (int)m_vecData.size() ? nIndexData : nIndexData = ii + m_nIndexShow * 4;

		CEsayMemDC* pMemDC = m_vecMemDC[ii];
		CRect& rc = pMemDC->GetMemRect();
		CData* pData = m_vecData[nIndexData];
		vector<BusArrivalInfo> vecArrival;
		pData->GetBusInfo(vecArrival);

		Gdiplus::Graphics gr(pMemDC->GetDC());

		DrawArrive(gr, vecArrival, rc, pData);

		pMemDC->BltMem(*pDC);
	}

	ReleaseDC(pDC);

// 	m_nIndexShow ++;
// 	m_nIndexShow >= m_nCount ? m_nIndexShow = 0 : m_nIndexShow;
}

void CMediumPanelDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	DrawBk();

	DrawLine();
}

void CMediumPanelDlg::UpdateArrival()
{
	m_lock.Lock();
	DrawLine();
	m_lock.UnLock();
}

void CMediumPanelDlg::ChangeLine()
{
	int nLineCount = m_vecData.size();
	m_nIndexShow++;
	if((m_nIndexShow * 4 > nLineCount) || (m_nIndexShow * 4 - nLineCount == 0))
		m_nIndexShow = 0;

	if(nLineCount <= 4)
		return;

	//先计算当前有多少条线路要翻页的
	int nTmpMemDCCount = 4;
	nLineCount % 4 == 0 ? nTmpMemDCCount : nTmpMemDCCount = nLineCount % 4;

	CDC* pDC = GetDC();

	//创建临时的内存画布
	vector<CEsayMemDC*> vecTemMemDC;
	for(int ii = 0; ii < nTmpMemDCCount; ii++)
	{
		CEsayMemDC* pMemDC = m_vecMemDC[ii];
		CEsayMemDC* pTmpMemDC = new CEsayMemDC(*pDC, pMemDC->GetMemRect());
		vecTemMemDC.push_back(pTmpMemDC);
	}

	//将下一页的数据画到内存画布中

	for(int ii = 0; ii < nTmpMemDCCount; ii++)
	{
		CData* pData = m_vecData[ii+m_nIndexShow*4];
		vector<BusArrivalInfo> vecArrive;
		pData->GetBusInfo(vecArrive);

		CEsayMemDC* pTmpMemDC = vecTemMemDC[ii];
		Gdiplus::Graphics gr(pTmpMemDC->GetDC());

		DrawArrive(gr, vecArrive, pTmpMemDC->GetMemRect(), pData);
	}


	//两张画布切换动画
	for(int ii = 0; ii < nTmpMemDCCount; ii++)
	{
		CEsayMemDC* pMemDC = m_vecMemDC[ii];
		CEsayMemDC* pTmpMemDC = vecTemMemDC[ii];

		CRect rc(0, 0, pMemDC->GetMemRect().Width(), pMemDC->GetMemRect().Height());

		MoveUpDown(&pMemDC->GetDC(), &pTmpMemDC->GetDC(), pMemDC->GetMemRect(), pDC);

		//pMemDC->BltMem(*pDC);


		//pTmpMemDC->BltMem(*pDC);

		delete pTmpMemDC;
	}

	//将临时的内存画布更新到原本的内存画布中

	ReleaseDC(pDC);

}

void CMediumPanelDlg::MoveUpDown(CDC* pDCSrc, CDC* pDCDst, CRect& rc, CDC* pDCSreen)
{
	CRect rcSrc = rc, rcDst = rc;

	for (int ii = 1; ii <= rc.Height(); ii++)
	{
		//pDCSreen->BitBlt(rc.left, rc.top, rc.Width(), rc.Height() - ii, pDCSrc, rcSrc.left, rcSrc.top + ii, SRCCOPY);
		pDCSreen->BitBlt(rc.left, rc.top + rc.Height() - ii, rc.Width(), rc.Height() - rc.Height() + ii, 
			pDCDst, 0, 0, SRCCOPY);

		Sleep(10);
	}

}

void CMediumPanelDlg::OnDestroy()
{
	CDialog::OnDestroy();

	vector<Gdiplus::Image*>::iterator iter;
	for (iter = m_vecImComfort.begin(); iter != m_vecImComfort.end(); iter++)
	{
		Gdiplus::Image* pIm = *iter;
		if(pIm)
		{
			delete pIm;
			pIm = NULL;
		}
	}
	m_vecImComfort.clear();

	vector<CEsayMemDC*>::iterator iterDC;
	for (iterDC = m_vecMemDC.begin(); iterDC != m_vecMemDC.end(); iterDC++)
	{
		CEsayMemDC* pMemDC = *iterDC;
		if(pMemDC)
		{
			delete pMemDC;
			pMemDC = NULL;
		}
	}
	m_vecMemDC.clear();

	m_vecData.clear();

	// TODO: 在此处添加消息处理程序代码
}

void CMediumPanelDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_lock.Lock();
	ChangeLine();
	m_lock.UnLock();
	CDialog::OnTimer(nIDEvent);
}

void CMediumPanelDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CMediumPanelDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}
