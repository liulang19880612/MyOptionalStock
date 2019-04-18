// ChartObject.cpp : 实现文件
//

#include "StdAfx.h"
#include "SeriesObject.h"
#include "KChart.h"
#include "math.h"
#include "TimeAxis.h"

SStringW CreateTip(TradePoint& opt, int nPect)
{
	SStringW ret;
	switch (opt.Type)
	{
		case 0:
		ret += GETSTRING(L"@string/IDS_10092");
		break;
		case 1:
		ret += GETSTRING(L"@string/IDS_10093");
		break;
		case 2:
		ret += GETSTRING(L"@string/IDS_10094");
		break;
		case 3:
		ret += GETSTRING(L"@string/IDS_10095");
		break;
		default:
		return L"";
	}

	ret += GETSTRING(L"@string/IDS_10096");

	ret += S_CA2W(opt.ci.ContractNo);
	ret += L"#";


	SStringW tmp;
	tmp.Format(GETSTRING(L"@string/IDS_10097"), nPect, opt.Price);
	ret += tmp;

	tmp.Format(GETSTRING(L"@string/IDS_10098"), opt.Volume);
	ret += tmp;
	return ret;

}

//-----------------------------------------CSeriesObject--------------------------
CSeriesObject::CSeriesObject()
{
	m_bUseSysColor = false;
	m_nPenThick = 1;
	m_crPen    = RGB(255, 0, 0);
	m_pAxisHoriz = NULL;
	m_pAxisVert = new CAxis(false, NULL);
	m_bOwnAxis = false;
	m_bSelected = false;
	m_ptLastViewBox.SetPoint(-1, -1);
	m_pChart = NULL;
	m_nIcon = -1;
	m_bShowWavePrice = false;
}

CSeriesObject::~CSeriesObject()
{
	if (m_pAxisVert)
	{
		delete m_pAxisVert;
		m_pAxisVert = NULL;
	}
}

void CSeriesObject::SetMinMaxValue(double fMin, double fMax)
{
	if (m_pAxisVert)
	{
		m_pAxisVert->SetMinMaxValue(fMin, fMax);

	}
}

void CSeriesObject::SetChart(CKChart *pChart)
{
	m_pChart = pChart;
	m_pAxisVert->SetChart(pChart);
}

CAxis* CSeriesObject::GetSelectAxis()
{
	return this->m_pAxisVert;
}





//-------------------------------------------------------CandleSeries-----------------------------------

CCandleSeries::CCandleSeries(void) :
m_bHaveSetPect(false),
m_bIsBuffing(false),
m_ID(-1),
m_nLastAppendIdx(0),
m_bSubK(false),
m_bCombol(false),
m_tmStart(0),
m_fSumPrice(0),
m_fLastKPrice(0),
m_nLastKCount(0),
m_fAppendRate(1),
m_nWaveLastKCount(0),
m_nWaveLastStartIdx(0)
{

}

CCandleSeries::~CCandleSeries(void)
{

}


double CCandleSeries::GetAvgPrcie()
{
	int cnt = m_SerirsData.GetCount();
	if (cnt == 0)
		return 0;
	else
	{
		return m_fSumPrice / cnt;
	}
}
void CCandleSeries::CaluAvgPrice()
{
	//计算均价
	int curCnt = m_SerirsData.GetCount();
	if (curCnt > 0)
	{
		if (curCnt == m_nLastKCount)
		{
			DC_KData& curK = m_SerirsData.GetAt(curCnt - 1);
			m_fSumPrice -= m_fLastKPrice;
			m_fSumPrice += curK.Close;
		}
		else
		{
			m_fSumPrice = 0;
			for (int i = 0; i < curCnt; i++)
			{
				if (m_SerirsData.GetAt(i).Close == InvalidNumeric) continue;
				m_fSumPrice += m_SerirsData.GetAt(i).Close;
			}
		}
		m_fLastKPrice = m_SerirsData.GetAt(curCnt - 1).Close;
	}
	m_nLastKCount = m_SerirsData.GetCount();
}

void CCandleSeries::UpdateHisData(DC_KData* pData, int nCount, int nData)
{

	bool bRefresh = m_SerirsData.GetCount() > nCount || m_SerirsData.GetCount() == 0;
	m_SerirsData.SetData(pData, nCount, m_pAxisVert->GetPrect(), bRefresh);
	CaluAvgPrice();
}

double CCandleSeries::GetWavePercent()
{
	double dWavePercent = 60;

	return dWavePercent*0.01;
}
void CCandleSeries::CalWaveMinMaxPrice(int nMin, int nMax)
{
	static double RetracePct = 0;

	double curPct = GetWavePercent();

	nMax = min(m_SerirsData.GetCount() - 1, nMax);
	int curCnt = nMax - nMin + 1;

	if (curCnt > 1)
	{
		if (curCnt == m_nWaveLastKCount && nMin == m_nWaveLastStartIdx && curPct == RetracePct)
		{

		}
		else
		{
			int UpDn = 0;  //1 波峰 -1波谷 0初始状态
			double SwingPrice = 0;
			m_arrMaxMinPrice.RemoveAll();
			RetracePct = curPct;

			double fMin = 10000000, fMax = 0;
			GetMinMaxValue(fMin, fMax);

			double fAdd = (fMax - fMin)*RetracePct;
			int nCount = 0; //高低点计数
			for (int i = nMin; i < nMin + curCnt; i++)
			{
				if (m_SerirsData.GetAt(i).High == InvalidNumeric)
				{
					m_arrMaxMinPrice.Add(0);
					continue;
				}
				if (i < nMin + 2)
				{
					SwingPrice = m_SerirsData.GetAt(i).High;
					m_arrMaxMinPrice.Add(0);
					continue;
				}
				double RightHPrice = m_SerirsData.GetAt(i).High;
				double CurHPrice = m_SerirsData.GetAt(i - 1).High;
				double LeftHPrice = m_SerirsData.GetAt(i - 2).High;
				double RightLPrice = m_SerirsData.GetAt(i).Low;
				double CurLPrice = m_SerirsData.GetAt(i - 1).Low;
				double LeftLPrice = m_SerirsData.GetAt(i - 2).Low;
				double SwingHighPrice = Pivot(true, LeftHPrice, CurHPrice, RightHPrice);
				double SwingLowPrice = Pivot(false, LeftLPrice, CurLPrice, RightLPrice);
				bool SaveSwing = false;
				bool UpdateTL = false;
				int barState = 0; //波峰 1 波谷 -1
				if (SwingHighPrice != -1) //波峰
				{
					if (UpDn == -1 && SwingHighPrice >= SwingPrice + fAdd || UpDn == 0)
					{
						UpDn = 1;
						SaveSwing = true;
						barState = 1;
					}
					else if (UpDn == 1 && SwingHighPrice >= SwingPrice)
					{
						UpdateTL = true;
						SaveSwing = true;
					}
					if (SaveSwing)
					{
						SwingPrice = SwingHighPrice;
					}
				}
				else if (SwingLowPrice != -1)//波谷
				{
					if (UpDn == 1 && SwingLowPrice <= SwingPrice - fAdd || UpDn == 0)
					{
						UpDn = -1;
						SaveSwing = true;
						barState = -1;
					}
					else if (UpDn == -1 && SwingLowPrice <= SwingPrice)
					{
						UpdateTL = true;
						SaveSwing = true;
					}

					if (SaveSwing)
					{
						SwingPrice = SwingLowPrice;
					}
				}
				bool bValue = false;
				if (UpdateTL)
				{

					for (int t = m_arrMaxMinPrice.GetCount() - 1; t >= 0; t--)
					{
						int CurState = m_arrMaxMinPrice.GetAt(t);
						if (UpDn == 1 && CurState == 1)
						{
							m_arrMaxMinPrice[t] = 0;
							barState = 1;
							bValue = true;
							break;
						}
						else if (UpDn == -1 && CurState == -1)
						{
							m_arrMaxMinPrice[t] = 0;
							barState = -1;
							bValue = true;
							break;
						}
					}
				}
				if (barState != 0 && !bValue)
					nCount++;
				m_arrMaxMinPrice.Add(barState);
			}
			if (nCount <= 2)
				m_bShowWavePrice = false;
			else
				m_bShowWavePrice = true;
		}
	}
	m_nWaveLastKCount = nMax - nMin + 1;
	m_nWaveLastStartIdx = nMin;
}

//求转折点
double CCandleSeries::Pivot(bool bHiLo, double LeftPrice, double CurPrice, double RightPrice)
{
	double dPivotPrice = CurPrice;
	//先判断左边
	if ((bHiLo && CurPrice < LeftPrice) || (!bHiLo&&CurPrice > LeftPrice) || (bHiLo && CurPrice <= RightPrice) || (!bHiLo&&CurPrice >= RightPrice))
	{
		dPivotPrice = -1;
	}

	return dPivotPrice;
}

