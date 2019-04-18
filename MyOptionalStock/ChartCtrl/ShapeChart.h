#pragma once
#include "AxisChart.h"
#include "ShapeFactory.h"
/////////////////////////////////////
////��������ʵ���û����Զ����ͼ����
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
	virtual  void SelectPen(int type);	    //ѡ��һ������
	virtual  int  GetCursor(CPoint& pt);	//ȡ�ù��
	virtual  bool OnClick(CPoint &pt, bool bLeft = true);
	virtual  bool OnDrag(CPoint& from, CPoint &pt, bool bLeft = true);
	virtual  bool OnDragUp(CPoint&from, CPoint &pt, bool bLeft = true);
	virtual  bool HitTest(ObjectInfo& info, CPoint pt);	//��ײ���
public:
	void AfterShapePenClick();   //���ʱ������һ���Ժ�Ĵ���
	BOOL IsDrawShape();		     //�Ƿ����ڻ�ͼ
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
	ChartPenType                m_curPen;	          //�������,ö��ֵ
	CShapePen*                  m_pCurShapePen;
	SArray<CShape*>             m_arrShape;
	int                         m_penWid;
	COLORREF                    m_clrPen;
	CShape*                     m_pCurShape;         //��ǰ��ý������״
};

