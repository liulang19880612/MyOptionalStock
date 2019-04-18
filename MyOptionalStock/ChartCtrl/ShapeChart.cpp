#include "stdafx.h"
#include "ShapeChart.h"
#include "ShapeFactory.h"
#include "TimeAxis.h"
#include "ChartCtrl.h"

CShapeChart::CShapeChart()
{

}
CShapeChart::~CShapeChart()
{

}

bool CShapeChart::OnMouseDown(CPoint& pt)
{
	if (m_pCurShapePen)
	{
		m_pCurShapePen->OnClick(pt);
		AfterShapePenClick();
		Refresh();
		return true;
	}

	if (m_pCurShape != NULL)
	{
		m_pCurShape->OnClick(pt);
		m_pCurShape = NULL;
		Refresh();
		return true;
	}
	else
	{
		int cnt = m_arrShape.GetCount();
		for (int i = cnt - 1; i >= 0; i--)
		{
			m_arrShape[i]->OnClick(pt);
			if (m_arrShape[i]->IsSelected())
			{
				if (m_curPen == doDELETE)
				{
					if (m_pCurShape == m_arrShape[i])
					{
						m_pCurShape = NULL;
					}
					delete m_arrShape[i];
					m_arrShape.RemoveAt(i);
				}
				else
				{
					m_pCurShape = m_arrShape[i];
				}

				Refresh();
				return true;
			}
		}
	}
	return __super::OnMouseDown(pt);
}
bool CShapeChart::OnClick(CPoint &pt, bool bLeft)
{
	return __super::OnClick(pt, bLeft);
}
bool CShapeChart::OnDrag(CPoint& from, CPoint &pt, bool bLeft)
{
	return __super::OnDrag(from, pt, bLeft);

}
bool CShapeChart::OnDragUp(CPoint&from, CPoint &pt, bool bLeft)
{
	return __super::OnDragUp(from, pt, bLeft);
}
bool CShapeChart::HitTest(ObjectInfo& info, CPoint pt)
{
	int ret = -1;

	int cnt = m_arrShape.GetCount();
	for (int i = 0; i < cnt; i++)
	{
		if (m_arrShape[i]->HitTest(pt) != -1)
		{
			ret = i;
			info.nPenType = m_arrShape[i]->GetType();
			info.pShape = dynamic_cast<IKShape*>(m_arrShape[i]);
			break;
		}
	}

	if (ret != -1)
	{
		info.type = OT_SHAPE;
		info.idx = ret;
		info.fValue = m_pAxisY->Pix2Value(pt.y);
		info.nPos = (int)m_pAxisX->Pix2Value(pt.x);
		info.time = (__time32_t)m_pAxisX->GetHisData()->GetAt(info.nPos).time;
		return true;
	}
	else
	{
		return false;
	}
}
bool CShapeChart::OnMouseMove(CPoint& pt)
{
	if (m_pAxisX == NULL || m_pAxisY == NULL)
	{
		return __super::OnMouseMove(pt);
	}
	if (m_pCurShapePen)
	{
		Refresh();
	}
	if (m_pCurShape)
	{
		m_pCurShape->OnMove(pt);
		Refresh();
		return true;
	}
	else
	{
		int cnt = m_arrShape.GetCount();
		for (int i = 0; i < cnt; i++)
		{
			if (m_arrShape[i]->HitTest(pt) != -1)
			{
				m_arrShape[i]->SetHot(true);
			}
			else
			{
				m_arrShape[i]->SetHot(false);
			}
		}
		if (cnt > 0)
		{
			Refresh();
		}
	}
	return __super::OnMouseMove(pt);
}