void  CCandleSeries::SetStock(DC_ContractInfo *pInfo, Period*pPrd, time_t tmStart)
{
	m_nLastKCount = 0;
	m_nWaveLastKCount = 0;
	m_fSumPrice = 0;
	m_fLastKPrice = 0;
	m_contract = *pInfo;
	m_prd = *pPrd;
	m_bHaveSetPect = false;
	m_tmStart = tmStart;
}

bool CCandleSeries::GetMinMaxValue(double& fMin, double& fMax)
{
	bool bRet = false;
	if (m_SerirsData.GetCount() == 0)return bRet;

	int nMin = (int)m_pAxisHoriz->GetMinValue();
	int nMax = (int)m_pAxisHoriz->GetMaxValue();
	if (nMax >= m_SerirsData.GetCount()) nMax = m_SerirsData.GetCount() - 1;
	if (nMax < nMin)
	{
		fMin = 0;
		fMax = 99;
		return false;
	}
	fMin = DBL_MAX;
	fMax = DBL_MIN;

	//如果当前横坐标区间和上次计算最大最小值时的区间一致，则只计算最后一个数据
	if (m_ptLastViewBox.x == nMin && m_ptLastViewBox.y <= nMax && !m_SerirsData.NeedReCaluMinMax())
	{
		if (m_ptLastViewBox.y != nMax)
		{
			nMax = nMax;
		}
		fMin = m_fMin;
		fMax = m_fMax;
		for (int i = m_ptLastViewBox.y; i <= nMax; i++)
		{
			if (i >= m_SerirsData.GetCount()) break;
			DC_KData aa = m_SerirsData.GetAt(i);

			if (aa.Close == InvalidNumeric)
				continue;
			bRet = true;
			fMin = min(m_SerirsData.GetAt(i).Low, fMin);
			fMax = max(m_SerirsData.GetAt(i).High, fMax);
		}
	}
	else
	{

		for (int i = nMin; i <= nMax; i++)
		{
			if (i >= m_SerirsData.GetCount()) break;
			if (m_SerirsData.GetAt(i).Close == InvalidNumeric)
				continue;
			fMin = min(m_SerirsData.GetAt(i).Low, fMin);
			fMax = max(m_SerirsData.GetAt(i).High, fMax);
			bRet = true;
		}
	}

	m_fMax = fMax;
	m_fMin = fMin;

	if (fabs(m_fMax - m_fMin) < 0.000001)
	{
		if (fabs(m_fMax) < 0.000001)
		{
			m_fMax += 10;
			m_fMin -= 10;
		}
		else
		{
			double fHet = fabs(m_fMax - m_fMin);
			double fMid = (m_fMax + m_fMin) / 2;

			m_fMax = fMid + fHet*1.2;
			m_fMin = fMid - fHet*1.2;
		}
	}

	m_ptLastViewBox.x = nMin;
	m_ptLastViewBox.y = nMax;

	return bRet;
}

double CCandleSeries::GetFirstVisibleValue()
{
	if (m_pAxisHoriz == NULL) return 0;
	int nMin = (int)m_pAxisHoriz->GetMinValue();
	if (nMin < 0)
		return 0;
	else
		return m_SerirsData.GetAt(nMin).Open;
}


