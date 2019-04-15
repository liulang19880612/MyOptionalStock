#pragma once
#include "CandleChart.h"
#include "SeriesData.h"
#include <atltime.h>

#define PRICE_BTM   (-19873)
#define PRICE_TOP   (-19872)

ColorSet*  ES_Color();
class CAxisChart;
// 坐标轴
class CAxis: public CRect
{
	friend class CChartCtrl;
public:
	CAxis(bool bHorizonal, CAxisChart* pChart);
	virtual ~CAxis();
public:
	virtual void SetMinMaxValue(double fMin, double fMax);	// 坐标最大值最小值
	virtual double GetMinValue(){ return m_fMin; }          // 获取最大值
	virtual double GetMaxValue() { return m_fMax; }         // 获取最小值
	virtual double GetDuration(){ return 1000000.0; }	//获得坐标最大范围
public:
	void SetFitWidth(int nFitWid){ m_nFitWid = nFitWid; }
	int GetFitWidth(){ return m_nFitWid; }
	virtual int Value2Pix(double fValue);			//根据数值计算出像素坐标位置
	virtual double Pix2Value(int pix);				//根据像素值计算出数值
	virtual void Draw(IRenderTarget*pRender);
	void DrawPriceAxis(IRenderTarget*pRender);	            //绘制普通坐标
	void DrawDengbi(IRenderTarget*pRender);		            //绘制等比坐标
	void DrawPecentAxis(IRenderTarget*pRender);	            //绘制百分比坐标
	void DrawHJFG(IRenderTarget*pRender, int nType);        //绘制黄金分割点
	virtual void DrawAxisTip(IRenderTarget *pRender);		//绘制游标

	int GetPrect(){ return m_nPrec; }
	void SetChart(CAxisChart*pChart){ m_pChart = pChart; }
	CAxisChart* GetChart(){ return m_pChart; }
	void SetReVert(bool bRevert = false){ m_bRevert = bRevert; }	//反转
	bool IsRevert(){ return m_bRevert; }	//是否反转
	bool IsMouseLineVisble();		//是否显示鼠标线
	void SetAlign(int nALign){ m_nAlign = nALign % 2; }
	void SetOriginValue(double value){ m_fOrigin = value; }		//设置原点值
	double GetOriginValue(){ return m_fOrigin; }  //获得原点值
	double GetMinTick(){ return m_fTick; }
	void SetPrect(int n, double fTick);
	void EnableDraging(bool b){ m_bIsDraging = b; }
	CPoint GetCursorPos();
protected:
	int                    m_nFitWid;
	double                 m_fTick;
	double                 m_fOrigin;	     //原点值,供计算百分比坐标
	int                    m_nAlign;	     //停靠位置
	bool                   m_bSub;
	double                 m_fMin;	         // 最小值
	double                 m_fMax;	         // 最大值
	CAxisChart*            m_pChart;	     // 依赖图表
	bool                   m_bHorizonal;     // 是否是横坐标
	bool                   m_bRevert;        // 是否反转
	int	                   m_nPrec;	         // 小数点位数
	AXI_SCALE_STYPE        m_nAxisType;      // 坐标类型
	bool                   m_bIsDraging;     // 是否正在拖动
};

class CDynaPctAxis : public CAxis
{
public:
	CDynaPctAxis(bool bHorizonal, CAxisChart* pChart);
	virtual ~CDynaPctAxis();
public:
	virtual void Draw(IRenderTarget *pRender);
};