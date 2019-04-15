#pragma once

//////////////////////////////////////////////////////////////////////////
// ��չ��ͷ�ؼ�
//////////////////////////////////////////////////////////////////////////

struct MerginInfo
{
	int nBeginIndex, nEndIndex;
	SStringW  strMerginName;
};
class CGridHeaderCtrl : public SWindow
{
	SOUI_CLASS_NAME(CGridHeaderCtrl, L"gridheaderctrl")
public:
	CGridHeaderCtrl(void);
	~CGridHeaderCtrl(void);
public:
	int  InsertItem(int iItem, LPCTSTR pszText, int nWidth, SHDSORTFLAG stFlag, LPARAM lParam);
	int  InsertItem(int iItem, LPCTSTR pszText, int nWidth, SLayoutSize::Unit unit, SHDSORTFLAG stFlag, LPARAM lParam);
	BOOL GetItem(int iItem, SHDITEM *pItem);
	size_t GetItemCount() const{ return m_arrItems.GetCount(); }
	int GetTotalWidth();
	int GetItemWidth(int iItem);
	BOOL DeleteItem(int iItem);
	void DeleteAllItems();
	void SetItemSort(int iItem, SHDSORTFLAG stFlag);
	void SetItemVisible(int iItem, bool visible);
	bool IsItemVisible(int iItem) const;
	SOUI::CRect GetItemRect(UINT iItem) const;
	int GetOriItemIndex(int iOrder) const;
	bool isViewWidthMatchParent() const;
protected:
	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"itemSkin", m_pSkinItem, FALSE)
		ATTR_SKIN(L"sortSkin", m_pSkinSort, FALSE)
		ATTR_INT(L"fixWidth", m_bFixWidth, FALSE)
		ATTR_INT(L"itemSwapEnable", m_bItemSwapEnable, FALSE)
		ATTR_INT(L"sortHeader", m_bSortHeader, FALSE)
		ATTR_INT(L"matchParent", m_bMatchParent, FALSE)
		ATTR_INT(L"fixedcol", m_nFixedItems, FALSE)
		
	SOUI_ATTRS_END()
protected:
	virtual BOOL CreateChildren(pugi::xml_node xmlNode);
	virtual BOOL OnSetCursor(const SOUI::CPoint &pt);
	virtual void DrawItem(IRenderTarget * pRT, SOUI::CRect rcItem, const LPSHDITEM pItem);
	virtual void OnColorize(COLORREF cr);
	virtual HRESULT OnLanguageChanged();
	virtual BOOL OnRelayout(const SOUI::CRect &rcWnd);


	DWORD  HitTest(SOUI::CPoint pt);
	BOOL IsItemHover(DWORD dwHitTest);
	HBITMAP CreateDragImage(UINT iItem);
	void RedrawItem(int iItem);
	void DrawDraggingState(DWORD dwDragTo);
	void OnPaint(IRenderTarget * pRT);
	void OnLButtonDown(UINT nFlags, SOUI::CPoint pt);
	void OnLButtonUp(UINT nFlags, SOUI::CPoint pt);
	void OnMouseMove(UINT nFlags, SOUI::CPoint pt);
	void OnMouseLeave();
	void OnDestroy();
	void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	void OnSize(UINT nType, CSize size);
	void UpdataWidth();
protected:
	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_ACTIVATEAPP(OnActivateApp)
		MSG_WM_SIZE(OnSize)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER_EX(UM_SETSCALE, OnSetScale)
	SOUI_MSG_MAP_END()
protected:
	ISkinObj *      m_pSkinItem;           /**< ��ͷ����Skin */
	ISkinObj *      m_pSkinSort;           /**< �����־Skin */
	BOOL            m_bSortHeader;         /**< ��ͷ���Ե������ */
	BOOL            m_bFixWidth;           /**< �����ȹ̶����� */
	BOOL            m_bItemSwapEnable;     /**< �����϶�����λ�ÿ��� */

	BOOL            m_bDragging;           /**< �����϶���־ */
	HBITMAP         m_hDragImg;            /**< ��ʾ�϶����ڵ���ʱλͼ */
	SOUI::CPoint          m_ptClick;             /**< ��ǰ������� */
	DWORD           m_dwHitTest;           /**< ���λ�� */
	DWORD           m_dwDragTo;            /**< �Ϸ�Ŀ�� */
	int             m_nAdjItemOldWidth;    /**< ���汻�϶����ԭʼ��� */
	SArray<SHDITEM> m_arrItems;            /**< �б���� */
	BOOL            m_bMatchParent;        /**< �п�ռ�� */
	SArray<int>     m_widItems;            /**< �б���� */
	
	SArray<MerginInfo> m_arrMerginInfo;    /**< �б�ϲ���Ϣ */
	int                m_nFixedItems;      /**< �̶����� */    
};