int CCandleSeries::HitTest(CPoint pt)
{
	if (m_titleRc.PtInRect(pt)) return -2;
	int i = (int)m_pAxisHoriz->Pix2Value(pt.x);
	if (i<0 || i >= m_pAxisHoriz->GetDataCount()) return -1;

	int pix = m_pAxisHoriz->Value2Pix(i);

	int barWid = m_pAxisHoriz->GetCandleWidth();

	int centerX = (int)(pix + barWid*0.5);

	DC_KData pData = m_SerirsData.GetAt(i);

	int h = m_pAxisVert->Value2Pix(pData.High);
	int o = m_pAxisVert->Value2Pix(pData.Open);
	int c = m_pAxisVert->Value2Pix(pData.Close);
	int l = m_pAxisVert->Value2Pix(pData.Low);


	if (m_nKType == csPRICE)
	{
		if (pt.x>centerX)
		{
			DC_KData data1 = m_SerirsData.GetAt(i + 1);
			int c1 = m_pAxisVert->Value2Pix(data1.Close);
			if (PtInLine(pt, CPoint(centerX, c), CPoint(centerX + barWid, c1)))
				return i;
			else
				return -1;
		}
		else
		{
			DC_KData data1 = m_SerirsData.GetAt(i - 1);
			int c1 = m_pAxisVert->Value2Pix(data1.Close);
			if (PtInLine(pt, CPoint(centerX - barWid, c1), CPoint(centerX, c)))
				return i;
			else
				return -1;
		}
	}
	else
	{
		CRect rcBar((int)(centerX - barWid*0.4) - 2, o, centerX + (int)(barWid*0.4) + 2, c);
		if (m_nKType == csDot)
		{
			rcBar.SetRect(centerX - 3, c - 3, centerX + 3, c + 3);
		}
		rcBar.NormalizeRect();
		if (barWid < 2)
		{
			rcBar.top = h;
			rcBar.bottom = l;
		}
		if (rcBar.PtInRect(pt) || CRect(centerX - 2, h, centerX + 2, l).PtInRect(pt))
		{
			return i;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

void CCandleSeries::DrawGBL(IRenderTarget* pRender)
{
}

//绘制点状图
void CCandleSeries::DrawDot(IRenderTarget* pRender)
{
}
void CCandleSeries::Draw(IRenderTarget* pRender)
{
	if (m_SerirsData.GetCount() == 0)
	{
		return;
	}

	int nMin = (int)m_pAxisHoriz->GetMinValue();
	int nMax = (int)m_pAxisHoriz->GetMaxValue();

	if (nMax - nMin < 1) return;

	CalWaveMinMaxPrice(nMin, nMax);

	double pixOneK = m_pAxisHoriz->Width() / ((double)(nMax - nMin));

	if (pixOneK >= 1)
	{
		if (m_nKType == csGBL)
		{
			//绘制鬼变脸
			DrawGBL(pRender);
			return;
		}
		else if (m_nKType == csDot)
		{
			DrawDot(pRender);
			return;
		}
	}

	int nFullHalfWid = m_pAxisHoriz->GetCandleWidth() / 2;
	int nHalfBarWid = nFullHalfWid;
	if (nHalfBarWid > 2)
	{
		nHalfBarWid = (int)(nHalfBarWid*0.9);
	}
	if (nHalfBarWid == 0) nHalfBarWid = 1;
	
	COLORREF clrUp = ES_Color()->clrUp;
	COLORREF clrDown = ES_Color()->clrDown;
	COLORREF clrNormal = ES_Color()->clrTxtNormal;

	if (m_bSubK && !m_bCombol)
	{
		int nGray = (GetRValue(clrUp) + GetGValue(clrUp) + GetBValue(clrUp)) / 3;
		clrUp = RGB(nGray, nGray, nGray);

		nGray = (GetRValue(clrDown) + GetGValue(clrDown) + GetBValue(clrDown)) / 3;
		clrDown = RGB(nGray, nGray, nGray);

		nGray = (GetRValue(clrNormal) + GetGValue(clrNormal) + GetBValue(clrNormal)) / 3;
		clrNormal = RGB(nGray, nGray, nGray);
	}

	CAutoRefPtr<IBrush> brBk, brUp, brDown, oldBrush;
	pRender->CreateSolidColorBrush(ES_Color()->clrBK, &brBk);
	pRender->CreateSolidColorBrush(clrUp, &brUp);
	pRender->SelectObject(brUp, (IRenderObj**)&oldBrush);
	pRender->CreateSolidColorBrush(clrDown, &brDown);


	CAutoRefPtr<IPen> penUp, penDown, oldPen;
	pRender->CreatePen(PS_SOLID, clrUp, 1, &penUp);
	pRender->SelectObject(penUp, (IRenderObj**)&oldPen);
	pRender->CreatePen(PS_SOLID, clrDown, 1, &penDown);


	CRect rcBar;
	BOOL bUpFill = ES_Color()->bUpFill;
	//判断当前图表中是否有变色K线序列
	CSeriesData<SeriesItem> *pColorSeires = NULL;
	if (m_pChart)
	{
		pColorSeires = m_pChart->GetColorSeries();
	}
	int nLastFocusX = 0;
	int h, o, c, l;
	SArray<CPoint> arrPoint;
	int maxX = -1;
	double maxY = -10000000;
	int minX = -1;
	double minY = 10000000;
	int ptFocusX = -100;
	bool bDrawPrice = false;

	for (int i = nMin; i <= nMax; i++)
	{
		if (i >= m_SerirsData.GetCount()) break;

		DC_KData data = m_SerirsData.GetAt(i);
		if (data.Close == InvalidNumeric) continue;
		h = m_pAxisVert->Value2Pix(data.High);
		o = m_pAxisVert->Value2Pix(data.Open);
		c = m_pAxisVert->Value2Pix(data.Close);
		l = m_pAxisVert->Value2Pix(data.Low);

		int x = m_pAxisHoriz->Value2Pix(i);
		int xCenter = x + nFullHalfWid;

		if (m_nKType == csPRICE || pixOneK < 1)
		{
			//价位线
			bDrawPrice = true;
			CPoint ptCur(xCenter, c);
			arrPoint.Add(ptCur);
			if (m_bSelected && (xCenter - ptFocusX > 30 || nHalfBarWid > 10))
			{
				//DrawFocusPoint(pRender, CPoint(xCenter, c));
				ptFocusX = xCenter;
			}
			continue;
		}

		if (data.High > maxY)
		{
			maxY = data.High;
			maxX = i;
		}
		if (data.Low<minY)
		{
			minY = data.Low;
			minX = i;
		}
		if (o == c) c++;

		BOOL bUp = data.Close>data.Open;
		pRender->SelectObject(bUp ? penUp : penDown);
		if (m_nKType == csUSA)
		{
			//美国线
			if (h == l) l++;
			DrawLine(pRender, PS_SOLID, xCenter, h, xCenter, l, bUp ? clrUp : clrDown);
			DrawLine(pRender, PS_SOLID, xCenter - nHalfBarWid, o, xCenter, o, bUp ? clrUp : clrDown);
			DrawLine(pRender, PS_SOLID, xCenter, c, xCenter + nHalfBarWid, c, bUp ? clrUp : clrDown);

			if (m_bSelected && (xCenter - ptFocusX > 30 || nHalfBarWid > 10))
			{
				//DrawFocusPoint(pRender, CPoint(xCenter, o));
				ptFocusX = xCenter;
			}
		}
		else
		{
			//普通K线
			DrawLine(pRender, PS_SOLID, xCenter, h, xCenter, l, bUp ? clrUp : clrDown);
			if (bUp)
			{
				pRender->SelectObject(bUpFill ? brUp : brBk);
			}
			else
			{
				pRender->SelectObject(brDown);
			}
			pRender->FillRectangle(CRect(xCenter - nHalfBarWid, min(o, c), xCenter + nHalfBarWid, max(o, c)));
			if (m_bSelected && (xCenter - ptFocusX > 30 || nHalfBarWid > 10))
			{
				ptFocusX = xCenter;
				//DrawFocusPoint(pRender, CPoint(xCenter, max(o, c)));
			}
		}
		//绘制波段高低价
		DrawWaveMinMaxPrice(pRender, i, clrUp, clrDown);
	} //end for

	if (bDrawPrice && arrPoint.GetCount() > 0)
	{
		if (arrPoint.GetCount() == 1)
		{
			POINT tmpPt = arrPoint[0];
			arrPoint[0].x = 0;
			tmpPt.x += nFullHalfWid;
			arrPoint.Add(tmpPt);
		}
		DrawPolyLine(pRender, PS_SOLID, arrPoint.GetData(), arrPoint.GetCount(), clrNormal, m_nPenThick);
	}
}

void CCandleSeries::DrawNameLabel(IRenderTarget* pRender)
{
	int cnt = m_SerirsData.GetCount();
	if (cnt<1) return;

	int candleWid = m_pAxisHoriz->GetCandleWidth();
	int x = m_pAxisHoriz->Value2Pix(cnt - 1) + candleWid;
	int y = m_pAxisVert->Value2Pix(m_SerirsData.GetAt(cnt - 1).Close);
	
	SStringW strLabel = S_CA2W(m_contract.ContractName);

	CSize sz;
	pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
	CRect rcLabel(x + 10, y + 10, x + 10 + sz.cx + 4, y + 10 + sz.cy + 4);

	if (rcLabel.bottom>m_pChart->bottom)
	{
		DrawLine(pRender, PS_SOLID, x, y, x + 10, y - 10, RGB(255, 255, 0));
		rcLabel.MoveToY(y - 10 - rcLabel.Height());
	}
	else
	{
		DrawLine(pRender, PS_SOLID, x, y, x + 10, y + 10, RGB(255, 255, 0));
	}

	DrawRect(pRender, rcLabel, RGB(0, 0, 0), RGB(255, 255, 255), 1, 160);

	pRender->SetTextColor(RGB(255, 255, 0));
	pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_VCENTER | DT_SINGLELINE | DT_CENTER);


	for (int i = 0; i < cnt; i++)
	{
		if (m_SerirsData.GetAt(i).Close != InvalidNumeric)
		{
			double hh = m_SerirsData.GetAt(i).High;
			int y = m_pAxisVert->Value2Pix(hh);
			y -= 20;
			x = m_pAxisHoriz->Value2Pix(i) + candleWid / 2;
			DrawPointer(pRender, x, y, RGB(255, 255, 0), RGB(255, 255, 0), psCircle, 8, 8);
			DrawPointer(pRender, x, y, RGB(255, 0, 0), RGB(255, 255, 0), psCircle);

			m_rcDrager.SetRect(x - 8, y - 8, x + 8, y + 8);
			break;
		}
	}
}

void CCandleSeries::DrawScreenMinMaxPrice(IRenderTarget* pRender, int maxX, int minX, COLORREF clrNormal)
{
	if (maxX != -1 && m_ID == 0)
	{

		int x1 = m_pAxisHoriz->Value2Pix(maxX) + m_pAxisHoriz->GetCandleWidth() / 2;
		double price = m_SerirsData.GetAt(maxX).High;
		int x2 = x1 + 30;
		int y1 = m_pAxisVert->Value2Pix(price);
		int y2 = y1 - 10;

		SStringW txt = NumericToString(price, m_pAxisHoriz->GetPrect(), 1);
		CSize sz;
		pRender->MeasureText(txt,txt.GetLength(), &sz);
		CRect rcTxt(x2, y2, x2 + sz.cx + 8, y2 + sz.cy + 6);
		if (m_pChart->GetObjRect().right - (x2 + sz.cx) < 80)
		{
			x2 = x1 - 30;
			rcTxt.SetRect(x2, y2, x2 - sz.cx - 4, y2 + sz.cy);
		}
		rcTxt.NormalizeRect();
		DrawLine(pRender, PS_SOLID, x1, y1, x2, y2, clrNormal);
		DrawRect(pRender, rcTxt, ES_Color()->clrBK, clrNormal, 1, 100);
		pRender->SetTextColor(clrNormal);
		pRender->DrawText(txt, txt.GetLength(), &rcTxt, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}

	if (minX != -1 && m_ID == 0)
	{
		int x1 = m_pAxisHoriz->Value2Pix(minX) + m_pAxisHoriz->GetCandleWidth() / 2;
		double price = m_SerirsData.GetAt(minX).Low;
		int x2 = x1 + 30;
		int y1 = m_pAxisVert->Value2Pix(price);
		int y2 = y1 - 10;


		SStringW txt = NumericToString(price, m_pAxisHoriz->GetPrect(), 1);
		CSize sz;
		pRender->MeasureText(txt, txt.GetLength(), &sz); 
		CRect rcTxt(x2, y2, x2 + sz.cx + 8, y2 + sz.cy + 6);

		if (m_pChart->GetObjRect().right - (x2 + sz.cx) < 80)
		{
			x2 = x1 - 30;
			rcTxt.SetRect(x2, y2, x2 - sz.cx - 4, y2 + sz.cy);
		}
		rcTxt.NormalizeRect();
		DrawLine(pRender, PS_SOLID, x1, y1, x2, y2, clrNormal);
		DrawRect(pRender, rcTxt, ES_Color()->clrBK, clrNormal, 1, 100);
		pRender->SetTextColor(clrNormal);
		pRender->DrawText(txt, txt.GetLength(), &rcTxt, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}

//绘制波段高低价
void CCandleSeries::DrawWaveMinMaxPrice(IRenderTarget* pRender, int nCur, COLORREF clrUp, COLORREF clrDown)
{
	if (m_arrMaxMinPrice.IsEmpty())
		return;
	int nArrIdx = nCur - (int)m_pAxisHoriz->GetMinValue();
	if (nArrIdx + 1 >= (int) m_arrMaxMinPrice.GetCount())
		return;
	if (m_arrMaxMinPrice[nArrIdx + 1] == 1 && m_ID == 0)
	{
		int x1 = m_pAxisHoriz->Value2Pix(nCur) + m_pAxisHoriz->GetCandleWidth() / 2;
		double price = m_SerirsData.GetAt(nCur).High;
		int x2 = x1 + 20;
		int y1 = m_pAxisVert->Value2Pix(price);
		int y2 = y1 + 6;
		SStringW txt = NumericToString(price, m_pAxisHoriz->GetPrect(), 1);
		CSize sz;
		pRender->MeasureText(txt, txt.GetLength(), &sz);
		CRect rcTxt(x2, y2, x2 + sz.cx + 8, y2 + sz.cy + 6);
		rcTxt.NormalizeRect();
		DrawLine(pRender, PS_SOLID,  x1, y1, x2, y2, ES_Color()->clrTxtNormal);
		DrawLine(pRender, PS_SOLID,  x1, y1, x1 + 6, y1 - 2, ES_Color()->clrTxtNormal);
		DrawLine(pRender, PS_SOLID,  x1, y1, x1 + 4, y1 + 5, ES_Color()->clrTxtNormal);

		//DrawRect(pRender,rcTxt,ES_Color()->clrBK,clrUp,1,100);
		pRender->SetTextColor(ES_Color()->clrTxtNormal);
		pRender->DrawText(txt,txt.GetLength(), &rcTxt, DT_LEFT);
	}

	if (m_arrMaxMinPrice[nArrIdx + 1] == -1 && m_ID == 0)
	{
		int x1 = m_pAxisHoriz->Value2Pix(nCur) + m_pAxisHoriz->GetCandleWidth() / 2;
		double price = m_SerirsData.GetAt(nCur).Low;
		int x2 = x1 + 10;
		int y1 = m_pAxisVert->Value2Pix(price) + 1;
		int y2 = y1;
		SStringW txt = NumericToString(price, m_pAxisHoriz->GetPrect(), 1);
		CSize sz;
		pRender->MeasureText(txt, txt.GetLength(), &sz);
		CRect rcTxt(x2, y2 - sz.cy / 2 - 3, x2 + sz.cx + 8, y2 + sz.cy / 2 + 3);
		rcTxt.NormalizeRect();
		DrawLine(pRender, PS_SOLID, x1, y1, x1 + 4, y1 - 4, ES_Color()->clrTxtNormal);
		DrawLine(pRender, PS_SOLID, x1, y1, x1 + 4, y1 + 4, ES_Color()->clrTxtNormal);
		DrawLine(pRender, PS_SOLID, x1, y1, x2, y2, ES_Color()->clrTxtNormal);
		pRender->SetTextColor(ES_Color()->clrTxtNormal);
		pRender->DrawText(txt, txt.GetLength(), &rcTxt, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	}
}

//-------------------------------CIndicatorSeries-------------------------
CIndicatorSeries::CIndicatorSeries()
{
	m_pGroup = NULL;
}
void CIndicatorSeries::SetSeriesData(Series *pSeries, bool bCover)
{
	SetData(pSeries->pData, pSeries->nCount, pSeries->nPrec, bCover);
	m_strName = S_CA2W( pSeries->name);
	m_nSeriesType = pSeries->nType;
	SetOwnAxis(pSeries->bOwnAxis);
	this->m_bUseSysColor = !pSeries->bUserColor;

	switch (pSeries->clr)
	{
		case 93001:
		m_crPen = ES_Color()->clrUp;
		break;
		case 93002:
		m_crPen = ES_Color()->clrDown;
		break;
		case 93003:
		m_crPen = ES_Color()->clrBK;
		break;
		default:
		m_crPen = pSeries->clr;
		break;
	}


	m_nPenThick = pSeries->nThick < 1 ? 1 : pSeries->nThick;
}

double CIndicatorSeries::GetFirstVisibleValue()
{
	int nFirst = (int)m_pAxisHoriz->GetMinValue();
	int nLast = (int)m_pAxisHoriz->GetMaxValue();
	int minX = 0;
	int maxX = this->GetMaxIndex();

	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		double curValue = GetAt(i).fValue;
		if (curValue == InvalidNumeric) continue;
		return curValue;
	}

	return 0;
}

void CIndicatorSeries::Draw(IRenderTarget* pRender)
{
	if (m_pAxisHoriz == NULL) return;
	//坐标是否反转

	switch (this->m_nSeriesType)
	{
		case TP_INDICATOR:
		DrawIndicator(pRender); break; 	//指标线
		case TP_BAR:
		DrawBar(pRender);   break;			//柱状线
		case TP_VERTLINE:
		DrawVertLine(pRender); break;		//竖直直线
		case TP_STICKLINE:
		DrawStickLine(pRender);  break;	//竖直线段
		case TP_PARTLINE:
		DrawPartLine(pRender);   break;    //线段
		case TP_ICON:
		DrawIcon(pRender); break;		//图标
		case TP_DOT:
		DrawDot(pRender); break;			//点
		case TP_CANDLE:
		DrawCandle(pRender); break;
	}
}

//若本次调用时X坐标起始位置与本次一致，且本次范围不小于上一次，只比较新增的元素
bool CIndicatorSeries::GetMinMaxValue(double& fMin, double& fMax)
{
	if (m_pAxisHoriz == NULL) return false;


	int nFirst = (int)m_pAxisHoriz->GetMinValue();
	int nLast = (int)m_pAxisHoriz->GetMaxValue();

	if (nLast >= GetCount()) nLast = GetCount() - 1;

	int minX = 0;
	int maxX = this->GetMaxIndex();

	if (nFirst < minX) nFirst = minX;
	if (nLast > maxX)  nLast = maxX;
	bool ret = false;

	double curValue;
	if (nFirst == m_ptLastViewBox.x && nLast >= m_ptLastViewBox.y)
	{
		fMin = m_fMin;
		fMax = m_fMax;

		for (int i = m_ptLastViewBox.y; i <= nLast; i++)
		{
			if (i < minX) continue;
			if (i > maxX)
			{
				nLast = maxX;
				break;
			}
			const SeriesItem& curItem = GetAt(i);
			curValue = curItem.fValue;



			if (curValue == InvalidNumeric)
			{
				continue;
			}
			if ((int)curValue == PRICE_BTM || (int)curValue == PRICE_TOP) continue;
			fMin = min(fMin, curValue);
			fMax = max(fMax, curValue);


			if (m_nSeriesType == TP_STICKLINE)
			{
				curValue = curItem.fStickValue;
				if ((int)curValue == PRICE_BTM || (int)curValue == PRICE_TOP) continue;
				fMin = min(min(fMin, curValue), curItem.fValue);

				fMax = max(max(fMax, curValue), curItem.fValue);
			}
			else if (m_nSeriesType == TP_BAR)
			{
				curValue = curItem.fBarValue;
				if ((int)curValue == PRICE_BTM || (int)curValue == PRICE_TOP) continue;
				fMin = min(min(fMin, curValue), curItem.fValue);
				fMax = max(max(fMax, curValue), curItem.fValue);
			}
			else if (m_nSeriesType == TP_PARTLINE)
			{
				curValue = curItem.fLineValue;
				fMin = min(fMin, curValue);
				fMax = max(fMax, curValue);

			}
			else if (m_nSeriesType == TP_CANDLE)
			{
				curValue = curItem.fValue;
				fMin = min(fMin, curItem.fLow);
				fMax = max(fMax, curItem.fHigh);
			}
			ret = true;
		}
	}
	else
	{
		fMin = 100000000;
		fMax = -100000000;

		int minX = 0;
		int maxX = this->GetMaxIndex();

		for (int i = nFirst; i <= nLast; i++)
		{
			if (i < minX) continue;
			if (i > maxX) break;
			const SeriesItem& curItem = GetAt(i);
			curValue = GetAt(i).fValue;
			if (curValue == InvalidNumeric) continue;
			if ((int)curValue == PRICE_BTM || (int)curValue == PRICE_TOP) continue;
			fMin = min(fMin, curValue);
			fMax = max(fMax, curValue);

			if (m_nSeriesType == TP_STICKLINE)
			{
				curValue = curItem.fStickValue;
				if ((int)curValue == PRICE_BTM || (int)curValue == PRICE_TOP) continue;
				fMin = min(fMin, curValue);
				fMax = max(fMax, curValue);
			}
			else if (m_nSeriesType == TP_BAR)
			{
				curValue = curItem.fBarValue;
				if ((int)curValue == PRICE_BTM || (int)curValue == PRICE_TOP) continue;
				fMin = min(fMin, curValue);
				fMax = max(fMax, curValue);
			}
			ret = true;
		}
	}


	if (!ret)
	{
		m_ptLastViewBox.x = -1;
		m_ptLastViewBox.y = -1;
	}
	else
	{
		m_ptLastViewBox.x = nFirst;
		m_ptLastViewBox.y = nLast;
		m_fMin = fMin;
		m_fMax = fMax;
	}

	return ret;
}

int CIndicatorSeries::HitTestBar(CPoint pt) //指标线碰撞检测
{
	int i = (int)m_pAxisHoriz->Pix2Value(pt.x);
	int x = m_pAxisHoriz->Value2Pix(i);

	int barWid = m_pAxisHoriz->GetCandleWidth();

	int nDec = (int)(barWid*0.2);

	if (i > this->GetMaxIndex() || i < 0) return -1;

	SeriesItem item = GetAt(i);
	if (item.fValue == InvalidNumeric) return -1;
	int pixY = m_pAxisVert->Value2Pix(item.fValue);
	int pixY2 = m_pAxisVert->Value2Pix(item.fBarValue);

	CRect rcBar(x + nDec, pixY, x + barWid - 2 * nDec, pixY2);
	if (rcBar.PtInRect(pt))
	{
		return i;
	}
	else
		return -1;
}
int CIndicatorSeries::HitTestStickLine(CPoint pt) //指标线碰撞检测
{
	int i = (int)m_pAxisHoriz->Pix2Value(pt.x);
	int x = m_pAxisHoriz->Value2Pix(i);

	int barWid = m_pAxisHoriz->GetCandleWidth();


	if (i > this->GetMaxIndex() || i < 0) return -1;

	SeriesItem item = GetAt(i);
	if (item.fValue == InvalidNumeric) return -1;

	int pixY = m_pAxisVert->Value2Pix(item.fValue);
	int pixY2 = m_pAxisVert->Value2Pix(item.fStickValue);

	CRect rcBar(x + barWid / 2 - 2, pixY - 2, x + barWid / 2 + 2, pixY2);
	rcBar.NormalizeRect();
	if (rcBar.PtInRect(pt))
	{
		return i;
	}
	else
		return -1;
}

int CIndicatorSeries::HitTestDot(CPoint pt)
{
	int offset = m_nSeriesType == TP_DOT ? 4 : 8;

	int i = (int)m_pAxisHoriz->Pix2Value(pt.x);
	int x = m_pAxisHoriz->Value2Pix(i);

	int barWid = m_pAxisHoriz->GetCandleWidth();


	if (i > this->GetMaxIndex() || i < 0) return -1;

	SeriesItem item = GetAt(i);
	if (item.fValue == InvalidNumeric) return -1;

	int pixY = m_pAxisVert->Value2Pix(item.fValue);


	CRect rcBar(x + barWid / 2 - offset, pixY - offset, x + barWid / 2 + offset, pixY + offset);
	rcBar.NormalizeRect();
	if (rcBar.PtInRect(pt))
	{
		return i;
	}
	else
		return -1;
}



//指标线碰撞检测
int CIndicatorSeries::HitTestIndicator(CPoint pt)
{
	int i = (int)m_pAxisHoriz->Pix2Value(pt.x);
	int pix = m_pAxisHoriz->Value2Pix(i);

	int barWid = m_pAxisHoriz->GetCandleWidth();
	if (pt.x > pix + barWid / 2) i++;

	if (i > this->GetMaxIndex() || i < 0) return -1;

	SeriesItem item = GetAt(i);

	int idxMax = -1, idxMin = -1; //线的两个端点元素下标

	if (item.fValue == InvalidNumeric)
	{
		//无效数据，需要找到最近的有效数据位置
		int nMax = GetMaxIndex();
		for (int j = i + 1; j <= nMax; j++)
		{
			if (GetAt(j).fValue != InvalidNumeric)
			{
				idxMax = j;
				break;
			}
		}
	}
	else
	{
		idxMax = i;
	}

	//找上一个有效元素的下标
	int nMin = 0;
	for (int j = i - 1; j >= nMin; j--)
	{
		if (GetAt(j).fValue != InvalidNumeric)
		{
			idxMin = j;
			break;
		}
	}

	if (idxMin == -1 || idxMin == -1) return -1;

	int ptX = m_pAxisHoriz->Value2Pix(idxMin) + (int)(barWid*0.5);
	int ptX2 = m_pAxisHoriz->Value2Pix(idxMax) + (int)(barWid*0.5);
	int ptY = m_pAxisVert->Value2Pix(GetAt(idxMin).fValue);
	int ptY2 = m_pAxisVert->Value2Pix(GetAt(idxMax).fValue);

	if (PtInLine(pt, CPoint(ptX, ptY), CPoint(ptX2, ptY2)))
		return i;
	else
		return -1;
}
int CIndicatorSeries::HitTest(CPoint pt)
{
	if (m_nSeriesType == TP_INDICATOR)
	{
		//检测指标线
		return HitTestIndicator(pt);
	}
	else if (m_nSeriesType == TP_BAR)
	{
		return HitTestBar(pt);
	}
	else if (m_nSeriesType == TP_VERTLINE)
	{
		return -1;
	}
	else if (m_nSeriesType == TP_STICKLINE)
	{
		return HitTestStickLine(pt);
	}
	else if (m_nSeriesType == TP_DOT || m_nSeriesType == TP_ICON)
	{
		return HitTestDot(pt);
	}
	else if (m_nSeriesType == TP_PARTLINE)
	{
		return HitTestPartLine(pt);
	}

	return -1;
}
void CIndicatorSeries::DrawIndicator(IRenderTarget* pRender)		//绘制指标线序列
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int) this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();

	int linWid = m_nPenThick;
	if (m_bSelected) linWid *= 2;

	COLORREF clrLine = m_crPen;
	if (m_bUseSysColor)
	{
		clrLine = ES_Color()->clrIndicator[m_crPen % 16];
	}

	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	bool bFirst = true;

	POINT ptCur;
	SArray<POINT> arrPoints;
	int ppp = -100;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;

		double curPrice = GetAt(i).fValue;
		if (curPrice == InvalidNumeric)
		{
			if (i == nFirst)
			{
				//对于第一个就为无效数据的情况，需要向前找到原点
				for (int j = nFirst; j >= minX; j--)
				{
					curPrice = GetAt(j).fValue;
					if (curPrice != InvalidNumeric)
					{
						bFirst = false;

						ptCur.x = m_pAxisHoriz->Value2Pix(j);
						ptCur.y = m_pAxisVert->Value2Pix(curPrice);

						arrPoints.Add(ptCur);

						break;
					}
				}
			}
			else if (i == nLast)
			{
				if (bFirst) break;  //如果根本没有开始画，直接退出

				//最后一个为无效数据的情况，需要向后找到重点
				for (int j = nLast; j <= maxX; j++)
				{
					curPrice = GetAt(j).fValue;
					if (curPrice != InvalidNumeric)
					{

						ptCur.x = m_pAxisHoriz->Value2Pix(j);
						ptCur.y = m_pAxisVert->Value2Pix(curPrice);
						arrPoints.Add(ptCur);
						break;
					}
				}
				break;
			}
			continue;
		}

		ptCur.x = m_pAxisHoriz->Value2Pix(i) + nBarWid / 2;
		ptCur.y = m_pAxisVert->Value2Pix(curPrice);

		arrPoints.Add(ptCur);

		if (m_bSelected && (ptCur.x - ppp > 30 || nBarWid > 20))
		{
			//DrawFocusPoint(pRender, CPoint(ptCur.x, ptCur.y));
			ppp = ptCur.x;
		}

		bFirst = false;
	}

	if (!arrPoints.IsEmpty())
	{
		//DrawPolyLine(pRender, clrLine, linWid, arrPoints.GetData(), arrPoints.GetCount());
	}
}
void CIndicatorSeries::DrawDot(IRenderTarget* pRender)		//绘制点序列
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int)this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();
	int nBarWid = m_pAxisHoriz->GetCandleWidth();

	COLORREF clrDot = m_crPen;
	if (m_bUseSysColor)
	{
		clrDot = ES_Color()->clrIndicator[m_crPen % 16];
	}
	int ppp = -100;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		SeriesItem item = GetAt(i);

		double curPrice = item.fValue;
		if (curPrice == InvalidNumeric) continue;

		int pixX = m_pAxisHoriz->Value2Pix(i);
		int pixY = m_pAxisVert->Value2Pix(curPrice);

		//DrawPointer(pRender, pixX + nBarWid / 2, pixY, clrDot, clrDot, item.icon % 16, 4, 4, 1);

		if (m_bSelected && (pixX - ppp > 30 || nBarWid > 20))
		{
			//DrawFocusPoint(pRender, CPoint(pixX + nBarWid / 2, pixY));
			ppp = pixX;
		}
	}

}
void CIndicatorSeries::DrawStickLine(IRenderTarget* pRender)		//绘制竖线段序列
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int) this->m_pAxisHoriz->GetMinValue();
	int nLast = (int) this->m_pAxisHoriz->GetMaxValue();

	int linWid = m_nPenThick;
	if (linWid == 1 && m_bSelected) linWid = 3;

	if (nLast <= nFirst) return;

	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	SeriesItem item;
	int ppp = -100;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		item = GetAt(i);

		if (item.clrStick == 93001)
			item.clrStick = ES_Color()->clrUp;
		if (item.clrStick == 93002)
			item.clrStick = ES_Color()->clrDown;
		if (item.clrStick == 93003)
			item.clrStick = ES_Color()->clrBK;

		if (item.fValue == InvalidNumeric) continue;
		int pixX = m_pAxisHoriz->Value2Pix(i);
		int pixY = m_pAxisVert->Value2Pix(item.fValue);
		int pixY2 = m_pAxisVert->Value2Pix(item.fStickValue);

		pRender->FillSolidRect(CRect(pixX + nBarWid / 2 - linWid / 2, pixY, linWid, pixY2 - pixY), item.clrStick);

		if (m_bSelected && (pixX - ppp > 30 || nBarWid > 20))
		{
			//DrawFocusPoint(pRender, CPoint(pixX + nBarWid / 2, pixY));
			ppp = pixX;
		}
	}

}

