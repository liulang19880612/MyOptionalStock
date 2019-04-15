#pragma once
#include "CandleChart.h"


enum CURSOR_TYPE
{
	CURSOR_NORMAL = 0,
	CURSOR_SPLIT_HERZ,
	CURSOR_SPLIT_VERT,
	CURSOR_AXIS_HERZ,
	CURSOR_AXIS_VERT,
	CURSOR_PEN,
	CURSOR_EREASE,
	CURSOR_TEXT,
	CURSOR_HAND,
	CURSOR_HANDCLICK,	//鼠标位于锚点上
	CURSOR_MOVING,		//拖动
	CURSOR_DRAG			//拖动锚点
};
struct CursorTip
{
	CRect rc;
	SStringW strTip;
};

class  CRectChart;
class  CTimeAxis;
struct DC_Data;
class  CCandleSeries;
class  CShape;

class CChartCtrl: public SWindow
{
	SOUI_CLASS_NAME(CChartCtrl, L"chartctrl")
public:
	CChartCtrl();
	virtual ~CChartCtrl();
public:
	void    SetContractInfo(const DC_ContractInfo*pInfo, const Period*pPrd);
public:
	virtual void   ShowMouseLine(bool bShow = true);
	virtual bool   IsMouseLineVisible(DC_KData& data);	//是否显示十字光标
	virtual int    AddK(DC_ContractInfo* pInfo, Period *pPrd, size_t nChartIdx, double rate = 1, bool bCombol = false);			//增加副K线，返回索引值
	virtual bool   OnMouseDown(CPoint& pt);
	virtual bool   OnDrag(CPoint& from, CPoint &to, bool bLeft = true);
	virtual bool   OnClick(CPoint &pt, bool bLeft = true);
	virtual bool   OnDragUp(CPoint&from, CPoint &to, bool bLeft = true);
	virtual bool   OnDblClk(CPoint &pt, bool bLeft = true);
public:
	CRect  GetTitleRect();
	void   DrawAxisX(IRenderTarget *pRender);       // 绘制X轴
	void   DrawIcon(IRenderTarget *pRender, int x, int y, int idx);
	void   DrawCursorLine(IRenderTarget* pRender); // 绘制十字线
	void   DrawSpliter(IRenderTarget* pRender);        //绘制分割线
	void   DrawMouseTip(IRenderTarget *pRender);
public:
	int    GetChatAxisYFitWidth();
	void   SetCandlePix(double CandlePix);
	double GetCandlePix(){ return m_fCandlePix; }
	void   UpdateHisData(DC_Data* pHisData, int nCount, int nData, int nIDK, double fPriceOrigin);
	void   RefreshCtrl();
	void   ReCalcLayout();
	bool   IsShowAxis(){ return m_bShowAxis; }
	void   SetCursor(CURSOR_TYPE type);
	int    GetChartByPos(CPoint& pt);   //根据坐标获得图表索引
	void   SetCursorByPos(CPoint& pt);   //设置鼠标样式
	int    GetSpliterByPos(CPoint& pt);  //根据坐标判断当前的分隔条
	CRect  GetPricePadRect(){ return m_rcPricePad; }
	CRect  GetInfoPanelRect(){ return m_rcInfoPanel; }
	bool   IsMouseLineVisible(){ return m_bShowMouseLine; }
	void   CheckMouseTip(CPoint pt);   //鼠标提示检查
	int    GetMaxKNum(){ return m_nMaxPointNum; }
	CPoint GetCursorPos(){ return m_curCursorPos; }
	RSStruct& GetRsStruct(){ return m_rsData; }
	void CalRegionStatisticData(int nStar = 0, int nEnd = 0, bool bRecal = false);
	CRectChart* GetCurrentChart();
	bool        IsMouseDown();
public:
	void  Reset();
	void  CancelSelect();
private:
	void  SizeToRateSize();      //将绝对坐标转化为相对坐标
	void  OnDragAxisX(CPoint& from, CPoint &pt, bool bLeft);//拖动x轴
private:
	virtual UINT OnGetDlgCode();
	int    OnCreate(LPCREATESTRUCT lpCreateStruct);
	void   OnPaint(IRenderTarget *pRender);
	void   OnSize(UINT nType, CSize size);
	void   OnLButtonDown(UINT nFlags, CPoint point);
	void   OnLButtonUp(UINT nFlags, CPoint point);
	void   OnLButtonDblClk(UINT nFlags, CPoint point);
	void   OnMouseMove(UINT nFlags, CPoint point);
	void   OnMouseLeave();
	void   OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void   OnKillFocus(SWND wndFocus);
	void   OnSetFocus(SWND wndOld);
protected:
	SOUI_MSG_MAP_BEGIN()
		MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_SIZE(OnSize)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_SETFOCUS_EX(OnSetFocus)
		MSG_WM_KILLFOCUS_EX(OnKillFocus)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	SOUI_MSG_MAP_END()

	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"backgroundskin", m_pSkinBackGround, TRUE)
		ATTR_INT(L"defaultaxiswidth", m_DefYAxisWidth, TRUE)
		ATTR_INT(L"titlehight", m_nTitleHeight, TRUE)
		ATTR_COLOR(L"clrCursorLine", m_clrCursorLine,TRUE)
	SOUI_ATTRS_END()
