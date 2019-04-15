#include "StdAfx.h"
#include "ShapePen.h"
#include "AxisChart.h"
#include "TimeAxis.h"
#include <math.h>

inline DWORD ColorReverse()
{
	return 0xFFFFFF- ES_Color()->clrBK;
}
CLine DrawZhiXian(IRenderTarget*pRender,const CPoint& pt1,const CPoint& pt2, COLORREF clr, int nWid, int left,int right);
int PointToLine(const CPoint& pt, const CPoint& linePt1,const CPoint& linePt2);
CPoint Getplumb(const CPoint& pt1,const CPoint& pt2, const CPoint& ptOut); //求垂足
CLine DrawSheXian(IRenderTarget*pRender,const CPoint& ptOrigin, const CPoint& pt2 ,COLORREF clr, int nPenWid);
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor);		//求线性回归

CShapePen::CShapePen():
	m_bOk(false),
	m_pAxisX(NULL),
	m_pAxisY(NULL),
	m_type(doSelect)
{

}


CShapePen::~CShapePen(void)
{
}

void CShapePen::Create(RAPenType type, CAxis *pAixsX, CAxis *pAxisY)
{
	m_type = type;
	m_pAxisX = pAixsX;
	m_pAxisY = pAxisY;
}

bool CShapePen::IsDrawing()
{
	return m_arrData.GetCount()>0 && !m_bOk;
}
bool CShapePen::IsOk()
{
	return m_bOk;
}
void CShapePen::Clear()
{
	m_arrData.RemoveAll();
	m_arrPixPoint.RemoveAll();
}
void CShapePen::GetData(SArray<ChartDot>& arr)   //取得绘制的图形 
{
	arr.RemoveAll();
	if(!m_arrData.IsEmpty()) arr.Copy(m_arrData);
}

void CShapePen::OnClick(const CPoint& pt)
{
	if(IsOk()) return;

	ChartDot dot;
	dot.x = (int)m_pAxisX->Pix2Value(pt.x);
	dot.y = (double)m_pAxisY->Pix2Value(pt.y);
	m_arrData.Add(dot);

	if(m_arrData.GetCount() == GetMaxGriperCount())
	{
		m_bOk = true;
	}
	else
	{
		m_arrPixPoint.Add(pt);
	}
	
}

//-------------------------------------------------------水平线和竖直线-----------------------------------------

void CVertHozPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();
	if(m_type == doSPX)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else
		DrawLine(pRender,ptCursor.x,m_pAxisY->top,ptCursor.x,m_pAxisY->bottom,ColorReverse());
}

