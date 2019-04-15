#include "stdafx.h"
#include "GridHeaderCtrl.h"
#include "souistd.h"
#include "DragView.h"

#define CX_HDITEM_MARGIN    4

CGridHeaderCtrl::CGridHeaderCtrl(void)
	:m_bFixWidth(FALSE)
	, m_bItemSwapEnable(TRUE)
	, m_bSortHeader(TRUE)
	, m_pSkinItem(GETBUILTINSKIN(SKIN_SYS_HEADER))
	, m_pSkinSort(NULL)
	, m_dwHitTest((DWORD)-1)
	, m_bDragging(FALSE)
	, m_hDragImg(NULL)
	, m_nFixedItems(0)
{
	m_bClipClient = TRUE;
	m_evtSet.addEvent(EVENTID(EventHeaderClick));
	m_evtSet.addEvent(EVENTID(EventHeaderItemChanged));
	m_evtSet.addEvent(EVENTID(EventHeaderItemChanging));
	m_evtSet.addEvent(EVENTID(EventHeaderItemSwap));
	m_evtSet.addEvent(EVENTID(EventHeaderRelayout));
}

CGridHeaderCtrl::~CGridHeaderCtrl(void)
{
}

int CGridHeaderCtrl::InsertItem(int iItem, LPCTSTR pszText, int nWidth, SHDSORTFLAG stFlag, LPARAM lParam)
{
	return InsertItem(iItem, pszText, nWidth, SLayoutSize::px, stFlag, lParam);
}

int CGridHeaderCtrl::InsertItem(int iItem, LPCTSTR pszText, int nWidth, SLayoutSize::Unit unit, SHDSORTFLAG stFlag, LPARAM lParam)
{
	SASSERT(pszText);
	SASSERT(nWidth >= 0);
	if (iItem == -1) iItem = (int)m_arrItems.GetCount();
	SHDITEM item;
	item.mask = 0xFFFFFFFF;
	item.cx.setSize((float)nWidth, unit);
	item.strText.SetCtxProvider(this);
	item.strText.SetText(pszText);
	item.stFlag = stFlag;
	item.state = 0;
	item.iOrder = iItem;
	item.lParam = lParam;
	m_arrItems.InsertAt(iItem, item);
	//需要更新列的序号
	for (size_t i = 0; i < GetItemCount(); i++)
	{
		if (i == (size_t)iItem) continue;
		if (m_arrItems[i].iOrder >= iItem)
			m_arrItems[i].iOrder++;
	}
	EventHeaderRelayout e(this);
	FireEvent(e);

	Invalidate();
	return iItem;
}

BOOL CGridHeaderCtrl::GetItem(int iItem, SHDITEM *pItem)
{
	if ((UINT)iItem >= m_arrItems.GetCount()) return FALSE;
	if (pItem->mask & SHDI_TEXT)
	{
		pItem->strText.SetText(m_arrItems[iItem].strText.GetText(FALSE));
	}
	if (pItem->mask & SHDI_WIDTH) pItem->cx = m_arrItems[iItem].cx;
	if (pItem->mask & SHDI_LPARAM) pItem->lParam = m_arrItems[iItem].lParam;
	if (pItem->mask & SHDI_SORTFLAG) pItem->stFlag = m_arrItems[iItem].stFlag;
	if (pItem->mask & SHDI_ORDER) pItem->iOrder = m_arrItems[iItem].iOrder;
	return TRUE;
}

void CGridHeaderCtrl::OnPaint(IRenderTarget * pRT)
{
	SPainter painter;
	BeforePaint(pRT, painter);
	SOUI::CRect rcClient;
	GetClientRect(&rcClient);
	SOUI::CRect rcItem(rcClient.left, rcClient.top, rcClient.left, rcClient.bottom);
	// 绘制固定可见列
	for(UINT i =0 ; i < m_nFixedItems; ++i)
	{
		if (!m_arrItems[i].bVisible) continue;
		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + m_arrItems[i].cx.toPixelSize(GetScale());
		DrawItem(pRT, rcItem, m_arrItems.GetData() + i);
		if (rcItem.right >= rcClient.right) break;
	}
	// 绘制非固定可见列
	for (UINT i = m_nFixedItems; i < m_arrItems.GetCount(); i++)
	{
		if (!m_arrItems[i].bVisible) continue;
		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + m_arrItems[i].cx.toPixelSize(GetScale());
		DrawItem(pRT, rcItem, m_arrItems.GetData() + i);
		if (rcItem.right >= rcClient.right) break;
	}
	if (rcItem.right < rcClient.right)
	{
		rcItem.left = rcItem.right;
		rcItem.right = rcClient.right;
		if (m_pSkinItem) m_pSkinItem->Draw(pRT, rcItem, 3);
	}
	AfterPaint(pRT, painter);
}

