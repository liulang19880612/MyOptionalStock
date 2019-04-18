#pragma once
#include "AxisChart.h"

/////////////////////////////////////
////本类用于实现用户的自定义绘图操作
/////////////////////////////////////

class CShapePen;
class CShape;
enum  RAPenType;

class CShapeChart : public CAxisChart
{
public:
	CShapeChart();
	virtual ~CShapeChart();
public:
	virtual  bool OnMouseDown(CPoint& pt);
	virtual void SelectPen(int type);	//选择一个画笔
public:
	void     DrawObjects(IRenderTarget* pRender, bool bRedraw);
protected:
	void AfterShapePenClick();   //画笔被点击了一下以后的处理
	BOOL IsDrawShape();		//是否正在画图
protected:
	RAPenType                   m_curPen;	          //鼠标类型,枚举值
	CShapePen*                  m_pCurShapePen;
	SArray<CShape*>             m_arrShape;
	int                         m_penWid;
	COLORREF                    m_clrPen;
	CShape*                     m_pCurShape;         //当前获得焦点的形状
};