//-----------------------------------------------------------普通线-----------------------------------------------------------
void CNoramlLinePen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());

	if(m_arrData.GetCount() ==1)
	{
		DotLine(pRender,m_arrPixPoint[0].x,m_arrPixPoint[0].y,m_pAxisX->right,m_arrPixPoint[0].y,ColorReverse());
		pRender->SetTextColor(ColorReverse());
		SStringW str;
		double k = 0;

		CPoint& ptLast = m_arrPixPoint[0];
		if(ptCursor.x != ptLast.x)
			k =  abs((ptCursor.y - ptLast.y) / (double)(ptCursor.x-ptLast.x));

		double angle = (ptCursor.x == ptLast.x) ? (3.14159/2) : atan(k);  
		
		int angle2 =(int)(angle/3.14159 * 180);
		if(ptCursor.x<ptLast.x) 
		{
			//第2，3象限
			angle2 = 180-angle2;
		}
		else
		{
			if(ptCursor.y>ptLast.y)
			{
				//第四象限
				angle2 = -angle2;
			}
		}
		str.Format(L"%d", angle2);

		CRect rcLabel(m_arrPixPoint[0].x - 30, m_arrPixPoint[0].y-20, m_arrPixPoint[0].x+30,m_arrPixPoint[0].y+20);
		pRender->DrawText(str, str.GetLength(),&rcLabel, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		

		if (m_type == doJTXD)
			;//DrawArrowLine(pRender,m_arrPixPoint[0],ptCursor,ES_Color()->clrSelect);
		else if(m_type == doQSX)
		{
			DrawZhiXian(pRender,m_arrPixPoint[0],ptCursor,ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
		}
		else	
			DrawLine(pRender,m_arrPixPoint[0].x,  m_arrPixPoint[0].y,ptCursor.x,  ptCursor.y,  ColorReverse());
	}
	
}

//-----------------------------------------------------------平行线-----------------------------------------------------------
void CChannelPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();
	if(m_arrData.GetCount()==0)
	{
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	}
	else if(m_arrData.GetCount()==1)
	{
		DrawZhiXian(pRender,m_arrPixPoint[0],ptCursor,ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
	}
	else
	{
		DrawZhiXian(pRender,m_arrPixPoint[0],m_arrPixPoint[1],ColorReverse(),2,m_pAxisX->left,m_pAxisX->right);

		CPoint ptCuiZu = Getplumb(m_arrPixPoint[0],m_arrPixPoint[1],ptCursor);

		
		
		int offX = ptCursor.x-ptCuiZu.x;
		int offY = ptCursor.y - ptCuiZu.y;

		DrawZhiXian(pRender,CPoint(m_arrPixPoint[0].x+offX,m_arrPixPoint[0].y+offY),CPoint(m_arrPixPoint[1].x+offX,m_arrPixPoint[1].y+offY),ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
	

	}
}
void CPXXPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();
	if(m_arrData.GetCount()==0)
	{
		DrawLine(pRender,PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	}
	else if(m_arrData.GetCount()==1)
	{
		DrawZhiXian(pRender,m_arrPixPoint[0],ptCursor,ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
	}
	else
	{
		DrawZhiXian(pRender,m_arrPixPoint[0],m_arrPixPoint[1],ColorReverse(),2,m_pAxisX->left,m_pAxisX->right);
		
		CPoint ptCuiZu = Getplumb(m_arrPixPoint[0],m_arrPixPoint[1],ptCursor);
		
		int maxDistance = (int)sqrt((double)(m_pAxisX->Width()*m_pAxisX->Width() +  m_pAxisY->Height() * m_pAxisY->Height()));
		
		int perDistance = PointToLine(ptCursor,m_arrPixPoint[0],m_arrPixPoint[1]);
		if(perDistance<5) return;

		int offX = ptCursor.x-ptCuiZu.x;
		int offY = ptCursor.y - ptCuiZu.y;

		int nStep = maxDistance / perDistance;

		for(int i=1;i<nStep;i++)
		{
			DrawZhiXian(pRender,CPoint(m_arrPixPoint[0].x+offX*i,m_arrPixPoint[0].y+offY*i),CPoint(m_arrPixPoint[1].x+offX*i,m_arrPixPoint[1].y+offY*i),ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
			DrawZhiXian(pRender,CPoint(m_arrPixPoint[0].x-offX*i,m_arrPixPoint[0].y-offY*i),CPoint(m_arrPixPoint[1].x-offX*i,m_arrPixPoint[1].y-offY*i),ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
		}
	}
}


//--------------------------------------------TrianglePen---------------------------------------------------------

void CTrianglePen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else if(m_arrData.GetCount()==1)
	{
		DrawLine(pRender,PS_SOLID,m_arrPixPoint[0],ptCursor,ColorReverse());
	}
	else
	{
		DrawLine(pRender,PS_SOLID,m_arrPixPoint[0],m_arrPixPoint[1],ColorReverse());
		DrawLine(pRender,PS_SOLID,m_arrPixPoint[0],ptCursor,ColorReverse());
		DrawLine(pRender,PS_SOLID,m_arrPixPoint[1],ptCursor,ColorReverse());
	}
}



//------------------------------RectPen--------------------------------------
void CRectPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();
	
	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else
	{
		CRect rc(m_arrPixPoint[0],ptCursor);
		rc.NormalizeRect();

		DrawRect(pRender,rc,TRANSPARENT,ColorReverse(),1,255);
	}
}


//------------------------------------Ellipse-----------------------------------
void CArcPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else
	{
		CPoint &ptPix1 = m_arrPixPoint[0];
		CPoint &ptPix2 = ptCursor;

		int width = abs(2*(ptPix2.x - ptPix1.x));
		int height = abs(2*(ptPix2.y-ptPix1.y));

		int x = ptPix2.x> ptPix1.x ?  ptPix1.x : (ptPix1.x-width);
		int y = ptPix2.y< ptPix1.y ?  ptPix2.y : (ptPix2.y - height);

		DotLine(pRender,x,y+height/2,x+width,y+height/2,ColorReverse());
		DotLine(pRender,ptPix2.x, ptPix2.y, x+width/2,y+height/2,ColorReverse());

		if(ptPix2.y>ptPix1.y)
		{
			DrawArc(pRender,ColorReverse(), x,y,width,height,0,180);
		}
		else
		{
			DrawArc(pRender,ColorReverse(), x,y,width,height,180,180);
		}
		
	}
}

//-------------------------------------------------Circle------------------------------------

void CCirclePen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else
	{
		CPoint &ptPix1 = m_arrPixPoint[0];
		
		CPoint &ptCenter = ptCursor;
		DotLine(pRender,ptPix1.x,ptPix1.y,ptCenter.x,ptCenter.y,ColorReverse());
		//半径
		int nBJ = (int)sqrt((double)(ptCenter.x-ptPix1.x)*(ptCenter.x-ptPix1.x) + (ptCenter.y-ptPix1.y)*(ptCenter.y-ptPix1.y)); 

		if(nBJ<10) return;

		int nCurBJ = nBJ/3;
		int nAddBJ = nBJ/8;

		for(int i=0;i<3;i++)
		{
			if(ptPix1.y<ptCenter.y)
			{
				DrawArc(pRender,ColorReverse(),ptCenter.x-nCurBJ,ptCenter.y-nCurBJ,nCurBJ*2,nCurBJ*2,180,180);
			}
			else
			{
				DrawArc(pRender,ColorReverse(),ptCenter.x-nCurBJ,ptCenter.y-nCurBJ,nCurBJ*2,nCurBJ*2,0,180);
			}

			nCurBJ += nAddBJ;
		}
	}
}

//--------------------------------------------------ZQXPen-------------------------------
void CZQXPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		DrawLine(pRender,ptCursor.x, m_pAxisY->top,ptCursor.x,m_pAxisY->bottom,ColorReverse());
	else
	{
		int nFirst = (int)m_arrData[0].x;
		int nStep = (int)m_pAxisX->Pix2Value(ptCursor.x) - nFirst;

		if(nStep==0) return;

		int nBarWid = m_pAxisX->Value2Pix(2)-m_pAxisX->Value2Pix(1);
		int nCount = 0;
		for(int i = nFirst;;i+=nStep)
		{
			int pixX = m_pAxisX->Value2Pix(i) + nBarWid/2;

			if(pixX<m_pAxisX->left || pixX>m_pAxisX->right) break;
			if(nCount<2)
			{
				DrawLine(pRender,pixX,m_pAxisY->top,pixX,m_pAxisY->bottom,ColorReverse());	 
			}
			else
				DotLine(pRender,pixX,m_pAxisY->top,pixX,m_pAxisY->bottom,ColorReverse());

			nCount++;
		}
	}
}


