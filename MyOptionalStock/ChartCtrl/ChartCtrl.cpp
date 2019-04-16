#include "stdafx.h"
#include "ChartCtrl.h"
#include "ChartTitleCtrl.h"
#include "Axis.h"
#include "RectChart.h"
#include "CandleChart.h"
#include "DynaChart.h"
#include "TimeAxis.h"
#include "SeriesObject.h"

void IninPeriodWords(SArray<SStringW>& strPeriodWords)
{
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10100"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10101"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10102"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10103"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10104"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10105"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10106"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10107"));
	strPeriodWords.Add(GETSTRING(L"@string/IDS_10108"));
}

CChartCtrl::CChartCtrl() 
	:m_bShowAxis(true),
	m_pMemRender(NULL),
	m_DefYAxisWidth(30),
	m_nXAxisHeight(20),
	m_bShowMouseLine(false),
	m_bIsDraging(false),
	m_bMouseDown(false),
	m_bIsBuffing(false),
	m_bDataComplete(false),
	m_fCandlePix(120.0),
	m_curSpliter(-1),
	m_nMaxPointNum(1000),
	m_clrCursorLine(RGBA(255,255,255,255)),
	m_bMouseIn(false)
{
	m_bFocusable = TRUE;

}


CChartCtrl::~CChartCtrl()
{
	delete m_pAxisX;
}
void CChartCtrl::Reset()
{
	m_bIsDraging = false;
	m_bMouseDown = false;
	m_bIsDbClick = false;
	m_bIsBuffing = false;
	m_nAnimateStep = 0;
	KillTimer(32112);
	KillTimer(11);
}
int CChartCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	IninPeriodWords(m_strPeriodWords);
	m_bDesytoryed = false;
	return 0;
}
void CChartCtrl::SetContractInfo(const DC_ContractInfo*pInfo, const Period*pPrd)
{
	if (!ContractEqual(m_ContractInfo, *pInfo))
	{
		m_Period = *pPrd;
		m_ContractInfo = *pInfo;
		// 创建主图
		CKChart *pChart = NULL;
		if (m_Period.kind == DYNA_DATA)
		{
			pChart = new CDynaChart();
		}
		else
		{
			pChart = new CKChart();
		}
		pChart->SetChartCtrl(this);
		pChart->SetRateSize(1);
		pChart->SetChartIndex(0);

		//创建横坐标
		if (m_pAxisX == NULL)
		{
			if (m_Period.kind == DYNA_DATA)
			{
				m_pAxisX = new CDynaTimeAxis(true, pChart);
			}
			else
			{
				m_pAxisX = new CTimeAxis(true, pChart);
			}
			m_pAxisX->CopyRect(GetClientRect());
			m_pAxisX->m_Period = m_Period;
		}

		pChart->SetTimeAxisX(m_pAxisX);
		pChart->SetMainChart();
		pChart->CreateAxis();

		m_arrChart.Add(pChart);
		m_nCurChart = 0;

		m_pMainCandleSeries = NULL;
		m_curKIdx = 0;
	}

	if (!m_bDesytoryed && m_arrChart.GetCount() > (size_t)0)
	{
	
		if (m_Period.kind == DYNA_DATA)
		{
			CDynaChart *pChart = dynamic_cast<CDynaChart*>(m_arrChart[0]);
			pChart->SetStock(&m_ContractInfo);
			m_pAxisX->SetContract(&m_ContractInfo);
		}
		else
		{
			AddK(&m_ContractInfo, &m_Period, 0);
		}
	}
}
void CChartCtrl::OnPaint(IRenderTarget *pRT)
{
	if ( !m_pMemRender)
	{
		m_pMemRender = pRT;
	}
	if ( m_pSkinBackGround)
	{
		m_pSkinBackGround->Draw(pRT, GetClientRect(), true);
	}

	RefreshCtrl();
}
void  CChartCtrl::RefreshCtrl()
{
	CRect rc = GetClientRect();
	if (rc.IsRectEmpty())return;
	if ( m_pMemRender)
	{
		ReCalcLayout();
		m_bRedrawControls = true;
		for (size_t i = 0; i < m_arrChart.GetCount(); i++)
		{
			m_arrChart[i]->Draw(m_pMemRender, m_bRedrawControls);
		}
		DrawAxisX(m_pMemRender);
		m_bRedrawControls = false;
		DrawSpliter(m_pMemRender);
		// 绘制十字线
		DrawCursorLine(m_pMemRender);
		//绘制焦点窗体
		if (m_arrChart.GetCount() > 0 && m_nCurChart > 0)
		{
			CRect rcFocus = m_arrChart[m_nCurChart];
			int nYWith = 0;
			if (m_pAxisX->m_Period.kind == DYNA_DATA)
				rcFocus.DeflateRect(nYWith, 1, nYWith, 0);
			else
				rcFocus.DeflateRect(0, 1, nYWith, 0);
			//m_pMemRender->Draw3dRect(&rcFocus, ES_Color()->clrBorder, ES_Color()->clrBorder);
			rcFocus.DeflateRect(1, 0, 0, 1);
			//m_pMemRender->Draw3dRect(&rcFocus, ES_Color()->clrBorder, ES_Color()->clrBorder);

		}
		CheckMouseTip(m_curCursorPos);

		if (m_nShowMouseTip != -1 && m_nShowMouseTip == m_nCurMouseTip)
		{
			DrawMouseTip(m_pMemRender);
		}
	}
}
void CChartCtrl::DrawSpliter(IRenderTarget *pRender)
{
	int yWith = 0;
	for (size_t i = 1; i < m_arrChart.GetCount(); i++)
	{
		int y = m_arrChart[i]->top;
		yWith = m_arrChart[i]->GetAxisYWith();
		int x1 = 0;
		if (m_pAxisX->m_Period.kind == DYNA_DATA)
			x1 = yWith;
		int x2 = m_arrChart[i]->right - yWith;
		DrawLine(pRender, PS_SOLID, x1, y, x2, y, ES_Color()->clrBorder);
	}

	//绘制虚分隔条
	if (m_curSpliter != -1)
	{
		int x1 = 0;
		if (m_pAxisX->m_Period.kind == DYNA_DATA)
			x1 = yWith;
		DrawLine(pRender, PS_SOLID, x1, m_curCursorPos.y, m_arrChart[0]->right - yWith, m_curCursorPos.y, ES_Color()->clrBorder);
	}
}
void CChartCtrl::DrawMouseTip(IRenderTarget *pRender)
{
	if (m_nShowMouseTip >= 0 && m_nShowMouseTip < static_cast<int>(m_arrTip.GetCount()))
	{
		SArray<SStringW> arr;
		SpliteString(arr, m_arrTip[m_nShowMouseTip].strTip);
		if (arr.GetCount() > (size_t)0)
		{
			CRect rcClient = GetClientRect();
			CSize szLabel(0, 0);
			for (size_t i = 0; i < arr.GetCount(); i++)
			{
				CSize curSz;
				pRender->MeasureText(arr[i], arr[i].GetLength(), &curSz);
				szLabel.cx = max(szLabel.cx, curSz.cx + 20);
				szLabel.cy += max(curSz.cy + 4, 22);
			}

			CPoint ptTopLeft = m_curCursorPos;
			if (ptTopLeft.y - rcClient.top < 50)
			{
				ptTopLeft.y += 20;
			}

			if (rcClient.bottom - m_curCursorPos.y < szLabel.cy + 10)
			{
				ptTopLeft.y -= szLabel.cy;
			}

			if (rcClient.right - m_curCursorPos.x < szLabel.cx + 10)
			{
				ptTopLeft.x -= szLabel.cx;
			}

			DrawRect(pRender, CRect(ptTopLeft.x, ptTopLeft.y, ptTopLeft.x + szLabel.cx, ptTopLeft.y + szLabel.cy), ES_Color()->clrBK, ES_Color()->clrBorder, 1, 170);
			CRect rcTxt(ptTopLeft.x + 5, ptTopLeft.y, ptTopLeft.x + szLabel.cx, ptTopLeft.y + 10);
			for (size_t i = 0; i < arr.GetCount(); i++)
			{
				CSize curSz;
				pRender->MeasureText(arr[i], arr[i].GetLength(), &curSz);
				rcTxt.bottom = rcTxt.top + max(curSz.cy + 4, 22);
				if (i == 0 && arr.GetCount()>1)
				{
					DrawRect(pRender, CRect(ptTopLeft.x, rcTxt.top, ptTopLeft.x + szLabel.cx, rcTxt.bottom + 1), ES_Color()->clrBorder, ES_Color()->clrBorder, 1, 170);
					pRender->SetTextColor(RGBA(255, 255, 255, 255));
					pRender->DrawText(arr[i], arr[i].GetLength(), & rcTxt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{
					pRender->SetTextColor(ES_Color()->clrTxtNormal);
					pRender->DrawText(arr[i], arr[i].GetLength(), &rcTxt, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				}
				rcTxt.top += rcTxt.Height();
			}
		}
	}
}


int CChartCtrl::AddK(DC_ContractInfo* pInfo, Period *pPrd, size_t nChartIdx, double rate, bool bCombol/*=false*/)
{
	if (pInfo == NULL) return 0;

	if (m_pAxisX->m_Period.kind == DYNA_DATA)
	{
		CDynaChart *pChart = dynamic_cast<CDynaChart*>(m_arrChart[0]);
		return pChart->AppendK(pInfo);
	}

	SPOSITION pos = m_mapCandleSeries.GetStartPosition();
	while (pos)
	{
		int nKey = 0;
		CCandleSeries* pSeries = NULL;
		m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			if (ContractEqual(*pSeries->GetContractInfo(), *pInfo))
				return -1;		//已经存在该合约
		}
	}

	CCandleSeries *pSeries = new CCandleSeries;
	pSeries->SetContractInfo(pInfo);
	pSeries->SetAxisHoriz(m_pAxisX);
	pSeries->SetID(m_curKIdx);
	pSeries->SetKType(m_nKType);
	pSeries->SetAppendRate(rate);

	m_mapCandleSeries.SetAt(m_curKIdx, pSeries);

	if (nChartIdx >= 0 && nChartIdx < m_arrChart.GetCount())
	{
		CKChart *pChart = dynamic_cast<CKChart*>(m_arrChart[nChartIdx]);
		pChart->AddK(m_curKIdx, pSeries);
	}

	if (m_curKIdx == 0)
	{
		m_pAxisX->SetHisData(pSeries->GetSeriesData());
		m_pAxisX->SetContract(pInfo);
		m_pMainCandleSeries = pSeries;
	}
	else
	{
		pSeries->EnableSubK(true);
	}

	m_curKIdx++;

	pSeries->SetStock(pInfo, pPrd);

	return pSeries->GetID();
}
void CChartCtrl::CancelSelect()
{
	for (size_t i = 0; i < m_arrChart.GetCount(); i++)
	{
		m_arrChart[i]->KillFocus();
	}
}
void CChartCtrl::OnSize(UINT nType, CSize size)
{
	ReCalcLayout();
}
void CChartCtrl::ReCalcLayout()
{
	// 调整标题栏
	CRect rcClient = GetClientRect();
	CChartTitleCtrl *pTitle = FindChildByName2<CChartTitleCtrl>(L"title");
	if (pTitle && m_pAxisX)
	{
		int yWith = GetChatAxisYFitWidth();
		if (m_pAxisX->m_Period.kind == DYNA_DATA)
		{
			m_pAxisX->left = rcClient.left + yWith;
		}
		if (m_bShowAxis)
		{
			m_pAxisX->right = rcClient.right - yWith;
		}
		pTitle->Move(CRect(m_pAxisX->left, rcClient.top, m_pAxisX->right, rcClient.top + m_nTitleHeight));
	}

	if (m_arrChart.IsEmpty()) return;
	int curY = 0;
	for (size_t i = 0; i < m_arrChart.GetCount(); i++)
	{
		m_arrChart[i]->top= curY;

		if (m_MaxmizeChart > 0) //如果存在最大化的图表
		{
			if (i == 0)
			{
				int mainHet = (int)(rcClient.Height()*m_arrChart[0]->GetRateSize());
				curY += mainHet > m_nTitleHeight ? mainHet : m_nTitleHeight;
			}
			else if (m_MaxmizeChart == i)
			{
				int maxHeight = rcClient.Height() - m_arrChart[0]->Height() - (m_arrChart.GetCount() - 2)*m_nTitleHeight;

				curY += maxHeight > m_nTitleHeight ? maxHeight : m_nTitleHeight;
			}
			else
			{
				curY += m_nTitleHeight;
			}
		}
		else
		{
			int hh = (int)(rcClient.Height()*m_arrChart[i]->GetRateSize());
			curY += hh > m_nTitleHeight ? hh : m_nTitleHeight;
		}

		m_arrChart[i]->bottom = curY;

		m_arrChart[i]->right = rcClient.right;

	}
	CRect rcX(GetClientRect());

	rcX.right -= GetChatAxisYFitWidth();
	rcX.top = rcClient.bottom - m_nXAxisHeight;
	rcX.bottom++;
	if (m_pAxisX->m_Period.kind == DYNA_DATA)
		rcX.left += m_arrChart[0]->GetAxisYWith();
	m_pAxisX->CopyRect(&rcX);
	if (!m_bShowAxis)
	{
		m_pAxisX->left = rcClient.left;
		m_pAxisX->right = rcClient.right;
	}


	m_arrChart[m_arrChart.GetCount() - 1]->bottom = rcClient.bottom - m_nXAxisHeight;

	for (size_t i = 0; i < m_arrChart.GetCount(); i++)
	{
		m_arrChart[i]->ReCalcLayout();
	}
}

void CChartCtrl::DrawAxisX(IRenderTarget *pRender)
{
	if (m_pAxisX)
	{
		CPoint curPt = GetCursorPos();
		bool bFocus = (m_pAxisX->PtInRect(curPt)==TRUE);
		m_pAxisX->SetFocus(bFocus);

		CRect  rcClient = GetClientRect();
		int yWith = GetChatAxisYFitWidth();
		if (m_pAxisX->m_Period.kind == DYNA_DATA)
		{
			m_pAxisX->left = rcClient.left + yWith;
		}
		if (m_bShowAxis)
		{
			m_pAxisX->right = rcClient.right - yWith;
		}
		m_pAxisX->Draw(pRender);

		// 绘制右边空白区域
		CRect rcPrd(m_pAxisX->right + 1, m_pAxisX->top - 1, rcClient.right, rcClient.bottom);
		pRender->FillSolidRect(&rcPrd, RGBA(255,0,201,255));
		if (m_arrChart.GetCount() <= 0)
			return;
		pRender->SetTextColor(ES_Color()->clrBorder);
		SStringW strTxt = m_strPeriodWords[m_pAxisX->m_Period.kind];
		if (m_pAxisX->m_Period.kind >= SEC_DATA && m_pAxisX->m_Period.kind <= DAY_DATA)
		{
			SStringW dd;
			if (m_pAxisX->m_Period.multi > 1 || m_pAxisX->m_Period.kind != DAY_DATA)
			{
				dd.Format(L"%d", m_pAxisX->m_Period.multi);
			}
			else
			{
				if (strTxt.Find(GETSTRING(L"@string/IDS_10110")) == -1)
				{
					strTxt += GETSTRING(L"@string/IDS_10110");
				}
			}
			strTxt = dd + strTxt;
		}

		if (m_bShowAxis)
		{
			rcPrd.left += 6;
			pRender->DrawText(strTxt, strTxt.GetLength(), &rcPrd, DT_VCENTER | DT_SINGLELINE);
		}

		//绘制焦点K线的虚框
		if (m_pAxisX->m_nFocusIndex != -1)
		{
			int nPixX = m_pAxisX->Value2Pix(m_pAxisX->m_nFocusIndex);
			int nBarWid = m_pAxisX->GetCandleWidth();
			CRect rcRect;
			rcRect.SetRect(nPixX, -1, nPixX + nBarWid, m_pAxisX->top + 1);
			DrawRect(pRender, &rcRect, ES_Color()->clrBorder, 0, 0, 128);
		}
		bFocus = m_pAxisX->PtInRect(GetCursorPos());
		m_pAxisX->SetFocus(bFocus);
		m_pAxisX->DrawAxisTip(pRender);
	}
}
int CChartCtrl::GetChatAxisYFitWidth()
{
	int nMaxWith = m_DefYAxisWidth;
	for (size_t i = 0; i<m_arrChart.GetCount(); i++)
	{
		if (m_arrChart[i]->GetAxisYWith()>nMaxWith)
			nMaxWith = m_arrChart[i]->GetAxisYWith();
	}
	return nMaxWith;
}
void CChartCtrl::SetCursor(CURSOR_TYPE type)
{
	m_cursor = type;
	//m_pStatic->SendMessage(WM_SETCURSOR);
}
CRect CChartCtrl::GetTitleRect()
{
	CRect retRc;
	CChartTitleCtrl*pTitle = FindChildByName2<CChartTitleCtrl>(L"title");
	SASSERT(pTitle);
	if ( pTitle && pTitle->IsVisible(false))
	{
		retRc.CopyRect(pTitle->GetClientRect());
	}
	return retRc;
}
void CChartCtrl::UpdateHisData(DC_Data* pHisData, int nCount, int nData, int nIDK, double fPriceOrigin)
{
	if (m_pAxisX->m_Period.kind == 0)
	{
		CDynaChart *pChart = dynamic_cast<CDynaChart*>(m_arrChart[0]);
		if (pChart)
		{
			pChart->SetOriginPrice(fPriceOrigin, nIDK);
			pChart->UpdateHisData((DC_KData*)pHisData, nCount, nData, nIDK);
		}
	}
	else
	{
		CCandleSeries *pSeries;
		if (m_mapCandleSeries.Lookup(nIDK, pSeries))
		{
			pSeries->UpdateHisData((DC_KData*)pHisData, nCount, nData);
		}

	}
	if (nIDK == 0)
	{
		m_pAxisX->UpdateHisData();
	}
	RefreshCtrl();
}

int CChartCtrl::GetChartByPos(CPoint& pt)
{
	for (size_t i = 0; i < m_arrChart.GetCount(); i++)
	{
		if (m_arrChart[i]->PtInRect(pt))
			return i;
	}
	return -1;
}
bool CChartCtrl::IsMouseLineVisible(DC_KData& data)
{
	if (m_bShowMouseLine)
	{
		memset(&data, 0, sizeof(DC_KData));
		if (m_nCurCandleIdx >= 0 && m_nCurCandleIdx < m_pAxisX->GetDataCount())
		{
			data = m_pAxisX->GetHisData()->GetAt(m_nCurCandleIdx);
		}
	}
	return m_bShowMouseLine;
}
void CChartCtrl::SetCursorByPos(CPoint& pt)
{
	if (m_curSpliter != -1)
	{
		m_cursor = CURSOR_SPLIT_HERZ;
		return;
	}
	else
	{
		int curspliter = GetSpliterByPos(pt);
		if (curspliter != -1)
		{
			m_cursor = CURSOR_SPLIT_HERZ;
			return;
		}
		else
		{
			int idx = GetChartByPos(pt);
			if (idx != -1)
			{
				CPoint newPt = pt;
				m_cursor = (CURSOR_TYPE)(m_arrChart[idx]->GetCursor(newPt));
				return;
			}
			else
			{
				if (m_pAxisX->PtInRect(pt))
				{
					if (m_pAxisX->GetScrllBarRect().PtInRect(pt))
					{
						m_cursor = CURSOR_HAND;
					}
					else
						m_cursor = CURSOR_AXIS_HERZ;
					return;
				}
			}
		}
	}
	m_cursor = CURSOR_NORMAL;
}
int CChartCtrl::GetSpliterByPos(CPoint& pt)
{
	for (size_t i = 1; i < m_arrChart.GetCount(); i++)
	{
		if (abs(pt.y - m_arrChart[i - 1]->bottom) <= 5)
		{
			int minY = m_arrChart[i - 1]->top + GetTitleRect().Height();
			int maxY = m_arrChart[i]->bottom - GetTitleRect().Height();

			if (maxY - minY > 1)
			{
				return i - 1;
			}
			else
			{
				return -1;
			}
		}
	}
	return -1;
}
void CChartCtrl::OnKillFocus(SWND wndFocus)
{
	__super::OnKillFocus(wndFocus);
}

void CChartCtrl::OnSetFocus(SWND wndOld)
{
	__super::OnSetFocus(wndOld);
}
UINT CChartCtrl::OnGetDlgCode()
{
	 return SC_WANTARROWS;
}
bool CChartCtrl::OnMouseDown(CPoint& pt)
{
	int idx = GetChartByPos(pt);

	if (idx != -1)
	{
		bool ret = m_arrChart[idx]->OnMouseDown(pt);
		SetCursorByPos(pt);
		return ret;
	}
	return false;
}
void   CChartCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	if (OnMouseDown(point))
	{
		return;
	}
	m_bIsDraging = false;
	m_bIsDbClick = false;
	m_bMouseDown = true;
	m_PtMouseDown = point;
	m_bLeftBtnClick = true;
}
void   CChartCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
	if (!m_bMouseDown)
	{
		__super::OnLButtonUp(nFlags, point);
		return;
	}
	bool isdbclick = m_bIsDbClick;
	m_bIsDbClick = false;
	m_bMouseDown = false;
	ReleaseCapture();
	if (m_bIsDraging)
	{
		m_bIsDraging = false;
		OnDragUp(m_PtMouseDown, point);
	}
	else
	{
		if (!isdbclick)
		{
			OnClick(point);
			//GetParent()->PostMessage(CHART_MSG_LBT_CLICK, point.x, point.y);
		}
	}
}
bool CChartCtrl::IsMouseDown()
{
	return m_bMouseDown;
}
void   CChartCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}
void   CChartCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bUserMoveMouse)
	{
		m_bUserMoveMouse = true;
		__super::OnMouseMove(nFlags, point);
		return;
	}
	if (!m_bMouseIn)
	{
		m_bMouseIn = true;
	}
	CPoint pt = point;
	if (!m_cursorRc.IsRectEmpty())
	{
		CRect tmp(m_cursorRc);
		//ScreenToClient(GetContainer()->GetHostHwnd(),&tmp);
		pt.x = tmp.left > pt.x ? tmp.left : pt.x;
		pt.x = tmp.right < pt.x ? tmp.right : pt.x;
		pt.y = tmp.top > pt.y ? tmp.top : pt.y;
		pt.y = tmp.bottom < pt.y ? tmp.bottom : pt.y;

	}

	if (m_bMouseDown && (abs(m_PtMouseDown.x - pt.x)>3 || abs(m_PtMouseDown.y - pt.y) > 3))
		m_bIsDraging = true;
	if (m_bIsDraging)
	{
		OnDrag(m_PtMouseDown, pt, m_bLeftBtnClick);
	}
	else
	{
		m_curCursorPos = pt;
		if (m_pAxisX && m_pAxisX->GetDataCount() > 0)
			m_nCurCandleIdx = (int)m_pAxisX->Pix2Value(pt.x);
		////鼠标样式
		SetCursorByPos(pt);
		int idx = GetChartByPos(pt);
		if (idx != -1)
			m_arrChart[idx]->OnMouseMove(pt);
		CheckMouseTip(pt);
		Invalidate();
		return;
	}
	__super::OnMouseMove(nFlags, pt);
}
void CChartCtrl::CheckMouseTip(CPoint pt)
{
	m_nCurMouseTip = -1;
	for (int i = m_arrTip.GetCount() - 1; i >= 0; i--)
	{
		if (m_arrTip[i].rc.PtInRect(pt))
		{
			m_nCurMouseTip = i;
		}
	}

	if (m_nCurMouseTip != -1)
	{
		SetTimer(11, 100);
	}
	else
	{
		m_nShowMouseTip = -1;
		KillTimer(11);
	}
}
void CChartCtrl::SizeToRateSize()
{
	for (size_t i = 0; i < m_arrChart.GetCount(); i++)
	{
		if (GetClientRect().Height()>0)
		{
			m_arrChart[i]->SetRateSize(((float)m_arrChart[i]->Height()) / ((float)GetClientRect().Height()));
		}
	}
}
bool CChartCtrl::OnDrag(CPoint& from, CPoint &pt, bool bLeft/* =true */)
{
	m_curCursorPos = pt;
	if (m_curSpliter == -1)
	{
		m_curSpliter = GetSpliterByPos(from);
		if (m_curSpliter != -1)
		{
			SetCapture();
			SetCursorByPos(from);
		}
	}

	if (m_curSpliter != -1)
	{
		if (m_MaxmizeChart != -1)
		{
			m_MaxmizeChart = -1;
			SizeToRateSize();
		}

		int minY = m_arrChart[m_curSpliter]->top + m_nTitleHeight;
		if (m_curSpliter == 0)
		{
			minY += m_nXAxisHeight;
		}
		int maxY = m_arrChart[m_curSpliter]->bottom;

		if (m_curSpliter < (int)(m_arrChart.GetCount() - 1))
			maxY = m_arrChart[m_curSpliter + 1]->bottom - m_nTitleHeight;

		if (pt.y<minY)
			m_curCursorPos.y = minY;
		else if (pt.y>maxY)
			m_curCursorPos.y = maxY;

		Invalidate();
	}
	else
	{
		bool ret = false;
		int idx = GetChartByPos(from);
		if (idx != -1)
		{
			m_nCurChart = idx;
			ret = m_arrChart[idx]->OnDrag(from, pt, bLeft);
		}
		OnDragAxisX(from, pt, bLeft);
		if (ret)
		{
			//m_pStatic->SendMessage(WM_SETCURSOR);
			Invalidate();
			return true;
		}
	}
	return true;
}
void CChartCtrl::OnDragAxisX(CPoint& from, CPoint &pt, bool bLeft)
{
	if (m_pAxisX->m_Period.kind == DYNA_DATA)
		return;
	if (m_arrChart.IsEmpty()) return;
	static double fLastAxisMin, fLastAxisMax;
	static int nLastDuration, nOffsetScrollBar;
	if (!m_bDragingAxisX)
	{
		if (m_pAxisX->PtInRect(from))
		{
			//正拖动横坐标
			m_arrChart[0]->SetAutoAxix(true);		//设置自动调整纵坐标
			m_bDragingAxisX = true;
			SetCapture();
			fLastAxisMin = m_pAxisX->GetMinValue();
			fLastAxisMax = m_pAxisX->GetMaxValue();
			nLastDuration = (int)m_pAxisX->GetDuration();

			if (m_pAxisX->GetScrllBarRect().PtInRect(from))
			{
				m_pAxisX->m_bIsDraging = true;
				m_bDragingScrollBar = true;
				nOffsetScrollBar = from.x - m_pAxisX->GetScrllBarRect().left;
			}
		}
	}

	if (m_bDragingAxisX)
	{
		//拖动横坐标
		if (!bLeft)
		{
			//计算缩放后的坐标
			int pixOffSet = pt.x - from.x;
			int allOther = nLastDuration - (int)fLastAxisMin;
			int oldW = m_pAxisX->right - from.x;
			int newW = m_pAxisX->right - pt.x;

			if (newW <= 0) newW = 1;
			double fRange = (float)newW / (float)oldW;

			double fOldRange = (fLastAxisMax - fLastAxisMin) / allOther;

			double fLength = allOther * fOldRange / fRange;

			if (fLength <= MIN_CANDLE_NUM) fLength = MIN_CANDLE_NUM - 1;

			if (fLength > m_nMaxPointNum)
				fLength = m_nMaxPointNum;


			int newMin = (int)(m_pAxisX->GetMaxValue() - fLength);
			if (newMin<0) newMin = 0;

			int nOldNum = m_pAxisX->m_nScreenCandleNum;

			m_pAxisX->m_nScreenCandleNum = (int)(m_pAxisX->GetMaxValue() - newMin + 1);
			m_pAxisX->SetMinMaxValue(newMin, m_pAxisX->GetMaxValue());

			if (m_pAxisX->GetMaxValue()>m_pAxisX->GetMinValue() && nOldNum != m_pAxisX->m_nScreenCandleNum)
			{
				m_fCandlePix = (m_pAxisX->GetMaxValue() - m_pAxisX->GetMinValue() + 1);
			}
			Invalidate();
		}
		else
		{
			if (m_bDragingScrollBar)
			{
				//拖动滚动条
				int curScrollBarLeft = pt.x - nOffsetScrollBar;
				int barWid = m_pAxisX->GetScrllBarRect().Width();

				if (curScrollBarLeft <= m_pAxisX->left)
					curScrollBarLeft = m_pAxisX->left;
				if (curScrollBarLeft + barWid > m_pAxisX->right) curScrollBarLeft = m_pAxisX->right - barWid;

				double fRate = ((double)curScrollBarLeft - m_pAxisX->left) / (m_pAxisX->Width() - barWid);

				int nMin = (int)((m_pAxisX->GetDuration() - (fLastAxisMax - fLastAxisMin)) * fRate);
				if (nMin < 0)
					nMin = 0;

				int nMax = nMin + (int)(fLastAxisMax - fLastAxisMin);
				if (nMax > m_pAxisX->GetDuration()) nMax = (int)m_pAxisX->GetDuration();

				m_pAxisX->SetMinMaxValue(nMin, nMax);

			}
			else
			{
				//拖动坐标栏
				int pixOffSet = from.x - pt.x;
				if (!(pixOffSet < 0 && fLastAxisMin <= 0))
				{
					double fAdd = (fLastAxisMax - fLastAxisMin) * ((double)pixOffSet) / (m_pAxisX->Width());
					int nMax = (int)(fLastAxisMax + fAdd);
					if (nMax > m_pAxisX->GetDuration()) nMax = (int)m_pAxisX->GetDuration();
					int nMin = nMax - (int)(fLastAxisMax - fLastAxisMin);
					if (nMin < 0) nMin = 0;
					m_pAxisX->SetMinMaxValue(nMin, nMax);
				}
			}
			Invalidate();
		}
	}
}
void  CChartCtrl::SetCandlePix(double fPix)
{
	m_fCandlePix = max(fPix, 10);
}
void CChartCtrl::DrawIcon(IRenderTarget *pRender, int x, int y, int idx)
{
// 	if (m_imageList.m_hImageList)
// 	{
// 		int cnt = m_imageList.GetImageCount();
// 		if (idx < 0)idx = 0;
// 		idx = idx%cnt;
// 
// 		m_imageList.Draw(pRender, idx, CPoint(x, y), ILD_TRANSPARENT);
// 	}
}
void CChartCtrl::DrawCursorLine(IRenderTarget* pRender)
{
	if (m_bShowMouseLine && m_curSpliter == -1 && !m_bIsDraging && m_bMouseIn)
	{
		int idx = GetChartByPos(m_curCursorPos);
		if (idx < 0) return;
		if (!m_arrChart[idx]->PtInRect(m_curCursorPos)) return;
		// 绘制十字线
		CRect rcClient = GetClientRect();
		CPoint pt = m_curCursorPos;
		DrawLine(pRender, PS_SOLID, rcClient.left + (m_Period.kind!=0 ? 0:  GetChatAxisYFitWidth()), pt.y, m_arrChart[0]->right - GetChatAxisYFitWidth(), pt.y, m_clrCursorLine);
		DrawLine(pRender, PS_SOLID, pt.x, rcClient.top, pt.x, rcClient.bottom - m_nXAxisHeight, m_clrCursorLine);
		m_ptCursorLastDraw = pt;
	}
}

