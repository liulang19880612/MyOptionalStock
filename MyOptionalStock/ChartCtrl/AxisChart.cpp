#include "stdafx.h"
#include "AxisChart.h"
#include "TimeAxis.h"
#include "ChartCtrl.h"

CAxisChart::CAxisChart():m_bDragingPricePad(false)
{
	m_pAxisY = NULL;
	m_pAxisSub = NULL;
	m_bCanDragAxis = true;
	m_pAxisX = NULL;
	m_bMainChart = false;
	m_bAutoAxis = true;
	m_zoomRc.SetRectEmpty();
	m_curDragingAxis = -1;
	m_bDragingScrollBar = false;
	m_pCurAxisVert = NULL;
	m_bShowSubAixs = false;
	m_regionRc.SetRectEmpty();


}
CAxisChart::~CAxisChart()
{

}

void CAxisChart::ReCalcLayout()
{
	m_rcObj.CopyRect(this);
	m_rcObj.top = m_pChartCtrl->GetTitleRect().bottom;

	int nAxisYWidth = m_pChartCtrl->GetChatAxisYFitWidth();
	if (m_pChartCtrl->IsShowAxis())
	{
		m_rcObj.right -= nAxisYWidth;
	}

	// 计算Y坐标区域
	CRect rcY(*this);
	rcY.bottom++;
	rcY.left = m_rcObj.right;
	rcY.top = m_pChartCtrl->GetTitleRect().bottom;
	if (m_bShowSubAixs)
	{
		m_pAxisY->CopyRect(&rcY);
		rcY.MoveToX(left);
		m_rcObj.left += rcY.Width();
		m_pAxisSub->CopyRect(&rcY);

		m_pAxisY->SetAlign(1);
		m_pAxisSub->SetAlign(0);
	}
	else
	{
		m_pAxisY->CopyRect(&rcY);
		m_pAxisY->SetAlign(1);
		for (size_t i = 0; i < m_arrOwnAixs.GetCount(); i++)
		{
			m_arrOwnAixs[i]->CopyRect(&rcY);
			m_arrOwnAixs[i]->SetAlign(1);
		}
	}
}