//---------------------------------------------HJFG---BFB----------------------------
void CHJF_BFBGPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else
	{
		CPoint &ptPix1 = m_arrPixPoint[0];

		int nDiff = ptCursor.y - ptPix1.y;
		if(abs(nDiff)<10) return;

		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptPix1.y,m_pAxisX->right,ptPix1.y,ColorReverse());
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
		
		pRender->SetTextColor(ColorReverse());

		SArray<double> arrPixY;

		if(m_type == doHJFG)
		{
			arrPixY.Add(0.382);
			arrPixY.Add(0.618);
			arrPixY.Add(1.382);
			arrPixY.Add(1.618);
		}
		else if(m_type == doBFB)
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
			DotLine(pRender,m_pAxisX->left,(int)(ptPix1.y+arrPixY[i]*nDiff) ,m_pAxisX->right,(int)(ptPix1.y+arrPixY[i]*nDiff), ColorReverse());
			str.Format(L"%.3f",arrPixY[i]);
			rcLabel.SetRect(m_pAxisY->left-sz.cx-10, (int)(ptPix1.y+arrPixY[i]*nDiff),m_pAxisY->left,(int)(ptPix1.y+arrPixY[i]*nDiff+40));
			pRender->DrawText(str, str.GetLength(),&rcLabel, DT_LEFT | DT_SINGLELINE);
		}
	}
}

//---------------------FBLQ---------------------
void CFBLQPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		DrawLine(pRender,ptCursor.x,m_pAxisY->top,ptCursor.x,m_pAxisY->bottom,ColorReverse());

}