int CIndicatorSeries::HitTestPartLine(CPoint pt)
{
	int i = (int)m_pAxisHoriz->Pix2Value(pt.x);
	int x = m_pAxisHoriz->Value2Pix(i);

	int barWid = m_pAxisHoriz->GetCandleWidth();


	if (i > this->GetMaxIndex() || i < 0) return -1;

	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int) this->m_pAxisHoriz->GetMinValue();
	int nLast = (int) this->m_pAxisHoriz->GetMaxValue();
	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	for (int j = i; j <= nLast; j++)
	{
		if (j < minX) continue;
		if (j > maxX) break;
		SeriesItem item = GetAt(j);

		if (item.fValue == InvalidNumeric || item.fLineValue == InvalidNumeric) continue;

		int ptX = m_pAxisHoriz->Value2Pix(j);
		int ptY = m_pAxisVert->Value2Pix(item.fValue);

		int ptX2 = m_pAxisHoriz->Value2Pix(item.nIdx2);
		int ptY2 = m_pAxisVert->Value2Pix(item.fLineValue);

		if (PtInLine(pt, CPoint(ptX, ptY), CPoint(ptX2, ptY2)))
			return i;
	}

	return -1;

}

void CIndicatorSeries::DrawVertLine(IRenderTarget* pRender)
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int) this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();

	if (nLast <= nFirst) return;

	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	if (nBarWid == 0) nBarWid = 1;

	int ppp = -100;
	SeriesItem item;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		item = GetAt(i);

		if (item.fValue == InvalidNumeric) continue;
		int pixX = m_pAxisHoriz->Value2Pix(i) + nBarWid / 2;
		int pixY = m_pAxisVert->top;
		int pixY2 = m_pAxisVert->bottom;

		int centerX = pixX + nBarWid / 2;

		if (item.clrBar == 93001)
			item.clrBar = ES_Color()->clrUp;
		if (item.clrBar == 93002)
			item.clrBar = ES_Color()->clrDown;
		if (item.clrBar == 93003)
			item.clrBar = ES_Color()->clrBK;

		if (m_bSelected && (pixX - ppp > 30 || nBarWid > 20))
		{
			ppp = pixX;
			//DrawFocusPoint(pRender, CPoint(pixX, (pixY + pixY2) / 2));
		}

	}
}

