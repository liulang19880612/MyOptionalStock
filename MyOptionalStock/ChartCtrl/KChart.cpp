#include "stdafx.h"
#include "KChart.h"
#include "Axis.h"
#include "ChartCtrl.h"
#include "TimeAxis.h"

CKChart::CKChart() 
    :m_bPadAliginLeft(FALSE),
	m_pSelSeries(NULL),
	m_pMainK(NULL),
	m_pMainIndicator(NULL),
	m_pClrSeires(NULL),
	m_nIcon(-1),
	m_bHaveSelObject(FALSE),
	m_userTradePoint(_T(""))
{
	m_userTradePoint.pData = NULL;
	m_userTradePoint.nCount = 0;
	m_userTradePoint.EnableDrawLine(false);
	m_pDragingSub = NULL;
}
CKChart::~CKChart()
{

}

void CKChart::AddK(int id, CSeriesObject *pSeries)
{
	m_mapCandleSeries.SetAt(id, pSeries);
	pSeries->SetAxisHoriz(m_pAxisX);
	pSeries->SetChart(this);

	if (m_pMainK == NULL)
	{
		m_pMainK = pSeries;
	}
	else
	{
		pSeries->SetAxisVert(new CAxis(FALSE, this));
		m_arrOwnAixs.Add(pSeries->GetAxisVert());
		pSeries->SetOwnAxis();
	}

	m_pMainIndicator = NULL;
	if (m_pChartCtrl)
		m_pChartCtrl->ReCalcLayout();
}
//����������ø����е�����
void CKChart::CaluAixs()	      
{
	//��������������꣬�����������ֵ
	if (m_bAutoAxis)
	{
		CalcMinMaxValue();
	}
	//����ÿ�����е�����
	SPOSITION pos = m_mapCandleSeries.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;
		int nKey = -1;
		m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries && pSeries->GetAxisVert())
		{
			pSeries->GetAxisVert()->CopyRect(m_pAxisY);
			if (!pSeries->IsOnwnAxis())
			{
				//������Ƕ�������K�ߣ�ֱ��ʹ��ͼ�������
				pSeries->SetMinMaxValue(m_pAxisY->GetMinValue(), m_pAxisY->GetMaxValue());
			}
			//���÷�ת
			pSeries->GetAxisVert()->SetReVert(m_pChartCtrl->IsRevert());
		}
	}

	//ָ��
	pos = m_mapIndiCatorSeries.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;
		int nKey = -1;
		m_mapIndiCatorSeries.GetNextAssoc(pos, nKey, pSeries);

		if (pSeries && pSeries->GetAxisVert())
		{
			pSeries->GetAxisVert()->CopyRect(m_pAxisY);
			if (!pSeries->IsOnwnAxis())
			{
				pSeries->SetMinMaxValue(m_pAxisY->GetMinValue(), m_pAxisY->GetMaxValue());

			}
			else
			{
				//����Ƕ��������ָ�꣬��Ҫ����һ�£�Ȼ�����ø���
				//GetMinMaxValu�Ѿ������Ż���ֻ�к�����䶯�Ż���㣬����ֱ�ӷ����ϴμ���Ľ��
				double fMin, fMax;
				if (pSeries->GetMinMaxValue(fMin, fMax))
				{
					if (fabs(fMax - fMin) < 0.0001)
					{
						fMax *= 1.1;
						fMin *= 0.9;
					}
					double ll = (fMax - fMin) / ES_Color()->fAxis_Y_Zoom;

					double fff = (ll - (fMax - fMin)) / 2;
					if (fMin == 0)
					{
						pSeries->SetMinMaxValue(0, fMax + fff);
					}
					else
						pSeries->SetMinMaxValue(fMin - fff, fMax + fff);
				}
			}
		}
	}

	//����ÿ�����е�����
	pos = m_mapTradePoint.GetStartPosition();

	while (pos)
	{
		CSeriesObject *pSeries = NULL;

		int nKey = -1;
		m_mapTradePoint.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries && pSeries->GetAxisVert())
		{
			pSeries->GetAxisVert()->CopyRect(m_pAxisY);
			//���÷�ת
			pSeries->GetAxisVert()->SetReVert(m_pChartCtrl->IsRevert());
			pSeries->SetMinMaxValue(m_pAxisY->GetMinValue(), m_pAxisY->GetMaxValue());
		}
	}

	//����ÿ�����е�����
	pos = m_mapAccTradePoint.GetStartPosition();

	while (pos)
	{
		CSeriesObject *pSeries = NULL;

		int nKey = -1;
		m_mapAccTradePoint.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries && pSeries->GetAxisVert())
		{
			pSeries->GetAxisVert()->CopyRect(m_pAxisY);
			//���÷�ת
			pSeries->GetAxisVert()->SetReVert(m_pChartCtrl->IsRevert());
			pSeries->SetMinMaxValue(m_pAxisY->GetMinValue(), m_pAxisY->GetMaxValue());
		}
	}

	m_userTradePoint.GetAxisVert()->CopyRect(m_pAxisY);
	//���÷�ת
	m_userTradePoint.GetAxisVert()->SetReVert(m_pChartCtrl->IsRevert());
	m_userTradePoint.SetMinMaxValue(m_pAxisY->GetMinValue(), m_pAxisY->GetMaxValue());

	if (m_pAxisSub)
	{
		double fMin = m_pAxisY->GetMinValue();
		double fMax = m_pAxisY->GetMaxValue();
		m_pAxisSub->SetMinMaxValue(fMin, fMax);
	}
}
bool CKChart::HitTest(ObjectInfo& info, CPoint pt)
{
	return true;
}
void CKChart::CalcMinMaxValue()
{
	double fZoom = ES_Color()->fAxis_Y_Zoom;
	if (fabs(fZoom) < 0.0001) fZoom = 0.9;

	if (m_pMainK)
	{
		double fMin = 10000000, fMax = -10000000;
		BOOL bCaluOk = FALSE;
		if (m_pAxisX->GetDataCount() > 0)
		{
			SPOSITION pos = m_mapCandleSeries.GetStartPosition();
			while (pos)
			{
				int nKey = -1;
				CSeriesObject *pSeries = NULL;
				m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
				if (pSeries)
				{
					double curfMin, curfMax;
					if (pSeries->GetMinMaxValue(curfMin, curfMax))
					{
						if (m_pMainK == pSeries)
						{
							fMin = min(curfMin, fMin);
							fMax = max(curfMax, fMax);
							bCaluOk = TRUE;
						}
						else
						{
							if (pSeries->GetAxisVert())
								pSeries->GetAxisVert()->SetMinMaxValue(curfMin, curfMax);
						}
					}
				}
			}

			if (bCaluOk)
			{
				SPOSITION pos = m_mapIndiCatorSeries.GetStartPosition();
				while (pos)
				{
					CSeriesObject *pSeries = NULL;
					int nKey = -1;
					m_mapIndiCatorSeries.GetNextAssoc(pos, nKey, pSeries);
					double curfMin, curfMax;
					if (pSeries&& pSeries->GetMinMaxValue(curfMin, curfMax))
					{
						fMin = min(curfMin, fMin);
						fMax = max(curfMax, fMax);
					}
				}
				if (fabs(fMax - fMin) < 0.0000001)
				{
					if (fabs(fMax) < 0.0000001)
					{
						fMax = 10;
						fMin = -10;
					}
					else
					{
						fMax *= 1.1;
						fMin *= 0.9;
					}
				}
				double ll = (fMax - fMin) / fZoom;
				double fff = (ll - (fMax - fMin)) / 2;
				m_pAxisY->SetMinMaxValue(fMin - fff, fMax + fff);
				m_pAxisY->SetOriginValue(m_pMainK->GetFirstVisibleValue());
			}
			m_pAxisY->SetPrect(m_pAxisX->GetContract()->pect, m_pAxisX->GetContract()->TickPrice);
		}

		if (!bCaluOk)
		{
			m_pAxisY->SetMinMaxValue(0, 99);
		}
	}
	else if (m_pMainIndicator)
	{
		double fMin, fMax;
		if (m_pMainIndicator->GetMinMaxValue(fMin, fMax))
		{
			if (fabs(fMin - fMax) < 0.0001)
			{
				fMax *= 1.1;
				fMin *= 0.9;
			}
			double ll = (fMax - fMin) / fZoom;

			double fff = (ll - (fMax - fMin)) / 2;
			if (fMin == 0)
			{
				m_pAxisY->SetMinMaxValue(0, fMax + fff);
			}
			else
			{
				m_pAxisY->SetMinMaxValue(fMin - fff, fMax + fff);
			}
			m_pAxisY->SetOriginValue(m_pMainIndicator->GetFirstVisibleValue());
			m_pAxisY->SetPrect(m_pMainIndicator->GetAxisVert()->GetPrect(), m_pMainIndicator->GetAxisVert()->GetMinTick());
		}
		else
		{
			m_pAxisY->SetMinMaxValue(0, 99);
		}
	}
	else
	{
		m_pAxisY->SetMinMaxValue(0, 99);
	}
	m_pAxisSub->SetMinMaxValue(m_pAxisY->GetMinValue(), m_pAxisY->GetMaxValue());
	m_pAxisSub->SetOriginValue(m_pAxisY->GetOriginValue());
}
void CKChart::KillFocus()
{
	//�ж��ĸ����б������ˣ���������Ϊ����״̬�������Ķ�����Ϊ�Ǽ���״̬
	SPOSITION pos = m_mapCandleSeries.GetStartPosition();
	while (pos)
	{
		int nKey;
		CSeriesObject *pSeries = NULL;
		m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Select(FALSE);
		}
	}
	//�ж��ĸ����б������ˣ���������Ϊ����״̬�������Ķ�����Ϊ�Ǽ���״̬
	pos = m_mapIndiCatorSeries.GetStartPosition();
	while (pos)
	{
		int nKey;
		CSeriesObject *pSeries = NULL;
		m_mapIndiCatorSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Select(FALSE);
		}
	}
	m_pCurAxisVert = NULL;

	Refresh();
}
void CKChart::DrawSeries(IRenderTarget* pRender)
{
	CRgn rgn, rgnOld;
	CRect rcClip;
	pRender->GetClipBox(&rcClip);

	rgn.CreateRectRgn(m_rcObj.left, m_rcObj.top, m_rcObj.right, m_rcObj.bottom);
	rgnOld.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);

	//���Ƹ�K��
	SPOSITION pos = m_mapCandleSeries.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;
		int nKey = -1;
		m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Draw(pRender);
		}
	}

	//���Ƹ�ָ��
	pos = m_mapIndiCatorSeries.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;
		int nKey = -1;
		m_mapIndiCatorSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Draw(pRender);
		}
	}

	//���Ƹ�����ָ��
	pos = m_mapTradePoint.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;
		int nKey = -1;
		m_mapTradePoint.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Draw(pRender);
		}
	}

	pos = m_mapAccTradePoint.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;

		int nKey = -1;
		m_mapAccTradePoint.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Draw(pRender);
		}
	}


	//�û�����ָ��
	m_userTradePoint.SetAxisHoriz(m_pAxisX);
	m_userTradePoint.SetChart(this);
	m_userTradePoint.Draw(pRender);

	//����K�߱�ǩ
	if (m_pDragingSub == NULL)
	{
		pos = m_mapCandleSeries.GetStartPosition();
		int i = 0;
		while (pos)
		{
			CSeriesObject *pSeries = NULL;

			int nKey = -1;
			m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
			if (pSeries && i > 0)
			{
				CCandleSeries *pCC = dynamic_cast<CCandleSeries*>(pSeries);
				pCC->DrawNameLabel(pRender);
			}
			i++;
		}
	}
	else
	{
		CPoint pt = GetCursorPos();
		int x = pt.x;
		int y = pt.y;
		DrawPointer(pRender, x, y, RGB(255, 255, 0), RGB(255, 255, 0), psCircle, 8, 8);
		DrawPointer(pRender, x, y, RGB(255, 0, 0), RGB(255, 255, 0), psCircle);
	}
}

