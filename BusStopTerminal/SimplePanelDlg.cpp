// SimplePanelDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusStopTerminal.h"
#include "SimplePanelDlg.h"


// CSimplePanelDlg �Ի���

IMPLEMENT_DYNAMIC(CSimplePanelDlg, CDialog)

CSimplePanelDlg::CSimplePanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimplePanelDlg::IDD, pParent)
{
	m_vecData.clear();
	m_nIndexShow = 0;
	m_nCount = 0;
}

CSimplePanelDlg::~CSimplePanelDlg()
{
}

void CSimplePanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSimplePanelDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSimplePanelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSimplePanelDlg::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSimplePanelDlg ��Ϣ�������

void CSimplePanelDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CSimplePanelDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnCancel();
}

BOOL CSimplePanelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitPannel();

	SetTimer(1, 10000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSimplePanelDlg::InitPannel()
{
	CDC* pDC = GetDC();

	char curDir[160] = {0};
	WCHAR wcurDir[160] = {0};

	GetCurrentDirectory(160, curDir);
	ZeroMemory(wcurDir, sizeof(wcurDir));

	CString strImPath = curDir;
	strImPath += _T("\\Res\\linepannel_simple.png");

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strImPath, strImPath.GetLength(), wcurDir, 160);

	m_pImLineBK = new Gdiplus::Image(wcurDir);

	int nHeight = m_pImLineBK->GetHeight();
	int nWidth = m_pImLineBK->GetWidth();


	CRect rc1(0, 46, 539, nHeight + 46);
	CEsayMemDC* pMemDC =  new CEsayMemDC(*pDC, rc1);
	m_vecMemDC.push_back(pMemDC);

	CRect rc2(542, 46, 1080, nHeight + 46);
	pMemDC =  new CEsayMemDC(*pDC, rc2);
	m_vecMemDC.push_back(pMemDC);

	CRect rc3(0, nHeight + 52, 540, nHeight*2 + 52);
	pMemDC =  new CEsayMemDC(*pDC, rc3);
	m_vecMemDC.push_back(pMemDC);

	CRect rc4(542, nHeight + 52, 1080, nHeight*2 + 52);
	pMemDC =  new CEsayMemDC(*pDC, rc4);
	m_vecMemDC.push_back(pMemDC);

	CRect rc5(0, nHeight*2 + 58, 540, nHeight*3 + 58);
	pMemDC =  new CEsayMemDC(*pDC, rc5);
	m_vecMemDC.push_back(pMemDC);

	CRect rc6(542, nHeight*2 + 58, 1080, nHeight*3 + 58);
	pMemDC =  new CEsayMemDC(*pDC, rc6);
	m_vecMemDC.push_back(pMemDC);

	ReleaseDC(pDC);
}

void CSimplePanelDlg::UpdateArrival()
{
	m_lock.Lock();
	DrawLine();
	m_lock.UnLock();
}

void CSimplePanelDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	LogObject;

	LogOutInfo("����忪ʼ�ػ�");

	DrawBk();
	DrawLine();
}

