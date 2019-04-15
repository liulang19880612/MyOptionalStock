#include "StdAfx.h"
#include "Shape.h"
#include "math.h"
#include "AxisChart.h"
#include "CandleChart.h"
#include "TimeAxis.h"

//-----------------------------通用函数-----------------------------------------
//求点到直线的距离
int PointToLine(const CPoint& pt, const CPoint& linePt1,const CPoint& linePt2)
{
	double x1=(double)linePt1.x, y1= (double)linePt1.y;
	double x2=(double)linePt2.x, y2= (double)linePt2.y;
	
	double l=(y2-y1)/(x2-x1);
	
	double x = (double)pt.x ,y = (double)pt.y;
	
	return (int)(fabs(l*x-y+y1-l*x1)/sqrt(l*l+1));
	
}

//功能：已知一点，一条直线求垂足  
//参数：  
//CPoint pt1，直线起始点  
//CPoint pt2，直线终止点  
//CPoint pt3,直线外一点  
//输出垂足  
CPoint Getplumb(const CPoint& pt1,const CPoint& pt2,const CPoint& ptOut)  
{  
	double dba,dbb;
	CPoint ptPlum;

	//a = sqr( (x2 - x1)^2 +(y2 - y1)^2 )  
	dba = sqrt((long double)((pt2.x - pt1.x) * (pt2.x - pt1.x) + (pt2.y - pt1.y) * (pt2.y - pt1.y) ));  
	//b = (x2-x1) * (x3-x1) +(y2 -y1) * (y3 -y1)  
	dbb = ((pt2.x - pt1.x) * (ptOut.x -pt1.x) + (pt2.y - pt1.y) * (ptOut.y - pt1.y) );  
	//a = b / (a*a)  
	dba = dbb / (dba * dba);  
	//x4 = x1 +(x2 - x1)*a  
	ptPlum.x =(LONG)( pt1.x + (pt2.x - pt1.x) * dba);  
	//y4 = y1 +(y2 - y1)*a  
	ptPlum.y =(LONG)(pt1.y + (pt2.y - pt1.y) * dba);  
	
	return ptPlum;  
}  

//绘制穿越两个点的直线。返回直线的两个边界点
CLine DrawZhiXian(IRenderTarget*pRender,const CPoint& pt1,const CPoint& pt2, COLORREF clr, int nWid, int left,int right)
{
	CPoint out1,out2;
	if(pt1.x == pt2.x)
	{
		out1.y = -10000;
		out2.y = 10000;
		out1.x = pt1.x;
		out2.x = pt1.x;
	}
	else if(pt1.y == pt2.y)
	{
		out1.y = out2.y = pt1.y;
		out1.x = left;
		out2.x = right;
	}
	else
	{
		out1.x = left;
		out2.x = right;
		out1.y = pt1.y + (left - pt1.x)*(pt2.y-pt1.y)/(pt2.x-pt1.x);
		out2.y = pt1.y + (right - pt1.x)*(pt2.y-pt1.y)/(pt2.x-pt1.x);
	}

	DrawLine(pRender,PS_SOLID,out1,out2,clr,nWid);
	return CLine(out1,out2);
}

//绘制射线，返回射线的边界点
CLine DrawSheXian(IRenderTarget*pRender,const CPoint& ptOrigin, const CPoint& pt2 ,COLORREF clr, int nPenWid)
{
	double yy = pt2.y - ptOrigin.y;
	double xx = pt2.x - ptOrigin.x;

	if(xx == 0 && yy==0)
	{
		return CLine(ptOrigin,ptOrigin);
	}
	CPoint ptDes;

	if(xx ==0)
	{
		int desY = (yy<0 ? 0 : 10000);
		ptDes.SetPoint(ptOrigin.x,desY);
		//竖直线
		DrawLine(pRender,PS_SOLID,ptOrigin,ptDes,clr,nPenWid);
	}
	else
	{
		int desX = (xx>0 ? 10000 : 0);
		int desY = static_cast<int>(ptOrigin.y + (desX - ptOrigin.x)*(yy / xx));
		ptDes.SetPoint(desX,desY);

		DrawLine(pRender, PS_SOLID, ptOrigin, ptDes, clr, nPenWid);
	}

	return CLine(ptOrigin,ptDes);
}


// 求线性回归方程：Y = a + bx
// dada[rows*2]数组：X, Y；rows：数据行数；a, b：返回回归系数
// SquarePoor[4]：返回方差分析指标: 回归平方和，剩余平方和，回归平方差，剩余平方差
// 返回值：0求解成功，-1错误
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor)
{
	int m;
	double *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
	if (data == 0 || a == 0 || b == 0 || rows < 1)
		return -1;
	for (p = data, m = 0; m < rows; m ++)
	{
		xa += *p ++;
		ya += *p ++;
	}
	xa /= rows;                                     // X平均值
	ya /= rows;                                     // Y平均值
	for (p = data, m = 0; m < rows; m ++, p += 2)
	{
		Lxx += ((*p - xa) * (*p - xa));             // Lxx = Sum((X - Xa)平方)
		Lxy += ((*p - xa) * (*(p + 1) - ya));       // Lxy = Sum((X - Xa)(Y - Ya))
	}
	*b = Lxy / Lxx;                                 // b = Lxy / Lxx
	*a = ya - *b * xa;                              // a = Ya - b*Xa
	if (SquarePoor == 0)
		return 0;
	// 方差分析
	SquarePoor[0] = SquarePoor[1] = 0.0;
	for (p = data, m = 0; m < rows; m ++, p ++)
	{
		Lxy = *a + *b * *p ++;
		SquarePoor[0] += ((Lxy - ya) * (Lxy - ya)); // U(回归平方和)
		SquarePoor[1] += ((*p - Lxy) * (*p - Lxy)); // Q(剩余平方和)
	}
	SquarePoor[2] = SquarePoor[0];                  // 回归方差
	SquarePoor[3] = SquarePoor[1] / (rows - 2);     // 剩余方差
	return 0;
}