void CKChart::DrawObjects(IRenderTarget* pRender, bool bRedraw /*= true*/)
{
	if (bRedraw)
	{
		//��������е�����
		CaluAixs(); 
		//����������
		DrawAxis(pRender);
		//����K�ߺ�ָ��
		DrawSeries(pRender);

		DrawZoomRect(pRender);

		DrawRegionRect(pRender);
		__super::DrawObjects(pRender, bRedraw);
	}
 	//���Ƽ۸���ʾ���
 	DrawPricePad(pRender);
 
 	//���������ϵ��α�
	m_pAxisY->DrawAxisTip(pRender);
 	m_pAxisSub->DrawAxisTip(pRender);
 
 	DrawInfoPanelButton(pRender);
 	if (m_bMainChart)
 		DrawNewPrice(pRender);
}
// ���Ƽ۸����
void CKChart::DrawPricePad(IRenderTarget* pRender)
{
	if (IsMouseLineVisble() && m_bMainChart)
	{
		CPoint pt = GetCursorPos();
		if (!m_pChartCtrl->GetClientRect().PtInRect(pt)) return;

		if (pt.x > m_rcObj.left && pt.x < m_rcObj.right)
		{
			CTimeAxis *pAxis = m_pAxisX;
			int nPrec = m_pAxisX->GetContract()->pect;
			int nDemor = 1;
			if (pAxis->GetHisData() == NULL || pAxis->GetHisData()->GetCount() == 0) return;

			int idx = (int)pAxis->Pix2Value(pt.x);
			if (idx >= pAxis->GetDataCount() - 1)
			{
				idx = pAxis->GetDataCount() - 1;
			}
			DC_KData pData = pAxis->GetHisData()->GetAt(idx);
			DC_KData pDataLast = pAxis->GetHisData()->GetAt(idx - 1);


			COLORREF clrNormal = ES_Color()->clrTxtNormal;
			COLORREF clrDown = ES_Color()->clrTxtDown;
			COLORREF clrUp = ES_Color()->clrTxtUp;
			pRender->SetTextColor(clrNormal);

			CSize sz; pRender->MeasureText(L"12345", _tcslen(L"12345"), &sz);
			int ht = sz.cy;
			ht = (int)(ht*1.3);

			CRect rcPad;
			rcPad.top = m_rcObj.top;
			
			int wid = GetStringPix(pRender, ES_Color()->ftAxis, GETSTRING(L"@string/IDS_10010")).cx;
			SStringW sstrHighPrice = NumericToString(pData.High, nPrec, nDemor);
			wid = max(wid, GetStringPix(pRender, ES_Color()->ftAxis, sstrHighPrice).cx) + 18;

			int x = GetCursorPos().x;
			if (x < wid * 2)
			{
				rcPad.left = m_rcObj.right - wid;
				rcPad.right = m_rcObj.right;
			}
			else
			{
				rcPad.left = 0;
				rcPad.right = wid;
			}

			rcPad.bottom = rcPad.top + ht * 20;
			if (rcPad.Height()>m_pAxisY->bottom) rcPad.bottom = m_pAxisY->bottom;
			DrawRect(pRender, rcPad, ES_Color()->clrBK, ES_Color()->clrBorder, 1, 200);

			int rcBottom = rcPad.top + 6;
			while (rcBottom + ht <= rcPad.bottom)
			{
				rcBottom += ht;
			}

			CRect rcClip;
			pRender->GetClipBox(&rcClip);

			CRect rcTxt(&rcPad);
			rcTxt.bottom = rcTxt.top + ht;
			CTime curTm = pData.time;
			SStringW strTmp;

			rcTxt.OffsetRect(0, 6);
			strTmp = GETSTRING(L"@string/IDS_10063");
			pRender->DrawText(strTmp ,strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 2 * ht);
			strTmp = GETSTRING(L"@string/IDS_10064");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 2 * ht);	
			strTmp = GETSTRING(L"@string/IDS_10065");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 2 * ht);
			strTmp = GETSTRING(L"@string/IDS_10066");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 2 * ht);
			strTmp = GETSTRING(L"@string/IDS_10067");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 3 * ht);
			strTmp = GETSTRING(L"@string/IDS_10068");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 3 * ht);
			strTmp = GETSTRING(L"@string/IDS_10069");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, 3 * ht);
			strTmp = GETSTRING(L"@string/IDS_10070");
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

			pRender->SetTextColor(pData.Close > pData.Open ? ES_Color()->clrTxtUp : ES_Color()->clrTxtDown);

			rcTxt.OffsetRect(0, -16 * ht);
			rcTxt.right -= 4;
			//���ƿ��̼�
			strTmp = NumericToString(pData.Open, nPrec, nDemor);
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//��߼�
			rcTxt.OffsetRect(0, 2 * ht);
			strTmp = NumericToString(pData.High, nPrec, nDemor);
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			//���
			rcTxt.OffsetRect(0, 2 * ht);
			strTmp = NumericToString(pData.Low, nPrec, nDemor);
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			//����
			rcTxt.OffsetRect(0, 2 * ht);
			strTmp = NumericToString(pData.Close, nPrec, nDemor);
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//�ɽ���
			pRender->SetTextColor(pData.Volume > pDataLast.Volume ? ES_Color()->clrTxtUp : ES_Color()->clrTxtDown);
			rcTxt.OffsetRect(0, 2 * ht);

			strTmp.Format(L"%lld", pData.Volume);
			strTmp = FormatLongInteger(strTmp);

			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//�ɽ�����
			pRender->SetTextColor(pData.Volume > pDataLast.Volume ? ES_Color()->clrTxtUp : ES_Color()->clrTxtDown);
			rcTxt.OffsetRect(0, ht);

			strTmp.Format(L"%lld", pData.Volume - pDataLast.Volume);
			strTmp = FormatLongInteger(strTmp);

			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//�ɽ����
			pRender->SetTextColor(pData._Amount > pDataLast._Amount ? ES_Color()->clrTxtUp : ES_Color()->clrTxtDown);
			rcTxt.OffsetRect(0, 2 * ht);

			strTmp.Format(L"%lld", pData._Amount);
			strTmp = FormatLongInteger(strTmp);

			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//�ɽ�����
			pRender->SetTextColor(pData._Amount > pDataLast._Amount ? ES_Color()->clrTxtUp : ES_Color()->clrTxtDown);
			rcTxt.OffsetRect(0, ht);

			strTmp.Format(L"%lld", pData._Amount - pDataLast._Amount);
			strTmp = FormatLongInteger(strTmp);

			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//�Ƿ�
			pRender->SetTextColor(pData.Close > pDataLast.Close ? ES_Color()->clrTxtUp : ES_Color()->clrTxtDown);
			rcTxt.OffsetRect(0, 2 * ht);
			if (pDataLast.Close == 0 || pDataLast.Close == InvalidNumeric)
			{
				strTmp = L"--";
			}
			else
			{
				strTmp = NumericToString(pData.Close - pDataLast.Close, nPrec, nDemor);
			}
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			rcTxt.OffsetRect(0, ht);
			if (pDataLast.Close == 0 || pDataLast.Close == InvalidNumeric)
			{
				strTmp = L"--";
			}
			else
			{
				strTmp.Format(L"%.2f%%", (pData.Close - pDataLast.Close)*100.0 / pDataLast.Close);
			}

			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			//���
			pRender->SetTextColor(ES_Color()->clrTxtNormal);
			rcTxt.OffsetRect(0, 2 * ht);
			if (pDataLast.Close == 0 || pDataLast.Close == InvalidNumeric)
			{
				strTmp = L"--";
			}
			else
			{
				strTmp.Format(L"%.2f%%", (pData.High - pData.Low) * 100 / pDataLast.Close);
			}
			pRender->DrawText(strTmp, strTmp.GetLength(), &rcTxt, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
		}
	}
}


