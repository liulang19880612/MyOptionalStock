#include "stdafx.h"
#include "Axis.h"
#include "../utils.h"
#include "AxisChart.h"
#include "ChartCtrl.h"
#include "TimeAxis.h"

CAxis::CAxis(bool bHorizonal, CAxisChart* pChart):
	m_nPrec(0),
	m_bIsDraging(false),
	m_bSub(false),
	m_fMin(0),	//最小值
	m_fMax(100),	//最大值
	m_fOrigin(50),
	m_nAxisType(AXIS_SCALE_NORMAL),
	m_bRevert(false),
	m_bHorizonal(bHorizonal),
	m_pChart(pChart),
	m_nAlign(1),
	m_fTick(1),
	m_nFitWid(30)
{
}


CAxis::~CAxis()
{
}

void CAxis::SetMinMaxValue(double fMin, double fMax)
{
	if (fMax< LDBL_MAX && fMin>-LDBL_MAX)
	{
		m_fMin = fMin;
		m_fMax = fMax;
	}
	else
	{
		m_fMin = 0;
		m_fMax = 100;
	}
}

void CAxis::Draw(IRenderTarget*pRender)
{
	if (m_nAxisType == AXIS_SCALE_NORMAL)
	{
		DrawPriceAxis(pRender);
	}
	else if (m_nAxisType == AXIS_SCALE_PERCENT)
	{
		DrawPecentAxis(pRender);
	}
	else if (m_nAxisType == AXIS_SCALE_HJ)
	{
		DrawHJFG(pRender, 1);
	}
	else if (m_nAxisType == AXIS_SCALE_HJ2)
	{
		DrawHJFG(pRender, 2);
	}
	else if (m_nAxisType == AXIS_SCALE_EQPCT)
	{
		DrawDengbi(pRender);
	}
}
SStringW Double2String(double curV)
{
	SStringW strLabel;
	if (fabs(curV) >= 10000)
	{
		SStringW strInteger;
		strInteger.Format(L"%.0f", curV);
		strLabel = FormatLongInteger(strInteger);
	}
	else if (fabs(curV) < 10)
	{
		strLabel.Format(L"%.3f", curV);
		strLabel.TrimRight(L'0');
		strLabel.TrimRight(L'.');
	}
	else if (fabs(curV) < 100)
	{
		strLabel.Format(L"%.2f", curV);
		strLabel.TrimRight(L'0');
		strLabel.TrimRight(L'.');
	}
	else
	{
		strLabel.Format(L"%.1f", curV);
		strLabel.TrimRight(L'0');
		strLabel.TrimRight(L'.');
	}
	return strLabel;
}
//////////////////////////////////////////////////////////////////////////
//绘制普通坐标
//////////////////////////////////////////////////////////////////////////
void CAxis::DrawPriceAxis(IRenderTarget*pRender)	
{
	CRect rcBorder;
	rcBorder.CopyRect(this);

	if (m_nAlign == 0) rcBorder.left--;
	if (m_nAlign == 1) rcBorder.right++;

	if (m_nAlign == 0)
		DrawLine(pRender, PS_SOLID, rcBorder.right, rcBorder.top, rcBorder.right, rcBorder.bottom, ES_Color()->clrBorder);
	else
		DrawLine(pRender, PS_SOLID, rcBorder.left, rcBorder.top, rcBorder.left, rcBorder.bottom, ES_Color()->clrBorder);

	COLORREF clrTxt = ES_Color()->clrBorder;
	if (ES_Color()->clrBK == 0)
	{
		clrTxt = RGBA(255, 50, 50, 255);
	}
	COLORREF clrLabel = pRender->SetTextColor(clrTxt);
	COLORREF clr = ES_Color()->clrBorder;

	int nPix = m_bHorizonal ? Width() : Height();
	if (nPix == 0 || m_bHorizonal) return;

	if (m_fMax < m_fMin)
	{
		double tmp = m_fMin;
		m_fMin = m_fMax;
		m_fMax = tmp;
	}

	if (m_fMax - m_fMin < m_fTick && m_pChart->IsMainChart()) return;
	if (fabs(m_fTick) < 0.00000001) m_fTick = 1;
	if (fabs(m_fMax - m_fMin) < 0.00000001) return;
	double fGridTick = m_fTick;
	int lineCount = max(3, Height() / 30);
	if (Height() < 40)
		lineCount = 1;

	if (!m_pChart->IsMainChart())
	{
		fGridTick = (m_fMax - m_fMin) / lineCount;
	}
	else
	{
		int curLineCount = 0;
		for (int i = 0;; i++)
		{
			curLineCount = (int)(max(1, (m_fMax - m_fMin) / fGridTick));
			if (curLineCount <= lineCount)
			{
				break;
			}
			fGridTick *= (i % 2 == 0 ? 2 : 5);
		}
		if (Height() / curLineCount > 120 && fGridTick / m_fTick > 4.999)
		{
			fGridTick /= 5;
		}
		else if (Height() / curLineCount > 39 && fGridTick / m_fTick > 1.999)
		{
			fGridTick /= 2;
		}
	}

	int nFirstGrid = ((int)(m_fMin / fGridTick)) - 1;
	for (double curV = nFirstGrid * fGridTick; curV < m_fMax; curV += fGridTick)
	{
		if (curV<m_fMin || curV>m_fMax) continue;
		int i = Value2Pix(curV);
		if ( i < top || i > bottom) break;
		if (m_nAlign == 0)
		{
			DrawLine(pRender, PS_SOLID, right, i, right - 6, i, clr);
			if (ES_Color()->bShowHerzAxisLine)
			{
				// 绘制网格线
				DotLine(pRender, right, i, m_pChart->GetObjRect().right, i, clr, PS_SOLID);
			}
		}
		else
		{
			DrawLine(pRender, PS_SOLID, left, i, left + 6, i, clr);
			if (ES_Color()->bShowHerzAxisLine)
			{
				// 绘制网格线
				DotLine(pRender, m_pChart->GetObjRect().left, i, left, i, clr, PS_SOLID);
			}
		}

		int nFenMu = 1;
		if (!m_pChart->IsMainChart()) nFenMu = 1;

		SStringW strLabel = NumericToString(curV, m_nPrec, nFenMu);
		if (!m_pChart->IsMainChart())
		{
			strLabel = Double2String(curV);
		}
		CSize sz; pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
		m_nFitWid = max(m_nFitWid, sz.cx + 12);
		if (m_nAlign == 0)
		{
			CRect rcLabel(left, i - 10, right - 8, i + 10);
			pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
		}
		else
		{
			CRect rcLabel(left + 8, i - 10, right, i + 10);
			pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_SINGLELINE | DT_VCENTER);
		}
	}
}