void CShapeChart::SetCurShape(CShape* pShape)
{
	if (m_pCurShape != NULL)
		m_pCurShape->Select(false);
	m_pCurShape = pShape;
	if (m_pCurShape != NULL)
		m_pCurShape->Select(true);
}
CShape *CShapeChart::GetShap(int idx)
{
	if (idx < 0 || idx >= static_cast<int>(m_arrShape.GetCount()))
		return NULL;
	return m_arrShape[idx];
}
void CShapeChart::DeleteAlleShape()
{
	for (size_t i = 0; i < m_arrShape.GetCount(); i++)
	{
		delete m_arrShape[i];
		m_arrShape[i] = NULL;
	}
	m_arrShape.RemoveAll();

	if (m_pCurShapePen)
	{
		delete m_pCurShapePen;
		m_pCurShapePen = NULL;
	}
}
void CShapeChart::SelectPen(int type)
{
	if (m_bMainChart)
	{
		SetCurShape(NULL);
		if (m_pCurShapePen != NULL)
		{
			delete m_pCurShapePen;
			m_pCurShapePen = NULL;
			Refresh();
		}
		m_curPen = (ChartPenType)type;
// 		if (m_curPen == doICON)
// 		{
// 			CDlgIconSelect dlg;
// 			if (dlg.DoModal() != IDOK)
// 			{
// 				m_curPen = doSelect;
// 			}
// 			else
// 			{
// 				m_pCurShapePen = ShapeFactory()->CreatePen((ChartPenType)type, m_pAxisX, m_pAxisY);
// 				m_pCurShapePen->SetIcon(dlg.GetSelectIcon());
// 
// 			}
// 		}
// 		else if (m_curPen == doCHOOSELINE)
// 		{
// 			CDlgLineTypeSel dlg;
// 			dlg.SetData(m_penWid, m_clrPen);
// 			if (dlg.DoModal() == IDOK)
// 			{
// 				m_penWid = dlg.GetLineWidth();
// 				m_clrPen = dlg.GetLineColor();
// 			}
// 			m_curPen = doSelect;
// 		}
// 		else if (m_curPen == doCLEARALL)
// 		{
// 			m_curPen = doSelect;
// 			if (m_arrShape.IsEmpty()) return;
// 
// 			if (AfxMessageBox(GETSTRING(L"@string/IDS_10099), MB_OKCANCEL) == IDOK)
// 			{
// 				//删除所有图形
// 				int cnt = m_arrShape.GetCount();
// 				for (int i = 0; i < cnt; i++)
// 				{
// 					delete m_arrShape[i];
// 				}
// 				m_arrShape.RemoveAll();
// 				//清空画线下单监听器
// 				CMyStatic* pStatic = m_pBrowser->m_pStatic;
// 
// 			}
// 		}
// 		else
// 		{
// 			m_pCurShapePen = ShapeFactory()->CreatePen((ChartPenType)type, m_pAxisX, m_pAxisY);
// 		}
		Refresh();
	}
}
void CShapeChart::DrawObjects(IRenderTarget* pRender, bool bRedraw)
{
	if (IsDrawShape())
	{
		m_pChartCtrl->ShowMouseLine(false);
	}
	__super::DrawObjects(pRender, bRedraw);
	CRgn rgn, rgnOld;
	CRect rcClip;
	pRender->GetClipBox(&rcClip);

	rgn.CreateRectRgn(m_rcObj.left, m_rcObj.top, m_rcObj.right, m_rcObj.bottom);
	rgnOld.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);

// 	pRender->SelectClipRgn(&rgn, RGN_AND);
// 	DrawShape(pRender);
// 	pRender->SelectClipRgn(&rgnOld, RGN_OR);
}

void CShapeChart::DrawShape(IRenderTarget* pRender)
{
	if (m_bMainChart)
	{
		for (size_t i = 0; i < m_arrShape.GetCount(); i++)
		{
			m_arrShape[i]->ResetAxis(m_pAxisX, m_pAxisY);
		}
	}

	for (size_t i = 0; i < m_arrShape.GetCount(); i++)
	{
		m_arrShape[i]->BeginDraw(pRender);
		m_arrShape[i]->Draw(pRender);
		m_arrShape[i]->EndDraw(pRender);
	}

	if (m_pCurShapePen)
	{
		m_pCurShapePen->Draw(pRender);
	}
}