// ���ƴ���Ϣ��尴ť
void CKChart::DrawInfoPanelButton(IRenderTarget* pRender)
{
	SASSERT(m_pChartCtrl);
	if (m_pAxisY&& m_bMainChart && m_pChartCtrl)
	{
		CRect &rcPad = m_pChartCtrl->GetInfoPanelRect();
		rcPad.top = top + m_pChartCtrl->GetTitleRect().Height() / 6;
		rcPad.left = m_pAxisY->right - 5 * m_pChartCtrl->GetTitleRect().Height() / 6;
		rcPad.right = m_pAxisY->right - m_pChartCtrl->GetTitleRect().Height() / 6;
		rcPad.bottom = top + 5 * m_pChartCtrl->GetTitleRect().Height() / 6;

		CRect rcI = rcPad;
		rcI.left = (LONG)(rcI.right - rcPad.Width()*0.4);
		DrawLine(pRender,PS_SOLID, rcI.left, rcI.top, rcI.left, rcI.bottom - 1, ES_Color()->clrBorder);
		DrawRect(pRender, rcI, ES_Color()->clrBorder, ES_Color()->clrBorder, 1, 100);
	}
}
// �������¼�
void CKChart::DrawNewPrice(IRenderTarget* pRender)
{
	int nPrec = m_pAxisX->GetContract()->pect;
	int nDemor = 1;
	if (m_pAxisX->GetHisData() == NULL || m_pAxisX->GetHisData()->GetCount() == 0)
		return;
	DC_KData pData = m_pAxisX->GetHisData()->GetAt(m_pAxisX->GetDataCount() - 1);
	//���¼�
	SStringW strTmp = NumericToString(pData.Close, nPrec, nDemor);
	int hight = m_pAxisY->Value2Pix(pData.Close);
	CSize sz;
	pRender->MeasureText(strTmp, strTmp.GetLength(), &sz);
	int ht = sz.cy;
	CRect rt(m_pAxisY);
	rt.top = hight - ht / 2;
	rt.bottom = rt.top + ht;
	rt.left++;
	rt.right = rt.left + sz.cx + ht / 2 + 8;

	POINT pts[] = { { rt.left, hight }, { ht / 2, -ht / 2 }, { rt.Width() - ht / 2, 0 }, { 0, ht }, { -(rt.Width() - ht / 2) }, { -ht / 2 - 1, -ht / 2 - 1 } };


	//GFillPath(pRender, pts, 6, ES_Color()->clrBK, ES_Color()->clrBK, ES_Color()->clrBK, ES_Color()->clrBK);
	//DrawPath(pRender, pts, 6, ES_Color()->clrBorder);

	pRender->SetTextColor(ES_Color()->clrTxtNormal);
	rt.left += (ht / 2 + 4);
	//�������¼�
	pRender->DrawText(strTmp, strTmp.GetLength(), &rt, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}


int GetBtnRect(int place, int topbase, int bottombase, int setup)
{
	int y = 0;
	switch (place)
	{
		case BOTTOM_POS_TOP1:
		case BOTTOM_POS_TOP2:
		case BOTTOM_POS_TOP3:
		{
			y = topbase + (place - BOTTOM_POS_TOP1)*setup;
		}break;
		case BOTTOM_POS_BOTTOM1:
		case BOTTOM_POS_BOTTOM2:
		case BOTTOM_POS_BOTTOM3:
		{
			y = bottombase - (place - BOTTOM_POS_BOTTOM1)*setup;
		}break;
	}
	return y;
}
void CKChart::DrawTipButtons(IRenderTarget* pRender)
{
	for (size_t i = 0; i < m_arrButtons.GetCount(); i++)
	{
		int x = m_pAxisX->Value2Pix(m_arrButtons[i].barIdx);
		if (x< left - 20 || x> right + 20) continue;
		int y = GetBtnRect(m_arrButtons[i].place, m_rcObj.top + 4, bottom - 20, 20);
		m_arrButtons[i].rc.SetRect(x + 4, y + 4, x + 14, y + 14);
		//m_pBrowser->m_imageTipButton.Draw(pRender, m_arrButtons[i].nImage, CPoint(x, y), ILD_TRANSPARENT);
	}
}
void CKChart::SetColorSeries(CSeriesData<SeriesItem> *pSeries)
{
	if (m_pClrSeires == NULL)
		m_pClrSeires = pSeries;
}
void CKChart::RemoveColorSeries(CSeriesData<SeriesItem> *pSeries)
{
	if (m_pClrSeires == pSeries)
	{
		m_pClrSeires = NULL;
	}
}
bool CKChart::OnDrag(CPoint& from, CPoint &pt, bool bLeft)
{
	if (m_pDragingSub) return true;
	{
		Refresh();
	}
	SPOSITION pos = m_mapCandleSeries.GetStartPosition();
	int i = 0;
	while (pos)
	{
		//�ж��ĸ����б������ˣ���������Ϊ����״̬�������Ķ�����Ϊ�Ǽ���״̬
		int nKey;
		CSeriesObject *pSeries = NULL;
		m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries && i > 0)
		{
			CCandleSeries *pCandleSeries = dynamic_cast<CCandleSeries*>(pSeries);

			if (pCandleSeries->m_rcDrager.PtInRect(from))
			{
				m_pDragingSub = pCandleSeries;

				m_idDragingSub = nKey;
				Refresh();
				return true;
			}
		}
		i++;
	}

	return __super::OnDrag(from, pt, bLeft);
}

