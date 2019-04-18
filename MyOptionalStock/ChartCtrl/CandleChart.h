#pragma once 
#include <vector>

#define InvalidNumeric (1.175494351e-38F)

enum AXI_SCALE_STYPE
{
	AXIS_SCALE_NORMAL,		//��ͨ����
	AXIS_SCALE_PERCENT,		//�ٷֱ�����
	AXIS_SCALE_HJ,	        //�ƽ�ָ�����
	AXIS_SCALE_HJ2,	        //�ƽ�ָ�����
	AXIS_SCALE_EQPCT,		//�ȱ�����
};

enum KLINE_TYPE
{csK, csUSA, csPRICE, csGBL,csDot}; //���������ߣ���λ�ߣ������, ��״ͼ

enum SeriesType
{
	TP_VERTLINE,	//��ֱֱ��
	TP_INDICATOR,	//ָ����
	TP_BAR,			//����
	TP_STICKLINE,	//���߶�
	TP_COLORK,		//��ɫK��
	TP_PARTLINE,    //�߶�
	TP_ICON,		//ͼ��
	TP_DOT,			//��
	TP_ANY,		    //λ�ø�ʽ
	TP_CANDLE	    //K��
};

enum Period_Type
{
	DYNA_DATA,  //��ʱ 
	TRACE_DATA, //��ϸ
	SEC_DATA,   //����
	MIN_DATA,	//����
	HOUR_DATA,	//Сʱ
	DAY_DATA,	//����
	WEEK_DATA,	//����
	MONTH_DATA,	//���ߣ�
	YEAR_DATA	//����
};



struct ColorSet
{
	long clrBorder;		        // �߿�
	long clrBK;			        // ����ɫ
	long clrSelect;		        // ����ɫ
	long clrUp;			        // ����ɫ
	long clrDown;		        // �µ�ɫ
	long clrTxtDown;			// �µ�������ɫ
	long clrTxtUp;				// ��������ɫ
	long clrTxtNormal;		    // ƽ������ɫ
	bool bShowVertAxisLine;		// ˮƽ������
	bool bShowHerzAxisLine;		// ��������
	bool bUpFill;		        // ���ǿ���
	long clrDynaLine;		    // ��ʱ����ɫ
	long clrDynaLine2;	        // ��ʱͼ����
	long clrDynaLine3;	        // ��ʱͼ�ֲ���
	float fAxis_Y_Zoom;
	long clrIndicator[16];		//16ָ������ɫ
	
	CAutoRefPtr<SOUI::IFont>         ftAxis;
};




enum BuySellType
{
	atBuy,
	atBuyToCover,
	atSellShort,
	atSell
};

struct DC_ContractInfo
{
	char     ContractNo[30];			 // ��Լ����  
	char     Exchange[30];               // ������
	char     ContractName[30];		     // ��Լ��  
	double   TickPrice;		             // ��С�䶯��λ
	int      pect;				         // С����λ��
	DC_ContractInfo()
	{
		memset(this, 0, sizeof(DC_ContractInfo));
	}
	DC_ContractInfo(const DC_ContractInfo&obj)
	{
		memcpy(this, &obj, sizeof(DC_ContractInfo));
	}
	void operator=(const DC_ContractInfo&obj)
	{
		if (this == &obj)return;
		memcpy(this, &obj, sizeof(DC_ContractInfo));
	}
};
struct Period
{
	int kind;   // ���� //0����ʱ 1����ϸ 2:���ߣ�3�����ӣ�4��Сʱ��5�����ߣ�6�����ߣ�7�����ߣ�8������
	int multi;  // ���� 
};

struct DC_Data
{
	long long	time;		//ʱ��,64λUNIXʱ��
	double      value;
};
//K������Ԫ��
struct DC_KData: public DC_Data
{
	double	    Open;		//����
	double    	High;		//���
	double	    Low;		//���
	double	    Close;		//���̼ۻ����¼�
	unsigned long long	Volume;		    //�ɽ���
	unsigned long long	_Amount;		//�ɽ���
	unsigned long long  Position;		//�ֲ���
	long				TradeDate;		//��Ӧ�����գ�����20130101
};

//K�����ݼ���
struct DC_HisData
{
	struct DC_ContractInfo ci;		//��Լ��Ϣ
	char		type;               //����
	int			multi;              //����
	int	Count;			            //��ʷ������Ŀ
	std::vector<DC_KData> data;

};
//�����ź�
struct TradePoint
{
	struct DC_ContractInfo ci;//��Լ
	unsigned long tm;         //ʱ��
	BuySellType Type;         //��������//0���򿪣�1����ƽ��2��������3����ƽ
	double Price;             //�۸�����۸�����ʱ�����¼ۣ����������ֵ
	unsigned int Volume;      //��
	int bar;                  //��ǰbar�����
	bool bHide;
	char OrderFlag[201];
};

