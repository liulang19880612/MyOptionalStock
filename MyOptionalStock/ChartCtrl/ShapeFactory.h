#pragma once
#include "Shape.h"
#include "ShapePen.h"

class CShapeFactory
{
public:
	~CShapeFactory(void);
	CShape* CreateShape(ChartPenType type, int nWidLen, COLORREF clr, const SArray<ChartDot>& arr, CAxis *pAxisX, CAxis* pAxisY);   //����ê�����������һ��Shape
	CShapePen* CreatePen(ChartPenType type, CAxis *pAxisX, CAxis* pAxisY);

	static CShapeFactory* GetInstance()
	{
		static CShapeFactory instance;
		return &instance;
	}
private:
	CShapeFactory(void);
};

CShapeFactory *ShapeFactory();