bool CKChart::OnClick(CPoint &pt, bool bLeft/* = true*/)
{
	if (m_bMainChart)
	{
		if (m_pChartCtrl->GetInfoPanelRect().PtInRect(pt))
		{
			//::SendMessage(GetParent(m_pBrowser->m_pStatic->GetSafeHwnd()), CHART_MSG_BUTTON_CLICK, 0, 0);
			return true;
		}
	}

	m_pAxisX->SetFocusIndex(-1);

	if (IsDrawShape())
	{
		return __super::OnClick(pt, bLeft);
	}

	for (int i = 0; i < m_arrButtons.GetCount(); i++)
	{
		if (m_arrButtons[i].rc.PtInRect(pt))
		{
			auto it = m_arrButtons[i];
			//::SendMessage(GetParent(m_pBrowser->m_pStatic->GetSafeHwnd()), CHART_MSG_BUTTON_CLICK, 1, (LPARAM)m_arrButtons[i].lParam);
			return true;
		}
	}

	m_pSelSeries = NULL;
	BOOL bSel = FALSE;
	SPOSITION pos = m_mapCandleSeries.GetStartPosition();
	while (pos)
	{
		//�ж��ĸ����б������ˣ���������Ϊ����״̬�������Ķ�����Ϊ�Ǽ���״̬
		int nKey;
		CSeriesObject *pSeries = NULL;
		m_mapCandleSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Select(false);
			if (bSel)
			{
				//�Ѿ���ѡ�е��ˣ�ʣ�µ�ȫ��ȡ��ѡ��
				continue;
			}

			if (pSeries->HitTest(pt) != -1)
			{
				pSeries->Select(true);
				m_pSelSeries = pSeries;
				bSel = TRUE;
			}
		}
	}

	//���ָ��
	SPOSITION pos = m_mapIndiCatorSeries.GetStartPosition();
	while (pos)
	{
		//�ж��ĸ����б������ˣ���������Ϊ����״̬�������Ķ�����Ϊ�Ǽ���״̬
		int nKey;
		CSeriesObject *pSeries = NULL;
		m_mapIndiCatorSeries.GetNextAssoc(pos, nKey, pSeries);
		if (pSeries)
		{
			pSeries->Select(false);
			if (bSel)
			{
				//�Ѿ���ѡ�е��ˣ�ʣ�µ�ȫ��ȡ��ѡ��
				continue;
			}

			int nSel = pSeries->HitTest(pt);
			if (nSel != -1)
			{
				if (nSel == -2)
					nSel = -1;
				pSeries->Select(TRUE, nSel);
				m_pSelSeries = pSeries;
				bSel = TRUE;
			}
		}
	}

	//���ѡ����һ�����У�����������е�����������������
	m_pCurAxisVert = NULL;
	if (m_pSelSeries != NULL/* && m_pSelSeries->IsOnwnAxis()*/)
	{
		m_pCurAxisVert = m_pSelSeries->GetSelectAxis();
	}
	Refresh();
	if (!bSel)
	{
		if (m_bHaveSelObject)
		{
			m_bHaveSelObject = FALSE;
			return true;
		}
		else
		{
			if (pt.y > m_rcObj.top)
				return __super::OnClick(pt, bLeft);
			else
				return true;
		}
	}
	else
	{
		m_zoomRc.SetRectEmpty();//��ʱ���þ������������Ϊ���߸����OnClick��
		m_bHaveSelObject = TRUE;
		return true;
	}
}

bool CKChart::OnMouseMove(CPoint& pt)
{
	for (size_t i = 0; i < m_arrButtons.GetCount(); i++)
	{
		if (m_arrButtons[i].type != 0) continue;
		if (m_arrButtons[i].rc.PtInRect(pt))
		{
		}
	}

	bool bUpdate = false;
	SPOSITION pos = m_mapIndiCatorSeries.GetStartPosition();
	while (pos)
	{
		CSeriesObject *pSeries = NULL;
		int nKey = -1;
		m_mapIndiCatorSeries.GetNextAssoc(pos, nKey, pSeries);
		//�ж��ĸ����б������ˣ���������Ϊ����״̬�������Ķ�����Ϊ�Ǽ���״̬

		if (pSeries)
		{
			CIndicatorGroup* pIndiGroup = (CIndicatorGroup*)pSeries;
			CRect tmRc(pIndiGroup->GetTitleRect());
			if (pIndiGroup && tmRc.PtInRect(pt) && !m_pChartCtrl->IsMouseDown())
			{
				bUpdate = true;
			}
		}
	}
	if (bUpdate)
		Refresh();
	return CShapeChart::OnMouseMove(pt);
}