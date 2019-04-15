#pragma once
#include "Axis.h"

//��ͼ����

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
	long long tm;  //������������꣬������������
	int x;
	double y;  //������������꣬������������

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
	virtual void GetData(KShapeData& data) = 0;			//����ͼ�����ݣ��Ա�������л�����
	virtual int GetCurrentPrice(double* pData, int nCount) = 0;		//����ֵΪ0,��ʾ����ʧ��; pDataΪ��ʱ��ֱ�ӷ������ݸ�����pData��Ϊ�գ�����������pData�з��ء�
};

class CShapeFactory;

#define VERSION_SHAPE 2
//����
class CShape : public IKShape
{
public:
	CShape();
	virtual ~CShape(void){;}

	virtual void GetData(KShapeData& data) ;			//����ͼ�����ݣ��Ա�������л�����
	virtual int GetCurrentPrice(double* pData, int nCount) ;

	void Create(RAPenType type, CAxis * pAxisX, CAxis *pAxisY, const SArray<ChartDot>& data,COLORREF clr, int penWid);
	void ChangeSetting(COLORREF clr, int penWid);
	void Select(BOOL bSel,int nGripper=-1);		//ѡ��/ȡ��ѡ��
	RAPenType GetType(){return m_Type;}
	CPoint Griper2Pix(const ChartDot& dot);
	void SetIcon(int nIcon){m_nIcon = nIcon;}
	void SetText(const SStringW& text){m_strText = text;}
	void OnMove(const CPoint& pt);
	BOOL IsSelected(){return m_bSelected;}
	BOOL IsSizing(){return m_curMovingGripperIdx != -1;}
	void ResetX();
public:	
	virtual int HitTest(const CPoint& pt);   //��ײ���,����ֵ: >=0���������ê��, -1:��Ч, -2:��
	void BeginDraw(IRenderTarget*pRender);
	virtual void Draw(IRenderTarget*pRender){;}			//����
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
	void OnClick(const CPoint& pt);				//��굥���¼�
	void GetGripers(SArray<ChartDot>& ogripers ); //���ê��
	double GetXMaxValue();
	int GetXValueByTime(long time);
	void ResetAxis(CAxis * pAxisX, CAxis *pAxisY);
protected:
	double GetLineCurPrice(ChartDot dot1, ChartDot dot2);
	void DrawGripers(IRenderTarget*pRender);
	void MoveGriper(int idx, int x, int y);		//�ƶ�ê��
	int GetGriperByPoint(const CPoint& pt);	//��������������ê��������-1��ʾδѡ��ê��
	
protected:
	SArray<ChartDot> m_Gripers;
	SArray<CPoint> m_PointsOrigin;  //δ�ƶ�ǰʱ�Ŀ���
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
	BOOL m_bAlarmIcon;//�Ƿ���ʾԤ��ͼ��
private:
	RAPenType m_Type;
	BOOL m_bIsMoving;
	BOOL m_bFocus;
	COLORREF m_clrNormal;
	int m_nLineMonitId; //��֮�����Ļ����µ�������ID
}; 

class CHorizVertLine: public CShape   //ˮƽ��ֱ��
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CNormalLine: public CShape     //ֱ�ߣ����ߣ��߶Σ���ͷ�߶�
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class  CPXX : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CChannel:public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);
};
class CTriangle: public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CRectShape: public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CArcShape: public CShape
{
public:
	virtual int HitTest(const CPoint&  pt);   //��ײ���
	virtual void Draw(IRenderTarget*pRender);			//����
private:
	BOOL m_bUp;
	CRect m_circleRect;
};

class CCircleShape:public CShape
{
public:
	virtual int HitTest(const CPoint&  pt);   //��ײ���
	virtual void Draw(IRenderTarget*pRender);			//����
private:
	BOOL m_bUp;
	SArray<CRect> m_circleRects;
};

class CZQXShape:public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CHJFG_BFBShape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CFBLQShape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CSZX_GSXShape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CXXHG_Shape : public CShape
{
public:
	virtual void Draw(IRenderTarget*pRender);			//����
};

class CDotShape : public CShape
{
public:
	virtual int HitTest(const CPoint&  pt);   //��ײ���
	virtual void Draw(IRenderTarget*pRender);			//����
};