public:
	bool      IsRevert(){ return m_bIsRevert; }
protected:
	ISkinObj*                                   m_pSkinBackGround; // 背景皮肤
	IRenderTarget*                              m_pMemRender;
	SArray<CursorTip>                           m_arrTip;
	SArray<CRectChart*>                         m_arrChart;     // 图表列表
	int                                         m_nCurChart;    // 当前鼠标点击的图表
	int                                         m_MaxmizeChart; // 最大化图表
	CTimeAxis*                                  m_pAxisX;         // X轴
	bool                                        m_bShowAxis;
	int                                         m_DefYAxisWidth;
	int                                         m_nTitleHeight;   // 标题高度
	int                                         m_nXAxisHeight;   // 横坐标高度
	double                                      m_fCandlePix;
	SMap<int, CCandleSeries*>                  m_mapCandleSeries;
	bool                                        m_bRedrawControls;
	bool                                        m_bIsRevert;
	int                                         m_curKIdx;
	KLINE_TYPE                                  m_nKType;
	CCandleSeries*                              m_pMainCandleSeries;
	bool                                        m_bDesytoryed;
	int                                         m_curSpliter;
	CRect                                       m_cursorRc;    //鼠标位置
	CURSOR_TYPE                                 m_cursor;      //光标类型
	CRect                                       m_rcPricePad;
	CRect                                       m_rcInfoPanel;
	bool                                        m_bShowMouseLine;
	int                                         m_nCurCandleIdx;
	bool                                        m_bUserMoveMouse;
	bool                                        m_bMouseIn;
	CPoint                                      m_PtMouseDown;
	bool                                        m_bMouseDown;
	bool                                        m_bIsDbClick;
	bool                                        m_bLeftBtnClick;
	DWORD                                       m_tickLastDraw;
	bool                                        m_bAnimateFirstStep;
	bool                                        m_bPlaying;
	CPoint                                      m_curCursorPos;
	std::map<std::string, std::vector<CShape*>> m_mapShape;
	bool                                        m_bIsDraging;
	bool                                        m_bHaveMovedPricePad;
	int                                         m_nCurMouseTip;
	int                                         m_nShowMouseTip;
	bool                                        m_bCreated;
	bool                                        m_bDragingAxisX;
	bool                                        m_bDragingScrollBar;
	bool                                        m_bCanGetMore;
	SStringW                                    m_strContPeriod;
	int                                         m_nMaxPointNum;			//一屏幕显示最大数据量
	CPoint                                      m_ptCursorLastDraw;
	RSStruct                                    m_rsData;
	bool                                        m_bIsBuffing;
	int                                         m_nAnimateStep;
	bool                                        m_bDataComplete;
	COLORREF                                    m_clrCursorLine;
	SArray<SStringW>                            m_strPeriodWords;

	Period                                      m_Period;            // 周期类型
	DC_ContractInfo                             m_ContractInfo;      // 合约代码
};