#pragma once
#include "AxisChart.h"

/////////////////////////////////////
////��������ʵ���û����Զ����ͼ����
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
	virtual void SelectPen(int type);	//ѡ��һ������
public:
	void     DrawObjects(IRenderTarget* pRender, bool bRedraw);
protected:
	void AfterShapePenClick();   //���ʱ������һ���Ժ�Ĵ���
	BOOL IsDrawShape();		//�Ƿ����ڻ�ͼ
protected:
	RAPenType                   m_curPen;	          //�������,ö��ֵ
	CShapePen*                  m_pCurShapePen;
	SArray<CShape*>             m_arrShape;
	int                         m_penWid;
	COLORREF                    m_clrPen;
	CShape*                     m_pCurShape;         //��ǰ��ý������״
};

