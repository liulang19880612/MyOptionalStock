#pragma once
#include "Axis.h"

//////////////////////////////////////////////////////////////////////////
//  时间坐标轴
//////////////////////////////////////////////////////////////////////////

class CTimeAxis : public CAxis
{
public:
	CTimeAxis(bool bHorizonal, CAxisChart *pChart);
public:
	virtual void   Draw(IRenderTarget *pRender);
	virtual void   DrawAxisTip(IRenderTarget *pRender);// 绘制游标
	virtual double GetMinValue();
	virtual double GetMaxValue();
	virtual void   SetHisData(CSeriesData<DC_KData> *pData);
	virtual double Pix2Value(int pix);
	virtual int    Value2Pix(double fValue);
	virtual void   UpdateHisData();
	virtual int    GetFreeCandleCount(){ return FREE_CANDLE_COUNT; }
	virtual double GetDuration();		//返回值为K线个数-1，即最大元素下标
	CSeriesData<DC_KData>* GetHisData();
	int  GetDataCount();
	int  GetCandleWidth(); //获得一个蜡烛的宽度
	SStringW JudgeShowLabel(const CTime& curTm, const CTime& lastTm, const CTime& nxtTime, bool& bBefor);
	time_t  GetTimeDiff();
	void    SetFocus(bool bFocus = true){ m_bFocus = bFocus; }    //设置焦点
	CRect   GetScrllBarRect(){ return m_rcScrollBar; }
	int     GetXValueByTime(long time);
	void    SetContract(const DC_ContractInfo* ci);          // 设置合约信息
	DC_ContractInfo* GetContract(){ return &m_contract; }    // 获取合约信息
	void SetFocusIndex(int nIdx);
public:
	bool                   m_bFocus;              // 是否焦点
	Period                 m_Period;		      // 周期类型
	DC_ContractInfo        m_contract;            // 合约信息
	CRect                  m_rcScrollBar;         // 滚动条
	int                    m_nFocusIndex;
	CSeriesData<DC_KData> *m_pStkHisData;
	int                    m_nScreenCandleNum;
	int                    m_nOldCount;
	int                    m_nDataCount;
};

//////////////////////////////////////////////////////////////////////////
//  分时图时间坐标轴
//////////////////////////////////////////////////////////////////////////

class CDynaTimeAxis : public CTimeAxis
{
public:
	virtual double GetMaxValue();
	virtual ~CDynaTimeAxis(){ ; }
	virtual void Draw(IRenderTarget *pRender);
	virtual void DrawAxisTip(IRenderTarget *pRender);
	CDynaTimeAxis(bool bHorizonal, CAxisChart *pChart);
	int GetFreeCandleCount(){ return 0; }

};