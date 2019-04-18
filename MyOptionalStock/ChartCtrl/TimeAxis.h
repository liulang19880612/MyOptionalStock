#pragma once
#include "Axis.h"

//////////////////////////////////////////////////////////////////////////
//  ʱ��������
//////////////////////////////////////////////////////////////////////////

class CTimeAxis : public CAxis
{
public:
	CTimeAxis(bool bHorizonal, CAxisChart *pChart);
public:
	virtual void   Draw(IRenderTarget *pRender);
	virtual void   DrawAxisTip(IRenderTarget *pRender);// �����α�
	virtual double GetMinValue();
	virtual double GetMaxValue();
	virtual void   SetHisData(CSeriesData<DC_KData> *pData);
	virtual double Pix2Value(int pix);
	virtual int    Value2Pix(double fValue);
	virtual void   UpdateHisData();
	virtual int    GetFreeCandleCount(){ return FREE_CANDLE_COUNT; }
	virtual double GetDuration();		//����ֵΪK�߸���-1�������Ԫ���±�
	CSeriesData<DC_KData>* GetHisData();
	int  GetDataCount();
	int  GetCandleWidth(); //���һ������Ŀ��
	SStringW JudgeShowLabel(const CTime& curTm, const CTime& lastTm, const CTime& nxtTime, bool& bBefor);
	time_t  GetTimeDiff();
	void    SetFocus(bool bFocus = true){ m_bFocus = bFocus; }    //���ý���
	CRect   GetScrllBarRect(){ return m_rcScrollBar; }
	int     GetXValueByTime(long time);
	void    SetContract(const DC_ContractInfo* ci);          // ���ú�Լ��Ϣ
	DC_ContractInfo* GetContract(){ return &m_contract; }    // ��ȡ��Լ��Ϣ
	void SetFocusIndex(int nIdx);
public:
	bool                   m_bFocus;              // �Ƿ񽹵�
	Period                 m_Period;		      // ��������
	DC_ContractInfo        m_contract;            // ��Լ��Ϣ
	CRect                  m_rcScrollBar;         // ������
	int                    m_nFocusIndex;
	CSeriesData<DC_KData> *m_pStkHisData;
	int                    m_nScreenCandleNum;
	int                    m_nOldCount;
	int                    m_nDataCount;
};

//////////////////////////////////////////////////////////////////////////
//  ��ʱͼʱ��������
//////////////////////////////////////////////////////////////////////////

class CDynaTimeAxis : public CTimeAxis
{
public:
	virtual double GetMaxValue();
	virtual ~CDynaTimeAxis(){ ; }
	virtual void Draw(IRenderTarget *pRender);
	virtual void DrawAxisTip(IRenderTarget *pRender);
	CDynaTimeAxis(bool bHorizonal, CAxisChart *pChart);
	int GetFreeCandleCount(){ return 0; }

};