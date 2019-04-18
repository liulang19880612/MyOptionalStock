#pragma once 
#include <vector>

#define InvalidNumeric (1.175494351e-38F)

enum AXI_SCALE_STYPE
{
	AXIS_SCALE_NORMAL,		//普通坐标
	AXIS_SCALE_PERCENT,		//百分比坐标
	AXIS_SCALE_HJ,	        //黄金分割坐标
	AXIS_SCALE_HJ2,	        //黄金分割坐标
	AXIS_SCALE_EQPCT,		//等比坐标
};

enum KLINE_TYPE
{csK, csUSA, csPRICE, csGBL,csDot}; //蜡烛，美国线，价位线，鬼变脸, 点状图

enum SeriesType
{
	TP_VERTLINE,	//竖直直线
	TP_INDICATOR,	//指标线
	TP_BAR,			//柱子
	TP_STICKLINE,	//竖线段
	TP_COLORK,		//变色K线
	TP_PARTLINE,    //线段
	TP_ICON,		//图标
	TP_DOT,			//点
	TP_ANY,		    //位置格式
	TP_CANDLE	    //K线
};

enum Period_Type
{
	DYNA_DATA,  //分时 
	TRACE_DATA, //明细
	SEC_DATA,   //秒线
	MIN_DATA,	//分钟
	HOUR_DATA,	//小时
	DAY_DATA,	//日线
	WEEK_DATA,	//周线
	MONTH_DATA,	//月线，
	YEAR_DATA	//年线
};



struct ColorSet
{
	long clrBorder;		        // 边框
	long clrBK;			        // 背景色
	long clrSelect;		        // 高亮色
	long clrUp;			        // 上涨色
	long clrDown;		        // 下跌色
	long clrTxtDown;			// 下跌文字颜色
	long clrTxtUp;				// 上涨文字色
	long clrTxtNormal;		    // 平盘文字色
	bool bShowVertAxisLine;		// 水平坐标线
	bool bShowHerzAxisLine;		// 横坐标线
	bool bUpFill;		        // 上涨空心
	long clrDynaLine;		    // 分时线颜色
	long clrDynaLine2;	        // 分时图均线
	long clrDynaLine3;	        // 分时图持仓线
	float fAxis_Y_Zoom;
	long clrIndicator[16];		//16指标线颜色
	
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
	char     ContractNo[30];			 // 合约代码  
	char     Exchange[30];               // 交易所
	char     ContractName[30];		     // 合约名  
	double   TickPrice;		             // 最小变动价位
	int      pect;				         // 小数点位数
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
	int kind;   // 类型 //0：分时 1：明细 2:秒线，3：分钟，4：小时，5：日线，6：周线，7：月线，8：年线
	int multi;  // 倍数 
};

struct DC_Data
{
	long long	time;		//时间,64位UNIX时间
	double      value;
};
//K线数据元素
struct DC_KData: public DC_Data
{
	double	    Open;		//开盘
	double    	High;		//最高
	double	    Low;		//最低
	double	    Close;		//收盘价或最新价
	unsigned long long	Volume;		    //成交量
	unsigned long long	_Amount;		//成交额
	unsigned long long  Position;		//持仓量
	long				TradeDate;		//对应交易日，形如20130101
};

//K线数据集合
struct DC_HisData
{
	struct DC_ContractInfo ci;		//合约信息
	char		type;               //周期
	int			multi;              //倍数
	int	Count;			            //历史数据数目
	std::vector<DC_KData> data;

};
//交易信号
struct TradePoint
{
	struct DC_ContractInfo ci;//合约
	unsigned long tm;         //时间
	BuySellType Type;         //定单操作//0：买开，1：买平，2：卖开，3：卖平
	double Price;             //价格，这个价格当做当时的最新价，来计算相关值
	unsigned int Volume;      //量
	int bar;                  //当前bar的序号
	bool bHide;
	char OrderFlag[201];
};