BOOL PtInEllips(const CPoint& pt, const CRect& rc, BOOL bTop)
{
	CPoint ptCenter = rc.CenterPoint();
	if(abs(pt.x-ptCenter.x)<5)
	{
		return bTop ? (abs(pt.y-rc.top)<5) : (abs(pt.y-rc.bottom)<5);
	}

	if(abs(pt.y-ptCenter.y)<5)
	{
		return pt.x<ptCenter.x ? (abs(pt.x-rc.left)<5) : (abs(pt.x-rc.right)<5);
	}

	if(rc.Width() == rc.Height()) //圆形
	{
		int r2 = (pt.x-ptCenter.x) * (pt.x-ptCenter.x) + (pt.y-ptCenter.y)*(pt.y-ptCenter.y);
		int r = static_cast<int>(pow(r2, 0.5));
		if(bTop)
		{
			return pt.y<ptCenter.y && abs(r-rc.Width()/2)<5;
		}
		else
		{
			return pt.y>ptCenter.y && abs(r-rc.Width()/2)<5;
		}
	}
	
	double A = ptCenter.x;
	double B = ptCenter.y;
	double x = pt.x;
	double sss = 0;	
	if(rc.Width()>rc.Height())
	{
		double a=rc.Width()/2;
		double b=rc.Height()/2;
		sss = pow( (b*b-(b*b)*(x-A)*(x-A)/(a*a)),0.5);
	}
	else
	{
		double a = rc.Height()/2;
		double b = rc.Width()/2;
		sss = pow((a*a-(a*a)*(x-A)*(x-A)/(b*b)),0.5);
	}

	if(bTop)
	{
		return abs(pt.y - (B + sss))<5;
	}
	else
	{
		return abs(pt.y-(B-sss))<5;
	}
}
//------------------------------------------------CShape-------------------------------------------
CShape::CShape():
m_nPenWid(1),
m_bSelected(FALSE),
m_clr(0),
m_pAxisX(NULL),
m_pAxisY(NULL),
m_Type(doSelect),
m_nIcon(0),
m_bFocus(FALSE),
m_curMovingGripperIdx(-1),
m_bAlarmIcon(FALSE),
m_nLineMonitId(-1)
{

}

void CShape::Create(RAPenType type, CAxis * pAxisX, CAxis *pAxisY, const SArray<ChartDot>& data,COLORREF clr, int penWid)
{
	m_Type = type;
	m_pAxisX = pAxisX;
	m_pAxisY = pAxisY;
	m_Gripers.RemoveAll();
	m_Gripers.Copy(data);
	m_clr = clr;
	m_nPenWid = penWid;

	if (pAxisX == NULL) return;
	CTimeAxis *pTmAxis = dynamic_cast<CTimeAxis*>(pAxisX);
	if (pTmAxis == NULL) return;

	for (size_t i = 0; i < m_Gripers.GetCount(); i++)
	{
		if (m_Gripers[i].tm > 0)
		{
			
			m_Gripers[i].x = pTmAxis->GetXValueByTime(m_Gripers[i].tm);
		}
		else if (m_Gripers[i].x >= 0)
		{
			CTimeAxis *pTmAxis = (CTimeAxis*)pAxisX;
			if (m_Gripers[i].x < pTmAxis->GetDataCount())
			{
				m_Gripers[i].tm = pTmAxis->GetHisData()->GetAt(m_Gripers[i].x).time;
			}
		}
	}

}

void CShape::GetData(KShapeData& data)		//返回图形数据，以便进行序列化保存
{
	data.clr = m_clr;
	data.penWid = m_nPenWid;
	data.type = m_Type;
	data.nDotCount = m_Gripers.GetCount();
	memset(data.txt,0,sizeof(data.txt));
	strcpy_s(data.txt, S_CW2A(m_strText));
	data.nIcon = m_nIcon;
	for(int i=0;i<data.nDotCount;i++)
	{
		data.Dots[i].time = m_Gripers[i].tm;
		data.Dots[i].price = m_Gripers[i].y;
	}
	
}

double CShape::GetLineCurPrice(ChartDot dot1, ChartDot dot2)
{
	if(m_pAxisX == NULL) return 0;
	CTimeAxis *pTmAxis = dynamic_cast<CTimeAxis*>(m_pAxisX);
	if(pTmAxis == NULL) return 0;

	int x1 = pTmAxis->GetXValueByTime(dot1.tm);
	int x2 = pTmAxis->GetXValueByTime(dot2.tm);

	if(x1<0 || x2<0) return 0;

	if(x1 == x2)
	{
		return 0;
	}
	else if(dot1.y == dot2.y)
	{
		return dot1.y;
	}
	else
	{
		if(x1>x2)
		{
			ChartDot tmp = dot1;
			dot1 = dot2;
			dot2 = tmp;
			int tmpi = x1;
			x1 = x2;
			x2 = tmpi;
		}
		double xx = dot2.y - dot1.y;
		CTimeAxis *pTmAxis = dynamic_cast<CTimeAxis*>(m_pAxisX);
		if (pTmAxis == NULL) return 0;
		if(pTmAxis->GetDataCount()<1) return 0;

		int x3= pTmAxis->GetDataCount()-1;
	
		return dot1.y + xx*(x3-x1)/(x2-x1);
	}
}

