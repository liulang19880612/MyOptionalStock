#include "stdafx.h"
#include "CandleChart.h"
#include "DynaChart.h"
#include "TimeAxis.h"



CDynaChart::CDynaChart():
	m_nLastDayAmount(0),
	m_fMin(900),
	m_fMax(1100),
	m_fYClose(1000),
	m_bSimHistory(false),
	m_nDynaDays(1),
	m_bHaveDestroy(false)
{
	m_bShowSubAixs = true;
	m_pect = 0;
	m_fMinMove = 1;
	m_lastDataIdx = -1;
	m_RealDataCount = 0;

	m_strSymbol = L"无数据";
	m_bCanDragAxis = false;
	m_pAxisY = m_pAxisSub = NULL;
	m_bIsBuffing = false;

}
CDynaChart::~CDynaChart()
{
	m_bHaveDestroy = true;
}
void CDynaChart::SetOriginPrice(double fPrice, int idK)
{
	if (idK == 0)
	{
		m_fYClose = fPrice;
	}
	else
	{
		for (size_t i = 0; i < m_arrAppendData.GetCount(); i++)
		{
			if (m_arrAppendData[i].id == idK)
			{
				m_arrAppendData[i].fOriginPrice = fPrice;
			}
		}
	}
}

void CDynaChart::UpdateHisData(const DC_KData* pData, int nCount, int nData, int idK)
{
	if (m_bHaveDestroy) return;

	SASSERT(nCount >= nData);

	if (idK > 0)
	{
		for (size_t i = 0; i < m_arrAppendData.GetCount(); i++)
		{
			if (m_arrAppendData[i].id == idK)
			{
				for (int j = 0; j < nData; j++)
				{
					m_arrAppendData[i].arrData[j] = pData[j];
				}
				m_arrAppendData[i].nLastDataIdx = nData - 1;

				double xxMax = -1000000;
				double xxMin = 1000000;
				for (int j = 0; j < m_RealDataCount; j++)
				{
					if (m_arrAppendData[i].arrData.GetCount() <= (size_t)j) break;
					if (m_arrAppendData[i].arrData[j].Close == InvalidNumeric) continue;
					xxMax = max(m_arrAppendData[i].arrData[j].High, xxMax);
					xxMin = min(m_arrAppendData[i].arrData[j].Low, xxMin);
				}
				if (xxMax < 0.0001)
				{
					xxMax = 100;
					xxMin = 0;
				}
				m_arrAppendData[i].fMax = xxMax;
				m_arrAppendData[i].fMin = xxMin;

			}
		}
		Refresh();
		return;
	}
	if (m_arrData.IsEmpty())
	{
		for (int i = 0; i < nCount; i++)
		{
			m_arrData.Add(pData[i]);
		}
	}
	else
	{
		for (int i = max(0, m_RealDataCount - 1); i < nData; i++)
		{
			m_arrData[i] = pData[i];
		}
	}
	UpdateData(nData);
	Refresh();
}
void CDynaChart::UpdateData(int nDataCount)
{
	if (nDataCount < 0) return;

	if (m_RealDataCount < 0)
	{
		m_RealDataCount = 0;
	}
	if (m_arrData.GetCount() < 1) return;

	if (m_RealDataCount == 0)
	{
		m_fMin = DBL_MAX;
		m_fMax = DBL_MIN;
		for (int i = 0; i < nDataCount; i++)
		{
			m_fMax = max(m_arrData[i].value, m_fMax);
			m_fMin = min(m_arrData[i].value, m_fMin);
		}
	}
	else
	{
		for (int i = m_RealDataCount - 1; i < nDataCount; i++)
		{
			m_fMax = max(m_arrData[i].value, m_fMax);
			m_fMin = min(m_arrData[i].value, m_fMin);
		}
	}

	m_RealDataCount = nDataCount;
	if (m_RealDataCount == 0)
	{
		if (m_fYClose != 0)
		{
			m_fMin = m_fYClose*0.9;
			m_fMax = m_fYClose*1.1;
		}
		else
		{
			m_fMin = 0;
			m_fMax = 100;
		}

	}
	m_dynaData.SetData(m_arrData.GetData(), m_arrData.GetCount(), m_pect, true);
	m_lastDataIdx = m_RealDataCount - 1;

	SASSERT(m_RealDataCount >= 0);

	m_pAxisX->UpdateHisData();
	m_pAxisX->SetMinMaxValue(0, m_arrData.GetCount() - 1);
	m_pAxisX->m_nDataCount =  m_RealDataCount;
}

