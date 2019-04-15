#pragma once
#include "RectChart.h"
class CAxis;
class CTimeAxis;

class CAxisChart : public CRectChart
{
public:
	CAxisChart();
	virtual ~CAxisChart();
public:
	virtual void CreateAxis();    // 创建坐标轴
	virtual void ReCalcLayout();  // 计算区域
	virtual int  GetAxisYWith();  // 获取左右两边的边距
	virtual void SetAxisYWith(int nWidth);
	virtual void SetAutoAxix(bool bAuto){ m_bAutoAxis = bAuto; }
	virtual int  GetCursor(CPoint& pt);
	virtual bool OnDrag(CPoint& from, CPoint &pt, bool bLeft);
	virtual bool OnClick(CPoint &pt, bool bLeft = true);
	virtual bool OnDragUp(CPoint&from, CPoint &pt, bool bLeft = true);
	virtual bool OnDblClk(CPoint &pt, bool bLeft = true);
	virtual bool OnMouseMove(CPoint& pt);
public:
	void        SetTimeAxisX(CTimeAxis*pAxis){ m_pAxisX = pAxis; }
	CTimeAxis*  GetTimeAxisX(){ return m_pAxisX; }
	void        SetMainChart();                        //设置为主图
	bool        IsMainChart(){ return m_bMainChart; }
	CRect       GetRectObj(){ return m_rcObj; }
	void        DrawAxis(IRenderTarget* pRender);
	void        DrawZoomRect(IRenderTarget* pRender);
	void        DrawRegionRect(IRenderTarget* pRender);
	CAxis*      GetYAxis(){ return m_pAxisY; }
	CAxis*      GetSubAxis(){ return m_pAxisSub; }
	CTimeAxis*  GetXAxis(){ return m_pAxisX; }
	CRect       GetObjRect(){ return m_rcObj; };
	void        CalRegionStatisticRange(int& nStar, int& nEnd, bool bRecal = false);
protected:
	CTimeAxis*                m_pAxisX;         // 横坐标
	CAxis*                    m_pAxisY;         // 纵坐标
	CAxis*                    m_pAxisSub;		// 副坐标
	bool                      m_bShowSubAixs;	// 是否显示副坐标
	bool                      m_bAutoAxis;		// 是否自动更新纵坐标
	bool                      m_bCanDragAxis;	// 坐标是否允许拖动
	bool                      m_bMainChart;
	CRect                     m_rcObj;
	CAxis*                    m_pCurAxisVert;
	SArray<CAxis*>            m_arrOwnAixs;
	CPoint                    m_ptPadOffset;
	BOOL                      m_bDragingPricePad;
	int                       m_curDragingAxis;	    // 当前正在拖动的坐标
	BOOL                      m_bIsLeftDraging;		   
	CRect                     m_zoomRc;	            // 绘制缩放矩形
	double                    m_fLastAxisMin, 		   
		                      m_fLastAxisMax;	    // 记录开始拖动时刻的坐标最大、最小值、最大范围
	int                       m_nOffsetScrollBar;	// 拖动滚动条时，鼠标与滚动条左侧的偏移值
	int                       m_nLastDuration;		   
	BOOL                      m_bDragingScrollBar;	// 是否正在拖动滚动条
	CRect                     m_regionRc;

};