int CShape::GetCurrentPrice(double* pData, int nCount) 
{
	if(GetType() == doHJFG || GetType() == doBFB)
	{
		double fFrom = m_Gripers[0].y;

		double fBase = m_Gripers[1].y - fFrom;

		SArray<double> arrPixY;
		
		if(m_Type == doHJFG)
		{
			arrPixY.Add(0);
			arrPixY.Add(0.382);
			arrPixY.Add(0.618);
			arrPixY.Add(1.382);
			arrPixY.Add(1.618);
		}
		else
		{
			arrPixY.Add(0);
			arrPixY.Add(0.25);
			arrPixY.Add(0.5);
			arrPixY.Add(0.75);
			arrPixY.Add(1);
		}

		if(pData && nCount>0)
		{
			int cc = min(nCount,static_cast<int>(arrPixY.GetCount()));
			for(int i=0;i<cc;i++)
			{
				pData[i] = fFrom + arrPixY[i]*fBase;
			}
		}
		return arrPixY.GetCount();
	}
	else if(m_Type == doQSX)
	{
		double ret = GetLineCurPrice(m_Gripers[0],m_Gripers[1]);
		
		if(ret == 0) return 0;

		if(pData != NULL && nCount>0)
		{
			pData[0] =ret;
		}
		return 1;
		
	}
	else if(m_Type == doSZX)
	{
		double fAddPrice = (m_Gripers[1].y - m_Gripers[0].y)/3;
		
		ChartDot tmp = m_Gripers[1];
		tmp.y = m_Gripers[0].y;
		tmp.y += fAddPrice;
		double ret1 = GetLineCurPrice(m_Gripers[0],tmp);
		tmp.y +=fAddPrice;
		double ret2 = GetLineCurPrice(m_Gripers[0],tmp);
		tmp.y += fAddPrice;
		double ret3 = GetLineCurPrice(m_Gripers[0],tmp);

		if(pData != NULL && nCount>2)
		{
			pData[0] = ret1;
			pData[1] = ret2;
			pData[2] = ret3;
		}
		return 3;

	}
	else if(m_Type == doSPX)
	{
		int nLine = 1;

		if(nCount>=nLine && pData)
		{
			pData[0] = m_Gripers[0].y;
		}
		return nLine;
	}
	else if(m_Type == doRECT)
	{
		int nLine = 2;

		if(nCount>=nLine && pData)
		{
			pData[0] = m_Gripers[0].y;
			pData[1] = m_Gripers[1].y;
		}
		return nLine;
	}
	else 
	{
		return 0;
	}

	return 0;
}


void CShape::ChangeSetting(COLORREF clr, int penWid)
{
	m_clr = clr;
	m_nPenWid = penWid;
}
int CShape::GetGriperByPoint(const CPoint& pt)
{
	if(m_pAxisX == NULL ||	m_pAxisY == NULL) return -1;
	int nBarWid = m_pAxisX->Value2Pix(1)-m_pAxisX->Value2Pix(0);
	
	int cnt = m_Gripers.GetCount();
	for(int i=0;i<cnt;i++)
	{
		int pixX = m_pAxisX->Value2Pix(m_Gripers[i].x)+nBarWid/2;
		int pixY = m_pAxisY->Value2Pix(m_Gripers[i].y);

		CRect rc(pixX-6,pixY-6,pixX+6,pixY+6);
		if(rc.PtInRect(pt)) return i;
	}

	return -1;
}

void CShape::OnMove(const CPoint& pt)
{
	int offsetX = pt.x - m_PointOrigin.x;
	int offsetY = pt.y - m_PointOrigin.y;
	
	if(m_curMovingGripperIdx == -1)	m_Gripers.RemoveAll();
	
	CPoint curPt;
	ChartDot curDot;
	for(size_t i=0;i<m_PointsOrigin.GetCount();i++)
	{
		if(m_curMovingGripperIdx != -1)
		{
			if(i == m_curMovingGripperIdx)
			{
				curPt.x = m_PointsOrigin[i].x + offsetX;
				curPt.y = m_PointsOrigin[i].y + offsetY;
				curDot.x = m_pAxisX->Pix2Value(curPt.x);
				curDot.y = m_pAxisY->Pix2Value(curPt.y);

				CTimeAxis *pTmAxis = (CTimeAxis *)m_pAxisX;

				if (pTmAxis->GetDataCount() > 0)
				{
					if (pTmAxis->GetDataCount() > curDot.x && curDot.x >= 0)
					{
						curDot.tm = pTmAxis->GetHisData()->GetAt(curDot.x).time;
					}
					else
					{
						curDot.tm = pTmAxis->GetHisData()->GetAt(0).time;
					}

				}

				m_Gripers[i] = curDot;
			}
		}
		else
		{
			curPt.x = m_PointsOrigin[i].x + offsetX;
			curPt.y = m_PointsOrigin[i].y + offsetY;
			curDot.x = m_pAxisX->Pix2Value(curPt.x);
			curDot.y = m_pAxisY->Pix2Value(curPt.y);
			
			CTimeAxis *pTmAxis = (CTimeAxis *)m_pAxisX;
			
			if (pTmAxis->GetDataCount() > 0)
			{
				if (pTmAxis->GetDataCount() > curDot.x && curDot.x >= 0)
				{
					curDot.tm = pTmAxis->GetHisData()->GetAt(curDot.x).time;
				}
				else
				{
					curDot.tm = pTmAxis->GetHisData()->GetAt(0).time;
				}

			}
		
			m_Gripers.Add(curDot);
		}
		
	}
}

