#pragma once
#include "CandleChart.h"
#include "SeriesData.h"
#include <atltime.h>

#define PRICE_BTM   (-19873)
#define PRICE_TOP   (-19872)

ColorSet*  ES_Color();
class CAxisChart;
// ������
class CAxis: public CRect
{
	friend class CChartCtrl;
public:
	CAxis(bool bHorizonal, CAxisChart* pChart);
	virtual ~CAxis();
public:
	virtual void SetMinMaxValue(double fMin, double fMax);	// �������ֵ��Сֵ
	virtual double GetMinValue(){ return m_fMin; }          // ��ȡ���ֵ
	virtual double GetMaxValue() { return m_fMax; }         // ��ȡ��Сֵ
	virtual double GetDuration(){ return 1000000.0; }	//����������Χ
public:
	void SetFitWidth(int nFitWid){ m_nFitWid = nFitWid; }
	int GetFitWidth(){ return m_nFitWid; }
	virtual int Value2Pix(double fValue);			//������ֵ�������������λ��
	virtual double Pix2Value(int pix);				//��������ֵ�������ֵ
	virtual void Draw(IRenderTarget*pRender);
	void DrawPriceAxis(IRenderTarget*pRender);	            //������ͨ����
	void DrawDengbi(IRenderTarget*pRender);		            //���Ƶȱ�����
	void DrawPecentAxis(IRenderTarget*pRender);	            //���ưٷֱ�����
	void DrawHJFG(IRenderTarget*pRender, int nType);        //���ƻƽ�ָ��
	virtual void DrawAxisTip(IRenderTarget *pRender);		//�����α�

	int GetPrect(){ return m_nPrec; }
	void SetChart(CAxisChart*pChart){ m_pChart = pChart; }
	CAxisChart* GetChart(){ return m_pChart; }
	void SetReVert(bool bRevert = false){ m_bRevert = bRevert; }	//��ת
	bool IsRevert(){ return m_bRevert; }	//�Ƿ�ת
	bool IsMouseLineVisble();		//�Ƿ���ʾ�����
	void SetAlign(int nALign){ m_nAlign = nALign % 2; }
	void SetOriginValue(double value){ m_fOrigin = value; }		//����ԭ��ֵ
	double GetOriginValue(){ return m_fOrigin; }  //���ԭ��ֵ
	double GetMinTick(){ return m_fTick; }
	void SetPrect(int n, double fTick);
	void EnableDraging(bool b){ m_bIsDraging = b; }
	CPoint GetCursorPos();
protected:
	int                    m_nFitWid;
	double                 m_fTick;
	double                 m_fOrigin;	     //ԭ��ֵ,������ٷֱ�����
	int                    m_nAlign;	     //ͣ��λ��
	bool                   m_bSub;
	double                 m_fMin;	         // ��Сֵ
	double                 m_fMax;	         // ���ֵ
	CAxisChart*            m_pChart;	     // ����ͼ��
	bool                   m_bHorizonal;     // �Ƿ��Ǻ�����
	bool                   m_bRevert;        // �Ƿ�ת
	int	                   m_nPrec;	         // С����λ��
	AXI_SCALE_STYPE        m_nAxisType;      // ��������
	bool                   m_bIsDraging;     // �Ƿ������϶�
};

class CDynaPctAxis : public CAxis
{
public:
	CDynaPctAxis(bool bHorizonal, CAxisChart* pChart);
	virtual ~CDynaPctAxis();
public:
	virtual void Draw(IRenderTarget *pRender);
};