void CIndicatorSeries::DrawCandle(IRenderTarget* pRender)
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int)this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();

	if (nLast <= nFirst) return;

	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	if (nBarWid == 0) nBarWid = 1;
	int ppp = -100;
	SeriesItem item;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		item = GetAt(i);

		if (item.fValue == InvalidNumeric) continue;


		int pixX = m_pAxisHoriz->Value2Pix(i);

		int pixH = m_pAxisVert->Value2Pix(item.fHigh);
		int pixL = m_pAxisVert->Value2Pix(item.fLow);
		int pixO = m_pAxisVert->Value2Pix(item.fOpen);
		int pixC = m_pAxisVert->Value2Pix(item.fValue);
		if (pixH == pixL)
		{
			pixH--;
		}
		int centerX = pixX + nBarWid / 2;


		COLORREF clr = (item.fOpen >= item.fValue ? ES_Color()->clrDown : ES_Color()->clrUp);



		pRender->FillSolidRect(CRect(centerX, min(pixH, pixL), 1, abs(pixH - pixL)), clr);

		int top = min(pixO, pixC);
		int het = abs(pixO - pixC);

		bool bFilled = item.fOpen >= item.fValue;

		if (nBarWid >= 3)
		{
			if (bFilled)
			{
				pRender->FillSolidRect(CRect(centerX - (int)(nBarWid*0.4), top, (int)(nBarWid*0.8), het), clr);
			}
			else
			{
				pRender->FillSolidRect(CRect(centerX - (int)(nBarWid*0.4), top, (int)(nBarWid*0.8), het), ES_Color()->clrBK);
			}
		}
		else
		{
			if (bFilled)
			{
				pRender->FillSolidRect(CRect(pixX, top, nBarWid, het), clr);
			}
			else
			{
				pRender->FillSolidRect(CRect(pixX, top, nBarWid, het), ES_Color()->clrBK);
			}
		}

		if (m_bSelected && (pixX - ppp > 30 || nBarWid > 20))
		{
			//DrawFocusPoint(pRender, CPoint(pixX + nBarWid / 2, pixC));
			ppp = pixX;
		}

	}
}
void CIndicatorSeries::DrawBar(IRenderTarget* pRender)		//绘制柱状线序列
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int)this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();

	if (nLast <= nFirst) return;

	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	if (nBarWid == 0) nBarWid = 1;
	int ppp = -100;
	SeriesItem item;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		item = GetAt(i);

		if (item.fValue == InvalidNumeric) continue;
		int pixX = m_pAxisHoriz->Value2Pix(i);
		int pixY = m_pAxisVert->Value2Pix(item.fValue);
		int pixY2 = m_pAxisVert->Value2Pix(item.fBarValue);
		if (pixY2 == pixY)
		{
			pixY2--;
		}
		int centerX = pixX + nBarWid / 2;

		if (item.clrBar == 93001)
			item.clrBar = ES_Color()->clrUp;
		if (item.clrBar == 93002)
			item.clrBar = ES_Color()->clrDown;
		if (item.clrBar == 93003)
			item.clrBar = ES_Color()->clrBK;

		int het = abs(pixY2 - pixY);
		int top = min(pixY, pixY2);

		if (nBarWid >= 3)
		{
			if (item.bFilled)
			{
				pRender->FillSolidRect(CRect(centerX - (int)(nBarWid*0.4), top, (int)(nBarWid*0.8), het), item.clrBar);
			}
			else
			{
				//pRender->Draw3dRect(centerX - (int)(nBarWid*0.4), top, (int)(nBarWid*0.8), het, item.clrBar, item.clrBar);
			}
		}
		else
		{
			if (item.bFilled)
			{
				pRender->FillSolidRect(CRect(pixX, top, nBarWid, het), item.clrBar);
			}
			else
			{
				//pRender->Draw3dRect(pixX, top, nBarWid, het, item.clrBar, item.clrBar);
			}
		}

		if (m_bSelected && (pixX - ppp > 30 || nBarWid > 20))
		{
			//DrawFocusPoint(pRender, CPoint(pixX + nBarWid / 2, pixY));
			ppp = pixX;
		}

	}

}
void CIndicatorSeries::DrawIcon(IRenderTarget* pRender)		//绘制图标序列
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int)this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();
	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	COLORREF clrBorder = ES_Color()->clrBorder;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i < minX) continue;
		if (i > maxX) break;
		SeriesItem item = GetAt(i);

		double curPrice = item.fValue;
		if (curPrice == InvalidNumeric) continue;

		int pixX = m_pAxisHoriz->Value2Pix(i);
		pixX += nBarWid / 2 - 8;

		int pixY = m_pAxisVert->Value2Pix(curPrice) - 8;
		//m_pAxisHoriz->GetChart()->DrawIcon(pRender, pixX, pixY, item.icon);
		if (m_bSelected)
		{
			//pRender->Draw3dRect(pixX, pixY, 16, 16, clrBorder, clrBorder);
		}
	}
}
void CIndicatorSeries::DrawTradePoint(IRenderTarget* pRender)	//绘制交易指令
{

}
void CIndicatorSeries::DrawPartLine(IRenderTarget* pRender)		//绘制线段序列
{
	int minX = 0;
	int maxX = this->GetMaxIndex();
	int nFirst = (int)this->m_pAxisHoriz->GetMinValue();
	int nLast = (int)this->m_pAxisHoriz->GetMaxValue();
	int nBarWid = m_pAxisHoriz->GetCandleWidth();

	int linWid = m_nPenThick;
	if (linWid == 1 && m_bSelected) linWid = 3;


	for (int i = 0; i <= nLast; i++)
	{
		if (i > maxX) break;
		SeriesItem& item = GetAt(i);
		double curPrice = item.fValue;
		if (curPrice == InvalidNumeric || item.fLineValue == InvalidNumeric) continue;

		if (item.nIdx2 < minX) continue;

		if (item.clrLine == 93001)
			item.clrLine = ES_Color()->clrUp;
		if (item.clrLine == 93002)
			item.clrLine = ES_Color()->clrDown;
		if (item.clrLine == 93003)
			item.clrLine = ES_Color()->clrBK;


		int pixX = m_pAxisHoriz->Value2Pix(i) + nBarWid / 2;
		int pixX2 = m_pAxisHoriz->Value2Pix(item.nIdx2) + nBarWid / 2;

		int pixY = m_pAxisVert->Value2Pix(curPrice);
		int pixY2 = m_pAxisVert->Value2Pix(item.fLineValue);
		DrawLine(pRender, PS_SOLID, pixX, pixY, pixX2, pixY2, item.clrLine, item.nLinWid);

		if (m_bSelected)
		{
			DrawFocusPoint(pRender, CPoint(pixX, pixY));
			DrawFocusPoint(pRender, CPoint(pixX2, pixY2));
		}
	}
}



