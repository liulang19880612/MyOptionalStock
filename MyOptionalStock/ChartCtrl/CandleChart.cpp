#include "stdafx.h"
#include "CandleChart.h"
ColorSet                g_clrSet;
ColorSet*               ES_Color(){
	g_clrSet.clrBorder = RGBA(176, 0, 0, 255); // 网格颜色
	g_clrSet.clrBK = RGBA(0, 0, 0, 255);
	g_clrSet.clrTxtNormal = RGBA(192, 192, 192, 255); // 标题文字颜色
	g_clrSet.clrSelect = RGBA(0, 200, 255, 255);
	g_clrSet.clrUp = RGBA(255, 50, 50, 255); // 上涨
	g_clrSet.clrDown = RGBA(84, 255, 255, 255); // 下跌
	g_clrSet.clrTxtDown = RGBA(0, 230, 0, 255); // 坐标轴下跌
	g_clrSet.clrTxtUp = RGBA(255, 50, 50, 255);   // 坐标轴上涨 
	g_clrSet.clrDynaLine = RGBA(255, 255, 255, 255);
	g_clrSet.clrDynaLine2 = RGBA(192, 192, 0, 255);
	g_clrSet.clrDynaLine3 = RGBA(255, 100, 255, 255);	
	g_clrSet.bShowHerzAxisLine = true;
	g_clrSet.ftAxis = SFontPool::getSingleton().GetFont(L"face:宋体,bold:0,italic:1,underline:1,strike:1,adding:10", 1);
	return &g_clrSet; 
}
SStringW FormatLongInteger(SStringW strInteger)
{
	if (strInteger.Find('.') == -1)
	{
		double tmpDbl = _ttol(strInteger);
		if (fabs(tmpDbl) > 100000000)
		{
			strInteger.Format(L"%.2f", tmpDbl / 100000000.0);

			strInteger += L"亿";
		}
		else if (fabs(tmpDbl) >= 100000)
		{
			int weishu = 2;
			if (fabs(tmpDbl) >= 10000000)
				weishu = 1;

			strInteger.Format(L"%.*f", weishu, tmpDbl / 10000.0);

			strInteger += L"万";
		}
	}
	return strInteger;
}
SStringW NumericToString(double value, int nPect, int nDenom)
{
	if (nDenom < 1)
	{
		nDenom = 1;
	}
	SStringW ret = L"";
	if (nDenom != 1)
	{
		double f = value * nDenom;
		int nInt = (int)(f / nDenom);
		double yuShu = f - nInt*nDenom;
		if (nInt == 0)
		{
			ret.Format(L"%.0f/%d", yuShu, nDenom);
		}
		else
		{
			ret.Format(L"%d+%.0f/%d", nInt, yuShu, nDenom);
		}
	}
	else
	{
		ret.Format(L"%.*f", nPect, value);
		if (nPect == 0)
			ret = FormatLongInteger(ret);
	}

	if (ret == L"-0%" || ret == L"0%")
	{
		return L"0";
	}
	return ret;
}
CSize GetStringPix(IRenderTarget *pRender, SOUI::IFont*pFt, const SStringW& str)
{
	CSize sz;
#ifndef DEBUG
	if (pFt && pRender)
	{
		CAutoRefPtr<SOUI::IFont> oldFt;
		pRender->SelectObject(pFt, (IRenderObj**)&oldFt);
#endif 
		pRender->MeasureText(str, str.GetLength(), &sz);
#ifndef DEBUG
		pRender->SelectObject(oldFt);
	}
#endif 
	return sz;
}
bool PtInLine(const CPoint& pt, const CPoint& start, const CPoint& end)
{
	if (end.x == start.x)
	{

		return ((pt.y >= start.y && pt.y <= end.y) || (pt.y <= start.y && pt.y >= end.y)) && abs(pt.x - start.x) < 5;
	}
	else if (end.y == start.y)
	{
		return ((pt.x >= start.x && pt.x <= end.x) || (pt.x <= start.x && pt.x >= end.x)) && abs(pt.y - start.y) < 5;
	}
	else
	{
		double rate = (end.y - start.y) / (double)(end.x - start.x);

		if ((pt.x<start.x && pt.x<end.x) || (pt.x>start.x && pt.x>end.x))
		{
			return false;
		}

		int lineY = (int)(start.y + rate*(pt.x - start.x));

		return abs(lineY - pt.y) <= 10;
	}
}
void SpliteString(SArray<SStringW>& arr, SStringW str)
{
	str.TrimLeft(L'#');
	str.TrimRight(L'#');
	str.Trim();
	if (str.IsEmpty()) return;
	int pos = str.Find(L'#');
	while (pos != -1)
	{
		str.Trim();
		SStringW strleft = str.Left(pos);
		str = str.Mid(pos + 1);
		strleft.Trim();
		if (!strleft.IsEmpty())
			arr.Add(strleft);

		pos = str.Find(L'#');
	}
	str.Trim();
	if (!str.IsEmpty())
	{
		arr.Add(str);
	}
}
void LigherColor(COLORREF& lColor, int lScale)
{
	if (lScale > 0)
	{
		long R = MulDiv(255 - GetRValue(lColor), lScale, 255) + GetRValue(lColor);
		long G = MulDiv(255 - GetGValue(lColor), lScale, 255) + GetGValue(lColor);
		long B = MulDiv(255 - GetBValue(lColor), lScale, 255) + GetBValue(lColor);
		lColor = RGB(R, G, B);
	}
	else
	{
		lScale *= -1;
		long red = MulDiv(GetRValue(lColor), (255 - lScale), 255);
		long green = MulDiv(GetGValue(lColor), (255 - lScale), 255);
		long blue = MulDiv(GetBValue(lColor), (255 - lScale), 255);
		lColor = RGB(red, green, blue);
	}
}

