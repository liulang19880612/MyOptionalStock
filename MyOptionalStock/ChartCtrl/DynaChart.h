#pragma once
#include "KChart.h"
class CAxis;

struct AppendKItem
{
public:
	AppendKItem()
	{
		rcTitile.SetRectEmpty();
		m_bSelect = false;
	}
	DC_ContractInfo ci;
	int id;
	int nLastDataIdx;
	double fOriginPrice;
	SArray<DC_KData> arrData;
	bool m_bSelect;
	double fMin;
	double fMax;
	CAxis* pAxisVert;
	CRect rcTitile;
};

struct TimeSpan
{
	__time32_t tmStart;
	__time32_t tmEnd;
};


class CDynaChart : public CKChart
{
public:
	CDynaChart();
	virtual ~CDynaChart();
public:
	void  SetStock(DC_ContractInfo* pInfo);
	void  ClearData();
	void  SetPowerType(int nType);
	void  SetOriginPrice(double fPrice, int idK);
	void  UpdateHisData(const DC_KData* pData, int nCount, int nData, int idK);
	void  UpdateData(int nDataCount);	//����1�������ݣ�ת����ʱͼ������
	int   AppendK(DC_ContractInfo* ci);
	bool  RemoveK(DC_ContractInfo* ci);
	bool  IsBuffing(){ return m_bIsBuffing; }
	int   GetDataCount(){ return m_RealDataCount; }
public:
	virtual void CalcMinMaxValue();
	virtual void CreateAxis();
	virtual bool HitTest(ObjectInfo& info, CPoint pt);
	virtual void DrawSeries(IRenderTarget* pRender);		//����ָ��
	void DrawDynaSeries(IRenderTarget *pRender);		//���Ʒ�ʱͼ
protected:
	DC_HisData                m_hisData;
	SArray<DC_KData>          m_arrData;
	SArray<double>            m_arrAvg;              // ����
	CSeriesData<DC_KData>     m_dynaData;
	bool                      m_bHaveDestroy;
	bool                      m_bIsBuffing;
	double                    m_fMin;		         // �������
	double                    m_fMax;		         // �������
	bool                      m_bSimHistory;
	double                    m_fYClose;	         // ���գ������
	unsigned long long        m_nLastDayAmount;	     // ��ֲ�
	int                       m_pect;                // ��ȷ��-С��λ��
	double                    m_fMinMove;
	int                       m_lastDataIdx;	     // �ϴμ�������һ��ת����������ݵ��������

	unsigned long long        m_nLastAmount;	     // ���³ɽ���
	int                       m_RealDataCount;		 // �ϴμ����ʵ�����ݵĳ���
	SStringW                  m_strSymbol;
	int                       m_nDynaDays;
	SArray<AppendKItem>       m_arrAppendData;
	DC_ContractInfo           m_Contract;            //  ��Լ��Ϣ
};