//--------------------------------CIndicatorGroup--------------------
CIndicatorGroup::CIndicatorGroup()
{
	m_titleRc.SetRectEmpty();
	m_nSeleSeires = -1;
	m_nIcon = 4;
	m_nTextItemCount = 0;
	m_nTextItemSize = 0;
	m_pTxtItem = NULL;

}

CIndicatorGroup::~CIndicatorGroup(void)
{
	int ncount = m_arrIndicators.GetCount();
	for (int i = 0; i < ncount; i++)
	{
		delete m_arrIndicators[i];
	}
	m_arrIndicators.RemoveAll();

	m_pTxtItem = NULL;
	//if (m_pTxtItem)
	//{
	//	delete[] m_pTxtItem;
	//	m_pTxtItem = NULL;
	//}

}

void CIndicatorGroup::Select(bool bSelect, int nSubIdx)
{
	if (bSelect)
	{
		m_bSelected = true;
		m_nSeleSeires = nSubIdx;
	}
	else
	{
		m_nSeleSeires = -1;
		m_bSelected = false;
		int ncount = m_arrIndicators.GetCount();
		for (int i = 0; i < ncount; i++)
		{
			CIndicatorSeries *pSeries = m_arrIndicators[i];
			pSeries->Select(false);
		}
	}
}


