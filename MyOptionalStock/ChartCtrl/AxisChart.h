#pragma once
#include "RectChart.h"
class CAxis;
class CTimeAxis;

class CAxisChart : public CRectChart
{
public:
	CAxisChart();
	virtual ~CAxisChart();
public:
	virtual void CreateAxis();    // ����������
	virtual void ReCalcLayout();  // ��������
	virtual int  GetAxisYWith();  // ��ȡ�������ߵı߾�
	virtual void SetAxisYWith(int nWidth);
	virtual void SetAutoAxix(bool bAuto){ m_bAutoAxis = bAuto; }
	virtual int  GetCursor(CPoint& pt);
	virtual bool OnDrag(CPoint& from, CPoint &pt, bool bLeft);
	virtual bool OnClick(CPoint &pt, bool bLeft = true);
	virtual bool OnDragUp(CPoint&from, CPoint &pt, bool bLeft = true);
	virtual bool OnDblClk(CPoint &pt, bool bLeft = true);
	virtual bool OnMouseMove(CPoint& pt);
public:
	void        SetTimeAxisX(CTimeAxis*pAxis){ m_pAxisX = pAxis; }
	CTimeAxis*  GetTimeAxisX(){ return m_pAxisX; }
	void        SetMainChart();                        //����Ϊ��ͼ
	bool        IsMainChart(){ return m_bMainChart; }
	CRect       GetRectObj(){ return m_rcObj; }
	void        DrawAxis(IRenderTarget* pRender);
	void        DrawZoomRect(IRenderTarget* pRender);
	void        DrawRegionRect(IRenderTarget* pRender);
	CAxis*      GetYAxis(){ return m_pAxisY; }
	CAxis*      GetSubAxis(){ return m_pAxisSub; }
	CTimeAxis*  GetXAxis(){ return m_pAxisX; }
	CRect       GetObjRect(){ return m_rcObj; };
	void        CalRegionStatisticRange(int& nStar, int& nEnd, bool bRecal = false);
protected:
	CTimeAxis*                m_pAxisX;         // ������
	CAxis*                    m_pAxisY;         // ������
	CAxis*                    m_pAxisSub;		// ������
	bool                      m_bShowSubAixs;	// �Ƿ���ʾ������
	bool                      m_bAutoAxis;		// �Ƿ��Զ�����������
	bool                      m_bCanDragAxis;	// �����Ƿ������϶�
	bool                      m_bMainChart;
	CRect                     m_rcObj;
	CAxis*                    m_pCurAxisVert;
	SArray<CAxis*>            m_arrOwnAixs;
	CPoint                    m_ptPadOffset;
	BOOL                      m_bDragingPricePad;
	int                       m_curDragingAxis;	    // ��ǰ�����϶�������
	BOOL                      m_bIsLeftDraging;		   
	CRect                     m_zoomRc;	            // �������ž���
	double                    m_fLastAxisMin, 		   
		                      m_fLastAxisMax;	    // ��¼��ʼ�϶�ʱ�̵����������Сֵ�����Χ
	int                       m_nOffsetScrollBar;	// �϶�������ʱ����������������ƫ��ֵ
	int                       m_nLastDuration;		   
	BOOL                      m_bDragingScrollBar;	// �Ƿ������϶�������
	CRect                     m_regionRc;

};

