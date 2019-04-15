#pragma once
class CAxis;	    //纵坐标
class CTimeAxis;	//时间坐标
class CKChart;	    //图表

#include "SeriesData.h"
#include "CandleChart.h"

class CSeriesObject : public SObject
{
public:
	CSeriesObject();
	virtual ~CSeriesObject();
public:
	virtual void Draw(IRenderTarget* pRender){};
	virtual bool GetMinMaxValue(double& fMin, double& fMax){return FALSE;}		//计算横坐标范围内数据的最高值和最低值
	virtual double GetFirstVisibleValue(){return 0;}		                    //获得当前屏幕第一个元素的值
	virtual void SetChart(CKChart *pChart);
	virtual int HitTest(CPoint pt){ return -1; }	                             //返回值：<0 没有被选中，>=0坐标索引，从右向左
	virtual void Select(bool bSelect = TRUE, int nSubIdx = -1){ m_bSelected = bSelect; }	//激活
	virtual CAxis* GetSelectAxis();		                                    //返回当前激活序列的纵坐标
	virtual int DrawTitle(IRenderTarget* pRender, CRect& rc){ return 0; }	//绘制标题，返回标题宽度
public:
	CKChart* GetChart(){return m_pChart;}	             //获取所在的图表
	void     SetMinMaxValue(double fMin, double fMax);
	void     SetOwnAxis(bool bOwn=TRUE){m_bOwnAxis = bOwn;}  //是否使用独立坐标
	bool     IsOnwnAxis(){return m_bOwnAxis;}	//是否使用独立坐标
	bool     IsSelected(){return m_bSelected;}	//是否被选中了
	void     SetShowWavePrice(bool bShow){ m_bShowWavePrice = bShow;}
	SStringW GetParams(){return m_strParams;}
	void     SetAxisHoriz(CTimeAxis* pAxis){ m_pAxisHoriz = pAxis; }
	CTimeAxis*GetAxisHoriz(){ return m_pAxisHoriz; }
	void     SetAxisVert(CAxis*pAxis){ m_pAxisVert = pAxis; }
	CAxis*   GetAxisVert(){ return m_pAxisVert; }
protected:
	bool               m_bOwnAxis;	      // 是否为独立坐标
	SStringW           m_strParams;	      // 参数
	CPoint             m_ptLastViewBox;   // 记录横坐标的起点和终点位置
	double             m_fMin;
	double             m_fMax;
	bool               m_bSelected;
	CKChart*           m_pChart;
	CTimeAxis*         m_pAxisHoriz;
	CAxis*             m_pAxisVert;
	bool               m_bUseSysColor;
	int		           m_nPenThick;
	SStringW           m_strName;
	bool               m_bShowWavePrice;
	int                m_nIcon;
	COLORREF           m_crPen;
};

//K线
typedef CSeriesData<DC_KData> CCandleSeriesData;

class CCandleSeries : public CSeriesObject
{
public:
	CCandleSeries();
	void SetStock(DC_ContractInfo *pInfo, Period*pPrd, time_t tmStart = 0);
	void SetCombol(DC_ContractInfo *ci1, DC_ContractInfo* ci2,Period*pPrd, double fRate, time_t tmStart)
	{
		m_bCombol = TRUE;
		m_contract2 = *ci2;
		m_fAppendRate = fRate;
		SetStock(ci1, pPrd,tmStart);
	}
	virtual ~CCandleSeries(void);
	time_t GetStartTime(){return m_tmStart;}
	virtual void	Draw(IRenderTarget* pRender);
	virtual bool GetMinMaxValue(double& fMin, double& fMax);
	virtual int HitTest(CPoint pt);
	virtual double GetFirstVisibleValue();
	
	void SetKType(KLINE_TYPE type){m_nKType=type;}	//K线图样式
	bool IsCombol(){return m_bCombol;}
	double GetAvgPrcie();
	void UpdateHisData(DC_KData* pData, int nCount,int nData);
	double GetWavePercent();
	void CalWaveMinMaxPrice(int nMin, int nMax); //计算波段高低价
	double Pivot(bool bHiLo, double LeftPrice, double CurPrice, double RightPrice);//求转折
public:

	void DrawNameLabel(IRenderTarget* pRender);
	CRect m_rcDrager;
protected:
	//绘制屏幕高低价
	void DrawScreenMinMaxPrice(IRenderTarget* pRender,int maxX,int minX,COLORREF clrNormal);
	//绘制波段高低价
	void DrawWaveMinMaxPrice(IRenderTarget* pRender,int nCur,COLORREF clrUp,COLORREF clrDown);
	void CaluAvgPrice();		//计算均价	
protected:
	bool m_bCombol;	//是否为组合
	void DrawDot(IRenderTarget* pRender);
	void DrawGBL(IRenderTarget* pRender);
public:
	void SetID(int id){ m_ID = id; }
	int  GetID(){ return m_ID; }
	void EnableSubK(bool b){ m_bSubK = b; }
	void SetContractInfo(const DC_ContractInfo*pIfo){ m_contract = *pIfo; }
	DC_ContractInfo*GetContractInfo(){ return &m_contract; }
	void SetContractInfo2(const DC_ContractInfo*pIfo){ m_contract2 = *pIfo; }
	DC_ContractInfo*GetContractInfo2(){ return &m_contract2; }
	void SetAppendRate(double rate){ m_fAppendRate = rate; }
	CCandleSeriesData*GetSeriesData(){ return &m_SerirsData; }
protected:
	KLINE_TYPE             m_nKType;
	bool                   m_bIsBuffing;
	time_t                 m_tmStart;
	bool                   m_bHaveSetPect;  //当前合约是否已经设置了小数点位数
	int                    m_nLastAppendIdx;
	int                    m_nLastKCount;
	double                 m_fLastKPrice;
	double                 m_fSumPrice;
	SArray<int>            m_arrMaxMinPrice; //K线索引 1波峰 -1波谷 0 中间点
	int                    m_nWaveLastKCount;
	int                    m_nWaveLastStartIdx;
	CRect                  m_titleRc;
	int                    m_ID;
	double                 m_fAppendRate;
	bool                   m_bSubK;
	CCandleSeriesData      m_SerirsData;
	Period                 m_prd;
	DC_ContractInfo        m_contract;
	DC_ContractInfo        m_contract2;
};


//指标线
typedef CSeriesData<SeriesItem> CIndicatorSeriesData;
class CIndicatorSeries: public CSeriesObject, public CIndicatorSeriesData
{
public:	
	CIndicatorSeries();
	virtual ~CIndicatorSeries(void){}
	virtual void Draw(IRenderTarget* pRender);
	virtual bool GetMinMaxValue(double& fMin, double& fMax);
	virtual int HitTest(CPoint pt);
	void SetSeriesData(Series *pSeries,bool bCover=FALSE);
	virtual double GetFirstVisibleValue();
	SeriesType m_nSeriesType;
	CSeriesObject* m_pGroup;
protected:
	void DrawIndicator(IRenderTarget* pRender);		//绘制指标线序列
	void DrawDot(IRenderTarget* pRender);		//绘制点序列
	void DrawStickLine(IRenderTarget* pRender);		//绘制竖线段序列
	void DrawBar(IRenderTarget* pRender);		//绘制柱状线序列
	void DrawVertLine(IRenderTarget* pRender);
	void DrawIcon(IRenderTarget* pRender);		//绘制图标序列
	void DrawTradePoint(IRenderTarget* pRender);	//绘制交易指令
	void DrawPartLine(IRenderTarget* pRender);		//绘制线段序列
	void DrawCandle(IRenderTarget* pRender);
protected:
	//HitTest
	int HitTestIndicator(CPoint pt); //指标线碰撞检测
	int HitTestBar(CPoint pt); //指标线碰撞检测
	int HitTestStickLine(CPoint pt); //指标线碰撞检测
	int HitTestDot(CPoint pt);
	int HitTestIcon(CPoint pt);
	int HitTestPartLine(CPoint pt);	

};

//指标
class CIndicatorGroup : public CSeriesObject
{
public:
	CIndicatorGroup();
	virtual ~CIndicatorGroup(void);
public:
	virtual void Draw(IRenderTarget* pRender);
	virtual void SetData(const SeriesGroup* pSereisGroup);
	virtual bool GetMinMaxValue(double& fMin, double& fMax);
	virtual int HitTest(CPoint pt);
	virtual CAxis* GetSelectAxis();		//返回当前激活序列的纵坐标
	virtual void SetChart(CKChart *pChart);
	virtual double GetFirstVisibleValue();
	virtual	void Select(bool bSelect=TRUE,int nSubIdx=-1);
public:
	void DrawText(IRenderTarget* pRender);
	CRect GetTitleRect(){ return m_titleRc; }
protected:
	SArray<CIndicatorSeries*>      m_arrIndicators;
	SStringW                       m_strTip;
	SStringW                       m_strId;
	int                            m_nTextItemCount;
	int                            m_nTextItemSize;
	SeriesTxtItem*                 m_pTxtItem;
	CRect                          m_titleRc;
	int                            m_nSeleSeires;
};

struct TradePointRect
{
	int id;
	CRect rc;
};

struct TradePoint_Line
{
	int nIdxOpen;
	int nIdxClose;
	int nType;		//0:buy 1:sellShort
};


class CTradePointSeries : public CSeriesObject,public TradePointGroup
{
public:
	CTradePointSeries(SStringW strSereisGroup);
	virtual ~CTradePointSeries();
public:
	virtual void	Draw(IRenderTarget* pRender);
	virtual int HitTest(CPoint pt);
	void EnableDrawLine(bool enable){ m_bDrawLine = enable; }
protected:
	void DrawTradePointLine( IRenderTarget* pRender);
	void CaluTradePointLine();	//计算交易指令关联
protected:
	SArray<TradePoint_Line> m_arrTradePointLine;
	SArray<TradePoint>      m_arrCoveredTradePoint;
	int                     m_nFirstNotCoverIdx;
	bool                    m_bAccTradePoint;
	bool                    m_bDrawLine;
	SStringW                m_strSeriesGroup;	//该交易信号的发出者SeriesGroupID
	SArray<TradePointRect>  m_arrRectTradePoint;		//存放没有交易指令箭头的位置
};