//�����ź�����
struct TradePointGroup
{
	double LastAssist;
	double StartFund;
	int nCount;
	int nSize;
	TradePoint *pData;
};

struct SeriesItem
{
	double fValue;	//InvalidNumeric ��ʾ��Ч����
	union
	{
		struct //��ɫK��,��ֱ��
		{
			unsigned long clrK;	
		};

		struct //ͼ������,������
		{
			int icon;	
		};
		
		struct //����
		{
			unsigned long clrStick;
			double fStickValue;
		};

		struct  //����
		{
			unsigned long clrBar;
			bool          bFilled;
			double        fBarValue;
		};

		struct  //�߶�
		{
			int nIdx2;	            //�߶ε�ĩ��K������
			unsigned long clrLine;	//�߶���ɫ
			double fLineValue;		//�߶�ĩ������
			int nLinWid;		    //�߶ο��
		};
		
		struct
		{
			double fHigh;
			double fLow;
			double fOpen;
		};
	};
};


struct Series
{
	char name[64];	    //ָ��������
	SeriesType nType;	//����
	unsigned long clr;	//ָ������ɫ
	bool bUserColor;
	int nThick;		    //�߶ο��
	bool bOwnAxis;	    //�Ƿ��������
	SeriesItem *pData;	//ָ��������
	int nCount;       	//���ݸ���
	int nPrec;	        //С����λ��
	int nSize;
};

struct SeriesTxtItem
{
	int    idx;
	double fPrice;
	char   txt[64];
};

//ָ�꼯��
struct SeriesGroup
{
	char params[1024];
	bool bOwnAixs;	    //��ͼ/��ͼ
	Series* m_pSeries;	//����
	int nCount;		    //ָ���߸���
	int nFirstIdx;	    //��һ��Ԫ�������K�ߵ�����

	int nTxtItemCount;
	int nTxtItemSize;
	SeriesTxtItem* pTxtItem;
};


enum ChartPenType
{
	doSelect,	//ָ��
	doSPX,		//ˮƽ��
	doCZX,		//��ֱ��
	doQSX,		//������
	doXD,		//�߶�
	doJTXD,		//��ͷ�߶�
	doPXX,		//ƽ����
	doTRIANGLE, //������
	doRECT,		//����
	doHJFG,   	//�ƽ�ָ�
	doBFB,		//�ٷֱ���
	doZQX,		//������
	doFBLQX,	//�Ѳ�������
	doSZX,		//������
	doGSX,		//������
	doHGTD,		//�ع�ͨ��
	doARC,		//��
	doCIRCLE,	//��Բ
	doICON,  	//ͼ��
	doTEXT,		//���ֹ���
	doDELETE,	//ɾ��
	doCHOOSELINE,		//������ɫ
	doCLEARALL		//��� 
};


//������ؽӿڶ���
struct KShapeDot
{
public:
	int time;
	double price;
};

struct KShapeData
{
public:
	int type;
	int nDotCount;
	long clr;
	int penWid;
	char txt[1024];
	int nIcon;
	KShapeDot Dots[10];
	void operator=(const KShapeData&data)
	{
		if (this == &data)return;
		type = data.type;
		nDotCount = data.nDotCount;
		clr = data.clr;
		penWid = data.penWid;
		strcpy_s(txt, data.txt);
		nIcon = data.nIcon;
		memcpy(Dots,data.Dots,sizeof(KShapeDot)* 10);
	}
};

enum RAObjectType
{
	OT_NONE,
	OT_K,
	OT_INDICATOR,
	OT_SHAPE,
	OT_AXIS_X,
	OT_AXIS_Y,
	OT_INDICATOR_TITLE
};
interface IKShape;
struct ObjectInfo
{
	RAObjectType type;

	int x;		//��������
	int y;		//���������

	int idx; //K�ߡ�ָ�ꡢ�������ID

	int nPos;	//������ֵ(�ڼ���K��)
	__time32_t time;  //�������Ӧ��ʱ��
	double fValue;	//������ֵ
	
	ChartPenType nPenType;
	IKShape *pShape;
};



//һЩͼ������Զ���
#define MIN_CANDLE_NUM (10)      //������ʾ����ĸ���
#define FREE_CANDLE_COUNT (5)	 //�Ҷ˿հ�K�߸���
#define MAX_CHART_NUM  (5)       //�����ض��ٸ���ͼ