void CGridHeaderCtrl::DrawItem(IRenderTarget * pRT, SOUI::CRect rcItem, const LPSHDITEM pItem)
{
	if (!pItem->bVisible) return;
	if (m_pSkinItem) m_pSkinItem->Draw(pRT, rcItem, pItem->state);
	pRT->DrawText(pItem->strText.GetText(FALSE), pItem->strText.GetText(FALSE).GetLength(), rcItem, m_style.GetTextAlign());
	if (pItem->stFlag == ST_NULL || !m_pSkinSort) return;
	CSize szSort = m_pSkinSort->GetSkinSize();
	SOUI::CPoint ptSort;
	ptSort.y = rcItem.top + (rcItem.Height() - szSort.cy) / 2;

	if (m_style.GetTextAlign()&DT_RIGHT)
		ptSort.x = rcItem.left + 2;
	else
		ptSort.x = rcItem.right - szSort.cx - 2;

	if (m_pSkinSort) m_pSkinSort->Draw(pRT, SOUI::CRect(ptSort, szSort), pItem->stFlag == ST_UP ? 0 : 1);
}

BOOL CGridHeaderCtrl::DeleteItem(int iItem)
{
	if (iItem < 0 || (UINT)iItem >= m_arrItems.GetCount()) return FALSE;

	int iOrder = m_arrItems[iItem].iOrder;
	m_arrItems.RemoveAt(iItem);
	//更新排序
	for (UINT i = 0; i < m_arrItems.GetCount(); i++)
	{
		if (m_arrItems[i].iOrder > iOrder)
			m_arrItems[i].iOrder--;
	}
	EventHeaderRelayout e(this);
	FireEvent(e);

	Invalidate();
	return TRUE;
}

void CGridHeaderCtrl::DeleteAllItems()
{
	m_arrItems.RemoveAll();
	EventHeaderRelayout e(this);
	FireEvent(e);
	Invalidate();
}

void CGridHeaderCtrl::OnDestroy()
{
	GetEventSet()->setMutedState(true);
	DeleteAllItems();
	GetEventSet()->setMutedState(false);
	__super::OnDestroy();
}

SOUI::CRect CGridHeaderCtrl::GetItemRect(UINT iItem) const
{
	SOUI::CRect    rcClient;
	GetClientRect(&rcClient);
	if (!m_arrItems[iItem].bVisible)
		return SOUI::CRect();
	SOUI::CRect rcItem(rcClient.left, rcClient.top, rcClient.left, rcClient.bottom);
	for (UINT i = 0; i <= iItem && i < m_arrItems.GetCount(); i++)
	{
		if (!m_arrItems[i].bVisible) continue;
		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + m_arrItems[i].cx.toPixelSize(GetScale());
	}
	return rcItem;
}

