#pragma once
#include "ShapeChart.h"
#include "SeriesObject.h"

struct ButtonInfo
{
	int nImage;
	int type;
	SStringW title;
	SStringW detail;
	CRect rc;
	int barIdx;
	int place;
	LPARAM lParam;
};
class CKChart : public CShapeChart
{
public:
	CKChart();
	virtual ~CKChart();
public:
	virtual bool OnClick(CPoint &pt, bool bLeft = true);
	virtual bool OnMouseMove(CPoint& pt);
	virtual void CaluAixs();	        //计算或者设置个序列的坐标
	virtual void CalcMinMaxValue();		//计算纵坐标最大最小值，需要调整坐标时调用该函数
	virtual void DrawObjects(IRenderTarget* pRender, bool bRedraw = true);
	virtual void DrawSeries(IRenderTarget* pRender);		//绘制图表
	virtual bool HitTest(ObjectInfo& info, CPoint pt);
	virtual void KillFocus(); // 失去焦点
	virtual bool OnDrag(CPoint& from, CPoint &pt, bool bLeft);
public:
	void DrawPricePad(IRenderTarget* pRender);	      // 绘制价格面板
	void DrawInfoPanelButton(IRenderTarget* pRender); // 绘制打开信息面板按钮
	void DrawNewPrice(IRenderTarget* pRender);        // 绘制最新价
	void DrawTipButtons(IRenderTarget* pRender);
	void AddK(int id, CSeriesObject* pSeries);
	CSeriesData<SeriesItem> *GetColorSeries(){ return m_pClrSeires; };			//变色K线序列
	void SetColorSeries(CSeriesData<SeriesItem> *pSeries);
	void RemoveColorSeries(CSeriesData<SeriesItem> *pSeries);
	void AddTradePoint(TradePoint data);
protected:
	int                        m_idDragingSub;
	CCandleSeries *            m_pDragingSub;
	CSeriesData<SeriesItem> *  m_pClrSeires;			// 变色K线序列
	SMap<int, CSeriesObject*>  m_mapCandleSeries;
	SMap<int, CSeriesObject*>  m_mapIndiCatorSeries;
	SMap<int, CSeriesObject*>  m_mapTradePoint;
	SMap<int, CSeriesObject*>  m_mapAccTradePoint;
	BOOL                       m_bHaveSelObject;
	int                        m_nIcon;
	CSeriesObject*             m_pSelSeries;		    // 当前选择的K线或指标
	CSeriesObject*             m_pMainK;	            // 主K线
	CSeriesObject*             m_pMainIndicator;        // 主指标
	BOOL                       m_bPadAliginLeft;
	CTradePointSeries          m_userTradePoint;
	SArray<TradePoint>         m_arrTradePoint;
	SArray<ButtonInfo>         m_arrButtons;
};