//获得精确度
int GetPect(double value)
{
	if (value == 0) return 0;
	return (int)(log10(fabs(value)));
}
bool ContractEqual(const DC_ContractInfo& a, const DC_ContractInfo& b)
{
	return strcmp(a.ContractNo, b.ContractNo) == 0;
}
void DrawLine(IRenderTarget *pRender, int style, CPoint pt1, CPoint pt2, COLORREF clr, int nWid/* = 1*/)
{
	DrawLine(pRender,  style,  pt1.x,  pt1.y,  pt2.x,  pt2.y,  clr, nWid);
}
void DrawLine(IRenderTarget *pRender,int style ,int x1, int y1, int x2, int y2, COLORREF clr, int nWid /*= 1*/)
{
	CPoint pt[2] = { {x1,y1}, {x2,y2} };
	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(style, clr, nWid, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);
	pRender->DrawLines(pt, 2);
	pRender->SelectObject(oldPen);
}
void DrawPolyLine(IRenderTarget *pRender, int style, CPoint *pBuffer, int nCount, COLORREF clr, int nWid /*= 1*/)
{
	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(style, clr, nWid, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);
	pRender->DrawLines(pBuffer, nCount);
	pRender->SelectObject(oldPen);
}
void DrawKName(IRenderTarget *pRender, int x, int y, SStringW txt, COLORREF clr)
{
	COLORREF clrBorder = clr;
	LigherColor(clrBorder, -50);

	CSize sz;
	pRender->MeasureText(txt, txt.GetLength(), &sz);
	sz.cx += 4;
	sz.cy += 4;

	sz.cx = max(40, sz.cx);
	sz.cy = max(16, sz.cy);

	int hfHet = sz.cy / 2;

	POINT pts[] = { { x, y }, { x + hfHet, y - hfHet }, { x + hfHet + sz.cx, y - hfHet }, { x + hfHet + sz.cx, y + hfHet }, { x + hfHet, y + hfHet } };

	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(PS_SOLID, clrBorder, 1, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);
	CAutoRefPtr<IBrush> pBrush, oldBrush;
	pRender->CreateSolidColorBrush(clr, &pBrush);
	pRender->SelectObject(pBrush, (IRenderObj**)&oldBrush);
	pRender->SetTextColor(RGBA(0, 0, 0, 255));

	pRender->DrawLines(pts, 5);
	pRender->DrawText(txt,txt.GetLength(), &CRect(x + hfHet, y - hfHet, x + hfHet + sz.cx, y + hfHet), DT_VCENTER | DT_CENTER | DT_SINGLELINE);


	pRender->SelectObject(oldPen);
	pRender->SelectObject(oldBrush);
}

