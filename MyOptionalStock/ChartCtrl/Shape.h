#pragma once
#include "Axis.h"

//绘图对象

class ChartDot 
{
public:
	ChartDot()
	{
		x = 0;
		y = 0;
		tm = 0;
	}
	ChartDot(int dX,double dY, long tm)
	{
		x = dX;
		y=dY;
		this->tm = tm;
	}
public:
	long long tm;  //相对于数据坐标，而非像素坐标
	int x;
	double y;  //相对于数据坐标，而非像素坐标

};

class CLine
{
public:
	CLine():pt1(0,0),pt2(0,0){}
	CLine(const CPoint& ptA,const CPoint& ptB):pt1(ptA),pt2(ptB){}
	CLine(int x1,int y1,int x2, int y2):pt1(x1,y1),pt2(x2,y2){}
public:	
	CPoint pt1;
	CPoint pt2;

};

interface IKShape
{
public:
	virtual void GetData(KShapeData& data) = 0;			//返回图形数据，以便进行序列化保存
	virtual int GetCurrentPrice(double* pData, int nCount) = 0;		//返回值为0,表示计算失败; pData为空时，直接返回数据个数，pData不为空，则将数据填入pData中返回。
};

class CShapeFactory;

#define VERSION_SHAPE 2
//基类
class CShape : public IKShape
{
public:
	CShape();
	virtual ~CShape(void){;}

	virtual void GetData(KShapeData& data) ;			//返回图形数据，以便进行序列化保存
	virtual int GetCurrentPrice(double* pData, int nCount) ;

	void Create(RAPenType type, CAxis * pAxisX, CAxis *pAxisY, const SArray<ChartDot>& data,COLORREF clr, int penWid);
	void ChangeSetting(COLORREF clr, int penWid);
	void Select(BOOL bSel,int nGripper=-1);		//选中/取消选中
	RAPenType GetType(){return m_Type;}
	CPoint Griper2Pix(const ChartDot& dot);
	void SetIcon(int nIcon){m_nIcon = nIcon;}
	void SetText(const SStringW& text){m_strText = text;}
	void OnMove(const CPoint& pt);
	BOOL IsSelected(){return m_bSelected;}
	BOOL IsSizing(){return m_curMovingGripperIdx != -1;}
	void ResetX();
public:	
	virtual int HitTest(const CPoint& pt);   //碰撞检测,返回值: >=0：点击到了锚点, -1:无效, -2:线
	void BeginDraw(IRenderTarget*pRender);
	virtual void Draw(IRenderTarget*pRender){;}			//绘制
	void EndDraw(IRenderTarget*pRender);
	void SetHot(BOOL bHot){m_bFocus = bHot; }
	void SetColor(COLORREF col){m_clr = col;}
	COLORREF GetColor(){return m_clr;}
	void SetWidth(int wid){m_nPenWid = wid;}
	int GetWidth(){return m_nPenWid;}
	void SetAlarmIcon(BOOL bShow){m_bAlarmIcon = bShow;}
	void SetLineOrderMinitorId(int nId){m_nLineMonitId = nId;}
	int GetLineOrderMonitorId(){return m_nLineMonitId;}
public:
	void OnClick(const CPoint& pt);				//鼠标单击事件
	void GetGripers(SArray<ChartDot>& ogripers ); //获得锚点
	double GetXMaxValue();
	int GetXValueByTime(long time);
	void ResetAxis(CAxis * pAxisX, CAxis *pAxisY);
protected:
	double GetLineCurPrice(ChartDot dot1, ChartDot dot2);
	void DrawGripers(IRenderTarget*pRender);
	void MoveGriper(int idx, int x, int y);		//移动锚点
	int GetGriperByPoint(const CPoint& pt);	//根据像素坐标获得锚点索引，-1表示未选中锚点
	
protected:
	SArray<ChartDot> m_Gripers;
	SArray<CPoint> m_PointsOrigin;  //未移动前时的快照
	CPoint m_PointOrigin;
	SArray<CPoint> m_arrPixPoint;
	SArray<CLine> m_arrLines;
	int m_curMovingGripperIdx;
	int m_nPenWid;
	BOOL m_bSelected;
	COLORREF m_clr;
	CAxis *m_pAxisY;
	CAxis  *m_pAxisX;
protected:
	int m_nIcon;
	SStringW m_strText;
	CRect m_rcLabel;
	BOOL m_bAlarmIcon;//是否显示预警图标
private:
	RAPenType m_Type;
	BOOL m_bIsMoving;
	BOOL m_bFocus;
	COLORREF m_clrNormal;
	int m_nLineMonitId; //与之关联的画线下单监听器ID
}; 

class CHorizVertLine: public CShape   //水平竖直线
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CNormalLine: public CShape     //直线，射线，线段，箭头线段
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class  CPXX : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CChannel:public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);
};
class CTriangle: public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CRectShape: public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CArcShape: public CShape
{
public:
	virtual int HitTest(const CPoint&  pt);   //碰撞检测
	virtual void Draw(IRenderTarget*pRender);			//绘制
private:
	BOOL m_bUp;
	CRect m_circleRect;
};

class CCircleShape:public CShape
{
public:
	virtual int HitTest(const CPoint&  pt);   //碰撞检测
	virtual void Draw(IRenderTarget*pRender);			//绘制
private:
	BOOL m_bUp;
	SArray<CRect> m_circleRects;
};

class CZQXShape:public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CHJFG_BFBShape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CFBLQShape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CSZX_GSXShape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CXXHG_Shape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//绘制
};

class CDotShape : public CShape
{
public:
	virtual int HitTest(const CPoint&  pt);   //碰撞检测
	virtual void Draw(IRenderTarget*pRender);			//绘制
};