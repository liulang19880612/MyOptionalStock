#pragma once
#include "Shape.h"

class CShapePen
{
public:
	CShapePen();
	void Create(RAPenType type, CAxis *pAixsX, CAxis *pAxisY);
	bool IsDrawing();	//�ж��Ƿ����ڻ���
	bool IsOk();		//�ж��Ƿ�������
	RAPenType GetType();
	virtual ~CShapePen(void);
	virtual void OnClick(const CPoint& pt);
	virtual void Draw(IRenderTarget*pRender){;}	
	void GetData(SArray<ChartDot>& arr);   //ȡ�û��Ƶ�ͼ�� 
	void Clear();
	virtual int GetMaxGriperCount(){return 0;}
	void SetIcon(int nIcon){m_nIcon = nIcon;}
	SStringW GetText(){return m_strText;}
	int GetIcon(){return m_nIcon;}
protected:
	SArray<ChartDot> m_arrData;
	SArray<CPoint> m_arrPixPoint;
	
	SStringW m_strText;
	int m_nIcon;
	
	bool m_bOk;
	RAPenType m_type;
	CAxis *m_pAxisX;
	CAxis *m_pAxisY;
};

class CVertHozPen:public CShapePen  //ˮƽ����ֱ��
{
public:
	virtual int GetMaxGriperCount(){return 1;}
	virtual void Draw(IRenderTarget*pRender);	
};

class CNoramlLinePen:public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CPXXPen:public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 3;}
	virtual void Draw(IRenderTarget*pRender);
};

class CChannelPen:public CShapePen
{
	virtual int GetMaxGriperCount(){return 3;}
	virtual void Draw(IRenderTarget*pRender);
};
class CTrianglePen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 3;}
	virtual void Draw(IRenderTarget*pRender);
};

class CRectPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CArcPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CCirclePen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CZQXPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CHJF_BFBGPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CFBLQPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 1;}
	virtual void Draw(IRenderTarget*pRender);
};

class CSZX_GSXPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CXXHG_Pen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 2;}
	virtual void Draw(IRenderTarget*pRender);
};

class CDotPen : public CShapePen
{
public:
	virtual int GetMaxGriperCount(){return 1;}
	void OnClick(const CPoint& pt);
	void Draw(IRenderTarget*pRender);
};