void DotLine(IRenderTarget *pRender, int x1, int y1, int x2, int y2, COLORREF clr, int nWid, int nStyle)
{
	DrawLine(pRender, nStyle, x1, y1, x2, y2, clr, nWid);
}
void DrawRect(IRenderTarget* pRender, const CRect& rc, COLORREF clr, COLORREF bdclr, int bdWid, int alpha)
{

	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(PS_SOLID, bdclr, 1, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);


	if (clr != TRANSPARENT)
	{
		CAutoRefPtr<IBrush> pBrush, oldBrush;
		pRender->CreateSolidColorBrush(clr, &pBrush);
		pRender->SelectObject(pBrush, (IRenderObj**)&oldBrush);
		pRender->FillRectangle(&rc);
		pRender->SelectObject(oldBrush);
	}

	if (clr != bdclr && bdWid > 0)
	{
		pRender->FillRectangle(&rc);
	}
	pRender->SelectObject(oldPen);
}
void DrawArc(IRenderTarget* pRender, COLORREF clr, int x, int y, int width, int height, int startAngle, int sweepAngle, int nPenWid)
{

}

void DrawPointer(IRenderTarget* pRender, int nX, int nY, COLORREF rgb, COLORREF clrBk, int nPenStype, int cx /*= 6*/, int cy /*= 6*/, BOOL bFillBk/* = true*/, int nPenThick/* = 1*/)
{
	int m_nHorizSize = cx;
	int nXMinus = nX - m_nHorizSize;
	int nXPlus = nX + m_nHorizSize;
	int nYMinus = nY - cy;
	int nYPlus = nY + cy;
	CPoint poly[8];

	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(PS_SOLID, rgb, nPenThick, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);

	CAutoRefPtr<IBrush> pBrush, pBrushBk, oldBrush;
	pRender->CreateSolidColorBrush(rgb, &pBrush);
	pRender->SelectObject(pBrush, (IRenderObj**)&oldBrush);
	pRender->CreateSolidColorBrush(rgb, &pBrushBk);

	if (bFillBk)
		oldBrush = pRender->SelectObject(pBrush);
	else
		oldBrush = pRender->SelectObject(pBrushBk);

	switch (nPenStype)
	{
		case psDownArrow:
		{
			int nXMinus1 = nX - m_nHorizSize / 3;
			int nXPlus1 = nX + m_nHorizSize / 3;
			poly[0] = CPoint(nX, nYPlus);
			poly[1] = CPoint(nXMinus, nY);
			poly[2] = CPoint(nXMinus1, nY);
			poly[3] = CPoint(nXMinus1, nYMinus);
			poly[4] = CPoint(nXPlus1, nYMinus);
			poly[5] = CPoint(nXPlus1, nY);
			poly[6] = CPoint(nXPlus, nY);
			pRender->DrawLines(poly, 7);
		}
		break;
		case psRightArrow:
		{
			poly[0] = CPoint(nX, nY - cy / 3);
			poly[1] = CPoint(nX + cx - cx / 3, nY - cy / 3);
			poly[2] = CPoint(nX + cx - cx / 3, nY - cy);
			poly[3] = CPoint(nX + cx, nY);
			poly[4] = CPoint(nX + cx - cx / 3, nY + cy);
			poly[5] = CPoint(nX + cx - cx / 3, nY + cy / 3);
			poly[6] = CPoint(nX, nY + cy / 3);
			pRender->DrawLines(poly, 7);
		}
		break;
		case psUpArrow:
		{
			int nXMinus1 = nX - m_nHorizSize / 3;
			int nXPlus1 = nX + m_nHorizSize / 3;
			poly[0] = CPoint(nX, nYMinus);
			poly[1] = CPoint(nXPlus, nY);
			poly[2] = CPoint(nXPlus1, nY);
			poly[3] = CPoint(nXPlus1, nYPlus);
			poly[4] = CPoint(nXMinus1, nYPlus);
			poly[5] = CPoint(nXMinus1, nY);
			poly[6] = CPoint(nXMinus, nY);
			pRender->DrawLines(poly, 7);
		}
		break;
		case psUpTriArrow:
		case psTriangle:
		{
			poly[0] = CPoint(nXMinus, nYPlus);
			poly[1] = CPoint(nXPlus, nYPlus);
			poly[2] = CPoint(nX, nYMinus);
			pRender->DrawLines(poly, 3);
		}
		break;
		case psDownTriArrow:
		case psDownTriangle:
		{
			poly[0] = CPoint(nXMinus, nYMinus);
			poly[1] = CPoint(nXPlus, nYMinus);
			poly[2] = CPoint(nX, nYPlus);
			pRender->DrawLines(poly, 3);
		}
		break;
		case psRightTriangle:
		{
			poly[0] = CPoint(nXMinus, nYMinus);
			poly[1] = CPoint(nXMinus, nYPlus);
			poly[2] = CPoint(nX, nY);
			pRender->DrawLines(poly, 3);
		}
		break;
		case psLeftTriangle:
		{
			poly[0] = CPoint(nXPlus, nYMinus);
			poly[1] = CPoint(nXPlus, nYPlus);
			poly[2] = CPoint(nX, nY);
			pRender->DrawLines(poly, 3);
		}
		break;
		case psRectangle:
		{
			pRender->DrawRectangle(CRect(nXMinus, nYMinus, nXPlus + 1, nYPlus + 1));
		}
		break;
		case psCircle:
		{
			pRender->DrawEllipse(CRect(nXMinus, nYMinus, nXPlus, nYPlus));
		}
		break;
		case psCross:
		{
			CPoint pt[2] = { { nX, nYMinus }, { nX, nYPlus + 1 } };
			pRender->DrawLines(pt, 2);
			CPoint pt1[2] = { { nXMinus, nY }, { nXPlus + 1, nY } };
			pRender->DrawLines(pt1, 2);
		}
		break;
		case psDiagCross:
		{
			CPoint pt[2] = { { nXMinus, nYMinus }, { nXPlus + 1, nYPlus + 1 } };
			pRender->DrawLines(pt, 2);
			CPoint pt1[2] = { { nXPlus, nYMinus }, { nXMinus - 1, nYPlus + 1 } };
			pRender->DrawLines(pt1, 2);
		}
		break;
		case psStar:
		{
			CPoint pt[2] = { { nX, nYMinus }, { nX, nYPlus + 1 } };
			pRender->DrawLines(pt, 2);
			CPoint pt1[2] = { { nXMinus, nY }, { nXPlus + 1, nY } };
			pRender->DrawLines(pt1, 2);
			CPoint pt2[2] = { { nXMinus, nYMinus }, { nXPlus + 1, nYPlus + 1 } };
			pRender->DrawLines(pt2, 2);
			CPoint pt3[2] = { { nXPlus, nYMinus }, { nXMinus - 1, nYPlus + 1 } };
			pRender->DrawLines(pt3, 2);
		}
		case psDiamond:
		{
			poly[0] = CPoint(nXMinus, nY);
			poly[1] = CPoint(nX, nYMinus);
			poly[2] = CPoint(nXPlus, nY);
			poly[3] = CPoint(nX, nYPlus);
			pRender->DrawLines(poly, 4);
		}
		break;
		case psPoint:
		{
			pRender->DrawEllipse(CRect(nX - 3, nY - 3, nX + 3, nY + 3));
		}
		break;
	}
	pRender->SelectObject(oldBrush);
	pRender->SelectObject(oldPen);
}