void CShape::BeginDraw(IRenderTarget*pRender)
{
	m_clrNormal = m_clr;
	m_arrLines.RemoveAll();
	if(m_bSelected)
	{
		m_clr = 0xFFFFFF-ES_Color()->clrBK;
	}
}
void CShape::EndDraw(IRenderTarget*pRender)
{
	if(m_bSelected || m_bFocus)
	{
		DrawGripers(pRender);
	}
	m_clr = m_clrNormal;
}
void CShape::MoveGriper(int idx, int x, int y)
{
	if(m_pAxisX == NULL ||	m_pAxisY == NULL) return ;
	int cnt = m_Gripers.GetCount();

	if(idx<0 || idx>=cnt) return;

	m_Gripers[idx].x = (int)m_pAxisX->Pix2Value(x);
	m_Gripers[idx].y = m_pAxisY->Pix2Value(y);
}

CPoint CShape::Griper2Pix(const ChartDot& dot)
{
	if (m_pAxisX == NULL || m_pAxisY == NULL)
	{
		return CPoint(-1, -1);
	}
	int nBarWid = m_pAxisX->Value2Pix(1)-m_pAxisX->Value2Pix(0);

	CPoint pt;
	pt.x = (LONG)(m_pAxisX->Value2Pix(dot.x)+nBarWid/2);
	pt.y = (LONG)m_pAxisY->Value2Pix(dot.y);

	return pt;
}

void CShape::DrawGripers(IRenderTarget*pRender)
{
	if(GetType() == doTEXT || GetType() == doICON)
	{
		//pRender->Draw3dRect(m_rcLabel,m_clr,m_clr);
	}
	else
	{
		COLORREF clrBK = ES_Color()->clrBK;
		int cnt = m_Gripers.GetCount();
		CPoint ptCur;
		for(int i=0;i<cnt;i++)
		{
			ptCur = Griper2Pix(m_Gripers[i]);
			//DrawMaoDian(pRender,ptCur,m_clr,clrBK);
		}
	}
	
}

void CShape::ResetAxis(CAxis * pAxisX, CAxis *pAxisY)
{
	m_pAxisX = pAxisX;
	m_pAxisY = pAxisY;
	
	if (m_Gripers.IsEmpty()) return;

	if (m_Gripers[0].x == 0)
	{
		for (size_t i = 0; i < m_Gripers.GetCount(); i++)
		{
			if (m_Gripers[i].tm > 0)
			{
				CTimeAxis *pTmAxis = (CTimeAxis*)pAxisX;
				m_Gripers[i].x = pTmAxis->GetXValueByTime(m_Gripers[i].tm);
			}
		
		}
	}
}

void CShape::ResetX()
{
	for (size_t i = 0; i < m_Gripers.GetCount(); i++)
	{
		m_Gripers[i].x = 0;
	}
}

void CShape::GetGripers(SArray<ChartDot>& ogripers )
{
	ogripers.Copy(m_Gripers);
}
double CShape::GetXMaxValue()
{
	CTimeAxis* pAxis = static_cast<CTimeAxis*>(m_pAxisX);
	if (!pAxis)
		return -1;
	return pAxis->GetDataCount() - 1;
}
int CShape::GetXValueByTime(long time)
{
	CTimeAxis* pAxis = static_cast<CTimeAxis*>(m_pAxisX);
	if(!pAxis)
		return -1;
	return pAxis->GetXValueByTime(time);
}
void CShape::Select(BOOL bSel,int nGripper)
{
	m_bSelected = bSel;
	if(bSel)
	{
		m_PointsOrigin.RemoveAll();
		for(size_t i=0;i<m_Gripers.GetCount();i++)
		{
			m_PointsOrigin.Add(Griper2Pix(m_Gripers[i]));
		}
		if (nGripper >= 0)
		{
			m_curMovingGripperIdx = nGripper;
			STRACE(L"Cusr Griper: %d", nGripper);
		}
	}
	else
	{
		m_curMovingGripperIdx = -1;
	}
}
void CShape::OnClick(const CPoint& pt)				//鼠标单击事件
{
	if(m_bSelected)
	{
		Select(FALSE);
	}
	else
	{
		int ret = HitTest(pt);
		if(ret != -1)
		{
			m_PointOrigin = pt;
			Select(TRUE,ret);
		}
	}
}

int CShape::HitTest(const CPoint& pt)
{
	int cnt = m_Gripers.GetCount();
	CPoint ptPix;

	for(int i=0;i<cnt;i++)
	{
		ptPix = Griper2Pix(m_Gripers[i]);

		if(abs(ptPix.x-pt.x)<5 && abs(ptPix.y-pt.y)<5) return i;
	}
	cnt = m_arrLines.GetCount();
	for(int i=0;i<cnt;i++)
	{
		if(PtInLine(pt,m_arrLines[i].pt1,m_arrLines[i].pt2))
			return -2;
	}

	return -1;
}

//----------------------------CHorizVertLine----------------------------------------------------

void CHorizVertLine::Draw(IRenderTarget*pRender)			//绘制
{
	SASSERT(m_Gripers.GetCount()==1);
	SASSERT(m_pAxisX != NULL);
	SASSERT(m_pAxisY != NULL);

	CPoint ptPix = Griper2Pix(m_Gripers[0]);
	if(GetType() == doSPX) //水平方向
	{
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptPix.y,m_pAxisX->right,ptPix.y,m_clr);
		m_arrLines.Add(CLine(m_pAxisX->left,ptPix.y,m_pAxisX->right,ptPix.y));
		if(m_bAlarmIcon)
		{
			 m_pAxisX->GetChart()->DrawIcon(pRender,m_pAxisX->right-16,ptPix.y-16,7);
		}
	}
	else
	{
		DrawLine(pRender,ptPix.x,m_pAxisY->top,ptPix.x,m_pAxisY->bottom,m_clr);
		m_arrLines.Add(CLine(ptPix.x,m_pAxisY->top,ptPix.x,m_pAxisY->bottom));
	}
}