void CShapeChart::AfterShapePenClick()
{
	if (m_pCurShapePen)
	{
		if (m_pCurShapePen->IsOk())
		{
			SArray<ChartDot> arr;
			m_pCurShapePen->GetData(arr);
			CShape *pShape = ShapeFactory()->CreateShape(m_curPen, m_penWid, m_clrPen, arr, m_pAxisX, m_pAxisY);
			pShape->SetIcon(m_pCurShapePen->GetIcon());
			pShape->SetText(m_pCurShapePen->GetText());
			pShape->SetColor(m_clrPen);
			m_arrShape.Add(pShape);
			SelectPen(doSelect);
		}
	}
	Refresh();
}
BOOL CShapeChart::IsDrawShape()
{
	return m_curPen != doSelect;
}
void CShapeChart::GetShapeArray(SArray<CShape*>& arr)
{
	arr.RemoveAll();
	arr.Copy(m_arrShape);
	m_arrShape.RemoveAll();
}
void CShapeChart::SetArrShape(SArray<CShape*>& arrShape)
{
	m_arrShape.RemoveAll();
	m_arrShape.Copy(arrShape);
	for (size_t i = 0; i < m_arrShape.GetCount(); i++)
	{
		m_arrShape[i]->ResetX();
	}
	arrShape.RemoveAll();
}
IKShape * CShapeChart::AddShape(const KShapeData& data)
{
	if (m_pAxisX == NULL || m_pAxisX->GetDataCount() == 0) return NULL;

	SArray<ChartDot> arrDot;
	for (int i = 0; i < data.nDotCount; i++)
	{
		ChartDot cur;
		cur.tm = data.Dots[i].time;
		cur.y = data.Dots[i].price;
		cur.x = 0;
		arrDot.Add(cur);
	}
	CShape *pShape = ShapeFactory()->CreateShape((ChartPenType)data.type, data.penWid, data.clr, arrDot, m_pAxisX, m_pAxisY);
	pShape->SetText(S_CA2W(data.txt));
	pShape->SetIcon(data.nIcon);
	if (pShape == NULL) return NULL;
	m_arrShape.Add(pShape);
	return dynamic_cast<IKShape*>(pShape);
}

int CShapeChart::GetAllShape(IKShapePtr* pShapes, int count)
{
	int nAct = min(count, static_cast<int>(m_arrShape.GetCount()));

	if (pShapes)
	{
		for (int i = 0; i < nAct; i++)
		{
			pShapes[i] = dynamic_cast<IKShape*>(m_arrShape[i]);
		}
	}
	return m_arrShape.GetCount();
}

void CShapeChart::RemoveShape(IKShape* pShape)
{
	for (size_t i = 0; i < m_arrShape.GetCount(); i++)
	{
		if (pShape == m_arrShape[i])
		{
			CShape* p = dynamic_cast<CShape*>(pShape);
			delete p;
			m_arrShape.RemoveAt(i);
			break;
		}
	}
}
int CShapeChart::GetCursor(CPoint& pt)
{
	if (m_rcObj.PtInRect(pt) && m_bMainChart)
	{
		switch (m_curPen)
		{
			case  doSelect:
			{
				if (m_pCurShape != NULL)
				{
					if (m_pCurShape->IsSizing())
						return CURSOR_DRAG;
					else
						return CURSOR_MOVING;
				}
				else
				{
					int ret = -1;
					int cnt = m_arrShape.GetCount();
					for (int i = 0; i < cnt; i++)
					{
						m_arrShape[i]->ResetAxis(m_pAxisX, m_pAxisY);
						ret = m_arrShape[i]->HitTest(pt);
						if (ret != -1)
							break;
					}

					if (ret == -2)
					{
						return CURSOR_HAND;
					}
					else if (ret >= 0)
					{
						return CURSOR_HANDCLICK;
					}
					return __super::GetCursor(pt);
				}
			}
			case  doDELETE:
			return CURSOR_EREASE;
			case  doTEXT:
			{
				return CURSOR_TEXT;
			}
			default:
			return CURSOR_PEN;
		}
	}
	else
		return __super::GetCursor(pt);
}