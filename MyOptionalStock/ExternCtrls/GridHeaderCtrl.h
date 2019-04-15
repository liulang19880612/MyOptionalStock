#pragma once

//////////////////////////////////////////////////////////////////////////
// 扩展表头控件
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
	ISkinObj *      m_pSkinItem;           /**< 表头绘制Skin */
	ISkinObj *      m_pSkinSort;           /**< 排序标志Skin */
	BOOL            m_bSortHeader;         /**< 表头可以点击排序 */
	BOOL            m_bFixWidth;           /**< 表项宽度固定开关 */
	BOOL            m_bItemSwapEnable;     /**< 允许拖动调整位置开关 */

	BOOL            m_bDragging;           /**< 正在拖动标志 */
	HBITMAP         m_hDragImg;            /**< 显示拖动窗口的临时位图 */
	SOUI::CPoint          m_ptClick;             /**< 当前点击坐标 */
	DWORD           m_dwHitTest;           /**< 鼠标位置 */
	DWORD           m_dwDragTo;            /**< 拖放目标 */
	int             m_nAdjItemOldWidth;    /**< 保存被拖动项的原始宽度 */
	SArray<SHDITEM> m_arrItems;            /**< 列表项集合 */
	BOOL            m_bMatchParent;        /**< 列宽占比 */
	SArray<int>     m_widItems;            /**< 列表项集合 */
	
	SArray<MerginInfo> m_arrMerginInfo;    /**< 列表合并信息 */
	int                m_nFixedItems;      /**< 固定列数 */    
};