void CIndicatorGroup::Draw(IRenderTarget* pRender)
{
	int ncount = m_arrIndicators.GetCount();

	SArray<CIndicatorSeries*> arrOtherIndicator;

	for (int i = 0; i < ncount; i++)
	{
		CIndicatorSeries *pSeries = m_arrIndicators[i];
		pSeries->m_pGroup = this;

		pSeries->Select(m_bSelected && (m_nSeleSeires == i || m_nSeleSeires == -1));

		//pSeries->m_pAxisVert->CopyRect(m_pAxisVert);

		double fMin, fMax;

		//若序列为独立坐标，则需要计算该序列的垂直坐标范围
		if (!pSeries->IsOnwnAxis())
		{
			pSeries->SetMinMaxValue(m_pAxisVert->GetMinValue(), m_pAxisVert->GetMaxValue());
			//pSeries->m_pAxisVert->SetOriginValue(m_pAxisVert->GetOriginValue());
		}
		else
		{
			//独立坐标
			if (pSeries->GetMinMaxValue(fMin, fMax))
			{
				if (fabs(fMax - fMin) < 0.0001)
				{
					fMax *= 1.1;
					fMin *= 0.9;
				}
				double fZoom = ES_Color()->fAxis_Y_Zoom;
				if (fZoom<0.0001 && fZoom>-0.0001) fZoom = 0.9;

				double ll = (fMax - fMin) / fZoom;

				double fff = (ll - (fMax - fMin)) / 2;
				pSeries->SetMinMaxValue(fMin - fff, fMax + fff);
				//pSeries->m_pAxisVert->SetOriginValue(pSeries->GetFirstVisibleValue());

			}
		}

		if (pSeries->IsSelected())
		{
			//激活的指标线需要画在最前边，所以要放到末端
			arrOtherIndicator.Add(pSeries);
		}
		else if (pSeries->m_nSeriesType == TP_ICON || pSeries->m_nSeriesType == TP_DOT)
		{
			arrOtherIndicator.InsertAt(0, pSeries);
		}
		else if (pSeries->m_nSeriesType != TP_COLORK)
			pSeries->Draw(pRender);
	}

	for (size_t i = 0; i < arrOtherIndicator.GetCount(); i++)
	{
		arrOtherIndicator[i]->Draw(pRender);
	}

	DrawText(pRender);
}

void CIndicatorGroup::DrawText(IRenderTarget* pRender)
{
	int minX = 0;
	int nFirst = (int)m_pAxisHoriz->GetMinValue();
	int nLast = (int)m_pAxisHoriz->GetMaxValue();


	COLORREF clr = ES_Color()->clrTxtNormal;

	int nBarWid = m_pAxisHoriz->GetCandleWidth();
	bool bFirst = true;


	SArray<POINT> arrPoints;
	int ppp = -100;

	if (m_nTextItemCount == 0) return;

	int lastItemIdx = 0;
	for (int i = 0; i < m_nTextItemCount; i++)
	{
		if (m_pTxtItem[i].idx >= nFirst)
		{
			lastItemIdx = i;
			break;
		}

	}
	pRender->SetTextColor(ES_Color()->clrTxtNormal);


	COLORREF clrBorder = ES_Color()->clrBorder;
	COLORREF clrPanel = ES_Color()->clrBK;

	for (int i = nFirst; i <= nLast; i++)
	{


		for (int j = lastItemIdx; j < m_nTextItemCount; j++)
		{
			if (m_pTxtItem[j].idx > nLast) return;

			if (m_pTxtItem[j].idx > i)
			{
				lastItemIdx = j;
				break;
			}

			if (m_pTxtItem[j].idx == i)
			{
				lastItemIdx = j;

				double curPrice = m_pTxtItem[j].fPrice;

				if (curPrice == InvalidNumeric || m_pTxtItem[j].txt[0] == 0) continue;

				int pixX = m_pAxisHoriz->Value2Pix(i);
				pixX += nBarWid / 2;
				int pixY = m_pAxisVert->Value2Pix(curPrice);

				CSize sz;
				pRender->MeasureText(S_CA2W(m_pTxtItem[j].txt), S_CA2W(m_pTxtItem[j].txt).GetLength(),&sz);
				CRect rcLabel;
				if (sz.cx + 10 > 140)
				{
					sz.cy = ((sz.cx / 140) + 1) * sz.cy;
					sz.cx = 140;
				}
				else
				{
					sz.cx += 10;
				}


				if (pixY - 15 - sz.cy < m_pAxisVert->top + m_pAxisVert->Height() / 2)
				{
					rcLabel.SetRect(pixX + 15, pixY - 15 - sz.cy, pixX + 15 + sz.cx, pixY - 15);
					rcLabel.top -= 1;
					rcLabel.bottom += 1;
					if (rcLabel.top - 3 < m_pAxisVert->top)
					{
						rcLabel.MoveToY(m_pAxisVert->top + 3);
					}
					//DrawLine(pRender, pixX, pixY, rcLabel.left, rcLabel.bottom - 1, clrBorder);
				}
				else
				{
					rcLabel.SetRect(pixX + 15, pixY + 15, pixX + 15 + sz.cx, pixY + 15 + sz.cy);

					rcLabel.top -= 1;
					rcLabel.bottom += 1;

					if (rcLabel.bottom + 3 > m_pAxisVert->bottom)
					{
						rcLabel.MoveToY(m_pAxisVert->bottom - rcLabel.Height() - 3);
					}

					//DrawLine(pRender, pixX, pixY, rcLabel.left, rcLabel.top, clrBorder);
				}

				//DrawRect(pRender, rcLabel, clrPanel, clrBorder, 1, 230);
				rcLabel.left += 5;
				rcLabel.top++;

				pRender->DrawText(S_CA2W(m_pTxtItem[j].txt), S_CA2W(m_pTxtItem[j].txt).GetLength(),&rcLabel, DT_WORDBREAK | DT_END_ELLIPSIS);
			}
		}
		bFirst = false;
	}
}