int CGridHeaderCtrl::GetOriItemIndex(int iOrder) const
{
	for (UINT i = 0; i < m_arrItems.GetCount(); i++)
	{
		if (m_arrItems[i].iOrder == iOrder)
			return i;
	}
	return -1;
}
bool CGridHeaderCtrl::isViewWidthMatchParent() const
{
	return TRUE == m_bMatchParent;
}
void CGridHeaderCtrl::UpdataWidth()
{
	//更新不同步
	SASSERT(m_widItems.GetCount() == m_arrItems.GetCount());
	int totalWidth = 0;
	for (size_t i = 0; i < m_widItems.GetCount(); i++)
	{
		totalWidth += m_widItems[i];
	}
	int parentViewWid = GetWindowRect().Width();
	int remainingWid = parentViewWid;
	int nozeroPos = 0;//找到第一个不为0的项
	for (size_t i = 0; i < m_arrItems.GetCount(); i++)
	{
		//跳过为0的项
		if (m_widItems[i] == 0)
			continue;
		nozeroPos = i;
		break;
	}
	for (size_t i = m_arrItems.GetCount() - 1; i > (size_t)nozeroPos; i--)
	{
		//跳过为0的项
		if (m_widItems[i] == 0)
			continue;
		int itemwid = m_widItems[i] * parentViewWid / totalWidth;
		m_arrItems[i].cx.fSize = (float)itemwid;
		remainingWid -= itemwid;
	}
	//因为除法不一定能整除，把最后余下的全给第一个不为0的子项
	m_arrItems[nozeroPos].cx.fSize = (float)remainingWid;
}
void CGridHeaderCtrl::OnSize(UINT nType, CSize size)
{
	if (isViewWidthMatchParent())
		UpdataWidth();
}
void CGridHeaderCtrl::RedrawItem(int iItem)
{
	SOUI::CRect rcItem = GetItemRect(iItem);
	IRenderTarget *pRT = GetRenderTarget(rcItem, OLEDC_PAINTBKGND);
	DrawItem(pRT, rcItem, m_arrItems.GetData() + iItem);
	ReleaseRenderTarget(pRT);
}

void CGridHeaderCtrl::OnLButtonDown(UINT nFlags, SOUI::CPoint pt)
{
	SetCapture();
	m_ptClick = pt;
	m_dwHitTest = HitTest(pt);
	if (IsItemHover(m_dwHitTest))
	{
		if (m_bSortHeader)
		{
			m_arrItems[LOWORD(m_dwHitTest)].state = 2;//pushdown
			RedrawItem(LOWORD(m_dwHitTest));
		}
	}
	else if (m_dwHitTest != -1)
	{
		m_nAdjItemOldWidth = m_arrItems[LOWORD(m_dwHitTest)].cx.toPixelSize(GetScale());
	}
}

void CGridHeaderCtrl::OnLButtonUp(UINT nFlags, SOUI::CPoint pt)
{
	if (IsItemHover(m_dwHitTest))
	{
		if (m_bDragging)
		{//拖动表头项
			if (m_bItemSwapEnable)
			{
				CDragView::EndDrag();
				DeleteObject(m_hDragImg);
				m_hDragImg = NULL;

				m_arrItems[LOWORD(m_dwHitTest)].state = 0;//normal

				if (m_dwDragTo != m_dwHitTest && IsItemHover(m_dwDragTo))
				{
					SHDITEM t = m_arrItems[LOWORD(m_dwHitTest)];
					m_arrItems.RemoveAt(LOWORD(m_dwHitTest));
					int nPos = LOWORD(m_dwDragTo);
					if (nPos > LOWORD(m_dwHitTest)) nPos--;//要考虑将自己移除的影响
					m_arrItems.InsertAt(LOWORD(m_dwDragTo), t);
					//发消息通知宿主表项位置发生变化
					EventHeaderItemSwap evt(this);
					evt.iOldIndex = LOWORD(m_dwHitTest);
					evt.iNewIndex = nPos;
					FireEvent(evt);

					EventHeaderRelayout e(this);
					FireEvent(e);
				}
				m_dwHitTest = HitTest(pt);
				m_dwDragTo = (DWORD)-1;
				Invalidate();
			}
		}
		else
		{//点击表头项
			if (m_bSortHeader)
			{
				m_arrItems[LOWORD(m_dwHitTest)].state = 1;//hover
				RedrawItem(LOWORD(m_dwHitTest));
				EventHeaderClick evt(this);
				evt.iItem = LOWORD(m_dwHitTest);
				FireEvent(evt);
			}
		}
	}
	else if (m_dwHitTest != -1)
	{//调整表头宽度，发送一个调整完成消息
		EventHeaderItemChanged evt(this);
		evt.iItem = LOWORD(m_dwHitTest);
		evt.nWidth = m_arrItems[evt.iItem].cx.toPixelSize(GetScale());
		FireEvent(evt);

		EventHeaderRelayout e(this);
		FireEvent(e);
	}
	m_bDragging = FALSE;
	ReleaseCapture();
}

