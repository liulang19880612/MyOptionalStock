#pragma once
#include "ShapeChart.h"
#include "SeriesObject.h"

struct ButtonInfo
{
	int nImage;
	int type;
	SStringW title;
	SStringW detail;
	CRect rc;
	int barIdx;
	int place;
	LPARAM lParam;
};
class CKChart : public CShapeChart
{
public:
	CKChart();
	virtual ~CKChart();
public:
	virtual bool OnClick(CPoint &pt, bool bLeft = true);
	virtual bool OnMouseMove(CPoint& pt);
	virtual void CaluAixs();	        //����������ø����е�����
	virtual void CalcMinMaxValue();		//���������������Сֵ����Ҫ��������ʱ���øú���
	virtual void DrawObjects(IRenderTarget* pRender, bool bRedraw = true);
	virtual void DrawSeries(IRenderTarget* pRender);		//����ͼ��
	virtual bool HitTest(ObjectInfo& info, CPoint pt);
	virtual void KillFocus(); // ʧȥ����
	virtual bool OnDrag(CPoint& from, CPoint &pt, bool bLeft);
public:
	void DrawPricePad(IRenderTarget* pRender);	      // ���Ƽ۸����
	void DrawInfoPanelButton(IRenderTarget* pRender); // ���ƴ���Ϣ��尴ť
	void DrawNewPrice(IRenderTarget* pRender);        // �������¼�
	void DrawTipButtons(IRenderTarget* pRender);
	void AddK(int id, CSeriesObject* pSeries);
	CSeriesData<SeriesItem> *GetColorSeries(){ return m_pClrSeires; };			//��ɫK������
	void SetColorSeries(CSeriesData<SeriesItem> *pSeries);
	void RemoveColorSeries(CSeriesData<SeriesItem> *pSeries);
	void AddTradePoint(TradePoint data);
protected:
	int                        m_idDragingSub;
	CCandleSeries *            m_pDragingSub;
	CSeriesData<SeriesItem> *  m_pClrSeires;			// ��ɫK������
	SMap<int, CSeriesObject*>  m_mapCandleSeries;
	SMap<int, CSeriesObject*>  m_mapIndiCatorSeries;
	SMap<int, CSeriesObject*>  m_mapTradePoint;
	SMap<int, CSeriesObject*>  m_mapAccTradePoint;
	BOOL                       m_bHaveSelObject;
	int                        m_nIcon;
	CSeriesObject*             m_pSelSeries;		    // ��ǰѡ���K�߻�ָ��
	CSeriesObject*             m_pMainK;	            // ��K��
	CSeriesObject*             m_pMainIndicator;        // ��ָ��
	BOOL                       m_bPadAliginLeft;
	CTradePointSeries          m_userTradePoint;
	SArray<TradePoint>         m_arrTradePoint;
	SArray<ButtonInfo>         m_arrButtons;
};