void CAxis::DrawDengbi(IRenderTarget*pRender)		//绘制等比坐标
{

}
void CAxis::DrawPecentAxis(IRenderTarget*pRender)	//百分比坐标
{

}
void CAxis::DrawHJFG(IRenderTarget*pRender, int nType)
{

}

int CAxis::Value2Pix(double fValue)
{
	if ((int)fValue == PRICE_TOP)
	{
		return top + 5;
	}
	else if ((int)fValue == PRICE_BTM)
	{
		return bottom - 5;
	}
	
	double offset = fValue - m_fMin;
	int nPixAll = m_bHorizonal ? Width() : Height();
	int nPixOffset = (int)((nPixAll / (m_fMax - m_fMin)) * offset);
	if (m_bHorizonal)
	{
		return right - nPixOffset;
	}
	else
	{
		if (m_bRevert)
		{
			return top + nPixOffset;
		}
		else
		{
			return bottom - nPixOffset;
		}
	}
}
double CAxis::Pix2Value(int pix)
{
	int nPixOffset = m_bHorizonal ? (right - pix) : (m_bRevert ? pix - top + 10 : bottom - pix - 10);
	double allPix = m_bHorizonal ? Width() : (Height() - 10);
	double offset = (nPixOffset / allPix) * (m_fMax - m_fMin);
	return m_fMin + offset;
}

