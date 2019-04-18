#include "stdafx.h"
#include "TimeAxis.h"
#include "ChartCtrl.h"
#include "AxisChart.h"
#include "../utils.h"

CString Week2String(int i)
{
	CString strWeek[] = { GETSTRING(L"@string/IDS_10001"), GETSTRING(L"@string/IDS_10015"), GETSTRING(L"@string/IDS_10003"),
		GETSTRING(L"@string/IDS_10004"), GETSTRING(L"@string/IDS_10005"), GETSTRING(L"@string/IDS_10006"), GETSTRING(L"@string/IDS_10007") };
	return strWeek[(i - 1) % 7];
}

CTimeAxis::CTimeAxis(bool bHorizonal, CAxisChart *pChart) 
	:CAxis(bHorizonal,pChart)
{
	m_nFocusIndex = -1;
	m_fMin = -1;	//最小值
	m_fMax = -1;	//最大值
	m_pStkHisData = NULL;
	m_Period.kind = DAY_DATA;
	m_Period.multi = 1;
	m_nAxisType = AXIS_SCALE_NORMAL;
	m_bFocus = false;
	m_nOldCount = 0;
	m_rcScrollBar.SetRectEmpty();
	m_nScreenCandleNum = 120;
}

int CTimeAxis::GetCandleWidth()
{
	int wid = 0;
	int actNum = (int)(m_fMax - m_fMin + 1);	//实际蜡烛个数
	int num = actNum;
	if (actNum < MIN_CANDLE_NUM)
		num = MIN_CANDLE_NUM;
	wid = Width() / num;
	if (wid == 0) return 1;

	return wid;
}