void CDynaChart::CalcMinMaxValue()
{
	if (!m_bMainChart)
	{
		__super::CalcMinMaxValue();
		return;
	}
	if (m_fYClose == 0)
	{
		return;
	}

	m_pAxisSub->SetOriginValue(m_fYClose);
	m_pAxisY->SetOriginValue(m_fYClose);

	for (int i = 1; i<10000; i++)
	{
		double curV = i*m_pAxisX->GetMinTick() * 10; //增长比例

		double fMin = m_fYClose - curV;
		double fMax = m_fYClose + curV;

		if (fMax >= m_fMax && fMin <= m_fMin)
		{
			m_pAxisY->SetMinMaxValue(fMin, fMax);
			m_pAxisSub->SetMinMaxValue(fMin, fMax);
			return;
		}
	}
}

void CDynaChart::DrawSeries(IRenderTarget* pRender)
{
	__super::DrawSeries(pRender);
	if (m_bMainChart)
	{
		//绘制分时线
		DrawDynaSeries(pRender);
	}
}

void CDynaChart::DrawDynaSeries(IRenderTarget *pRender)
{
	CRect rcBody = m_rcObj;
	if (m_dynaData.IsEmpty()) return;
	if (m_lastDataIdx < 0) return;

	size_t cnt = min(m_arrData.GetCount(), (size_t)(m_lastDataIdx + 1));
	int x, y;
	bool bFirst = true;

	SArray<CPoint> arrPoint;
	//价格线
	for (size_t i = 0; i < cnt; i++)
	{
		DC_KData& data = m_dynaData.GetAt(i);
		if (data.Close == InvalidNumeric)
		{
			continue;
		}
		x = m_pAxisX->Value2Pix(i);
		y = m_pAxisY->Value2Pix(data.value);
		arrPoint.Add(CPoint(x, y));
	}

	DrawPolyLine(pRender, PS_SOLID, arrPoint.GetData(), arrPoint.GetCount(), ES_Color()->clrDynaLine);
	if (arrPoint.GetCount() > 0 && m_arrAppendData.GetCount() > 0)
	{
		x = arrPoint[arrPoint.GetCount() - 1].x;
		y = arrPoint[arrPoint.GetCount() - 1].y;
		DrawKName(pRender, x, y, S_CA2W(m_Contract.ContractName), ES_Color()->clrDynaLine);
	}


	//绘制叠加K线
// 	COLORREF appClr[] = { RGBA(255, 0, 255, 255), RGBA(0, 255, 0, 255), RGBA(200, 160, 0, 255), RGBA(0, 255, 255, 255) };
// 
// 	for (size_t i = 0; i < m_arrAppendData.GetCount(); i++)
// 	{
// 		m_arrAppendData[i].pAxisVert->CopyRect(m_pAxisY);
// 		m_arrAppendData[i].pAxisVert->SetOriginValue(m_arrAppendData[i].fOriginPrice);
// 		for (int j = 1; j < 10000; j++)
// 		{
// 			double curV = j*m_pAxisX->GetMinTick() * 10; //增长比例
// 
// 			double fMin = m_arrAppendData[i].fOriginPrice - curV;
// 			double fMax = m_arrAppendData[i].fOriginPrice + curV;
// 
// 			if (fMax >= m_arrAppendData[i].fMax && fMin <= m_arrAppendData[i].fMin)
// 			{
// 				m_arrAppendData[i].pAxisVert->SetMinMaxValue(fMin, fMax);
// 				break;
// 			}
// 		}
// 
// 		SArray<DC_KData>& cur = m_arrAppendData[i].arrData;
// 		arrPoint.RemoveAll();
// 		bFirst = true;
// 		COLORREF clrK = appClr[i % 4];
// 		for (size_t j = 0; j < cnt; j++)
// 		{
// 			if (j >= cur.GetCount()) break;
// 
// 			DC_KData& data = cur[j];
// 			if (data.Close == InvalidNumeric)
// 			{
// 				continue;
// 			}
// 			x = m_pAxisX->Value2Pix(j);
// 			if (j == 0)
// 			{
// 				y = m_arrAppendData[i].pAxisVert->Value2Pix(data.Open);
// 				arrPoint.Add(CPoint(m_pAxisX->left, y));
// 			}
// 
// 			y = m_arrAppendData[i].pAxisVert->Value2Pix(data.Close);
// 			arrPoint.Add(CPoint(x, y));
// 		}
// 		DrawPolyLine(pRender, PS_SOLID, arrPoint.GetData(), arrPoint.GetCount(), clrK);
// 		if (m_arrAppendData[i].nLastDataIdx >= 0)
// 		{
// 			x = m_pAxisX->Value2Pix(m_arrAppendData[i].nLastDataIdx);
// 			y = m_arrAppendData[i].pAxisVert->Value2Pix(cur[m_arrAppendData[i].nLastDataIdx].Close);
// 			DrawKName(pRender, x, y, S_CA2W(m_arrAppendData[i].ci.ContractName), clrK);
// 
// 		}
// 	}

// 	// 绘制均价线
// 	double fAmount = 0;  //今日成交额(∑成交量*价格)
// 	unsigned long long   fVolume = 0;  //今日成交量(∑成交量)
// 
// 	m_arrAvg.RemoveAll();
// 	bFirst = true;
// 
// 	double fAvgPrice = m_arrData[0].Close;
// 	arrPoint.RemoveAll();
// 	for (size_t i = 0; i < cnt; i++)
// 	{
// 		DC_KData &data = m_dynaData.GetAt(i);
// 		if (data.Close == InvalidNumeric)
// 		{
// 			break;
// 		}
// 		x = m_pAxisX->Value2Pix(i);
// 		if (bFirst)
// 		{
// 			y = m_pAxisY->Value2Pix(data.Close);
// 			arrPoint.Add(CPoint(x, y));
// 			bFirst = FALSE;
// 		}
// 
// 		if ((int)data.Volume < 0)
// 			data.Volume = 0;
// 
// 		fAmount += data.Volume * data.Close;
// 		fVolume += data.Volume;
// 
// 		if (fVolume != 0)
// 			fAvgPrice = fAmount / fVolume;
// 
// 		m_arrAvg.Add(fAvgPrice);
// 		y = m_pAxisY->Value2Pix(fAvgPrice);
// 		arrPoint.Add(CPoint(x, y));
// 	}
// 	DrawPolyLine(pRender, PS_SOLID, arrPoint.GetData(), arrPoint.GetCount(), ES_Color()->clrDynaLine2);
}