#define CHART_MSG_LBT_CLICK				37103     //�����������Ϣ
#define CHART_MSG_RBT_CLICK				37104	  //����Ҽ�������Ϣ
#define CHART_MSG_GETMORE				37106     //��ȡ��������
#define CHART_MSG_DBL_CLICK	            37107	  //˫��
#define CHART_MSG_BUTTON_CLICK			37108	  //��ť�����			WPARAM  0 InfoPanel  1��Ϣ����   2��ͼ�رհ�ť
#define CHART_MSG_SUBK_START_CHANGED    37109     //����K����ʼλ�ñ䶯

enum KLineBtnPos
{
	BOTTOM_POS_TOP1 = 0,
	BOTTOM_POS_TOP2 = 1,
	BOTTOM_POS_TOP3 = 2,

	BOTTOM_POS_BOTTOM1 = 3,
	BOTTOM_POS_BOTTOM2 = 4,
	BOTTOM_POS_BOTTOM3 = 5,
};

struct RSStruct
{
	DC_ContractInfo     ci;                      //   ��Լ
	long	            StarTime;		         //   ��ʼʱ��,32λʱ��
	long	            EndTime;		         //   ����ʱ��,32λʱ��
	double	            Star;		             //   �ڳ���
	double	            High;		             //   ���
	double	            Low;		             //   ���
	double	            End;		             //   ��ĩ��
	unsigned long long	Volume;		             //   �ɽ���
	long long	        AmountDif;               //   �ֲ�
	double              WeightAve;               //   ��Ȩ����
	double              rise;                    //   �Ƿ�
	double              swing;                   //   ���
	int                 kind;                    //   ���� //0: ��ʱ 1����ϸ 2:���ߣ�3�����ӣ�4��Сʱ��5�����ߣ�6�����ߣ�7�����ߣ�8������
	int	                Precision;               //   ���۾���
	int	                CommodityDenominator;    //   ���۷�ĸ
	int                 nStar;
	int                 nEnd;
	int                 nCount;                  //   K���ܸ���
};

enum POINTSTYLE
{
	psUpArrow, psDownArrow, psPoint, psRightArrow, psUpTriArrow, psDownTriArrow,
	psTriangle, psDownTriangle, psLeftTriangle, psRightTriangle,
	psRectangle,
	psCross, psDiagCross, psCircle, psStar, psDiamond
};
SStringW FormatLongInteger(SStringW strInteger);
SStringW NumericToString(double value, int nPect, int nDenom);
CSize GetStringPix(IRenderTarget *pRender, SOUI::IFont*pFt, const SStringW& str);
void SpliteString(SArray<SStringW>& arr, SStringW str);
bool PtInLine(const CPoint& pt, const CPoint& start, const CPoint& end);
//��þ�ȷ��
int GetPect(double value);
bool ContractEqual(const DC_ContractInfo& a, const DC_ContractInfo& b);
ColorSet*               ES_Color();
void DrawLine(IRenderTarget *pRender, int style, int x1, int y1, int x2, int y2, COLORREF clr, int nWid = 1);
void DrawLine(IRenderTarget *pRender, int style,CPoint pt1, CPoint pt2, COLORREF clr, int nWid = 1);
void DrawPolyLine(IRenderTarget *pRender, int style, CPoint *pBuffer, int nCount, COLORREF clr, int nWid =1 );
void DrawKName(IRenderTarget *pRender, int x, int y, SStringW txt, COLORREF clr);
void DotLine(IRenderTarget* pRender, int x1, int y1, int x2, int y2, COLORREF clr, int nPenWid = 1, int nStyle = 0);
void DrawRect(IRenderTarget* pRender, const CRect& rc, COLORREF clr, COLORREF bdclr, int bdWid, int alpha);
void DrawArc(IRenderTarget* pRender, COLORREF clr, int x, int y, int width, int height, int startAngle, int sweepAngle, int nPenWid = 1);
void DrawPointer(IRenderTarget* pRender, int nX, int nY, COLORREF rgb, COLORREF clrBK, int nPenStype, int cx = 6, int cy = 6, BOOL bFillBk = true, int nPenThick = 1);
void DrawTipRect(IRenderTarget *pRender, const CRect& rc, COLORREF clrBK, COLORREF clrBorder);
void DrawPath(IRenderTarget *pRender, LPPOINT pts, int nCount, COLORREF clr);
void DrawFocusPoint(IRenderTarget *pRender, const CPoint& pt);