int CTimeAxis::GetDataCount()
{
	if (m_pStkHisData == NULL)
		return 0;
	return m_pStkHisData->GetCount();
}
int CTimeAxis::Value2Pix(double fValue)
{
	if (m_pStkHisData == NULL || m_pStkHisData->GetCount() == 0) return 0;

	int actNum = (int)(m_fMax - m_fMin + 1);

	double candleNum = actNum;

	if (candleNum < MIN_CANDLE_NUM)
		candleNum = MIN_CANDLE_NUM;

	double barWid = Width() / candleNum;

	int offset = (int)(fValue - m_fMin);


	return (int)(left + offset*barWid);

}
double CTimeAxis::Pix2Value(int pix)
{
	int offset = 0;
	if (GetDataCount() + FREE_CANDLE_COUNT < MIN_CANDLE_NUM)
	{
		offset = (pix - left)* MIN_CANDLE_NUM / Width();
	}
	else
	{
		offset = (int)((m_fMax - m_fMin + 1) * (pix - left) / Width());
	}
	int ret = (int)(m_fMin + offset);

	return ret;
}
void CTimeAxis::SetFocusIndex(int nIdx)
{
	m_nFocusIndex = nIdx;
}
void CTimeAxis::Draw(IRenderTarget *pRender)
{
	CRect rcBorder;
	rcBorder.CopyRect(this);
	rcBorder.right++;
	rcBorder.left--;
	pRender->FillSolidRect(*this, ES_Color()->clrBK);
	//pRender->Draw3dRect(&rcBorder, ES_Color()->clrBorder, ES_Color()->clrBorder);

	if (m_pStkHisData == NULL || m_pStkHisData->GetCount() == 0) return;

	UpdateHisData();

	//时间轴中的数据为从右至左排列。故 nFirst为最右侧
	int nFirst = (int)m_fMin;
	int nLast = (int)m_fMax;


	CTime curTm = m_pStkHisData->GetAt(nFirst).time;
	CTime lastTm(1980, 1, 1, 0, 0, 0);
	CTime nextTm = m_pStkHisData->GetAt(nFirst + 1).time;
	
	CString strLabel;
	COLORREF clr = ES_Color()->clrBorder;
	COLORREF clrTxt = clr;
	if (ES_Color()->clrBK == 0)
	{
		clrTxt = RGBA(255, 50, 50,255);
	}

	COLORREF clrOldBk = pRender->SetTextColor(clrTxt);
	int nBarWid = this->GetCandleWidth();

	CRect lastRc(-50, 0, -50, 0);
	BOOL lastIsTime = FALSE;
	int nLastLineIdx = 0;
	for (int i = nFirst; i <= nLast; i++)
	{
		if (i >= m_pStkHisData->GetCount()) break;
		if (m_pStkHisData->GetAt(i).time < 0) continue;
		curTm = m_pStkHisData->GetAt(i).time;
		nextTm = m_pStkHisData->GetAt(i + 1).time;

		if (i == m_fMin)
		{
			bool bBefor = true;
			strLabel = JudgeShowLabel(curTm, lastTm, nextTm, bBefor);
			CSize sz; pRender->MeasureText(strLabel,strLabel.GetLength(),&sz);
			CRect rcLabel(left + 4, top, left + 4 + sz.cx, bottom);
			pRender->DrawText(strLabel, strLabel.GetLength(),&rcLabel, DT_SINGLELINE | DT_VCENTER);
			lastRc = rcLabel;
			lastTm = curTm;
			continue;
		}

		bool bBefor = true;
		strLabel = JudgeShowLabel(curTm, lastTm, nextTm, bBefor);
		if (!strLabel.IsEmpty() && nLastLineIdx != i - 1)
		{
			nLastLineIdx = i;
			//绘制刻度线
			int pixX = Value2Pix(i); 
			if (!bBefor)
			{
				pixX += nBarWid;
			}
			if (strLabel != L" ")
			{
				//绘制文字
				CSize sz; pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
				CRect rcLabel(pixX + 4, top, pixX + 4 + sz.cx, bottom);
				BOOL curIsTime = (strLabel.Find(L':') != -1);

				if (rcLabel.left > 64)
				{
					if (rcLabel.left - lastRc.right > 5)
					{
						pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_SINGLELINE | DT_VCENTER);
						lastRc = rcLabel;
					}
					else if (!curIsTime && lastIsTime)
					{
						pRender->FillSolidRect(CRect(lastRc.left, lastRc.top + 1, lastRc.Width(), lastRc.Height() - 2), ES_Color()->clrBK);
						pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_SINGLELINE | DT_VCENTER);
						lastRc = rcLabel;
					}
				}
				lastIsTime = curIsTime;
				//pRender->Draw3dRect(pixX, top, 1, bottom - top, clr, clr);
			}
			else
			{
				//pRender->Draw3dRect(pixX, top, 1, 4, clr, clr);
			}

			if (ES_Color()->bShowVertAxisLine)
			{
				DotLine(pRender, pixX, m_pChart->GetRectObj().top, pixX, top, clr);
			}
		}
		lastTm = curTm;
	}

	//计算滚动条位置和大小
	m_rcScrollBar.SetRectEmpty();
	if (fabs(m_fMax - m_fMin - GetDuration()) > 0.999)
	{
		CRect rcScrollBar;
		rcScrollBar.top = top + 1;
		rcScrollBar.bottom = bottom;

		//计算滚动条的宽度
		int wid = (int)(Width()* (m_fMax + 1 - m_fMin) / (GetDuration() + 1));
		if (wid < 50) wid = 50;

		//计算滚动条坐标
		rcScrollBar.left = (long)(left + (Width() - wid) * m_fMin / (GetDuration() - (m_fMax - m_fMin)));
		rcScrollBar.right = rcScrollBar.left + wid;
		m_rcScrollBar = rcScrollBar;
	}
}