//
//----------------------------CNormalLine----------------------------------------------------

void CNormalLine::Draw(IRenderTarget*pRender)			//绘制
{
	SASSERT(m_Gripers.GetCount()==2);
	SASSERT(m_pAxisX != NULL);
	SASSERT(m_pAxisY != NULL);

	CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	CPoint ptPix2 = Griper2Pix(m_Gripers[1]);


	if(GetType() == doJTXD)
	{
		//DrawArrowLine(pRender,ptPix1,ptPix2,m_clr,m_nPenWid);
		m_arrLines.Add(CLine(ptPix1,ptPix2));
	}
	else if(GetType() == doQSX)
	{
		//直线
		CLine tmpLine = DrawZhiXian(pRender,ptPix1,ptPix2,m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right);
		m_arrLines.Add(tmpLine);
		if(m_bAlarmIcon)
		{
			if(tmpLine.pt2.y >= m_pAxisY->bottom)
			{
				tmpLine.pt2.x = tmpLine.pt1.x+(m_pAxisY->bottom-tmpLine.pt1.y)*(tmpLine.pt2.x-tmpLine.pt1.x)/(tmpLine.pt2.y-tmpLine.pt1.y);
			    tmpLine.pt2.y = m_pAxisY->bottom;
			}
			else if(tmpLine.pt2.y <= m_pAxisY->top)
			{
				tmpLine.pt2.x = tmpLine.pt1.x+(m_pAxisY->top-tmpLine.pt1.y)*(tmpLine.pt2.x-tmpLine.pt1.x)/(tmpLine.pt2.y-tmpLine.pt1.y);
				tmpLine.pt2.y = m_pAxisY->top + 16;
			}
			m_pAxisX->GetChart()->DrawIcon(pRender,tmpLine.pt2.x-16,tmpLine.pt2.y-16,7);
			
		}
	}
	else
	{
		DrawLine(pRender,PS_SOLID,ptPix1,ptPix2,m_clr,m_nPenWid);
		m_arrLines.Add(CLine(ptPix1,ptPix2));
	}

}

//----------------------------PXX----------------------------------------------------

void CPXX::Draw(IRenderTarget*pRender)			//绘制
{
	SASSERT(m_Gripers.GetCount()==3);
	SASSERT(m_pAxisX != NULL);
	SASSERT(m_pAxisY != NULL);

	CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	CPoint ptPix2 = Griper2Pix(m_Gripers[1]);
	CPoint ptPix3 = Griper2Pix(m_Gripers[2]);
	DrawZhiXian(pRender,ptPix1,ptPix2,m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right);

	m_arrLines.Add(CLine(ptPix1,ptPix2));

	CPoint ptCuiZu = Getplumb(ptPix1,ptPix2,ptPix3);

	int maxDistance = sqrt((double)(m_pAxisX->Width()*m_pAxisX->Width() +  m_pAxisY->Height() * m_pAxisY->Height()));

	int perDistance = PointToLine(ptPix3,ptPix1,ptPix2);
	if(perDistance<5) return;

	int offX = ptPix3.x-ptCuiZu.x;
	int offY = ptPix3.y - ptCuiZu.y;

	int nStep = maxDistance / perDistance;

	for(int i=1;i<nStep;i++)
	{
		CLine tmpLine = DrawZhiXian(pRender,CPoint(ptPix1.x+offX*i,ptPix1.y+offY*i),CPoint(ptPix2.x+offX*i,ptPix2.y+offY*i),m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right);
		m_arrLines.Add(tmpLine);
		tmpLine = DrawZhiXian(pRender,CPoint(ptPix1.x-offX*i,ptPix1.y-offY*i),CPoint(ptPix2.x-offX*i,ptPix2.y-offY*i),m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right);
		m_arrLines.Add(tmpLine);
	}
}

 void CChannel::Draw(IRenderTarget*pRender)
 {
	 SASSERT(m_Gripers.GetCount()==3);
	 SASSERT(m_pAxisX != NULL);
	 SASSERT(m_pAxisY != NULL);

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 CPoint ptPix2 = Griper2Pix(m_Gripers[1]);
	 CPoint ptPix3 = Griper2Pix(m_Gripers[2]);
	 DrawZhiXian(pRender,ptPix1,ptPix2,m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right);

	 m_arrLines.Add(CLine(ptPix1,ptPix2));

	 CPoint ptCuiZu = Getplumb(ptPix1,ptPix2,ptPix3);

	
	 int perDistance = PointToLine(ptPix3,ptPix1,ptPix2);
	
	 int offX = ptPix3.x-ptCuiZu.x;
	 int offY = ptPix3.y - ptCuiZu.y;

	CLine tmpLine = DrawZhiXian(pRender,CPoint(ptPix1.x+offX,ptPix1.y+offY),CPoint(ptPix2.x+offX,ptPix2.y+offY),m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right);
	m_arrLines.Add(tmpLine);
	 
 }
//-------------------------------------Triangle--------------------------------------------

