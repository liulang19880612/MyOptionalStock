#include "stdafx.h"
#include "RectChart.h"
#include "ChartCtrl.h"

CRectChart::CRectChart()
{
	m_RateSize = 0.15;
	m_pChartCtrl = NULL;
	SetRectEmpty();
	m_nChartIndex = -1;
}
CRectChart::~CRectChart()
{

}
void CRectChart::Refresh(bool quick/* = false*/, bool bliji/* = false*/)
{

}
void CRectChart::ReCalcLayout()
{

}
bool CRectChart::PtInChartCtrl(CPoint pt)
{
	return m_pChartCtrl->GetClientRect().PtInRect(pt) == TRUE;
}
void CRectChart::SetCursor(int type)
{
	m_pChartCtrl->SetCursor((CURSOR_TYPE)type);
}

bool  CRectChart::IsMouseLineVisble()
{
	return m_pChartCtrl->IsMouseLineVisible(DC_KData());
}

void CRectChart::Draw(IRenderTarget*pRender, bool bRedraw /*= true*/)
{
	DrawObjects(pRender, bRedraw);
}
void CRectChart::DrawIcon(IRenderTarget*pRender, int x, int y, int idx)
{
	if (m_pChartCtrl)
	{
		m_pChartCtrl->DrawIcon(pRender, x, y, idx);
	}
}

CPoint CRectChart::GetCursorPos()
{
	return m_pChartCtrl->GetCursorPos();
}