bool CChartCtrl::OnClick(CPoint &pt, bool bLeft/* =true */)
{
	bool ret = false;
	int idx = GetChartByPos(pt);

	for (size_t i = 0; i < m_arrChart.GetCount(); i++)
	{
		if (i != idx)
		{
			m_arrChart[i]->KillFocus();
		}
	}
	if (idx != -1)
	{
		m_nCurChart = idx;
		if (m_nCurChart > 0)
		{
			CRect rcClose = m_arrChart[idx];
			rcClose.left = rcClose.right - 20;
			rcClose.bottom = rcClose.top + 20;

			if (rcClose.PtInRect(pt))
			{
				//::SendMessage(GetParent(m_pStatic->GetSafeHwnd()), CHART_MSG_BUTTON_CLICK, 2, idx);
				return true;
			}
		}
		ret = m_arrChart[idx]->OnClick(pt, bLeft);
	}

	if (ret) return true;
	//若图表不处理消息，则自己处理
	if (bLeft)
		m_bShowMouseLine = !m_bShowMouseLine;
	//若没有显示光标，设置当前K线为最右端的K线，即-1
	if (!m_bShowMouseLine)
	{
		m_nCurCandleIdx = -1;
	}
	else
	{
		m_nCurCandleIdx = (int)m_pAxisX->Pix2Value(m_curCursorPos.x);
	}
	Invalidate();
	return ret;
}