void CAxis::SetPrect(int n, double fTick)
{
	m_nPrec = n;
	if (fTick > 0.0000001) m_fTick = fTick;

}
CPoint CAxis::GetCursorPos()
{
	if (m_pChart)
	{
		return m_pChart->GetCursorPos();
	}
	else
		return CPoint(0, 0);
}
void CAxis::DrawAxisTip(IRenderTarget *pRender)
{
	CPoint pt = m_pChart->GetCursorPos();
	if (m_pChart != NULL && pt.y >= top && pt.y <= bottom && !PtInRect(pt) && m_pChart->GetChartCtrl()->GetClientRect().PtInRect(pt))
	{
		CRect rcLabel(left, pt.y, right, pt.y + 16);
		if (rcLabel.bottom > bottom)
		{
			rcLabel.bottom = bottom;
			rcLabel.top = bottom - 16;
		}
		double fV = Pix2Value(pt.y);
		CString str = NumericToString(fV, m_nPrec, 1);
		if (!m_pChart->IsMainChart())
		{
			str = Double2String(fV);
		}
		DrawRect(pRender, rcLabel, ES_Color()->clrBK, ES_Color()->clrBorder, 1, 200);
		pRender->SetTextColor(ES_Color()->clrTxtNormal);
		pRender->DrawText(str, str.GetLength(), &rcLabel, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
}
//////////////////////////////////////////////////////////////////////////
// CDynaPctAxis
//////////////////////////////////////////////////////////////////////////

CDynaPctAxis::CDynaPctAxis(bool bHorizonal, CAxisChart* pChart) 
	:CAxis(bHorizonal, pChart)
{
	m_fMin = 0;
	m_fMax = 20;
	m_fOrigin = 10;
}
CDynaPctAxis::~CDynaPctAxis()
{

}
void CDynaPctAxis::Draw(IRenderTarget *pRender)
{
	if (IsRectEmpty())return;
	CRect rcBorder;
	rcBorder.CopyRect(this);

	rcBorder.top -= m_pChart->GetChartCtrl()->GetTitleRect().Height();
	rcBorder.top--;

	if (m_nAlign == 0)
		rcBorder.left--;
	rcBorder.right++;

	if (m_pChart->GetTimeAxisX()->GetDataCount() == 0) return;
	if (fabs(m_fOrigin) < 0.00001 || m_fMax <= m_fOrigin || m_fMin >= m_fOrigin) return;

	m_fTick = m_pChart->GetTimeAxisX()->GetMinTick();
	if (m_fTick < 0.0000001) m_fTick = 1;

	double fPct = m_fTick * 10;
	while (true)
	{
		if (m_fOrigin + fPct >= m_fMax &&  m_fOrigin - fPct <= m_fMin) break;
		fPct += 10 * m_fTick;
	}

	fPct /= m_fOrigin;

	// 绘制刻度
	int nGridCount = 5;
	double fFirctPrice = m_fOrigin * (1 - fPct);
	bool bShowHerzLien = ES_Color()->bShowHerzAxisLine;
	for (int i = 0; i < nGridCount * 2; i++)
	{
		double value = fFirctPrice + m_fOrigin*i*fPct / nGridCount;
		int y = Value2Pix(value);

		if (y >= bottom - 10) continue;

		if (value < m_fOrigin)
		{
			pRender->SetTextColor(ES_Color()->clrTxtDown);
		}
		else if (value == m_fOrigin)
		{
			pRender->SetTextColor(ES_Color()->clrTxtNormal);
		}
		else
		{
			pRender->SetTextColor(ES_Color()->clrTxtUp);
		}
		int nFenMu = 1;
		if (!m_pChart->IsMainChart()) nFenMu = 1;
		SStringW str = NumericToString(value, GetPrect(), nFenMu);
		
		CSize sz;
		pRender->MeasureText(str, str.GetLength(), &sz);
		m_nFitWid = max(m_nFitWid, sz.cx + 16);
		if (m_nAlign == 0)	//左
		{
			CRect rcLabel(left, y - 10, right - 5, y + 10);
			pRender->DrawText(str, str.GetLength(), &rcLabel, DT_VCENTER | DT_RIGHT);
		}
		else
		{
			CRect rcLabel(left + 5, y - 10, right, y + 10);
			double fPercent = (value - m_fOrigin) / m_fOrigin;
			str.Format(L"%.2f", fPercent * 100);
			str.TrimRight(L'0');
			str.TrimRight(L'.');
			if (str != L"")
				str += L"%";

			if (str == L"-0%" || str == L"0%")
			{
				str = L"0";
			}
			ZeroMemory(&sz, sizeof(sz));
			pRender->MeasureText(str, str.GetLength(), &sz);
			m_nFitWid = max(m_nFitWid, sz.cx + 16);
			pRender->DrawText(str, str.GetLength(), &rcLabel, DT_VCENTER | DT_LEFT);
		}
		// 绘制横分割线
		if (bShowHerzLien && m_pChart->GetYAxis() == this)
			DrawLine(pRender,PS_SOLID , m_pChart->GetObjRect().left, y, m_pChart->GetObjRect().right, y, ES_Color()->clrBorder);
	}
}