COLORREF GFillPath(IRenderTarget* pRender, LPPOINT pts, int nCount, COLORREF clr1, COLORREF clr2, COLORREF clr3, COLORREF clr4)
{
// 	LPPOINT lpPointRgn = new POINT[nCount];
// 	CPoint pt(0);
// 	for (int i = 0; i < nCount; i++)
// 	{
// 		pt.Offset(pts[i]);
// 		lpPointRgn[i] = pt;
// 	}
// 
// 	CRect rcClip;
// 	pRender->GetClipBox(rcClip);
// 	CRgn rgnClip;
// 	rgnClip.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
// 
// 
// 	CRect rcFill;
// 	rgn.GetRgnBox(rcFill);
// 
// 	CBitmap bmp, *pOldBmp;
// 	bmp.CreateCompatibleBitmap(pRender, rcFill.right, rcFill.bottom);
// 	CDC memDC;
// 	memDC.CreateCompatibleDC(pRender);
// 	pOldBmp = memDC.SelectObject(&bmp);
// 
// 	int y = rcFill.Height() / 2;
// 	GradientFill(&memDC, &CRect(0, 0, rcFill.Width(), y), clr1, clr2, FALSE);
// 	GradientFill(&memDC, &CRect(0, y, rcFill.Width(), rcFill.Height()), clr3, clr4, FALSE);
// 
// 	pRender->BitBlt(rcFill.left, rcFill.top, rcFill.Width(), rcFill.Height(), &memDC, 0, 0, SRCCOPY);
// 
// 	memDC.SelectObject(pOldBmp);
// 	bmp.DeleteObject();
// 	memDC.DeleteDC();
	return 0;
}
void DrawPath(IRenderTarget *pRender, LPPOINT pts, int nCount, COLORREF clr)
{
	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(PS_SOLID, clr, 1, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);

	CPoint pt(pts[0]);
	CPoint* tmpPoint = new CPoint[nCount];
	memset(tmpPoint, 0, sizeof(CPoint)*nCount);
	tmpPoint[0] = pt;
	for (int i = 1; i < nCount; i++)
	{
		pt.Offset(pts[i]);
		tmpPoint[i] = pt;
	}
	pRender->DrawLines(tmpPoint, nCount);
	pRender->SelectObject(oldPen);
}
void DrawTipRect(IRenderTarget *pRender, const CRect& rc, COLORREF clrBK, COLORREF clrBorder)
{
	POINT pts[] = { { rc.left, rc.top }, { rc.Width(), 0 }, { 0, rc.Height() - 4 }, { -rc.Width() / 2 + 4, 0 }, { -4, 4 }, { -4, -4 }, { -rc.Width() / 2 + 4, 0 }, { 0, -rc.Height() + 4 } };
	GFillPath(pRender, pts, sizeof(pts) / sizeof(POINT), clrBK, clrBK, clrBK, clrBK);

	DrawPath(pRender, pts, sizeof(pts) / sizeof(POINT), clrBorder);
}
void DrawFocusPoint(IRenderTarget *pRender, const CPoint& pt)
{
	CAutoRefPtr<IPen> pPen, oldPen;
	pRender->CreatePen(PS_GEOMETRIC | PS_ENDCAP_SQUARE, RGB(255, 255, 255), 6, &pPen);
	pRender->SelectObject(pPen, (IRenderObj**)&oldPen);
	CPoint pts[2] = { { pt.x - 1, pt.y }, { pt.x, pt.y } };
	pRender->DrawLines(pts, 2);
	pRender->SelectObject(oldPen);
}