//-----------------------------甘氏，速阻线------------------------
void CSZX_GSXPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		 DrawLine(pRender, PS_SOLID,m_pAxisX->left,ptCursor.y,m_pAxisX->right,ptCursor.y,ColorReverse());
	else
	{
		CPoint& ptFrom = m_arrPixPoint[0];
		
		CRect rc(ptFrom,ptCursor);
		rc.NormalizeRect();
		DrawRect(pRender,rc,TRANSPARENT,ColorReverse(),1,255);
		if(ptCursor.x<ptFrom.x) return;

		DrawSheXian(pRender,ptFrom,ptCursor,ColorReverse(),1);
		
		SArray<double> arrPixAdd;
		if(m_type == doSZX)
		{
			arrPixAdd.Add(1.0/3);
			arrPixAdd.Add(2.0/3);
			for (size_t i = 0; i<arrPixAdd.GetCount(); i++)
			{
				int curPixY =(int)(ptFrom.y + rc.Height() * (ptCursor.y>ptFrom.y ?  arrPixAdd[i] : (-1*arrPixAdd[i])));
				DrawSheXian(pRender,ptFrom,CPoint(ptCursor.x,curPixY),ColorReverse(),1);
			}
		}
		else
		{
			//甘氏线
			arrPixAdd.Add(0.125);
			arrPixAdd.Add(0.25);
			arrPixAdd.Add(0.33);
			arrPixAdd.Add(0.5);
			for (size_t i = 0; i<arrPixAdd.GetCount(); i++)
			{
				int curPixY =(int)( ptFrom.y + rc.Height()* (ptCursor.y>ptFrom.y ?  arrPixAdd[i] : (-1*arrPixAdd[i])));
				DrawSheXian(pRender,ptFrom,CPoint(ptCursor.x,curPixY),ColorReverse(),1);

				int curPixX =(int)( ptFrom.x + rc.Width() * arrPixAdd[i]);

				DrawSheXian(pRender,ptFrom,CPoint(curPixX,ptCursor.y),ColorReverse(),1);
			}
		}
	}
}


//-----------------------XXHG----------------------------------------------------
void CXXHG_Pen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	CPoint ptCursor = m_pAxisX->GetCursorPos();

	if(m_arrData.GetCount()==0)
		DrawLine(pRender,ptCursor.x, m_pAxisY->top,ptCursor.x,m_pAxisY->bottom,ColorReverse());
	else
	{
		DrawLine(pRender,m_arrPixPoint[0].x, m_pAxisY->top,m_arrPixPoint[0].x,m_pAxisY->bottom,ColorReverse());
		DrawLine(pRender,ptCursor.x, m_pAxisY->top,ptCursor.x,m_pAxisY->bottom,ColorReverse());

		int xFrom =(int) m_arrData[0].x;
		int xTo   =(int) m_pAxisX->Pix2Value(ptCursor.x);
		if(xFrom == xTo) return;

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
			ptA.y =(LONG) (ptA.x * a + b);
			ptB.x = m_pAxisX->Value2Pix(xTo);
			ptB.y =(LONG)(ptB.x * a + b);

			DrawZhiXian(pRender,ptA,ptB,ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);

			int maxPixY = 0;
			int curPPP = 0;
			for(int i=xFrom;i<xTo;i++)
			{
				double curYPix = arrData[curPPP+1];
				double curHGYPix = arrData[curPPP] * a + b;
				maxPixY =(int) max(abs(curYPix-curHGYPix),maxPixY);
				curPPP += 2;
			}

			DrawZhiXian(pRender,CPoint(ptA.x,ptA.y-maxPixY),CPoint(ptB.x,ptB.y-maxPixY),ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
			DrawZhiXian(pRender,CPoint(ptA.x,ptA.y+maxPixY),CPoint(ptB.x,ptB.y+maxPixY),ColorReverse(),1,m_pAxisX->left,m_pAxisX->right);
		}
	}
}

//---------------------------Text   Icon-------------------
void CDotPen::OnClick(const CPoint& pt)
{
	__super::OnClick(pt);
}


void CDotPen::Draw(IRenderTarget*pRender)
{
	if(IsOk()) return;
	
	CPoint ptCursor = m_pAxisX->GetCursorPos();
	if(m_type == doICON)
	{
		m_pAxisX->GetChart()->DrawIcon(pRender,ptCursor.x-8,ptCursor.y-8,m_nIcon);
	}
	
}