void CGridHeaderCtrl::OnMouseMove(UINT nFlags, SOUI::CPoint pt)
{
	if (m_bDragging || nFlags&MK_LBUTTON)
	{
		if (!m_bDragging)
		{
			if (IsItemHover(m_dwHitTest) && m_bItemSwapEnable)
			{
				m_dwDragTo = m_dwHitTest;
				SOUI::CRect rcItem = GetItemRect(LOWORD(m_dwHitTest));
				DrawDraggingState(m_dwDragTo);
				m_hDragImg = CreateDragImage(LOWORD(m_dwHitTest));
				SOUI::CPoint pt = m_ptClick - rcItem.TopLeft();
				CDragView::BeginDrag(m_hDragImg, pt, 0, 128, LWA_ALPHA | LWA_COLORKEY);
				m_bDragging = TRUE;
			}
		}
		if (IsItemHover(m_dwHitTest))
		{
			if (m_bItemSwapEnable)
			{
				DWORD dwDragTo = HitTest(pt);
				SOUI::CPoint pt2(pt.x, m_ptClick.y);
				ClientToScreen(GetContainer()->GetHostHwnd(), &pt2);
				if (IsItemHover(dwDragTo) && m_dwDragTo != dwDragTo)
				{
					m_dwDragTo = dwDragTo;
					DrawDraggingState(dwDragTo);
				}
				CDragView::DragMove(pt2);
			}
		}
		else if (m_dwHitTest != -1)
		{//调节宽度
			if (!m_bFixWidth)
			{
				int cxNew = m_nAdjItemOldWidth + pt.x - m_ptClick.x;
				if (cxNew < 0) cxNew = 0;
				if (m_arrItems[LOWORD(m_dwHitTest)].cx.unit == SLayoutSize::px)
					m_arrItems[LOWORD(m_dwHitTest)].cx.setSize((float)cxNew, SLayoutSize::px);
				else if (m_arrItems[LOWORD(m_dwHitTest)].cx.unit == SLayoutSize::dp)
					m_arrItems[LOWORD(m_dwHitTest)].cx.setSize(cxNew * 1.0f / GetScale(), SLayoutSize::dp);
				// TODO: dip 和 sp 的处理（AYK）

				Invalidate();
				GetContainer()->UpdateWindow();//立即更新窗口
				//发出调节宽度消息
				EventHeaderItemChanging evt(this);
				evt.iItem = LOWORD(m_dwHitTest);
				evt.nWidth = cxNew;
				FireEvent(evt);

				EventHeaderRelayout e(this);
				FireEvent(e);
			}
		}
	}
	else
	{
		DWORD dwHitTest = HitTest(pt);
		if (dwHitTest != m_dwHitTest)
		{
			if (m_bSortHeader)
			{
				if (IsItemHover(m_dwHitTest))
				{
					WORD iHover = LOWORD(m_dwHitTest);
					m_arrItems[iHover].state = 0;
					RedrawItem(iHover);
				}
				if (IsItemHover(dwHitTest))
				{
					WORD iHover = LOWORD(dwHitTest);
					m_arrItems[iHover].state = 1;//hover
					RedrawItem(iHover);
				}
			}
			m_dwHitTest = dwHitTest;
		}
	}

}

void CGridHeaderCtrl::OnMouseLeave()
{
	if (!m_bDragging)
	{
		if (IsItemHover(m_dwHitTest) && m_bSortHeader)
		{
			m_arrItems[LOWORD(m_dwHitTest)].state = 0;
			RedrawItem(LOWORD(m_dwHitTest));
		}
		m_dwHitTest = (DWORD)-1;
	}
}