void CSimplePanelDlg::DrawBk()
{
	CDC* pDC = GetDC();

	CRect rc;
	GetClientRect(&rc);

	int nHeight = m_pImLineBK->GetHeight();
	int nWidth = m_pImLineBK->GetWidth();

	CEsayMemDC* pmemDC = new CEsayMemDC(*pDC, rc);
	Gdiplus::Color cl = Gdiplus::Color::White;
	Gdiplus::SolidBrush brush(cl);

	Gdiplus::Graphics gr(pmemDC->GetDC());
	gr.FillRectangle(&brush, rc.left, rc.top, rc.Width(), rc.Height());

	//��������
	Gdiplus::SolidBrush brushBkWord(Gdiplus::Color(149,158,168));
	std::wstring str = L"��һ����";

	Gdiplus::PointF pointFBkWord(60+nWidth, 0);
	Gdiplus::RectF rectBkWord(pointFBkWord, Gdiplus::SizeF(145, 46));
	Gdiplus::Font fontBkWord(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatBkWord;
	stringFormatBkWord.SetAlignment(Gdiplus::StringAlignmentFar);
	stringFormatBkWord.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(str.c_str(), str.size(), &fontBkWord, rectBkWord, &stringFormatBkWord, &brushBkWord);

	str = L"�ڶ�����";

	pointFBkWord.X = 60 + nWidth + 145;
	rectBkWord.X = pointFBkWord.X;
	gr.DrawString(str.c_str(), str.size(), &fontBkWord, rectBkWord, &stringFormatBkWord, &brushBkWord);

	str = L"��һ����";

	pointFBkWord.X = 60 + nWidth*2 + 145 * 2+ 100 + 10;
	rectBkWord.X = pointFBkWord.X;
	gr.DrawString(str.c_str(), str.size(), &fontBkWord, rectBkWord, &stringFormatBkWord, &brushBkWord);

	str = L"�ڶ�����";

	pointFBkWord.X = 60 + nWidth * 2 + 145 * 3+ 100 + 10;
	rectBkWord.X = pointFBkWord.X;
	gr.DrawString(str.c_str(), str.size(), &fontBkWord, rectBkWord, &stringFormatBkWord, &brushBkWord);

	//�ײ��ָ���
	Gdiplus::SolidBrush brushLine(Gdiplus::Color(215, 215, 215));
	Gdiplus::Pen penLine(&brushLine, 1);

	gr.DrawLine(&penLine, Gdiplus::Point(rc.left, rc.bottom-1), Gdiplus::Point(rc.right, rc.bottom-1));

	//�м�����
	Gdiplus::SolidBrush brushLineM(Gdiplus::Color(215, 215, 215));
	Gdiplus::Pen penLineM(&brushLineM, 2);

	gr.DrawLine(&penLineM, Gdiplus::Point(541, rc.top + 46), Gdiplus::Point(541, rc.top + nHeight*3 + 64));
	pmemDC->BltMem(*pDC);

	delete pmemDC;

	ReleaseDC(pDC);
}

void CSimplePanelDlg::DrawArrive(Gdiplus::Graphics& gr, vector<BusArrivalInfo>& vecArrival, CRect& rc, CData* pData)
{
	//ÿ����ˢ�±���
	Gdiplus::Color cl = Gdiplus::Color::White;
	Gdiplus::SolidBrush brush(cl);
	gr.FillRectangle(&brush, 0, 0, rc.Width(), rc.Height());

	//��ʾ��·�ű���
	Gdiplus::PointF pointLineBK(60, 0);
	gr.DrawImage(m_pImLineBK, pointLineBK);

	int nWidth = m_pImLineBK->GetWidth();
	int nHeight = m_pImLineBK->GetHeight();

	//��ʾ��·��
	CString strLineNum = pData->GetLineNumber();
	Gdiplus::SolidBrush brushLineNum(Gdiplus::Color::White);

	WCHAR wcLineNum[6] = {0};
	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strLineNum, strLineNum.GetLength(), wcLineNum, sizeof(wcLineNum));

	Gdiplus::PointF pointLineNum = pointLineBK;
	Gdiplus::RectF rectLineNum(pointLineNum, Gdiplus::SizeF(nWidth, nHeight));
	Gdiplus::Font fontLineNum(L"helvetica", 42 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatLineNum;
	stringFormatLineNum.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormatLineNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcLineNum, wcslen(wcLineNum), &fontLineNum, rectLineNum, &stringFormatLineNum, &brushLineNum);

	//��ʾ·
	std::wstring str = L"·";
	Gdiplus::SolidBrush brushLu(Gdiplus::Color::White);

	Gdiplus::PointF pointWord = pointLineBK;
	pointWord.X += 0.75 * nWidth;
	Gdiplus::RectF rectWord(pointWord, Gdiplus::SizeF(40, nHeight - 5));
	Gdiplus::Font fontLu(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatWord;
	stringFormatWord.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatWord.SetLineAlignment(Gdiplus::StringAlignmentFar);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(str.c_str(), str.size(), &fontLu, rectWord, &stringFormatWord, &brushLu);

	if(0 < vecArrival.size())
	{
		//��ʾ��һ������վ
		BusArrivalInfo& stArrive = vecArrival[0];
		if(stArrive.iNum > 0)
		{
			//����վ
			CString strNum;
			strNum.Format("%d", stArrive.iNum);
			Gdiplus::SolidBrush brushNum(Gdiplus::Color(23,117,231));

			WCHAR wcNum[6] = {0};
			MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strNum, strNum.GetLength(), wcNum, sizeof(wcNum));

			Gdiplus::PointF pointNum(60 + nWidth, 0);
			Gdiplus::RectF rectNum(pointNum, Gdiplus::SizeF(145, nHeight));
			Gdiplus::Font fontNum(L"helvetica", 42 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatNum;
			stringFormatNum.SetAlignment(Gdiplus::StringAlignmentCenter);
			stringFormatNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wcNum, wcslen(wcNum), &fontNum, rectNum, &stringFormatNum, &brushNum);

			std::wstring wstr = L"վ";
			Gdiplus::SolidBrush brushZhan(Gdiplus::Color(149,158,168));
			Gdiplus::PointF pointZhan(60 + nWidth + 145 - 32, 0);
			Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(40, nHeight - 5));
			Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatZhan;
			stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentNear);
			stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

		}
		else if(stArrive.iNum == 0)
		{
			if(stArrive.iDistance <= 50)
			{
				std::wstring wstr = L"�Ѿ���վ";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(60 + nWidth, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(145, nHeight - 5));
				Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);
			}
			else
			{
				std::wstring wstr = L"������վ";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(60 + nWidth, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(145, nHeight - 5));
				Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

			}
		}

	}
	else
	{
		std::wstring wstr = L"����";
		Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
		Gdiplus::PointF pointZhan(60 + nWidth, 0);
		Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(145, nHeight - 5));
		Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatZhan;
		stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

	}

	if(1 < vecArrival.size())
	{
		//��ʾ�ڶ�������վ
		BusArrivalInfo& stArrive = vecArrival[1];
		if(stArrive.iNum > 0)
		{
			//����վ
			CString strNum;
			strNum.Format("%d", stArrive.iNum);
			Gdiplus::SolidBrush brushNum(Gdiplus::Color(23,117,231));

			WCHAR wcNum[6] = {0};
			MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strNum, strNum.GetLength(), wcNum, sizeof(wcNum));

			Gdiplus::PointF pointNum(60 + nWidth + 145, 0);
			Gdiplus::RectF rectNum(pointNum, Gdiplus::SizeF(145, nHeight));
			Gdiplus::Font fontNum(L"helvetica", 42 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatNum;
			stringFormatNum.SetAlignment(Gdiplus::StringAlignmentCenter);
			stringFormatNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wcNum, wcslen(wcNum), &fontNum, rectNum, &stringFormatNum, &brushNum);

			std::wstring wstr = L"վ";
			Gdiplus::SolidBrush brushZhan(Gdiplus::Color(149,158,168));
			Gdiplus::PointF pointZhan(60 + nWidth + 145 + 145 - 32, 0);
			Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(40, nHeight - 5));
			Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			Gdiplus::StringFormat stringFormatZhan;
			stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentNear);
			stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

			gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
			gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

		}
		else if(stArrive.iNum == 0)
		{
			if(stArrive.iDistance <= 50)
			{
				std::wstring wstr = L"�Ѿ���վ";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(60 + nWidth + 145, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(145, nHeight - 5));
				Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);
			}
			else
			{
				std::wstring wstr = L"������վ";
				Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
				Gdiplus::PointF pointZhan(60 + nWidth + 145, 0);
				Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(145, nHeight - 5));
				Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::StringFormat stringFormatZhan;
				stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
				stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

				gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

			}
		}

	}
	else
	{
		std::wstring wstr = L"����";
		Gdiplus::SolidBrush brushZhan(Gdiplus::Color(23,117,231));
		Gdiplus::PointF pointZhan(60 + nWidth + 145, 0);
		Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(145, nHeight - 5));
		Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatZhan;
		stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentFar);
		stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);

	}

}