void CTriangle::Draw(IRenderTarget*pRender)			//绘制
{
	SASSERT(m_Gripers.GetCount()==3);
	SASSERT(m_pAxisX != NULL);
	SASSERT(m_pAxisY != NULL);

	CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	CPoint ptPix2 = Griper2Pix(m_Gripers[1]);
	CPoint ptPix3 = Griper2Pix(m_Gripers[2]);

	DrawLine(pRender, PS_SOLID, ptPix1, ptPix2, m_clr, m_nPenWid);
	DrawLine(pRender, PS_SOLID, ptPix1, ptPix3, m_clr, m_nPenWid);
	DrawLine(pRender, PS_SOLID, ptPix3, ptPix2, m_clr, m_nPenWid);

	m_arrLines.Add(CLine(ptPix1,ptPix2));
	m_arrLines.Add(CLine(ptPix1,ptPix3));
	m_arrLines.Add(CLine(ptPix3,ptPix2));
}


//---------------------------------Rect---------------------------------------
 void CRectShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 SASSERT(m_Gripers.GetCount()==2);
	 SASSERT(m_pAxisX != NULL);
	 SASSERT(m_pAxisY != NULL);

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 CPoint ptPix2 = Griper2Pix(m_Gripers[1]);
	 
	 CRect rc(ptPix1,ptPix2);
	 rc.NormalizeRect();

	 DrawRect(pRender,rc,TRANSPARENT,m_clr,m_nPenWid,255);

	 m_arrLines.Add(CLine(rc.left,rc.top,rc.right,rc.top));
	 m_arrLines.Add(CLine(rc.left,rc.top,rc.left,rc.bottom));
	 m_arrLines.Add(CLine(rc.left,rc.bottom,rc.right,rc.bottom));
	 m_arrLines.Add(CLine(rc.right,rc.top,rc.right,rc.bottom));
 }