BOOL CGridHeaderCtrl::CreateChildren(pugi::xml_node xmlNode)
{
	pugi::xml_node xmlItems = xmlNode.child(L"items");
	if (!xmlItems) return FALSE;
	pugi::xml_node xmlItem = xmlItems.child(L"item");
	LockUpdate();

	while (xmlItem)
	{
		SStringW strText = xmlItem.text().get();
		strText.TrimBlank();
		strText = tr(GETSTRING(strText));
		SStringT strText2 = S_CW2T(strText);
		int cx = xmlItem.attribute(L"width").as_int(50);
		if (isViewWidthMatchParent())
		{
			m_widItems.InsertAt(m_arrItems.GetCount(), cx);
		}
		LPARAM lParam = xmlItem.attribute(L"userData").as_uint(0);
		SHDSORTFLAG stFlag = (SHDSORTFLAG)xmlItem.attribute(L"sortFlag").as_uint(ST_NULL);

		InsertItem(-1, strText2, cx, stFlag, lParam);

		xmlItem = xmlItem.next_sibling(L"item");
	}

	UnlockUpdate();


	if (isViewWidthMatchParent())
	{
		m_bFixWidth = TRUE;
	}
	return TRUE;
}

BOOL CGridHeaderCtrl::OnSetCursor(const SOUI::CPoint &pt)
{
	if (m_bFixWidth) return FALSE;
	DWORD dwHit = HitTest(pt);
	if (HIWORD(dwHit) == LOWORD(dwHit)) return FALSE;
	HCURSOR hCursor = GETRESPROVIDER->LoadCursor(IDC_SIZEWE);
	SetCursor(hCursor);
	return TRUE;
}

DWORD CGridHeaderCtrl::HitTest(SOUI::CPoint pt)
{
	SOUI::CRect    rcClient;
	GetClientRect(&rcClient);
	if (!rcClient.PtInRect(pt)) return (DWORD)-1;

	SOUI::CRect rcItem(rcClient.left, rcClient.top, rcClient.left, rcClient.bottom);
	int nMargin = m_bSortHeader ? CX_HDITEM_MARGIN : 2;
	for (UINT i = 0; i < m_arrItems.GetCount(); i++)
	{
		if (m_arrItems[i].cx.toPixelSize(GetScale()) == 0 || !m_arrItems[i].bVisible) continue;    //越过宽度为0的项

		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + m_arrItems[i].cx.toPixelSize(GetScale());
		if (pt.x < rcItem.left + nMargin)
		{
			int nLeft = i > 0 ? i - 1 : 0;
			return MAKELONG(nLeft, i);
		}
		else if (pt.x < rcItem.right - nMargin)
		{
			return MAKELONG(i, i);
		}
		else if (pt.x < rcItem.right)
		{
			WORD nRight = (WORD)i + 1;
			if (nRight >= m_arrItems.GetCount()) nRight = (WORD)-1;//采用-1代表末尾
			return MAKELONG(i, nRight);
		}
	}
	return (DWORD)-1;
}

HBITMAP CGridHeaderCtrl::CreateDragImage(UINT iItem)
{
	if (iItem >= m_arrItems.GetCount()) return NULL;
	SOUI::CRect rcClient;
	GetClientRect(rcClient);
	SOUI::CRect rcItem(0, 0, m_arrItems[iItem].cx.toPixelSize(GetScale()), rcClient.Height());

	CAutoRefPtr<IRenderTarget> pRT;
	GETRENDERFACTORY->CreateRenderTarget(&pRT, rcItem.Width(), rcItem.Height());
	BeforePaintEx(pRT);
	DrawItem(pRT, rcItem, m_arrItems.GetData() + iItem);

	HBITMAP hBmp = CreateBitmap(rcItem.Width(), rcItem.Height(), 1, 32, NULL);
	HDC hdc = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hdc);
	::SelectObject(hMemDC, hBmp);
	HDC hdcSrc = pRT->GetDC(0);
	::BitBlt(hMemDC, 0, 0, rcItem.Width(), rcItem.Height(), hdcSrc, 0, 0, SRCCOPY);
	pRT->ReleaseDC(hdcSrc);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hdc);
	return hBmp;
}