//交易信号序列
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
	double fValue;	//InvalidNumeric 表示无效数据
	union
	{
		struct //变色K线,竖直线
		{
			unsigned long clrK;	
		};

		struct //图标类型,点类型
		{
			int icon;	
		};
		
		struct //竖线
		{
			unsigned long clrStick;
			double fStickValue;
		};

		struct  //柱子
		{
			unsigned long clrBar;
			bool          bFilled;
			double        fBarValue;
		};

		struct  //线段
		{
			int nIdx2;	            //线段的末端K线坐标
			unsigned long clrLine;	//线段颜色
			double fLineValue;		//线段末端数据
			int nLinWid;		    //线段宽度
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
	char name[64];	    //指标线名称
	SeriesType nType;	//类型
	unsigned long clr;	//指标线颜色
	bool bUserColor;
	int nThick;		    //线段宽度
	bool bOwnAxis;	    //是否独立坐标
	SeriesItem *pData;	//指标线数据
	int nCount;       	//数据个数
	int nPrec;	        //小数点位数
	int nSize;
};

struct SeriesTxtItem
{
	int    idx;
	double fPrice;
	char   txt[64];
};

//指标集合
struct SeriesGroup
{
	char params[1024];
	bool bOwnAixs;	    //主图/副图
	Series* m_pSeries;	//数据
	int nCount;		    //指标线个数
	int nFirstIdx;	    //第一个元素相对于K线的索引

	int nTxtItemCount;
	int nTxtItemSize;
	SeriesTxtItem* pTxtItem;
};


enum ChartPenType
{
	doSelect,	//指针
	doSPX,		//水平线
	doCZX,		//垂直线
	doQSX,		//趋势线
	doXD,		//线段
	doJTXD,		//箭头线段
	doPXX,		//平行线
	doTRIANGLE, //三角形
	doRECT,		//矩形
	doHJFG,   	//黄金分割
	doBFB,		//百分比线
	doZQX,		//周期线
	doFBLQX,	//费波拉契线
	doSZX,		//速阻线
	doGSX,		//甘氏线
	doHGTD,		//回归通道
	doARC,		//弧
	doCIRCLE,	//半圆
	doICON,  	//图标
	doTEXT,		//文字工具
	doDELETE,	//删除
	doCHOOSELINE,		//线型颜色
	doCLEARALL		//清空 
};


//画线相关接口定义
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

	int x;		//鼠标横坐标
	int y;		//鼠标纵坐标

	int idx; //K线、指标、坐标轴的ID

	int nPos;	//横坐标值(第几根K线)
	__time32_t time;  //横坐标对应的时间
	double fValue;	//纵坐标值
	
	ChartPenType nPenType;
	IKShape *pShape;
};



//一些图表的属性定义
#define MIN_CANDLE_NUM (10)      //最少显示蜡烛的根数
#define FREE_CANDLE_COUNT (5)	 //右端空白K线根数
#define MAX_CHART_NUM  (5)       //最多加载多少个子图





#define CHART_MSG_LBT_CLICK				37103     //鼠标左键点击消息
#define CHART_MSG_RBT_CLICK				37104	  //鼠标右键单击消息
#define CHART_MSG_GETMORE				37106     //获取更多数据
#define CHART_MSG_DBL_CLICK	            37107	  //双击
#define CHART_MSG_BUTTON_CLICK			37108	  //按钮被点击			WPARAM  0 InfoPanel  1信息地雷   2副图关闭按钮
#define CHART_MSG_SUBK_START_CHANGED    37109     //叠加K先起始位置变动

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
	DC_ContractInfo     ci;                      //   合约
	long	            StarTime;		         //   起始时间,32位时间
	long	            EndTime;		         //   结束时间,32位时间
	double	            Star;		             //   期初价
	double	            High;		             //   最高
	double	            Low;		             //   最低
	double	            End;		             //   期末价
	unsigned long long	Volume;		             //   成交量
	long long	        AmountDif;               //   仓差
	double              WeightAve;               //   加权均价
	double              rise;                    //   涨幅
	double              swing;                   //   振幅
	int                 kind;                    //   类型 //0: 分时 1：明细 2:秒线，3：分钟，4：小时，5：日线，6：周线，7：月线，8：年线
	int	                Precision;               //   报价精度
	int	                CommodityDenominator;    //   报价分母
	int                 nStar;
	int                 nEnd;
	int                 nCount;                  //   K线总根数
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
//获得精确度
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