bool CChartCtrl::OnDblClk(CPoint& pt, bool bLeft/*=true*/)
{
	bool ret = false;

	int idx = GetChartByPos(pt);
	if (idx != -1)
	{
		m_nCurChart = idx;
		ret = m_arrChart[idx]->OnDblClk(pt, bLeft);
	}

	if (ret) return ret;
	//若图表不处理消息，则自己处理
	if (m_arrChart.GetCount() < 3)
	{
		m_MaxmizeChart = -1;
	}
	else
	{
		int idx = GetChartByPos(pt);
		if (idx > 0)
		{
			if (m_MaxmizeChart == idx)
				m_MaxmizeChart = -1;
			else
				m_MaxmizeChart = idx;
		}
		ReCalcLayout();
		Invalidate();
	}
	return true;
}

bool CChartCtrl::OnDragUp(CPoint&from, CPoint &pt, bool bLeft/* =true */)
{
	m_bDragingAxisX = false;
	m_bDragingScrollBar = false;
	if (m_curSpliter != -1)
	{
		CRect rcClient = GetClientRect();
		double sumRate = m_arrChart[m_curSpliter]->GetRateSize() + m_arrChart[m_curSpliter + 1]->GetRateSize();
		int sumHeight = static_cast<int>(m_arrChart[m_curSpliter]->GetRateSize()*rcClient.Height() + m_arrChart[m_curSpliter + 1]->GetRateSize()*rcClient.Height());
		m_arrChart[m_curSpliter]->bottom = m_curCursorPos.y;
		m_arrChart[m_curSpliter + 1]->top = m_curCursorPos.y;

		m_arrChart[m_curSpliter]->SetRateSize(  sumRate * ((float)m_arrChart[m_curSpliter]->Height()) / ((float)sumHeight));
		m_arrChart[m_curSpliter + 1]->SetRateSize(sumRate - m_arrChart[m_curSpliter]->GetRateSize());
		ReCalcLayout();
	}
	else
	{
		bool ret = false;
		int idx = GetChartByPos(from);
		if (idx != -1)
		{
			ret = m_arrChart[idx]->OnDragUp(from, pt, bLeft);
		}

		if (ret)
			goto done;
	}

done:
	m_curSpliter = -1;
	m_cursor = CURSOR_NORMAL;
	if (m_pAxisX)m_pAxisX->EnableDraging(false);
	Invalidate();
	ReleaseCapture();
	return true;
}

void CChartCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAxisX->GetHisData() == NULL || m_pAxisX->GetDataCount() == 0) return;

	BOOL bCtrl = ::GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = ::GetKeyState(VK_SHIFT) & 0x8000;

	if (m_pAxisX->m_Period.kind == DYNA_DATA)
	{
		if (bCtrl) return;
		if (nChar != VK_LEFT && nChar != VK_RIGHT) return;
	}
	if (nChar == VK_LEFT)
	{
		ShowMouseLine(true);
		if (m_pAxisX->GetMinValue() == 0 && !m_bDataComplete && m_nCurCandleIdx==0)
		{
			return;
		}
		if (bCtrl)
		{
			int nStep = (int)max(1, (m_pAxisX->GetMaxValue() - m_pAxisX->GetMinValue()) / 10);
			if (m_pAxisX->GetMinValue() < nStep)
			{
				nStep = (int)m_pAxisX->GetMinValue();
			}
			if (nStep > 0)
				m_pAxisX->SetMinMaxValue(m_pAxisX->GetMinValue() - nStep, m_pAxisX->GetMaxValue() - nStep);

			m_nCurCandleIdx = (int)m_pAxisX->Pix2Value(m_curCursorPos.x);
		}
		else
		{
			m_nCurCandleIdx--;
			m_nCurCandleIdx = max(m_nCurCandleIdx, 0);
			m_nCurCandleIdx = min(m_nCurCandleIdx, m_pAxisX->GetDataCount() - 1);
			if (m_nCurCandleIdx<m_pAxisX->GetMinValue())
			{
				m_pAxisX->SetMinMaxValue(m_pAxisX->GetMinValue() - 1, m_pAxisX->GetMaxValue() - 1);
				m_nCurCandleIdx = (int)max(m_pAxisX->GetMinValue(), 0);
			}
		}

		int wid = m_pAxisX->GetCandleWidth();
		m_curCursorPos.x = m_pAxisX->Value2Pix(m_nCurCandleIdx) + wid / 2;
		double fclose = m_pAxisX->GetHisData()->GetAt(m_nCurCandleIdx).Close;
		CKChart *pChart = dynamic_cast<CKChart*>(m_arrChart[0]);
		m_curCursorPos.y = pChart->GetYAxis()->Value2Pix(fclose);
		CRect rc = *pChart;
		SASSERT(::IsWindow(GetContainer()->GetHostHwnd()));
		ClientToScreen(GetContainer()->GetHostHwnd(),(LPPOINT)&rc);
		if (m_curCursorPos.y>rc.Height() - m_nXAxisHeight - 3) 
			m_curCursorPos.y = rc.Height() - m_nXAxisHeight - 3;
		if (m_curCursorPos.y < m_nTitleHeight)
			m_curCursorPos.y = m_nTitleHeight;
		m_bUserMoveMouse = false;
		SetCursorPos(rc.left + m_curCursorPos.x, rc.top + m_curCursorPos.y);
		Invalidate();
	}
	else if (nChar == VK_RIGHT)
	{
		ShowMouseLine(true);
		if (bCtrl)
		{
			int nStep = (int)max(1, (m_pAxisX->GetMaxValue() - m_pAxisX->GetMinValue()) / 10);
			if (m_pAxisX->GetMaxValue() - m_pAxisX->GetDataCount() + nStep >= m_pAxisX->GetFreeCandleCount())
			{
				int dual = (int)(m_pAxisX->GetMaxValue() - m_pAxisX->GetMinValue() + 1);

				int nMax = m_pAxisX->GetDataCount() + m_pAxisX->GetFreeCandleCount() - 1;
				int nMin = max(0, nMax - dual + 1);
				m_pAxisX->SetMinMaxValue(nMin, nMax);
			}
			else
			{
				m_pAxisX->SetMinMaxValue(m_pAxisX->GetMinValue() + nStep, m_pAxisX->GetMaxValue() + nStep);
			}
			m_nCurCandleIdx = (int)m_pAxisX->Pix2Value(m_curCursorPos.x);
		}
		else
		{
			if (m_nCurCandleIdx == -1)
			{
				m_nCurCandleIdx = max(0, (int)m_pAxisX->GetMinValue());
			}
			else
			{
				m_nCurCandleIdx++;
				m_nCurCandleIdx = min(m_nCurCandleIdx, m_pAxisX->GetDataCount() - 1);
			}

			if (m_nCurCandleIdx > m_pAxisX->GetMaxValue())
			{
				if (m_nCurCandleIdx <m_pAxisX->GetDataCount() - 1)
					m_pAxisX->SetMinMaxValue(m_pAxisX->GetMinValue() + 1, m_pAxisX->GetMaxValue() + 1);
				else
					m_pAxisX->SetMinMaxValue(m_pAxisX->GetMinValue() + 1, m_pAxisX->GetDataCount() - 1 + m_pAxisX->GetFreeCandleCount());
			}
		}

		double fclose = m_pAxisX->GetHisData()->GetAt(min(m_pAxisX->GetDataCount() - 1, m_nCurCandleIdx)).Close;
		CKChart *pChart = dynamic_cast<CKChart*>(m_arrChart[0]);
		m_curCursorPos.y = pChart->GetYAxis()->Value2Pix(fclose);

		CRect rc = *pChart;
		SASSERT(::IsWindow(GetContainer()->GetHostHwnd()));
		ClientToScreen(GetContainer()->GetHostHwnd(),(LPPOINT)&rc);
		GetContainer()->GetHostHwnd();
		int wid = m_pAxisX->GetCandleWidth();
		m_curCursorPos.x = m_pAxisX->Value2Pix(m_nCurCandleIdx) + wid / 2;
		m_bUserMoveMouse = false;

		if (m_curCursorPos.y>rc.Height() - m_nXAxisHeight - 3) m_curCursorPos.y = rc.Height() - m_nXAxisHeight - 3;
		if (m_curCursorPos.y < m_nTitleHeight) m_curCursorPos.y = m_nTitleHeight;

		SetCursorPos(rc.left + m_curCursorPos.x, rc.top + m_curCursorPos.y);
		Invalidate();
	}
	else if (nChar == VK_UP)
	{
		int nMax = (int)m_pAxisX->GetMaxValue();
		int nMin = (int)m_pAxisX->GetMinValue();
		int nCount = nMax - nMin + 1;
		nCount = (int)(nCount *0.8); // 减少显示的K线数量
		if (nCount < MIN_CANDLE_NUM) nCount = MIN_CANDLE_NUM;
		nMin = nMax - nCount + 1;
		if (nMin<0) nMin = 0;

		m_pAxisX->SetMinMaxValue(nMin, nMax);
		if (m_pAxisX->GetMaxValue()>m_pAxisX->GetMinValue())
		{
			m_fCandlePix = (m_pAxisX->GetMaxValue() - m_pAxisX->GetMinValue() + 1);
		}
		CPoint ptCursor = GetCursorPos();
		m_nCurCandleIdx = (int)m_pAxisX->Pix2Value(ptCursor.x);
		Invalidate();
	}
	else if (nChar == VK_DOWN)
	{
		int nMax = (int)m_pAxisX->GetMaxValue();
		int nMin = (int)m_pAxisX->GetMinValue();
		if (nMin == 0 && !m_bDataComplete)
		{
			return;
		}
		int nCount = nMax - nMin + 1;
		int nAdd = static_cast<int>(min(nCount*1.2, 2000));
		nCount = nCount + nAdd;

		if (nCount > m_nMaxPointNum) nCount = m_nMaxPointNum;
		nMin = max(0, nMax - nCount + 1);

		m_pAxisX->SetMinMaxValue(nMin, nMax);
		CPoint ptCursor = GetCursorPos();
		m_nCurCandleIdx = (int)m_pAxisX->Pix2Value(ptCursor.x);

		if (m_pAxisX->GetMaxValue() > m_pAxisX->GetMinValue())
		{
			m_fCandlePix = (m_pAxisX->GetMaxValue() - m_pAxisX->GetMinValue() + 1);
		}
		Invalidate();
	}
	else if (nChar == VK_HOME)
	{
		ShowMouseLine(true);
		m_nCurCandleIdx = (int)m_pAxisX->GetMinValue();
		double fclose = m_pAxisX->GetHisData()->GetAt(m_nCurCandleIdx).Close;
		int pixX = m_pAxisX->Value2Pix(m_nCurCandleIdx);
		CKChart *pChart = dynamic_cast<CKChart*>(m_arrChart[0]);
		m_curCursorPos.y = pChart->GetYAxis()->Value2Pix(fclose);

		CRect rc = *pChart;
		SASSERT(::IsWindow(GetContainer()->GetHostHwnd()));
		ClientToScreen(GetContainer()->GetHostHwnd(), (LPPOINT)&rc);
		int wid = m_pAxisX->GetCandleWidth();
		m_curCursorPos.x = m_pAxisX->Value2Pix(m_nCurCandleIdx) + wid / 2;

		m_bUserMoveMouse = false;

		if (m_curCursorPos.y > rc.Height() - m_nXAxisHeight - 3) m_curCursorPos.y = rc.Height() - m_nXAxisHeight - 3;
		if (m_curCursorPos.y < m_nTitleHeight) m_curCursorPos.y = m_nTitleHeight;

		SetCursorPos(rc.left + m_curCursorPos.x, rc.top + m_curCursorPos.y);
		RefreshCtrl();
	}
	else if (nChar == VK_END)
	{
		ShowMouseLine(true);
		m_nCurCandleIdx = (int)m_pAxisX->GetMaxValue();
		if (m_nCurCandleIdx > m_pAxisX->GetDataCount() - 1) m_nCurCandleIdx = m_pAxisX->GetDataCount() - 1;
		double fclose = m_pAxisX->GetHisData()->GetAt(m_nCurCandleIdx).Close;
		int pixX = m_pAxisX->Value2Pix(m_nCurCandleIdx);
		CKChart *pChart = dynamic_cast<CKChart*>(m_arrChart[0]);
		m_curCursorPos.y = pChart->GetYAxis()->Value2Pix(fclose); 

		CRect rc = *pChart;
		SASSERT(::IsWindow(GetContainer()->GetHostHwnd()));
		ClientToScreen(GetContainer()->GetHostHwnd(),(LPPOINT)&rc);
		int wid = m_pAxisX->GetCandleWidth();
		m_curCursorPos.x = m_pAxisX->Value2Pix(m_nCurCandleIdx) + wid / 2;
		m_bUserMoveMouse = false;
		if (m_curCursorPos.y > rc.Height() - m_nXAxisHeight - 3) m_curCursorPos.y = rc.Height() - m_nXAxisHeight - 3;
		if (m_curCursorPos.y < m_nTitleHeight) m_curCursorPos.y = m_nTitleHeight;

		SetCursorPos(rc.left + m_curCursorPos.x, rc.top + m_curCursorPos.y);
		RefreshCtrl();
	}
}