void CSimplePanelDlg::DrawEmpty(Gdiplus::Graphics& gr, CRect& rc)
{
	//ÿ����ˢ�±���
	Gdiplus::Color cl = Gdiplus::Color::White;
	Gdiplus::SolidBrush brush(cl);
	gr.FillRectangle(&brush, 0, 0, rc.Width(), rc.Height());

	//��ʾ��·�ű���
	Gdiplus::PointF pointLineBK(60, 0);
	gr.DrawImage(m_pImLineBK, pointLineBK);

	int nWidth = m_pImLineBK->GetWidth();
	int nHeight = m_pImLineBK->GetHeight();

	//��ʾ--
	CString strLineNum = "--";
	Gdiplus::SolidBrush brushLineNum(Gdiplus::Color::White);

	WCHAR wcLineNum[6] = {0};
	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strLineNum, strLineNum.GetLength(), wcLineNum, sizeof(wcLineNum));

	Gdiplus::PointF pointLineNum = pointLineBK;
	Gdiplus::RectF rectLineNum(pointLineNum, Gdiplus::SizeF(nWidth, nHeight));
	Gdiplus::Font fontLineNum(L"helvetica", 42 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatLineNum;
	stringFormatLineNum.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormatLineNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(wcLineNum, wcslen(wcLineNum), &fontLineNum, rectLineNum, &stringFormatLineNum, &brushLineNum);

	//��ʾ·
	std::wstring str = L"·";
	Gdiplus::SolidBrush brushLu(Gdiplus::Color::White);

	Gdiplus::PointF pointWord = pointLineBK;
	pointWord.X += 0.75 * nWidth;
	Gdiplus::RectF rectWord(pointWord, Gdiplus::SizeF(40, nHeight - 5));
	Gdiplus::Font fontLu(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormatWord;
	stringFormatWord.SetAlignment(Gdiplus::StringAlignmentNear);
	stringFormatWord.SetLineAlignment(Gdiplus::StringAlignmentFar);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(str.c_str(), str.size(), &fontLu, rectWord, &stringFormatWord, &brushLu);

	//��һ��������վ
	if(1)
	{
		CString strNum = "- -";
		Gdiplus::SolidBrush brushNum(Gdiplus::Color(149,158,168));

		WCHAR wcNum[6] = {0};
		MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strNum, strNum.GetLength(), wcNum, sizeof(wcNum));

		Gdiplus::PointF pointNum(60 + nWidth, 0);
		Gdiplus::RectF rectNum(pointNum, Gdiplus::SizeF(145, nHeight));
		Gdiplus::Font fontNum(L"helvetica", 42 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatNum;
		stringFormatNum.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wcNum, wcslen(wcNum), &fontNum, rectNum, &stringFormatNum, &brushNum);

		std::wstring wstr = L"վ";
		Gdiplus::SolidBrush brushZhan(Gdiplus::Color(149,158,168));
		Gdiplus::PointF pointZhan(60 + nWidth + 0.7 * 145, 0);
		Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(40, nHeight - 5));
		Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatZhan;
		stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentNear);
		stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);	
	}


	if(1)
	{
		//�ڶ���������վ
		CString strNum = "- -";
		Gdiplus::SolidBrush brushNum(Gdiplus::Color(149,158,168));

		WCHAR wcNum[6] = {0};
		MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)strNum, strNum.GetLength(), wcNum, sizeof(wcNum));

		Gdiplus::PointF pointNum(60 + nWidth + 145, 0);
		Gdiplus::RectF rectNum(pointNum, Gdiplus::SizeF(145, nHeight));
		Gdiplus::Font fontNum(L"helvetica", 42 , Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatNum;
		stringFormatNum.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormatNum.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wcNum, wcslen(wcNum), &fontNum, rectNum, &stringFormatNum, &brushNum);

		std::wstring wstr = L"վ";
		Gdiplus::SolidBrush brushZhan(Gdiplus::Color(149,158,168));
		Gdiplus::PointF pointZhan(60 + nWidth + 0.7 * 145 + 145, 0);
		Gdiplus::RectF rectZhan(pointZhan, Gdiplus::SizeF(40, nHeight - 5));
		Gdiplus::Font fontZhan(L"������ͤ�ڼ���", 24 , Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringFormatZhan;
		stringFormatZhan.SetAlignment(Gdiplus::StringAlignmentNear);
		stringFormatZhan.SetLineAlignment(Gdiplus::StringAlignmentFar);

		gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gr.DrawString(wstr.c_str(), wstr.size(), &fontZhan, rectZhan, &stringFormatZhan, &brushZhan);
	}


}

