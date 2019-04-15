#pragma once
class CAxis;	    //������
class CTimeAxis;	//ʱ������
class CKChart;	    //ͼ��

#include "SeriesData.h"
#include "CandleChart.h"

class CSeriesObject : public SObject
{
public:
	CSeriesObject();
	virtual ~CSeriesObject();
public:
	virtual void Draw(IRenderTarget* pRender){};
	virtual bool GetMinMaxValue(double& fMin, double& fMax){return FALSE;}		//��������귶Χ�����ݵ����ֵ�����ֵ
	virtual double GetFirstVisibleValue(){return 0;}		                    //��õ�ǰ��Ļ��һ��Ԫ�ص�ֵ
	virtual void SetChart(CKChart *pChart);
	virtual int HitTest(CPoint pt){ return -1; }	                             //����ֵ��<0 û�б�ѡ�У�>=0������������������
	virtual void Select(bool bSelect = TRUE, int nSubIdx = -1){ m_bSelected = bSelect; }	//����
	virtual CAxis* GetSelectAxis();		                                    //���ص�ǰ�������е�������
	virtual int DrawTitle(IRenderTarget* pRender, CRect& rc){ return 0; }	//���Ʊ��⣬���ر�����
public:
	CKChart* GetChart(){return m_pChart;}	             //��ȡ���ڵ�ͼ��
	void     SetMinMaxValue(double fMin, double fMax);
	void     SetOwnAxis(bool bOwn=TRUE){m_bOwnAxis = bOwn;}  //�Ƿ�ʹ�ö�������
	bool     IsOnwnAxis(){return m_bOwnAxis;}	//�Ƿ�ʹ�ö�������
	bool     IsSelected(){return m_bSelected;}	//�Ƿ�ѡ����
	void     SetShowWavePrice(bool bShow){ m_bShowWavePrice = bShow;}
	SStringW GetParams(){return m_strParams;}
	void     SetAxisHoriz(CTimeAxis* pAxis){ m_pAxisHoriz = pAxis; }
	CTimeAxis*GetAxisHoriz(){ return m_pAxisHoriz; }
	void     SetAxisVert(CAxis*pAxis){ m_pAxisVert = pAxis; }
	CAxis*   GetAxisVert(){ return m_pAxisVert; }
protected:
	bool               m_bOwnAxis;	      // �Ƿ�Ϊ��������
	SStringW           m_strParams;	      // ����
	CPoint             m_ptLastViewBox;   // ��¼������������յ�λ��
	double             m_fMin;
	double             m_fMax;
	bool               m_bSelected;
	CKChart*           m_pChart;
	CTimeAxis*         m_pAxisHoriz;
	CAxis*             m_pAxisVert;
	bool               m_bUseSysColor;
	int		           m_nPenThick;
	SStringW           m_strName;
	bool               m_bShowWavePrice;
	int                m_nIcon;
	COLORREF           m_crPen;
};

//K��
typedef CSeriesData<DC_KData> CCandleSeriesData;

class CCandleSeries : public CSeriesObject
{
public:
	CCandleSeries();
	void SetStock(DC_ContractInfo *pInfo, Period*pPrd, time_t tmStart = 0);
	void SetCombol(DC_ContractInfo *ci1, DC_ContractInfo* ci2,Period*pPrd, double fRate, time_t tmStart)
	{
		m_bCombol = TRUE;
		m_contract2 = *ci2;
		m_fAppendRate = fRate;
		SetStock(ci1, pPrd,tmStart);
	}
	virtual ~CCandleSeries(void);
	time_t GetStartTime(){return m_tmStart;}
	virtual void	Draw(IRenderTarget* pRender);
	virtual bool GetMinMaxValue(double& fMin, double& fMax);
	virtual int HitTest(CPoint pt);
	virtual double GetFirstVisibleValue();
	
	void SetKType(KLINE_TYPE type){m_nKType=type;}	//K��ͼ��ʽ
	bool IsCombol(){return m_bCombol;}
	double GetAvgPrcie();
	void UpdateHisData(DC_KData* pData, int nCount,int nData);
	double GetWavePercent();
	void CalWaveMinMaxPrice(int nMin, int nMax); //���㲨�θߵͼ�
	double Pivot(bool bHiLo, double LeftPrice, double CurPrice, double RightPrice);//��ת��
public:

	void DrawNameLabel(IRenderTarget* pRender);
	CRect m_rcDrager;
protected:
	//������Ļ�ߵͼ�
	void DrawScreenMinMaxPrice(IRenderTarget* pRender,int maxX,int minX,COLORREF clrNormal);
	//���Ʋ��θߵͼ�
	void DrawWaveMinMaxPrice(IRenderTarget* pRender,int nCur,COLORREF clrUp,COLORREF clrDown);
	void CaluAvgPrice();		//�������	
protected:
	bool m_bCombol;	//�Ƿ�Ϊ���
	void DrawDot(IRenderTarget* pRender);
	void DrawGBL(IRenderTarget* pRender);
public:
	void SetID(int id){ m_ID = id; }
	int  GetID(){ return m_ID; }
	void EnableSubK(bool b){ m_bSubK = b; }
	void SetContractInfo(const DC_ContractInfo*pIfo){ m_contract = *pIfo; }
	DC_ContractInfo*GetContractInfo(){ return &m_contract; }
	void SetContractInfo2(const DC_ContractInfo*pIfo){ m_contract2 = *pIfo; }
	DC_ContractInfo*GetContractInfo2(){ return &m_contract2; }
	void SetAppendRate(double rate){ m_fAppendRate = rate; }
	CCandleSeriesData*GetSeriesData(){ return &m_SerirsData; }
protected:
	KLINE_TYPE             m_nKType;
	bool                   m_bIsBuffing;
	time_t                 m_tmStart;
	bool                   m_bHaveSetPect;  //��ǰ��Լ�Ƿ��Ѿ�������С����λ��
	int                    m_nLastAppendIdx;
	int                    m_nLastKCount;
	double                 m_fLastKPrice;
	double                 m_fSumPrice;
	SArray<int>            m_arrMaxMinPrice; //K������ 1���� -1���� 0 �м��
	int                    m_nWaveLastKCount;
	int                    m_nWaveLastStartIdx;
	CRect                  m_titleRc;
	int                    m_ID;
	double                 m_fAppendRate;
	bool                   m_bSubK;
	CCandleSeriesData      m_SerirsData;
	Period                 m_prd;
	DC_ContractInfo        m_contract;
	DC_ContractInfo        m_contract2;
};


//ָ����
typedef CSeriesData<SeriesItem> CIndicatorSeriesData;
class CIndicatorSeries: public CSeriesObject, public CIndicatorSeriesData
{
public:	
	CIndicatorSeries();
	virtual ~CIndicatorSeries(void){}
	virtual void Draw(IRenderTarget* pRender);
	virtual bool GetMinMaxValue(double& fMin, double& fMax);
	virtual int HitTest(CPoint pt);
	void SetSeriesData(Series *pSeries,bool bCover=FALSE);
	virtual double GetFirstVisibleValue();
	SeriesType m_nSeriesType;
	CSeriesObject* m_pGroup;
protected:
	void DrawIndicator(IRenderTarget* pRender);		//����ָ��������
	void DrawDot(IRenderTarget* pRender);		//���Ƶ�����
	void DrawStickLine(IRenderTarget* pRender);		//�������߶�����
	void DrawBar(IRenderTarget* pRender);		//������״������
	void DrawVertLine(IRenderTarget* pRender);
	void DrawIcon(IRenderTarget* pRender);		//����ͼ������
	void DrawTradePoint(IRenderTarget* pRender);	//���ƽ���ָ��
	void DrawPartLine(IRenderTarget* pRender);		//�����߶�����
	void DrawCandle(IRenderTarget* pRender);
protected:
	//HitTest
	int HitTestIndicator(CPoint pt); //ָ������ײ���
	int HitTestBar(CPoint pt); //ָ������ײ���
	int HitTestStickLine(CPoint pt); //ָ������ײ���
	int HitTestDot(CPoint pt);
	int HitTestIcon(CPoint pt);
	int HitTestPartLine(CPoint pt);	

};

//ָ��
class CIndicatorGroup : public CSeriesObject
{
public:
	CIndicatorGroup();
	virtual ~CIndicatorGroup(void);
public:
	virtual void Draw(IRenderTarget* pRender);
	virtual void SetData(const SeriesGroup* pSereisGroup);
	virtual bool GetMinMaxValue(double& fMin, double& fMax);
	virtual int HitTest(CPoint pt);
	virtual CAxis* GetSelectAxis();		//���ص�ǰ�������е�������
	virtual void SetChart(CKChart *pChart);
	virtual double GetFirstVisibleValue();
	virtual	void Select(bool bSelect=TRUE,int nSubIdx=-1);
public:
	void DrawText(IRenderTarget* pRender);
	CRect GetTitleRect(){ return m_titleRc; }
protected:
	SArray<CIndicatorSeries*>      m_arrIndicators;
	SStringW                       m_strTip;
	SStringW                       m_strId;
	int                            m_nTextItemCount;
	int                            m_nTextItemSize;
	SeriesTxtItem*                 m_pTxtItem;
	CRect                          m_titleRc;
	int                            m_nSeleSeires;
};

struct TradePointRect
{
	int id;
	CRect rc;
};

struct TradePoint_Line
{
	int nIdxOpen;
	int nIdxClose;
	int nType;		//0:buy 1:sellShort
};


class CTradePointSeries : public CSeriesObject,public TradePointGroup
{
public:
	CTradePointSeries(SStringW strSereisGroup);
	virtual ~CTradePointSeries();
public:
	virtual void	Draw(IRenderTarget* pRender);
	virtual int HitTest(CPoint pt);
	void EnableDrawLine(bool enable){ m_bDrawLine = enable; }
protected:
	void DrawTradePointLine( IRenderTarget* pRender);
	void CaluTradePointLine();	//���㽻��ָ�����
protected:
	SArray<TradePoint_Line> m_arrTradePointLine;
	SArray<TradePoint>      m_arrCoveredTradePoint;
	int                     m_nFirstNotCoverIdx;
	bool                    m_bAccTradePoint;
	bool                    m_bDrawLine;
	SStringW                m_strSeriesGroup;	//�ý����źŵķ�����SeriesGroupID
	SArray<TradePointRect>  m_arrRectTradePoint;		//���û�н���ָ���ͷ��λ��
};