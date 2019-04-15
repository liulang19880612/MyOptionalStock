#include "StdAfx.h"
#include "ShapeFactory.h"


CShapeFactory::CShapeFactory(void)
{

}

CShapeFactory::~CShapeFactory(void)
{

}

CShape* CShapeFactory::CreateShape(RAPenType type, int nWidLen, COLORREF clr, const SArray<ChartDot>& arr, CAxis *pAxisX, CAxis* pAxisY)   //根据锚点和数据生成一个Shape
{
	CShape *pShape = NULL;
	if(type == doSPX || type == doCZX)
	{
		pShape = new CHorizVertLine;
		
	}
	else if(type == doXD || type == doQSX || type ==doJTXD)
	{
		pShape = new CNormalLine;
	}
	else if(type == doPXX)
	{
		pShape = new CPXX;
	}
	/*else if(type == doChannel)
	{
		pShape = new CChannel;
	}*/
	else if(type == doTRIANGLE)
	{
		pShape =new CTriangle;
	}
	else if(type == doRECT)
	{
		pShape = new CRectShape;
	}
	else if(type == doARC)
	{
		pShape = new CArcShape;
	}
	else if(type == doCIRCLE)
	{
		pShape = new CCircleShape;
	}
	else if(type == doZQX)
	{
		pShape = new CZQXShape;
	}
	else if(type == doHJFG || type == doBFB)
	{
		pShape = new CHJFG_BFBShape;
	}
	else if(type == doFBLQX)
	{
		pShape = new CFBLQShape;
	}
	else if(type == doSZX || type == doGSX)
	{
		pShape = new CSZX_GSXShape;
	}
	else if(type == doHGTD)
	{
		pShape = new CXXHG_Shape;
	}
	else if(type == doICON || type == doTEXT)
	{
		pShape = new CDotShape;
	}
	pShape->Create(type,  pAxisX, pAxisY, arr, clr, nWidLen);

	return pShape;
}

CShapePen* CShapeFactory::CreatePen(RAPenType type, CAxis *pAxisX, CAxis* pAxisY)
{
	CShapePen *pPen = NULL;

	if(type == doSPX || type == doCZX)
	{
		pPen =  new CVertHozPen;
	}
	else if(type == doXD || type == doQSX || type ==doJTXD)
	{
		pPen = new CNoramlLinePen;
	}
	else if(type == doPXX)
	{
		pPen = new CPXXPen;
	}
	else if(type == doTRIANGLE)
	{
		pPen = new CTrianglePen;
	}
	else if(type == doRECT)
	{
		pPen = new CRectPen();
	}
	else if(type == doARC)
	{
		pPen = new CArcPen;
	}
	else if(type == doCIRCLE)
	{
		pPen = new CCirclePen;
	}
	else if(type == doZQX)
	{
		pPen = new CZQXPen;
	}
	else if(type == doHJFG || type == doBFB)
	{
		pPen = new CHJF_BFBGPen;
	}
	else if(type == doFBLQX)
	{
		pPen = new CFBLQPen;
	}
	else if(type == doSZX || type == doGSX)
	{
		pPen = new CSZX_GSXPen;
	}
	else if(type == doHGTD)
	{
		pPen = new CXXHG_Pen;
	}
	else if(type == doICON || type == doTEXT)
	{
		pPen = new CDotPen;
	}
	if(pPen)
	{
		pPen->Create(type, pAxisX, pAxisY);
	}
	return pPen;
}

CShapeFactory *ShapeFactory()
{
	return CShapeFactory::GetInstance();
}