void CGridHeaderCtrl::DrawDraggingState(DWORD dwDragTo)
{
	SOUI::CRect rcClient;
	GetClientRect(&rcClient);
	IRenderTarget *pRT = GetRenderTarget(rcClient, OLEDC_PAINTBKGND);
	SPainter painter;
	BeforePaint(pRT, painter);
	SOUI::CRect rcItem(rcClient.left, rcClient.top, rcClient.left, rcClient.bottom);
	int iDragTo = LOWORD(dwDragTo);
	int iDragFrom = LOWORD(m_dwHitTest);

	SArray<UINT> items;
	for (UINT i = 0; i < m_arrItems.GetCount(); i++)
	{
		if (i != (UINT)iDragFrom) items.Add(i);
	}
	items.InsertAt(iDragTo, iDragFrom);

	if (m_pSkinItem)
		m_pSkinItem->Draw(pRT, rcClient, 0);
	for (UINT i = 0; i < items.GetCount(); i++)
	{
		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + GetItemWidth(items[i]);
		if (items[i] != (UINT)iDragFrom)
			DrawItem(pRT, rcItem, m_arrItems.GetData() + items[i]);
	}
	AfterPaint(pRT, painter);
	ReleaseRenderTarget(pRT);
}

int CGridHeaderCtrl::GetTotalWidth()
{
	int nRet = 0;
	for (UINT i = 0; i < m_arrItems.GetCount(); i++)
	{
		nRet += GetItemWidth(i);
	}
	return nRet;
}

int CGridHeaderCtrl::GetItemWidth(int iItem)
{
	if (iItem < 0 || (UINT)iItem >= m_arrItems.GetCount()) return -1;
	if (!m_arrItems[iItem].bVisible) return 0;
	return m_arrItems[iItem].cx.toPixelSize(GetScale());
}

void CGridHeaderCtrl::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	if (m_bDragging)
	{
		if (m_bSortHeader && m_dwHitTest != -1)
		{
			m_arrItems[LOWORD(m_dwHitTest)].state = 0;//normal
		}
		m_dwHitTest = (DWORD)-1;

		CDragView::EndDrag();
		DeleteObject(m_hDragImg);
		m_hDragImg = NULL;
		m_bDragging = FALSE;
		ReleaseCapture();
		Invalidate();
	}
}

void CGridHeaderCtrl::SetItemSort(int iItem, SHDSORTFLAG stFlag)
{
	SASSERT(iItem >= 0 && iItem < (int)m_arrItems.GetCount());
	if (stFlag != m_arrItems[iItem].stFlag)
	{
		m_arrItems[iItem].stFlag = stFlag;
		SOUI::CRect rcItem = GetItemRect(iItem);
		InvalidateRect(rcItem);
	}
}

void CGridHeaderCtrl::OnColorize(COLORREF cr)
{
	__super::OnColorize(cr);
	if (m_pSkinItem) m_pSkinItem->OnColorize(cr);
	if (m_pSkinSort) m_pSkinSort->OnColorize(cr);
}

HRESULT CGridHeaderCtrl::OnLanguageChanged()
{
	__super::OnLanguageChanged();
	for (UINT i = 0; i < m_arrItems.GetCount(); i++)
	{
		m_arrItems[i].strText.TranslateText();
	}
	return S_FALSE;
}

BOOL CGridHeaderCtrl::OnRelayout(const SOUI::CRect & rcWnd)
{
	BOOL bRet = __super::OnRelayout(rcWnd);
	if (bRet)
	{
		EventHeaderRelayout e(this);
		FireEvent(e);
	}
	return bRet;
}
BOOL CGridHeaderCtrl::IsItemHover(DWORD dwHitTest)
{
	return dwHitTest != -1 && LOWORD(dwHitTest) == HIWORD(dwHitTest);
}
void CGridHeaderCtrl::SetItemVisible(int iItem, bool visible)
{
	SASSERT(iItem >= 0 && iItem < (int)m_arrItems.GetCount());
	m_arrItems[iItem].bVisible = visible;

	Invalidate();
	//发出调节宽度消息
	EventHeaderItemChanged evt(this);
	evt.iItem = iItem;
	evt.nWidth = GetItemWidth(iItem);
	FireEvent(evt);
}

bool CGridHeaderCtrl::IsItemVisible(int iItem) const
{
	SASSERT(iItem >= 0 && iItem < (int)m_arrItems.GetCount());
	return m_arrItems[iItem].bVisible;
}