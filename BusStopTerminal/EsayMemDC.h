#pragma once

class CEsayMemDC
{
public:
	static BOOL m_bUseMemoryDC;

	CEsayMemDC(CDC& dc, const CRect& rect);

	virtual ~CEsayMemDC();

	CDC& GetDC() { return m_dcMem; }
	BOOL IsMemDC() const { return m_bMemDC; }
	BOOL BltMem(CDC& dc);
	BOOL BltMem(CDC& dc, CRect& rc);
	CRect& GetMemRect() {return m_rect;}

protected:
	BOOL     m_bMemDC;
	CDC      m_dcMem;
	CBitmap  m_bmp;
	CBitmap* m_pOldBmp;
	CRect    m_rect;
};