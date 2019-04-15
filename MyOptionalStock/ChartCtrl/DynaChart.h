#pragma once
#include "KChart.h"
class CAxis;

struct AppendKItem
{
public:
	AppendKItem()
	{
		rcTitile.SetRectEmpty();
		m_bSelect = false;
	}
	DC_ContractInfo ci;
	int id;
	int nLastDataIdx;
	double fOriginPrice;
	SArray<DC_KData> arrData;
	bool m_bSelect;
	double fMin;
	double fMax;
	CAxis* pAxisVert;
	CRect rcTitile;
};

struct TimeSpan
{
	__time32_t tmStart;
	__time32_t tmEnd;
};


class CDynaChart : public CKChart
{
public:
	CDynaChart();
	virtual ~CDynaChart();
public:
	void  SetStock(DC_ContractInfo* pInfo);
	void  ClearData();
	void  SetPowerType(int nType);
	void  SetOriginPrice(double fPrice, int idK);
	void  UpdateHisData(const DC_KData* pData, int nCount, int nData, int idK);
	void  UpdateData(int nDataCount);	//根据1分钟数据，转化分时图的数据
	int   AppendK(DC_ContractInfo* ci);
	bool  RemoveK(DC_ContractInfo* ci);
	bool  IsBuffing(){ return m_bIsBuffing; }
	int   GetDataCount(){ return m_RealDataCount; }
public:
	virtual void CalcMinMaxValue();
	virtual void CreateAxis();
	virtual bool HitTest(ObjectInfo& info, CPoint pt);
	virtual void DrawSeries(IRenderTarget* pRender);		//绘制指标
	void DrawDynaSeries(IRenderTarget *pRender);		//绘制分时图
protected:
	DC_HisData                m_hisData;
	SArray<DC_KData>          m_arrData;
	SArray<double>            m_arrAvg;              // 均线
	CSeriesData<DC_KData>     m_dynaData;
	bool                      m_bHaveDestroy;
	bool                      m_bIsBuffing;
	double                    m_fMin;		         // 今日最低
	double                    m_fMax;		         // 今日最高
	bool                      m_bSimHistory;
	double                    m_fYClose;	         // 昨收，昨结算
	unsigned long long        m_nLastDayAmount;	     // 昨持仓
	int                       m_pect;                // 精确度-小数位数
	double                    m_fMinMove;
	int                       m_lastDataIdx;	     // 上次计算后，最后一个转换后可用数据的最后坐标

	unsigned long long        m_nLastAmount;	     // 最新成交量
	int                       m_RealDataCount;		 // 上次计算后，实际数据的长度
	SStringW                  m_strSymbol;
	int                       m_nDynaDays;
	SArray<AppendKItem>       m_arrAppendData;
	DC_ContractInfo           m_Contract;            //  合约信息
};