//----------------------------------------------------------CEllipseShape-----------------------------------
 void CArcShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 SASSERT(m_Gripers.GetCount()==2);
	 SASSERT(m_pAxisX != NULL);
	 SASSERT(m_pAxisY != NULL);

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 CPoint ptPix2 = Griper2Pix(m_Gripers[1]);
	 	 
	 int width = abs(2*(ptPix2.x - ptPix1.x));
	 int height = abs(2*(ptPix2.y-ptPix1.y));

	 int x = ptPix2.x> ptPix1.x ?  ptPix1.x : (ptPix1.x-width);
	 int y = ptPix2.y< ptPix1.y ?  ptPix2.y : (ptPix2.y - height);

	
	 if(ptPix2.y>ptPix1.y)
	 {
		 DrawArc(pRender,m_clr, x,y,width,height,0,180,m_nPenWid);
		 m_bUp = TRUE;
	 }
	 else
	 {
		 DrawArc(pRender,m_clr, x,y,width,height,180,180,m_nPenWid);
		 m_bUp = FALSE;
	 }
	 m_circleRect.SetRect(x,y,x+width,y+height);
 }
 int CArcShape::HitTest(const CPoint& pt)
 {
	 int cnt = m_Gripers.GetCount();
	 CPoint ptPix;

	 for(int i=0;i<cnt;i++)
	 {
		 ptPix = Griper2Pix(m_Gripers[i]);

		 if(abs(ptPix.x-pt.x)<5 && abs(ptPix.y-pt.y)<5) return i;
	 }

	 return PtInEllips(pt,m_circleRect,m_bUp) ? -2 : -1;
 }

 //----------------------------------------------------------CCircleShape-----------------------------------
 int CCircleShape::HitTest(const CPoint&  pt)
 {
	 size_t cnt = m_Gripers.GetCount();
	 CPoint ptPix;

	 for(size_t i=0;i<cnt;i++)
	 {
		 ptPix = Griper2Pix(m_Gripers[i]);
		 if(abs(ptPix.x-pt.x)<5 && abs(ptPix.y-pt.y)<5)
			 return i;
	 }

	 for(size_t i=0;i<m_circleRects.GetCount();i++)
	 {
		 if(PtInEllips(pt,m_circleRects[i],m_bUp))
		 {
			 return -2;
		 }
	 }
	 return -1;
 }

 void CCircleShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 SASSERT(m_Gripers.GetCount()==2);
	 SASSERT(m_pAxisX != NULL);
	 SASSERT(m_pAxisY != NULL);

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 CPoint ptCenter = Griper2Pix(m_Gripers[1]);

	 //半径
	 int nBJ = sqrt((double)(ptCenter.x-ptPix1.x)*(ptCenter.x-ptPix1.x) + (ptCenter.y-ptPix1.y)*(ptCenter.y-ptPix1.y)); 

	 if(nBJ<10) return;

	 int nCurBJ = nBJ/3;
	 int nAddBJ = nBJ/8;

	 for(int i=0;i<3;i++)
	 {
		 if(ptPix1.y<ptCenter.y)
		 {
			 DrawArc(pRender,m_clr,ptCenter.x-nCurBJ,ptCenter.y-nCurBJ,nCurBJ*2,nCurBJ*2,180,180,m_nPenWid);
			 m_bUp = TRUE;
		 }
		 else
		 {
			 DrawArc(pRender,m_clr,ptCenter.x-nCurBJ,ptCenter.y-nCurBJ,nCurBJ*2,nCurBJ*2,0,180,m_nPenWid);
			 m_bUp = FALSE;
		 }

		 m_circleRects.Add(CRect(ptCenter.x-nCurBJ,ptCenter.y-nCurBJ,ptCenter.x+nCurBJ,ptCenter.y+nCurBJ));
		 nCurBJ += nAddBJ;
	 }
 }


 //-----------------------------周期线-------------------------------------
 void CZQXShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 
	 int nFirst = m_Gripers[0].x;
	 int nStep = abs(m_Gripers[1].x-nFirst);

	 if(nStep == 0) return;
	 
	 int nBarWid = m_pAxisX->Value2Pix(2)-m_pAxisX->Value2Pix(1);
	 int nCount = 0;
	 for(int i = nFirst;i<m_pAxisX->GetDuration();i+=nStep)
	 {
		 int pixX = m_pAxisX->Value2Pix(i) + nBarWid/2;
		 
		 if(pixX>m_pAxisX->right) break;
		 if(nCount<2)
		 {
			DrawLine(pRender,pixX,m_pAxisY->top,pixX,m_pAxisY->bottom,m_clr,m_nPenWid);	
			m_arrLines.Add(CLine(pixX,m_pAxisY->top,pixX,m_pAxisY->bottom));
		 }
		 else
		 {	 
			 DotLine(pRender,pixX,m_pAxisY->top,pixX,m_pAxisY->bottom,m_clr);
			 m_arrLines.Add(CLine(pixX,m_pAxisY->top,pixX,m_pAxisY->bottom));
		 }
		 nCount ++;
	 }
	
 }


 //-----------------------------黄金分割和百分比-------------------------------------
 void CHJFG_BFBShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 SASSERT(m_Gripers.GetCount()==2);
	 SASSERT(m_pAxisX != NULL);
	 SASSERT(m_pAxisY != NULL);

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 CPoint ptPix2 = Griper2Pix(m_Gripers[1]);

	 int nDiff = ptPix2.y - ptPix1.y;
	 if(abs(nDiff)<10) return;

	  DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptPix1.y,m_pAxisX->right,ptPix1.y,m_clr,m_nPenWid);
	  DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptPix2.y,m_pAxisX->right,ptPix2.y,m_clr,m_nPenWid);
	 m_arrLines.Add(CLine(m_pAxisX->left,ptPix2.y,m_pAxisX->right,ptPix2.y));
	 m_arrLines.Add(CLine(m_pAxisX->left,ptPix1.y,m_pAxisX->right,ptPix1.y));

	 pRender->SetTextColor(m_clr);
	 SArray<float> arrPixY;
	 if(GetType() == doHJFG)
	 {
		 arrPixY.Add(0.382);
		 arrPixY.Add(0.618);
		 arrPixY.Add(1.382);
		 arrPixY.Add(1.618);
	 }
	 else if(GetType() == doBFB)
	 {
		 arrPixY.Add(0.25);
		 arrPixY.Add(0.5);
		 arrPixY.Add(0.75);
	 }

	 CSize sz;
	 pRender->MeasureText(L"0.618", _tcslen(L"0.618"), &sz);
	 SStringW str;
	 CRect rcLabel;
	 for(size_t i=0;i<arrPixY.GetCount();i++)
	 {
		 DotLine(pRender,m_pAxisX->left,ptPix1.y+arrPixY[i]*nDiff ,m_pAxisX->right,ptPix1.y+arrPixY[i]*nDiff, m_clr);
		 m_arrLines.Add(CLine(m_pAxisX->left,ptPix1.y+arrPixY[i]*nDiff ,m_pAxisX->right,ptPix1.y+arrPixY[i]*nDiff));

		 str.Format(L"%.3f",arrPixY[i]);
		 rcLabel.SetRect(m_pAxisX->right - sz.cx - 10, static_cast<int>(ptPix1.y + arrPixY[i] * nDiff), m_pAxisX->right, ptPix1.y + arrPixY[i] * nDiff + 40);
		 pRender->DrawText(str,str.GetLength(), &rcLabel,DT_LEFT|DT_SINGLELINE);
	 }
 }


 //-------------------------------FBLQ---------------
 void CFBLQShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 SASSERT(m_Gripers.GetCount()==1);
	 SASSERT(m_pAxisX != NULL);
	 SASSERT(m_pAxisY != NULL);

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 int nBarWid = m_pAxisX->Value2Pix(2)-m_pAxisX->Value2Pix(1);

	 int curIdx = 0;
	 int lastN = 0;
	 int lastN2 = 0;

	 for(int i=0;i<10000;i++)
	 {
		 curIdx = (i==1)? 1 : (lastN + lastN2);

		 int nCurPixX = ptPix1.x  + curIdx*nBarWid;
		 
		 if(nCurPixX>m_pAxisX->right) break;

		 DrawLine(pRender,nCurPixX,m_pAxisY->top,nCurPixX,m_pAxisY->bottom,m_clr,m_nPenWid);
		 m_arrLines.Add(CLine(nCurPixX,m_pAxisY->top,nCurPixX,m_pAxisY->bottom));
		 lastN2 = lastN;
		 lastN = curIdx;

	 }
 }

 //---------------------------SZGSX-----------------------------------------
 void CSZX_GSXShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 CPoint& ptFrom =  Griper2Pix(m_Gripers[0]);
	 CPoint& ptCursor =  Griper2Pix(m_Gripers[1]);

	 if(ptCursor.x<ptFrom.x) return;

	 CRect rc(ptFrom,ptCursor);
	 rc.NormalizeRect();
	 DrawRect(pRender,rc,TRANSPARENT,m_clr,1,255);
	 DrawSheXian(pRender,ptFrom,ptCursor,m_clr,1);

	 SArray<double> arrPixAdd;
	 if(GetType() == doSZX)
	 {
		 arrPixAdd.Add(1.0/3);
		 arrPixAdd.Add(2.0/3);
		 for(size_t i=0;i<arrPixAdd.GetCount();i++)
		 {
			 int curPixY = static_cast<int>(ptFrom.y + rc.Height() * (ptCursor.y>ptFrom.y ? arrPixAdd[i] : (-1 * arrPixAdd[i])));
			 m_arrLines.Add(DrawSheXian(pRender,ptFrom,CPoint(ptCursor.x,curPixY),m_clr,m_nPenWid));
		 }
	 }
	 else
	 {
		 //甘氏线
		 arrPixAdd.Add(0.125);
		 arrPixAdd.Add(0.25);
		 arrPixAdd.Add(0.33);
		 arrPixAdd.Add(0.5);
		 for(size_t i=0;i<arrPixAdd.GetCount();i++)
		 {
			 int curPixY = ptFrom.y + rc.Height() * (ptCursor.y>ptFrom.y ?  arrPixAdd[i] : (-1*arrPixAdd[i]));
			 m_arrLines.Add(DrawSheXian(pRender,ptFrom,CPoint(ptCursor.x,curPixY),m_clr,m_nPenWid));

			 int curPixX = static_cast<int>(ptFrom.x + rc.Width() * arrPixAdd[i]);
			 m_arrLines.Add(DrawSheXian(pRender,ptFrom,CPoint(curPixX,ptCursor.y),m_clr,m_nPenWid));
		 }
	 }
 }


 //--------------------------XXHG--------------------------------------
 void CXXHG_Shape::Draw(IRenderTarget*pRender)			//绘制
 {
	 
	 int xFrom = m_Gripers[0].x;
	 int xTo   = m_Gripers[1].x;



	 if(xFrom == xTo) return;

	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);
	 CPoint ptPix2 = Griper2Pix(m_Gripers[1]);
	 DrawLine( pRender,ptPix1.x, m_pAxisY->top,ptPix1.x,m_pAxisY->bottom,m_clr);
	 m_arrLines.Add(CLine(ptPix1.x, m_pAxisY->top,ptPix1.x,m_pAxisY->bottom));
	 DrawLine(pRender,ptPix2.x, m_pAxisY->top,ptPix2.x,m_pAxisY->bottom,m_clr);
	 m_arrLines.Add(CLine(ptPix2.x, m_pAxisY->top,ptPix2.x,m_pAxisY->bottom));

	 if(xFrom >xTo)
	 {
		 int tmpX = xFrom;
		 xFrom = xTo;
		 xTo = tmpX;
	 }


	 SArray<double> arrData;

	 for(int i=xFrom; i<xTo; i++)
	 {
		 arrData.Add(m_pAxisX->Value2Pix(i));
		 double yValue = m_pAxisY->Value2Pix(m_pAxisX->GetChart()->GetXAxis()->GetHisData()->GetAt(i).Close);
		 arrData.Add(yValue);
	 }

	 double a=0,b=0,other[4]={0};
	 //求线性回归
	 if(LinearRegression(arrData.GetData(),arrData.GetCount()/2,&b,&a,other) == 0)
	 {
		 //y = ax + b
		 CPoint ptA ,ptB;
		 ptA.x = m_pAxisX->Value2Pix(xFrom);
		 ptA.y = static_cast<LONG>(ptA.x * a + b);
		 ptB.x = m_pAxisX->Value2Pix(xTo);
		 ptB.y = static_cast<LONG>(ptB.x * a + b);

		 m_arrLines.Add(DrawZhiXian(pRender,ptA,ptB,m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right));

		 int maxPixY = 0;
		 int curPPP = 0;
		 for(int i=xFrom;i<xTo;i++)
		 {
			 double curYPix = arrData[curPPP+1];
			 double curHGYPix = arrData[curPPP] * a + b;
			 maxPixY = static_cast<int>((abs(curYPix-curHGYPix),maxPixY));
			 curPPP += 2;
		 }
		 
		 m_arrLines.Add(DrawZhiXian(pRender,CPoint(ptA.x,ptA.y-maxPixY),CPoint(ptB.x,ptB.y-maxPixY),m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right));
		 m_arrLines.Add(DrawZhiXian(pRender,CPoint(ptA.x,ptA.y+maxPixY),CPoint(ptB.x,ptB.y+maxPixY),m_clr,m_nPenWid,m_pAxisX->left,m_pAxisX->right));
	 }
}
 
 int CDotShape::HitTest(const CPoint&  pt)
 {
	if(m_rcLabel.PtInRect(pt))
		return 0;
	else
		return -1;
 }
 void CDotShape::Draw(IRenderTarget*pRender)			//绘制
 {
	 CPoint ptPix1 = Griper2Pix(m_Gripers[0]);


	 if(GetType() == doICON)
	 {	 
		 m_pAxisX->GetChart()->DrawIcon(pRender,ptPix1.x-8,ptPix1.y-8,m_nIcon);
		 m_rcLabel.SetRect(ptPix1.x-8,ptPix1.y-8,ptPix1.x+8,ptPix1.y+8);
	 }
	 else
	 {
		 if(m_strText == L"") return;
		 CSize sz;
		 pRender->MeasureText(m_strText, m_strText.GetLength(), &sz);
		 CRect rcLabel(ptPix1.x,ptPix1.y,sz.cx + 10+ptPix1.x, sz.cy+10+ptPix1.y);
		 
		 DrawRect(pRender,rcLabel,ES_Color()->clrBorder,ES_Color()->clrBorder,1,180);
		 
		 COLORREF oldClr = pRender->SetTextColor(m_clr);
		 pRender->DrawText(m_strText, m_strText.GetLength(),&rcLabel, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
		 m_rcLabel = rcLabel;
		 pRender->SetTextColor(oldClr);
	 }
 }
