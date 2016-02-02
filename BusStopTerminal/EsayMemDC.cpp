#include "StdAfx.h"
#include "EsayMemDC.h"

BOOL CEsayMemDC::m_bUseMemoryDC = TRUE;

CEsayMemDC::CEsayMemDC(CDC& dc, const CRect& rect) :
m_bMemDC(FALSE), m_pOldBmp(NULL), m_rect(rect)
{
	ASSERT(!m_rect.IsRectEmpty());

	if (m_bUseMemoryDC && m_dcMem.CreateCompatibleDC(&dc) && m_bmp.CreateCompatibleBitmap(&dc, m_rect.Width(), m_rect.Height()))
	{
		//-------------------------------------------------------------
		// Off-screen DC successfully created. Better paint to it then!
		//-------------------------------------------------------------
		m_bMemDC = TRUE;
		m_pOldBmp = m_dcMem.SelectObject(&m_bmp);
	}
}

BOOL  CEsayMemDC::BltMem(CDC& dc)
{
	if (m_bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//--------------------------------------
		CRect rectClip;
		rectClip = m_rect;

		dc.BitBlt(rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(), &m_dcMem, 0, 0, SRCCOPY);

		m_dcMem.SelectObject(m_pOldBmp);

		return TRUE;
	}

	return FALSE;
}

BOOL  CEsayMemDC::BltMem(CDC& dc, CRect& rc)
{
	if (m_bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-memery DC:
		//--------------------------------------
		CRect rectClip;
		rectClip = rc;

		dc.BitBlt(rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(), &m_dcMem, 0, 0, SRCCOPY);

		m_dcMem.SelectObject(m_pOldBmp);

		return TRUE;
	}

	return FALSE;
}

CEsayMemDC::~CEsayMemDC()
{

}