void CSimplePanelDlg::DrawLine()
{
	if(0 >= m_vecData.size())
		return;

	CDC* pDC = GetDC();

	int nSize = 0;
	m_vecData.size() < m_vecMemDC.size() ? nSize = (int)m_vecData.size() : nSize = (int)m_vecMemDC.size();

	for (int ii = 0; ii < 6; ii++)
	{
		int nIndexData = ii;

		//��ʾ��һ�������ݣ����������ʾ��ǰ����
		(ii + m_nIndexShow * 6) >= (int)m_vecData.size() ? nIndexData : nIndexData = ii + m_nIndexShow * 6;

		CEsayMemDC* pMemDC = m_vecMemDC[ii];
		CRect& rc = pMemDC->GetMemRect();

		Gdiplus::Graphics gr(pMemDC->GetDC());

		if(nIndexData < m_vecData.size())
		{
			CData* pData = m_vecData[nIndexData];
			vector<BusArrivalInfo> vecArrival;
			pData->GetBusInfo(vecArrival);

			DrawArrive(gr, vecArrival, rc, pData);
		}
		else
			DrawEmpty(gr, rc);

		pMemDC->BltMem(*pDC);
	}

	ReleaseDC(pDC);
}

void CSimplePanelDlg::ChangeLine()
{
	int nLineCount = m_vecData.size();
	m_nIndexShow++;
	if((m_nIndexShow * 6 > nLineCount) || (m_nIndexShow * 6 - nLineCount == 0))
		m_nIndexShow = 0;

	if(nLineCount <= 6)
		return;

	//�ȼ��㵱ǰ�ж�������·Ҫ��ҳ��
	int nTmpMemDCCount = 6;
	nLineCount % 4 == 0 ? nTmpMemDCCount : nTmpMemDCCount = nLineCount % 6;

	CDC* pDC = GetDC();

	//������ʱ���ڴ滭��
	vector<CEsayMemDC*> vecTemMemDC;
	for(int ii = 0; ii < nTmpMemDCCount; ii++)
	{
		CEsayMemDC* pMemDC = m_vecMemDC[ii];
		CEsayMemDC* pTmpMemDC = new CEsayMemDC(*pDC, pMemDC->GetMemRect());
		vecTemMemDC.push_back(pTmpMemDC);
	}

	//����һҳ�����ݻ����ڴ滭����

	for(int ii = 0; ii < nTmpMemDCCount; ii++)
	{
		CData* pData = m_vecData[ii+m_nIndexShow*6];
		vector<BusArrivalInfo> vecArrive;
		pData->GetBusInfo(vecArrive);

		CEsayMemDC* pTmpMemDC = vecTemMemDC[ii];
		Gdiplus::Graphics gr(pTmpMemDC->GetDC());

		DrawArrive(gr, vecArrive, pTmpMemDC->GetMemRect(), pData);
	}


	//���Ż����л�����
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

	//����ʱ���ڴ滭�����µ�ԭ�����ڴ滭����

	ReleaseDC(pDC);

}

void CSimplePanelDlg::MoveUpDown(CDC* pDCSrc, CDC* pDCDst, CRect& rc, CDC* pDCSreen)
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

void CSimplePanelDlg::OnDestroy()
{
	CDialog::OnDestroy();

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

	// TODO: �ڴ˴������Ϣ����������
}

void CSimplePanelDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_lock.Lock();
	ChangeLine();
	m_lock.UnLock();

	CDialog::OnTimer(nIDEvent);
}
