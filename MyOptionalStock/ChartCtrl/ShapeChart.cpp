#include "stdafx.h"
#include "ShapeChart.h"
#include "ShapeFactory.h"
#include "TimeAxis.h"

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
void CShapeChart::SelectPen(int type)
{
// 	if (m_bMainChart)
// 	{
// 		SetCurShape(NULL);
// 
// 		if (m_pCurShapePen != NULL)
// 		{
// 			delete m_pCurShapePen;
// 			m_pCurShapePen = NULL;
// 			Refresh();
// 		}
// 
// 		m_curPen = (RAPenType)type;
// 
// 		if (m_curPen == doICON)
// 		{
// 			CDlgIconSelect dlg;
// 			if (dlg.DoModal() != IDOK)
// 			{
// 				m_curPen = doSelect;
// 			}
// 			else
// 			{
// 				m_pCurShapePen = ShapeFactory()->CreatePen((RAPenType)type, m_pAxisX, m_pAxisY);
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
// 
// 		}
// 		else
// 		{
// 			m_pCurShapePen = ShapeFactory()->CreatePen((RAPenType)type, m_pAxisX, m_pAxisY);
// 		}
// 
// 		Refresh();
// 	}
}
void CShapeChart::DrawObjects(IRenderTarget* pRender, bool bRedraw)
{
// 	if (IsDrawShape())
// 	{
// 		m_pChartCtrl->ShowMouseLine(false);
// 	}
// 	__super::DrawObjects(pDC, bRedraw);
// 	CRgn rgn, rgnOld;
// 	CRect rcClip;
// 	pRender->GetClipBox(&rcClip);
// 
// 	rgn.CreateRectRgn(m_rcObj.left, m_rcObj.top, m_rcObj.right, m_rcObj.bottom);
// 	rgnOld.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
// 
// 	pRender->SelectClipRgn(&rgn, RGN_AND);
// 	DrawShape(pDC);
// 	pRender->SelectClipRgn(&rgnOld, RGN_OR);
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