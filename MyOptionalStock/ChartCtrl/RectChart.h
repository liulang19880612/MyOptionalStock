#pragma once
#include "SeriesData.h"

class CChartCtrl;

class CRectChart: public CRect
{
public:
	CRectChart();
	virtual ~CRectChart();
public:

	CChartCtrl*    GetChartCtrl(){ return m_pChartCtrl; }
	double         GetRateSize(){ return m_RateSize; };
	void           SetRateSize(double rz){ m_RateSize = rz; }
	bool           IsMouseLineVisble();
	void           SetChartCtrl(CChartCtrl*pChart){ m_pChartCtrl = pChart; }
	void           SetChartIndex(int nIndex){ m_nChartIndex = nIndex; }
public:
	virtual int    GetAxisYWith() = 0;
	virtual void   SetAxisYWith(int nWidth) = 0;
	virtual void   SetAutoAxix(bool bAuto) = 0;
	virtual int    GetCursor(CPoint& pt){ return 0; }
	virtual void   ReCalcLayout();
	virtual void   Draw(IRenderTarget*pRender, bool bRedraw = true);             // 绘制
	virtual void   DrawObjects(IRenderTarget* pDc, bool bRedraw = true){};       // 绘制其他东西（子类实现之）
	virtual void   Refresh(bool quick = false, bool bliji = false);              // 刷新
public:
	//鼠标事件
	virtual bool OnDrag(CPoint& from, CPoint &pt, bool bLeft = true){ return false; }
	virtual bool OnClick(CPoint &pt, bool bLeft = true){ return false; }
	virtual bool OnDragUp(CPoint&from, CPoint &pt, bool bLeft = true){ return false; }
	virtual bool OnDblClk(CPoint &pt, bool bLeft = true){ return false; }
	virtual bool OnMouseMove(CPoint& pt){ return false; }
	virtual bool OnMouseDown(CPoint& pt){ return false; }
	virtual void KillFocus() = 0; //失去焦点处理
	bool PtInChartCtrl(CPoint pt);	//点是否在控件内
	CPoint GetCursorPos();
	void DrawIcon(IRenderTarget *pRender, int x, int y, int idx); //画图标
	//消息
	void PostMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	void SetCursor(int type);  //设置鼠标
protected:
	CChartCtrl*     m_pChartCtrl;
	double          m_RateSize;
	int             m_nChartIndex;
};