void CAxisChart::SetAxisYWith(int nWidth)
{
	m_pAxisY->SetFitWidth(nWidth);
}
int CAxisChart::GetAxisYWith()
{
	return m_pAxisY->GetFitWidth();
}
bool CAxisChart::OnDrag(CPoint& from, CPoint &pt, bool bLeft)
{
	if (!m_bCanDragAxis) return false;

	m_bIsLeftDraging = bLeft;
	if (m_curDragingAxis == -1)
	{
		if (m_pAxisX->PtInRect(from))
		{
			//正拖动横坐标
			m_bAutoAxis = true;		//设置自动调整纵坐标
			m_curDragingAxis = 0;
			//SetCapture();
			m_fLastAxisMin = m_pAxisX->GetMinValue();
			m_fLastAxisMax = m_pAxisX->GetMaxValue();
			m_nLastDuration = static_cast<int>(m_pAxisX->GetDuration());

			if (m_pAxisX->GetScrllBarRect().PtInRect(from))
			{
				m_pAxisX->EnableDraging(true);
				m_bDragingScrollBar = true;
				m_nOffsetScrollBar = from.x - m_pAxisX->GetScrllBarRect().left;
			}


		}
		else if (m_pAxisY->PtInRect(from))
		{
			if (!m_bMainChart) return false;
			//正拖动纵坐标
			m_bAutoAxis = false;
			m_fLastAxisMin = m_pAxisY->GetMinValue();
			m_fLastAxisMax = m_pAxisY->GetMaxValue();
			m_curDragingAxis = 1;
			//SetCapture();
		}
		else if (m_bShowSubAixs && m_pAxisSub->PtInRect(from))
		{
			if (!m_bMainChart) return false;
			//正拖动纵坐标
			m_bAutoAxis = false;
			m_fLastAxisMin = m_pAxisSub->GetMinValue();
			m_fLastAxisMax = m_pAxisSub->GetMaxValue();
			m_curDragingAxis = 2;
			//SetCapture();
		}
		else
		{
			//SetCapture();
		}
	}

	if (m_curDragingAxis == 0)
	{
		//拖动横坐标
		if (!bLeft)
		{
			//计算缩放后的坐标
			int pixOffSet = pt.x - from.x;
			int allOther = static_cast<int>(m_nLastDuration - m_fLastAxisMin);
			int oldW = m_pAxisX->right - from.x;
			int newW = m_pAxisX->right - pt.x;

			if (newW <= 0) newW = 1;
			double fRange = (float)newW / (float)oldW;
			double fOldRange = (m_fLastAxisMax - m_fLastAxisMin) / allOther;

			double fLength = allOther * fOldRange / fRange;
			if (fLength <= MIN_CANDLE_NUM) fLength = MIN_CANDLE_NUM - 1;
			if (fLength > m_pChartCtrl->GetMaxKNum())
				fLength = m_pChartCtrl->GetMaxKNum();

			int newMin = static_cast<int>(m_pAxisX->GetMaxValue() - fLength);
			if (newMin<0) newMin = 0;
			m_pAxisX->m_nScreenCandleNum = static_cast<int>(m_pAxisX->GetMaxValue() - newMin + 1);
			m_pAxisX->SetMinMaxValue(newMin, m_pAxisX->GetMaxValue());
			Refresh();
		}
		else
		{
			if (m_bDragingScrollBar)
			{
				//拖动滚动条
				int curScrollBarLeft = pt.x - m_nOffsetScrollBar;
				int barWid = m_pAxisX->GetScrllBarRect().Width();

				if (curScrollBarLeft <= m_pAxisX->left)
					curScrollBarLeft = m_pAxisX->left;
				if (curScrollBarLeft + barWid > m_pAxisX->right) curScrollBarLeft = m_pAxisX->right - barWid;

				double fRate = ((double)curScrollBarLeft - m_pAxisX->left) / (m_pAxisX->Width() - barWid);

				int nMin = static_cast<int>((m_pAxisX->GetDuration() - (m_fLastAxisMax - m_fLastAxisMin)) * fRate);
				if (nMin < 0)
					nMin = 0;

				int nMax = static_cast<int>(nMin + (m_fLastAxisMax - m_fLastAxisMin));
				if (nMax > m_pAxisX->GetDuration()) nMax = static_cast<int>(m_pAxisX->GetDuration());

				m_pAxisX->SetMinMaxValue(nMin, nMax);
			}
			else
			{
				//拖动坐标栏
				int pixOffSet = from.x - pt.x;
				if (!(pixOffSet < 0 && m_fLastAxisMin <= 0))
				{
					double fAdd = (m_fLastAxisMax - m_fLastAxisMin) * ((double)pixOffSet) / (m_pAxisX->Width());

					int nMax = static_cast<int>(m_fLastAxisMax + fAdd);
					if (nMax > m_pAxisX->GetDuration()) nMax = static_cast<int>(m_pAxisX->GetDuration());

					int nMin = static_cast<int>(nMax - (m_fLastAxisMax - m_fLastAxisMin));
					if (nMin < 0) nMin = 0;

					m_pAxisX->SetMinMaxValue(nMin, nMax);
				}
				Refresh();
			}
			Refresh();
		}
		return true;
	}
	else if (m_curDragingAxis == 1 || m_curDragingAxis == 2)
	{
		//拖动纵坐标
		if (bLeft)
		{
			int pixOffSet = pt.y - from.y;
			double fAdd = (m_fLastAxisMax - m_fLastAxisMin) * ((double)pixOffSet) / (m_pAxisY->Height());
			if (m_pAxisY->IsRevert()) fAdd *= -1;
			m_pAxisY->SetMinMaxValue(m_fLastAxisMin + fAdd, m_fLastAxisMax + fAdd);
			m_pAxisSub->SetMinMaxValue(m_fLastAxisMin + fAdd, m_fLastAxisMax + fAdd);
			Refresh();
		}
		else
		{
			//计算缩放后的坐标
			double oldW = m_pAxisY->bottom - from.y;
			double newW = m_pAxisY->bottom - pt.y;
			double fRate = 1;
			fRate = oldW / newW;
			if (fRate > 5) fRate = 5;
			if (fRate < 0.2) fRate = 0.2;
			double pixPerValue = (m_fLastAxisMax - m_fLastAxisMin) / (m_pAxisY->Height());
			pixPerValue *= fRate;
			double newLength = pixPerValue * (m_pAxisY->Height());
			if (m_pAxisY->IsRevert())
			{
				m_pAxisY->SetMinMaxValue(m_fLastAxisMax - newLength, m_fLastAxisMax);
				m_pAxisSub->SetMinMaxValue(m_fLastAxisMax - newLength, m_fLastAxisMax);
			}
			else
			{
				m_pAxisY->SetMinMaxValue(m_fLastAxisMin, m_fLastAxisMin + newLength);
				m_pAxisSub->SetMinMaxValue(m_fLastAxisMin, m_fLastAxisMin + newLength);
			}
			Refresh();
		}
		return true;
	}
	else
	{
		//拖动其他位置
		CRect tmpRc(m_rcObj);
		m_rcObj.top = top;

		if (tmpRc.PtInRect(pt) && tmpRc.PtInRect(from))
		{
			m_zoomRc.left = from.x < pt.x ? from.x : pt.x;
			m_zoomRc.right = from.x > pt.x ? from.x : pt.x;
			m_zoomRc.top = from.y < pt.y ? from.y : pt.y;
			m_zoomRc.bottom = from.y > pt.y ? from.y : pt.y;

			Refresh();
		}
		return true;
	}
	return false;
}
bool CAxisChart::OnClick(CPoint &pt, bool bLeft /*= true*/)
{
	m_zoomRc.SetRectEmpty();
	m_regionRc.SetRectEmpty();
	CRect rcHorzAixs;
	rcHorzAixs.CopyRect(m_pAxisX);
	if (rcHorzAixs.PtInRect(pt))
	{
		CRect rcScrolBar = m_pAxisX->GetScrllBarRect();
		if (!rcScrolBar.PtInRect(pt))
		{
			int nMin = (int)m_pAxisX->GetMinValue();
			int nMax = (int)m_pAxisX->GetMaxValue();
			int nRange = nMax - nMin;
			int nAll = (int)m_pAxisX->GetDuration();
			if (pt.x > rcScrolBar.right)
			{
				nMax += nRange;
				if (nMax > nAll) nMax = nAll;
				nMin = nMax - nRange;
			}
			else if (pt.x < rcScrolBar.left)
			{
				nMin -= nRange;
				if (nMin < 0) nMin = 0;
				nMax = nMin + nRange;
			}
			m_pAxisX->SetMinMaxValue(nMin, nMax);
		}
	}
	return false;
}
bool CAxisChart::OnDragUp(CPoint&from, CPoint &pt, bool bLeft /*= true*/)
{
	m_curDragingAxis = -1;
	m_bDragingScrollBar = false;
	m_pAxisX->EnableDraging(false);
	m_bDragingPricePad = false;
	SetCursor(CURSOR_NORMAL);

	ReleaseCapture();
	if (m_zoomRc.Width()>50 && m_zoomRc.Height()>0)
	{
		//m_pBrowser->PopupRengionMenu(to);
	}
	else
	{
		m_zoomRc.SetRectEmpty();
	}
	return true;
}
bool CAxisChart::OnDblClk(CPoint &pt, bool bLeft /*= true*/)
{
	m_bAutoAxis = true;
	Refresh();
	return false;
}
bool CAxisChart::OnMouseMove(CPoint& pt)
{
	return false;
}
int CAxisChart::GetCursor(CPoint& pt)
{
	if (m_pAxisX->m_Period.kind == 0) return CURSOR_NORMAL;

	if (m_bDragingPricePad)
	{
		return CURSOR_DRAG;
	}
	else if (m_pChartCtrl->GetPricePadRect().PtInRect(pt) && m_pChartCtrl->IsMouseLineVisible())
	{
		return CURSOR_HAND;
	}

	if (m_curDragingAxis == 0)
	{
		if (m_bIsLeftDraging)
		{
			return CURSOR_HAND;
		}
		else
			return CURSOR_AXIS_HERZ;
	}
	else if (m_curDragingAxis == 1 || m_curDragingAxis == 2)
	{
		return CURSOR_AXIS_VERT;
	}
	else
	{
		if (m_pAxisY->PtInRect(pt) || m_pAxisSub->PtInRect(pt))
		{
			return CURSOR_AXIS_VERT;
		}
		else if (m_pAxisX->PtInRect(pt))
		{
			if (m_pAxisX->GetScrllBarRect().PtInRect(pt))
			{
				return CURSOR_HAND;
			}
			else
				return CURSOR_AXIS_HERZ;
		}
		return CURSOR_NORMAL;
	}
}
void CAxisChart::CreateAxis()
{
	m_pAxisY = new CAxis(false, this);
	m_pAxisSub = new CAxis(false, this);
}

