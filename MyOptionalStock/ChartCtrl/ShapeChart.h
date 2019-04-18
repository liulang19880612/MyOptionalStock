#pragma once
#include "AxisChart.h"
#include "ShapeFactory.h"
/////////////////////////////////////
////本类用于实现用户的自定义绘图操作
/////////////////////////////////////

class CShapePen;
class CShape;
enum  ChartPenType;

class CShapeChart : public CAxisChart
{
public:
	CShapeChart();
	virtual ~CShapeChart();
public:
	virtual void  DrawObjects(IRenderTarget* pRender, bool bRedraw);
	virtual void  DrawShape(IRenderTarget* pRender);
	virtual  bool OnMouseDown(CPoint& pt);
	virtual  bool OnMouseMove(CPoint& pt);
	virtual  void SelectPen(int type);	    //选择一个画笔
	virtual  int  GetCursor(CPoint& pt);	//取得光标
	virtual  bool OnClick(CPoint &pt, bool bLeft = true);
	virtual  bool OnDrag(CPoint& from, CPoint &pt, bool bLeft = true);
	virtual  bool OnDragUp(CPoint&from, CPoint &pt, bool bLeft = true);
	virtual  bool HitTest(ObjectInfo& info, CPoint pt);	//碰撞检测
public:
	void AfterShapePenClick();   //画笔被点击了一下以后的处理
	BOOL IsDrawShape();		     //是否正在画图
	void SetPenStyle(COLORREF clr, int nPenWidth){ m_penWid = nPenWidth; m_clrPen = clr; }
	ChartPenType GetCurPen(){ return m_curPen; }
	void DeleteShape(int idx);
	void SetShapAlermIcon(int idx, BOOL bshow);
	CShape *GetShap(int idx);
	void DeleteAlleShape();
	void GetShapeArray(SArray<CShape*>& arr);
	void SetArrShape(SArray<CShape*>& arrShape);
	IKShape * AddShape(const KShapeData& data);
	int GetAllShape(IKShapePtr* pShapes, int count);
	void RemoveShape(IKShape* pShape);
	void SetCurShape(CShape *pShape);
protected:
	ChartPenType                m_curPen;	          //鼠标类型,枚举值
	CShapePen*                  m_pCurShapePen;
	SArray<CShape*>             m_arrShape;
	int                         m_penWid;
	COLORREF                    m_clrPen;
	CShape*                     m_pCurShape;         //当前获得焦点的形状
};