time_t CTimeAxis::GetTimeDiff()
{
	if (m_pStkHisData == NULL) return 0;

	int firstTime = (int)m_pStkHisData->GetAt((unsigned int)m_fMax).time;
	int lastTime = (int)m_pStkHisData->GetAt((unsigned int)m_fMin).time;

	return abs(lastTime - firstTime);
}
//判断是否绘制坐标，返回值:空串（不绘制），空格（仅绘制坐标线），文字（坐标线和文字都绘制)
SStringW CTimeAxis::JudgeShowLabel(const CTime& curTm, const CTime& lastTm, const CTime& nxtTime, bool& bBefor)
{
	SStringW tmp;

	double barPix = Width() / (m_fMax - m_fMin);

	if (lastTm == CTime(1980, 1, 1, 0, 0, 0))
	{
		return curTm.Format(L"%Y");
	}
	else if (curTm.GetYear() != lastTm.GetYear())
	{
		return curTm.Format(L"%Y");
	}
	else if (curTm.GetMonth() != lastTm.GetMonth())
	{
		if (m_Period.kind < WEEK_DATA)
		{
			if (!(m_Period.kind == DAY_DATA && barPix < 2))
			{
				tmp.Format(GETSTRING(L"@string/IDS_10008"), curTm.GetMonth());
			}
		}
		else if (m_Period.kind == WEEK_DATA)
		{
			if (barPix > 10 || curTm.GetMonth() % 3 == 1)
				tmp.Format(L"%d", curTm.GetMonth());
		}
		else
			tmp = L" ";
		return tmp;
	}
	else if ((m_Period.kind < MIN_DATA) && barPix>2)
	{
		if (curTm.GetDay() != lastTm.GetDay())
		{
			tmp.Format(L"%d", curTm.GetDay());
			return tmp;
		}
		else if (curTm.GetHour() != lastTm.GetHour() && curTm.GetDay() == nxtTime.GetDay())
		{
			tmp.Format(L"%d:00", curTm.GetHour());
			bBefor = false;
			return tmp;
		}
	}
	else if ((m_Period.kind == MIN_DATA && m_Period.multi < 5) && barPix > 4)
	{

		if (curTm.GetDay() != lastTm.GetDay())
		{
			tmp.Format(L"%d", curTm.GetDay());
			return tmp;
		}
		else if (curTm.GetHour() != lastTm.GetHour() && curTm.GetDay() == nxtTime.GetDay())
		{
			//每小时占像素超过100pix，显示时间数
			float fday = GetTimeDiff() / (3600.0f * 24);
			int width = Width();
			float fDayPix = Width() / fday;
			if (fDayPix > 400)
			{
				tmp.Format(L"%d:00", curTm.GetHour());
				bBefor = false;
				return tmp;
			}
			else
			{
				return L" ";
			}
		}
	}
	else if (m_Period.kind < DAY_DATA)
	{
		if (curTm.GetDay() != lastTm.GetDay())
		{
			tmp.Format(L"%d", curTm.GetDay());
			return tmp;
		}
	}
	return L"";
}
void CTimeAxis::SetContract(const DC_ContractInfo* ci)
{
	if (!ci) return;
	m_contract = *ci;
	SetPrect(ci->pect, ci->TickPrice);
}
double CTimeAxis::GetMinValue()
{
	if (m_pStkHisData == NULL) return -1;
	return m_fMin;
}
double CTimeAxis::GetMaxValue()
{
	if (m_pStkHisData == NULL) return -1;
	return m_fMax;
}
void CTimeAxis::SetHisData(CSeriesData<DC_KData> *pData)
{
	m_pStkHisData = pData;
}
void CTimeAxis::UpdateHisData()
{
	if (m_pStkHisData == NULL)
		return;

	int curCount = m_pStkHisData->GetCount();
	if (curCount == 0)
		return;

	if (m_nOldCount == 0 || m_nOldCount > curCount || (m_fMax - m_fMin < FREE_CANDLE_COUNT))
	{
		int nFrees = GetFreeCandleCount();

		m_nScreenCandleNum = static_cast<int>(m_pChart->GetChartCtrl()->GetCandlePix());


		m_nScreenCandleNum = max(m_nScreenCandleNum, MIN_CANDLE_NUM);

		int nMax = curCount - 1 + GetFreeCandleCount();
		int nMin = (nMax + 1) - m_nScreenCandleNum;
		if (nMin < 0)
		{
			nMin = 0;
			m_nScreenCandleNum = nMax - nMin + 1;
		}
		SetMinMaxValue(nMin, nMax);

	}
	else if (m_nOldCount < curCount)
	{
		int nMin = (int)GetMinValue();
		int nMax = (int)GetMaxValue();

		int addCount = curCount - m_nOldCount;

		if (nMin == 0 && curCount>0)
		{
			int nPix = Width() / curCount;
			if (nPix > 10)
			{
				SetMinMaxValue(nMin, curCount - 1 + FREE_CANDLE_COUNT);
			}
			else
			{
				if (nMax >= m_nOldCount - 1)
					SetMinMaxValue(nMin + addCount, nMax + addCount);
			}
		}
		else
		{
			//向右平移增加的蜡烛个数
			if (nMax >= m_nOldCount - 1)
				SetMinMaxValue(nMin + addCount, nMax + addCount);
		}

	}

	m_nOldCount = curCount;

	if (m_Period.kind != 0)
	{
		m_nDataCount = curCount;
	}
}
CSeriesData<DC_KData>* CTimeAxis::GetHisData()
{
	return m_pStkHisData;
}
double CTimeAxis::GetDuration()
{
	if (m_pStkHisData == NULL || m_pStkHisData->GetCount() < 1) return GetFreeCandleCount() - 1;

	return m_pStkHisData->GetCount() + GetFreeCandleCount() - 1;
}
int CTimeAxis::GetXValueByTime(long time)
{
	if (m_pStkHisData == NULL)
		return 0;
	//二分查找
	int low = 0, high = GetDataCount() - 1, mid;
	while (high >= low)
	{
		mid = low + ((high - low) >> 1);//防止溢出  
		if (m_pStkHisData->GetAt(mid).time >= time)
		{
			//查找出第一个大于等于该时间的数值
			if (mid < 1 || m_pStkHisData->GetAt(mid - 1).time < time)
				return mid;
			else
				high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}
	return 0;
}
void CTimeAxis::DrawAxisTip(IRenderTarget *pRender)
{
	CRect rcScrollBar = m_rcScrollBar;
	int maskHet = rcScrollBar.Height() / 2;

	if (!m_rcScrollBar.IsRectEmpty())
	{
		//绘制滚动条
		rcScrollBar.top += 2;
		rcScrollBar.bottom -= 3;

		COLORREF clrScrol = ES_Color()->clrBorder;
		DrawRect(pRender, rcScrollBar, clrScrol, 0, 0, m_bFocus ? 220 : 100);
		if (m_bFocus)
		{
			CRect rcPosTip(m_rcScrollBar);
			rcPosTip.MoveToY(m_rcScrollBar.top - m_rcScrollBar.Height() - 10);
			rcPosTip.bottom = m_rcScrollBar.top - 2;
			CString strPosTip;
			strPosTip.Format(_T("%.0f:%.0f %d"), m_fMin, min(m_fMax, (m_pStkHisData->GetCount() - 1)), m_pStkHisData->GetCount());
			int xMid = rcPosTip.CenterPoint().x;
			CSize sz; pRender->MeasureText(strPosTip, strPosTip.GetLength(), &sz);
			rcPosTip.left = max(0, xMid - sz.cx / 2 - 6);
			rcPosTip.right = rcPosTip.left + sz.cx + 12;

			DrawTipRect(pRender, rcPosTip, ES_Color()->clrBK, ES_Color()->clrBorder);

			rcPosTip.bottom -= 4;
			pRender->SetTextColor(ES_Color()->clrTxtNormal);
			pRender->DrawText(strPosTip, strPosTip.GetLength(), &rcPosTip, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			CTime tmFirstBar = m_pStkHisData->GetAt(m_fMin).time;
			CString strTmFirstBar;

			strTmFirstBar = tmFirstBar.Format(L"%Y-%m-%d");

			memset(&sz, 0, sizeof(sz));
			pRender->MeasureText(strTmFirstBar, strTmFirstBar.GetLength(), &sz);
			CRect rcRectFirstBar;
			rcRectFirstBar.SetRect(left, top, left + sz.cx + 10, bottom);
			DrawRect(pRender, rcRectFirstBar, ES_Color()->clrBK, ES_Color()->clrBorder, 1, 200);
			pRender->SetTextColor(ES_Color()->clrTxtNormal);
			pRender->DrawText(strTmFirstBar, strTmFirstBar.GetLength(), &rcRectFirstBar, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
	}

	CPoint pt = m_pChart->GetCursorPos();
	if (m_pChart != NULL && !PtInRect(pt) && m_pChart->GetChartCtrl()->GetClientRect().PtInRect(pt))
	{
		//绘制时间标签
		if (pt.x > left && pt.x <= right)
		{
			if (m_pStkHisData == NULL || m_pStkHisData->GetCount() == 0) return;
			int idx = (int)this->Pix2Value(pt.x);

			if (idx >= m_pStkHisData->GetCount()) return;
			DC_KData data = m_pStkHisData->GetAt(idx);
			DC_KData *pData = &data;
			if (pData->time < 0)return;

			CTime time = pData->time;

			//若周期小于日线，显示时间，否则显示日期
			CString str;
			if (m_Period.kind < DAY_DATA)
			{
				str.Format(L"%d/%02d/%02d %02d:%02d:%02d", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			}
			else  if (m_Period.kind == DAY_DATA)
			{
				str.Format(L"%d/%02d/%02d %s", time.GetYear(), time.GetMonth(), time.GetDay(), Week2String(time.GetDayOfWeek()));
				str = str.Mid(2);
			}
			else
			{
				str.Format(L"%d/%02d/%02d", time.GetYear(), time.GetMonth(), time.GetDay());
				str = str.Mid(2);
			}
			CSize sz; pRender->MeasureText(str, str.GetLength(), &sz);
			int wid = sz.cx;
			wid += 12;
			CRect rcLabel(pt.x, top, pt.x + wid, bottom - 1);
			if (rcLabel.right > right)
			{
				rcLabel.left = right - wid;
				rcLabel.right = right;
			}

			DrawRect(pRender, rcLabel, ES_Color()->clrBK, ES_Color()->clrBorder, 1, 200);
			pRender->SetTextColor(ES_Color()->clrTxtNormal);
			pRender->DrawText(str, str.GetLength(), &rcLabel, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//  CDynaTimeAxis
//////////////////////////////////////////////////////////////////////////
CDynaTimeAxis::CDynaTimeAxis(bool bHorizonal, CAxisChart *pChart) 
	: CTimeAxis(bHorizonal,pChart)
{

}

double CDynaTimeAxis::GetMaxValue()
{
	return 1.0;
}
void CDynaTimeAxis::Draw(IRenderTarget *pRender)
{
	CRect rcBody;
	rcBody.CopyRect(this);
	rcBody.right++;
	pRender->FillSolidRect(*this, ES_Color()->clrBK);

	if (m_pStkHisData == NULL) return;
	int cnt = m_pStkHisData->GetCount();
	if (cnt == 0) return;

	COLORREF clr = ES_Color()->clrBorder;
	COLORREF clrOldBk = pRender->SetTextColor(clr);

	CTime lastTm;
	CRect rcLabel;
	rcLabel.CopyRect(this);
	rcLabel.right++;

	rcLabel.MoveToY(rcLabel.top);
	rcLabel.left++;
	rcLabel.right--;

	int lastRight = rcLabel.left;
	CSize sz;
	pRender->MeasureText(L"00:00", _tcslen(L"00:00"), &sz);
	int labelWid = sz.cx;
	for (int i = 0; i < cnt; i++)
	{
		const DC_KData& data = m_pStkHisData->GetAt(i);
		CTime curTime(data.time);
		if (i == 0)
		{
			SStringW strLabel = curTime.Format(L"%H:%M");
			ZeroMemory(&sz, sizeof(CSize));
			pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
			int txtWid = sz.cx;
			CRect tmRc(rcLabel);
			tmRc.left += 2;

			pRender->DrawText(strLabel, strLabel.GetLength(), &tmRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			lastRight = rcLabel.left + txtWid;
		}
		else if (i == cnt - 1)
		{
			SStringW strLabel = curTime.Format(L"%H:%M");
			ZeroMemory(&sz, sizeof(CSize));
			pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
			rcLabel.left = rcLabel.right - sz.cx;
			pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		else
		{
			const DC_KData& dataNxt = m_pStkHisData->GetAt(i + 1);
			CTime tmNext(dataNxt.time - 60);
			long dStr = data.TradeDate;
			long dStr2 = dataNxt.TradeDate;

			if (data.TradeDate != dataNxt.TradeDate)
			{
				//交易日发生切换
				//时间发生间隔
				SStringW strLabel = curTime.Format(L"%H:%M");
				strLabel += L" ";
				strLabel += tmNext.Format(L"%H:%M");

				int x = Value2Pix(i);
				ZeroMemory(&sz, sizeof(CSize));
				sz = pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
				rcLabel.left = x - sz.cx / 2;
				int txtWid = sz.cx;

				if (rcLabel.left - lastRight > 10 && rcLabel.left + txtWid < right - labelWid - 5)
				{
					pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					lastRight = rcLabel.left + txtWid;
				}
				CRect rcBody = GetChart()->GetRectObj();
				//DrawLine(pRender, x, rcBody.top, x, bottom, ES_Color()->clrBorder);
			}
			else if (tmNext.GetTime() - curTime.GetTime() > 60 && (tmNext.GetTime() - curTime.GetTime()) % (24 * 3600) > 60)
			{
				//时间发生间隔
				SStringW strLabel = curTime.Format(L"%H:%M");
				strLabel += L"/";
				tmNext += 60;
				strLabel += tmNext.Format(L"%H:%M");

				int x = Value2Pix(i);
				ZeroMemory(&sz, sizeof(CSize));
				pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
				rcLabel.left = x - sz.cx / 2;
				int txtWid = sz.cx;

				if (rcLabel.left - lastRight > 10 && rcLabel.left + txtWid < right - labelWid - 5)
				{
					CRect rcBody = GetChart()->GetRectObj();
					//DotLine(pRender, x, rcBody.top, x, rcLabel.top, ES_Color()->clrBorder);
					pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					lastRight = rcLabel.left + txtWid;
				}
			}
			else if (curTime.GetMinute() == 0)
			{
				SStringW strLabel = curTime.Format(L"%H:%M");
				int x = Value2Pix(i);	
				ZeroMemory(&sz, sizeof(CSize));
				pRender->MeasureText(strLabel, strLabel.GetLength(), &sz);
				rcLabel.left = x - sz.cx / 2;
				int txtWid = sz.cx;

				if (rcLabel.left - lastRight > 10 && rcLabel.left + txtWid < right - labelWid - 5)
				{
					//DotLine(pRender, x, rcBody.top, x, rcLabel.top, ES_Color()->clrBorder);
					CRect rcBody = GetChart()->GetRectObj();
					pRender->DrawText(strLabel, strLabel.GetLength(), &rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					lastRight = rcLabel.left + txtWid;
				}
			}
		}
	}
}

void CDynaTimeAxis::DrawAxisTip(IRenderTarget *pRender)
{
	CPoint pt = m_pChart->GetCursorPos();
	if (m_pChart != NULL && !this->PtInRect(pt) && m_pChart->GetChartCtrl()->GetClientRect().PtInRect(pt))
	{
		//绘制时间标签
		if (pt.x > left && pt.x <= right)
		{
			if (m_pStkHisData == NULL || m_pStkHisData->GetCount() == 0) return;
			int idx = (int)this->Pix2Value(pt.x);

			if (idx >= m_pStkHisData->GetCount()) return;
			DC_KData data = m_pStkHisData->GetAt(idx);
			DC_KData *pData = &data;
			CTime time = pData->time;
			//若周期小于日线，显示时间，否则显示日期
			SStringW str;
			str.Format(L"%d/%02d/%02d %02d:%02d", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());

			CSize sz; pRender->MeasureText(str, str.GetLength(), &sz);
			int wid = sz.cx;
			wid += 12;
			CRect rcLabel(pt.x, top, pt.x + wid, bottom - 1);
			if (rcLabel.right > right)
			{
				rcLabel.left = right - wid;
				rcLabel.right = right;
			}

			DrawRect(pRender, rcLabel, ES_Color()->clrBK, ES_Color()->clrBorder, 1, 200);
			pRender->SetTextColor(ES_Color()->clrTxtNormal);
			pRender->DrawText(str, str.GetLength(),  &rcLabel, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
}