void CAxisChart::SetMainChart()
{
	m_bMainChart = true;
}

void CAxisChart::DrawAxis(IRenderTarget* pRender)
{
	if (m_pCurAxisVert == NULL)
	{
		if (m_pAxisY)
		{
			m_pAxisY->SetReVert(m_pChartCtrl->IsRevert());
			m_pAxisY->Draw(pRender);
		}
	}
	else
	{
		m_pCurAxisVert->SetPrect(m_pAxisY->GetPrect(), m_pAxisY->GetMinTick());
		m_pCurAxisVert->SetReVert(m_pChartCtrl->IsRevert());
		m_pCurAxisVert->Draw(pRender);
	}

	if (m_bShowSubAixs)
	{
		m_pAxisSub->SetReVert(m_pChartCtrl->IsRevert());
		m_pAxisSub->Draw(pRender);
	}
}
void  CAxisChart::DrawZoomRect(IRenderTarget* pRender)
{

}
void  CAxisChart::DrawRegionRect(IRenderTarget* pRender)
{

}
void CAxisChart::CalRegionStatisticRange(int& nStar, int& nEnd, bool bRecal)
{
	if (nStar<0 || nEnd >= m_pAxisX->GetDataCount())
		return;
	if (!bRecal)
	{
		m_regionRc = *this;
		m_regionRc.top = m_pChartCtrl->GetTitleRect().Height();
		if (m_zoomRc.IsRectEmpty())
		{
			nStar = static_cast<int>(m_pAxisX->GetMinValue());
			nEnd = static_cast<int>(m_pAxisX->GetMaxValue()>m_pAxisX->GetDataCount() - 1 ? m_pAxisX->GetDataCount() - 1 : m_pAxisX->GetMaxValue());
		}
		else
		{
			m_regionRc.left = m_zoomRc.left;
			m_regionRc.right = m_zoomRc.right;
			nStar = static_cast<int>(m_pAxisX->Pix2Value(m_zoomRc.left));
			nEnd = static_cast<int>(m_pAxisX->Pix2Value(m_zoomRc.right) > m_pAxisX->GetDataCount() - 1 ? m_pAxisX->GetDataCount() - 1 : m_pAxisX->Pix2Value(m_zoomRc.right));
		}
	}
	int nBarWid = m_pAxisX->GetCandleWidth();
	m_regionRc.left = m_pAxisX->Value2Pix(nStar) + nBarWid / 2;
	m_regionRc.right = m_pAxisX->Value2Pix(nEnd) + nBarWid / 2;
	m_zoomRc.SetRectEmpty();
	Refresh();
}