void CChartCtrl::ShowMouseLine(bool bShow /*= true*/)
{
	if (!m_bShowMouseLine && bShow)
	{
		CPoint pt = m_curCursorPos;
		if (m_nCurCandleIdx == -1 && m_pAxisX->GetDataCount() > 0)
		{
			m_nCurCandleIdx = min(m_pAxisX->GetDataCount() - 1, (int)m_pAxisX->Pix2Value(pt.x));
		}
	}
	m_bShowMouseLine = bShow;
	RefreshCtrl();
}

CRectChart* CChartCtrl::GetCurrentChart()
{
	if (m_nCurChart < 0 || m_nCurChart >= (int)m_arrChart.GetCount())
		return NULL;
	return m_arrChart[m_nCurChart];
}

void CChartCtrl::CalRegionStatisticData(int nStar, int nEnd, bool bRecal)
{
	CAxisChart* pAxisChart = static_cast<CAxisChart*>(GetCurrentChart());
	if (!pAxisChart || !m_pAxisX)
		return;
	if (m_bShowMouseLine)
		m_bShowMouseLine = false;
	memset(&m_rsData, 0, sizeof(RSStruct));
	m_rsData.ci = *m_pAxisX->GetContract();
	m_rsData.Precision = m_rsData.ci.pect;
	m_rsData.CommodityDenominator = 1;
	pAxisChart->CalRegionStatisticRange(nStar, nEnd, bRecal);
	m_rsData.StarTime = m_pAxisX->GetHisData()->GetAt(nStar).time;
	m_rsData.EndTime = m_pAxisX->GetHisData()->GetAt(nEnd).time;
	m_rsData.Star = m_pAxisX->GetHisData()->GetAt(nStar).Open;
	m_rsData.End = m_pAxisX->GetHisData()->GetAt(nEnd).Close;
	m_rsData.kind = m_pAxisX->m_Period.kind;
	m_rsData.nStar = nStar;
	m_rsData.nEnd = nEnd;
	m_rsData.nCount = m_pAxisX->GetDataCount();
	if (nStar >= nEnd)
		return;
	//计算最高价最低价
	double maxY = -10000000;
	double minY = 10000000;
	double tolVolPrice = 0.0;
	for (int i = nStar; i <= nEnd; i++)
	{
		if (i<0 || i >= m_pAxisX->GetHisData()->GetCount()) break;

		DC_KData data = m_pAxisX->GetHisData()->GetAt(i);
		if (data.High>maxY)
		{
			maxY = data.High;
		}
		if (data.Low < minY)
		{
			minY = data.Low;
		}
		m_rsData.Volume += data.Volume;
		tolVolPrice += (double)(data.Close*data.Volume);
	}

	m_rsData.High = maxY;
	m_rsData.Low = minY;
	//仓差
	if (nStar == 0)
	{
		m_rsData.AmountDif = m_pAxisX->GetHisData()->GetAt(nEnd)._Amount - m_pAxisX->GetHisData()->GetAt(nStar)._Amount;
	}
	else
	{
		m_rsData.AmountDif = m_pAxisX->GetHisData()->GetAt(nEnd)._Amount - m_pAxisX->GetHisData()->GetAt(nStar - 1)._Amount;
	}
	//加权均价
	m_rsData.WeightAve = tolVolPrice / m_rsData.Volume;
	//涨幅
	m_rsData.rise = (m_rsData.End - m_rsData.Star) / m_rsData.Star;
	//振幅
	m_rsData.swing = (m_rsData.High - m_rsData.Low) / m_rsData.Star;
}
void CChartCtrl::OnMouseLeave()
{
	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(GetContainer()->GetHostHwnd(),&pt);

	if (!m_cursorRc.IsRectEmpty())
	{
		CRect tmp(m_cursorRc);
		::ScreenToClient(GetContainer()->GetHostHwnd(), (LPPOINT)&tmp);
		pt.x = tmp.left > pt.x ? tmp.left : pt.x;
		pt.x = tmp.right < pt.x ? tmp.right : pt.x;
		pt.y = tmp.top > pt.y ? tmp.top : pt.y;
		pt.y = tmp.bottom < pt.y ? tmp.bottom : pt.y;
	}

	m_bMouseIn = false;
	m_bMouseDown = false;
	if (m_bIsDraging)
	{
		m_bIsDraging = false;
		OnDragUp(m_PtMouseDown, pt, m_bLeftBtnClick);
		ReleaseCapture();
	}
	else
	{
		OnMouseMove(0, pt);
		Invalidate();
	}
	__super::OnMouseLeave();
}