void CDynaChart::CreateAxis()
{
	if (m_pAxisY)
	{
		delete m_pAxisY;
	}
	if (m_pAxisSub)
	{
		delete m_pAxisSub;
	}

	if (m_bMainChart)
	{
		m_pAxisY = new CDynaPctAxis(FALSE, this);
		m_pAxisY->SetAlign(0);
		m_pAxisSub = new CDynaPctAxis(FALSE, this);
		m_pAxisSub->SetAlign(1);
		m_pAxisX->SetHisData(&m_dynaData);
	}
	else
	{
		m_pAxisY = new CAxis(FALSE, this);
		m_pAxisY->SetAlign(0);
		m_pAxisSub = new CAxis(FALSE, this);
		m_pAxisSub->SetAlign(1);
	}
}
void  CDynaChart::ClearData()
{
	if (!m_arrData.IsEmpty())
		m_arrData.RemoveAll();

	if (!m_arrAvg.IsEmpty())
		m_arrAvg.RemoveAll();
	m_lastDataIdx = -1;
	m_RealDataCount = 0;
	m_arrAppendData.RemoveAll();
}
int CDynaChart::AppendK(DC_ContractInfo* ci)
{
	static int id = 1;

	for (size_t i = 0; i < m_arrAppendData.GetCount(); i++)
	{
		if (ContractEqual(m_arrAppendData[i].ci, *ci)) return -1;
	}
	AppendKItem item;
	item.ci = *ci;
	item.id = id++;
	item.nLastDataIdx = -1;
	item.fOriginPrice = 0;
	item.pAxisVert = new CAxis(FALSE, this);
	m_arrAppendData.Add(item);

	return item.id;
}


bool CDynaChart::RemoveK(DC_ContractInfo* ci)
{
	for (size_t i = 0; i < m_arrAppendData.GetCount(); i++)
	{
		if (ContractEqual(m_arrAppendData[i].ci, *ci))
		{
			m_arrAppendData.RemoveAt(i);
			return true;
		}
	}
	return false;
}
void CDynaChart::SetStock(DC_ContractInfo* pCode)
{
	if (pCode == NULL) return;

	m_Contract = *pCode;
	ClearData();
	m_strSymbol = S_CA2W(pCode->ContractNo);
	m_strSymbol += L" ";
	m_strSymbol += S_CA2W(pCode->ContractName);
	m_pect = pCode->pect;
	m_fMinMove = pCode->TickPrice;
	m_bIsBuffing = true;

	m_pAxisX->SetPrect(m_pect, m_fMinMove);
	m_pAxisSub->SetPrect(m_pect, m_fMinMove);
}
bool CDynaChart::HitTest(ObjectInfo& info, CPoint pt)
{
	return true;
}