void CIndicatorGroup::SetData(const SeriesGroup* pSereisGroup)
{
	m_bOwnAxis = pSereisGroup->bOwnAixs;
	m_strParams = S_CA2W(pSereisGroup->params);

	int oldCount = m_arrIndicators.GetCount();
	int newCount = pSereisGroup->nCount;

	if (oldCount < newCount)
	{
		int nAdd = newCount - oldCount;
		for (int i = 0; i < nAdd; i++)
		{
			CIndicatorSeries *pSeries = new CIndicatorSeries();
			pSeries->SetCopyData(false);
			pSeries->SetAxisHoriz(m_pAxisHoriz);
			m_arrIndicators.Add(pSeries);
		}
	}
	else if (oldCount > newCount)
	{
		for (int i = oldCount - 1; i >= newCount; i--)
		{
			CIndicatorSeries *pSeries = m_arrIndicators[i];
			delete pSeries;
			m_arrIndicators.RemoveAt(i);
		}
	}

	for (int i = 0; i < newCount; i++)
	{
		CIndicatorSeries *pIndicator = m_arrIndicators[i];
		Series& curSeries = pSereisGroup->m_pSeries[i];
		pIndicator->SetChart(m_pChart);
		pIndicator->SetSeriesData(&curSeries, pSereisGroup->nFirstIdx==0);

		if (curSeries.nType == TP_COLORK)
		{
			if (m_pChart)
			{
				m_pChart->SetColorSeries(pIndicator);
			}
		}
	}

	//字符串
	m_pTxtItem = pSereisGroup->pTxtItem;
	m_nTextItemCount = pSereisGroup->nTxtItemCount;
	m_nTextItemSize = pSereisGroup->nTxtItemSize;
}
bool CIndicatorGroup::GetMinMaxValue(double& fMin, double& fMax)
{
	int ncount = m_arrIndicators.GetCount();

	double firstMin, firstMax;

	fMin = 100000000;
	fMax = -100000000;
	bool ret = false;
	bool bHaveNoOwnAxis = false;

	for (int i = 0; i < ncount; i++)
	{
		CIndicatorSeries *pSeries = m_arrIndicators[i];

		double curMin, curMax;

		if (!ret)
		{
			if (pSeries->GetMinMaxValue(curMin, curMax))
			{
				firstMin = curMin;
				firstMax = curMax;
				ret = true;
			}

			if (ret && !pSeries->IsOnwnAxis())
			{
				bHaveNoOwnAxis = true;
				fMin = min(curMin, fMin);
				fMax = max(curMax, fMax);
			}

		}
		else
		{
			if (!pSeries->IsOnwnAxis() && pSeries->GetMinMaxValue(curMin, curMax))
			{
				bHaveNoOwnAxis = true;
				fMin = min(curMin, fMin);
				fMax = max(curMax, fMax);
			}
		}
		//若序列为独立坐标，则需要计算该序列的垂直坐标范围
	}
	//如果全部为独立坐标序列，取第一个做为序列组的最大最小值

	if (ret)
	{
		if (!bHaveNoOwnAxis)
		{
			fMin = firstMin;
			fMax = firstMax;
		}
		m_fMin = fMin;
		m_fMax = fMax;
	}
	return ret;
}
int CIndicatorGroup::HitTest(CPoint pt)
{
	if (m_titleRc.PtInRect(pt))
	{
		return -2;
	}

	int ret = -1;

	for (size_t i = 0; i < m_arrIndicators.GetCount(); i++)
	{
		ret = m_arrIndicators[i]->HitTest(pt);
		if (ret != -1)
		{
			return i;
		}
	}
	return -1;
}

CAxis* CIndicatorGroup::GetSelectAxis()		//返回当前激活序列的纵坐标
{
	if (m_nSeleSeires < 0) return NULL;
	return m_arrIndicators[m_nSeleSeires]->GetAxisVert();
}

void CIndicatorGroup::SetChart(CKChart *pChart)
{
	m_pChart = pChart;
	for (size_t i = 0; i < m_arrIndicators.GetCount(); i++)
	{
		m_arrIndicators[i]->SetChart(pChart);
	}
}

double CIndicatorGroup::GetFirstVisibleValue()
{
	if (m_pAxisHoriz == NULL) return 0;
	double ret = 0;
	double ret1 = 0;
	bool bHaveNotOwnAxis = false;
	for (size_t i = 0; i < m_arrIndicators.GetCount(); i++)
	{
		double tmp = m_arrIndicators[i]->GetFirstVisibleValue();

		if (m_arrIndicators[i]->IsOnwnAxis() && i == 0)
		{
			ret1 = tmp;
		}
		else
		{
			bHaveNotOwnAxis = true;
			ret = tmp;
			break;
		}
	}

	return bHaveNotOwnAxis ? ret : ret1;
}

//-------------------------------------------------------------CTradePointSeries----------------------------------
CTradePointSeries::CTradePointSeries(SStringW strSereisGroup)
{
	m_strSeriesGroup = strSereisGroup;
	pData = NULL;
	m_nFirstNotCoverIdx = 0;
	m_bAccTradePoint = false;
	m_bDrawLine = true;
}
CTradePointSeries::~CTradePointSeries()
{
	if (pData != NULL)
	{
		delete pData;
		pData = NULL;
	}
}


SStringW FormatNumeric(double fValue, int pect)
{
	SStringW strIntPart;
	SStringW strFloatPart;
	strIntPart.Format(L"%.6f", fValue);

	strFloatPart = strIntPart.Right(pect);

	strIntPart = strIntPart.Left(strIntPart.GetLength() - 7);

	strFloatPart = strFloatPart.Left(pect);
	strFloatPart.TrimRight(L'0');

	SStringW strFuhao;
	if (strIntPart.Left(1) == L"-")
	{
		strFuhao = L"-";
		strIntPart = strIntPart.Mid(1);
	}
	SStringW ret;
	while (true)
	{
		if (strIntPart.GetLength() <= 3)
		{
			ret = strIntPart + ret;
			break;
		}
		else
		{
			ret = L"," + strIntPart.Right(3) + ret;

			strIntPart = strIntPart.Left(strIntPart.GetLength() - 3);
		}
	}

	if (strFloatPart.IsEmpty())
	{
		return strFuhao + ret;
	}
	else
	{
		ret += L".";
		while (true)
		{
			if (strFloatPart.GetLength() <= 3)
			{
				ret += strFloatPart;
				break;
			}
			else
			{
				ret += strFloatPart.Left(3);
				ret += L",";
				strFloatPart = strFloatPart.Mid(3);
			}
		}
		return strFuhao + ret;
	}
}

void CTradePointSeries::Draw(IRenderTarget* pRender)
{

}

int CTradePointSeries::HitTest(CPoint pt)
{
	return 0;
}


void CTradePointSeries::CaluTradePointLine()
{
	//加入新的信号
	int arrCount = m_arrCoveredTradePoint.GetCount();

	if (arrCount > this->nCount)
	{
		//信号消失了
		m_arrCoveredTradePoint.RemoveAll();
		m_nFirstNotCoverIdx = 0;
		m_arrTradePointLine.RemoveAll();
		arrCount = 0;
	}

	for (int i = arrCount; i < this->nCount; i++)
	{
		m_arrCoveredTradePoint.Add(this->pData[i]);
	}

	arrCount = m_arrCoveredTradePoint.GetCount();
	bool bHaveVol = false;
	for (int i = m_nFirstNotCoverIdx; i < arrCount; i++)
	{
		if (m_arrCoveredTradePoint[i].Volume == 0)
		{
			if (!bHaveVol) m_nFirstNotCoverIdx = i + 1;
			continue;
		}

		if (m_arrCoveredTradePoint[i].Type == atBuyToCover || m_arrCoveredTradePoint[i].Type == atSell)
		{
			int typeOpen = m_arrCoveredTradePoint[i].Type == atBuyToCover ? atSellShort : atBuy;

			//找到一个平仓单
			TradePoint& tpCover = m_arrCoveredTradePoint[i];
			for (int j = m_nFirstNotCoverIdx; j <= i; j++)
			{
				TradePoint& tpOpen = m_arrCoveredTradePoint[j];
				if (tpOpen.Type == typeOpen && tpOpen.Volume>0)
				{
					int minVol = min(tpOpen.Volume, tpCover.Volume);
					TradePoint_Line line;
					line.nIdxOpen = j;
					line.nIdxClose = i;
					line.nType = (typeOpen == atBuy ? 0 : 1);

					if (m_arrCoveredTradePoint[i].bar != m_arrCoveredTradePoint[j].bar)
						m_arrTradePointLine.Add(line);
					tpCover.Volume -= minVol;
					tpOpen.Volume -= minVol;

					if (tpCover.Volume == 0)
						break;
				}
			}
		}

	}
}
void CTradePointSeries::DrawTradePointLine(IRenderTarget* pRender)
{

}