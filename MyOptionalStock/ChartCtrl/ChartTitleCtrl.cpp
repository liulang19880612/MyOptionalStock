#include "stdafx.h"
#include "ChartTitleCtrl.h"


CChartTitleCtrl::CChartTitleCtrl()
{
}


CChartTitleCtrl::~CChartTitleCtrl()
{

}

void CChartTitleCtrl::OnPaint(IRenderTarget *pRT)
{
	if (m_pSkinBackGround)
	{
		m_pSkinBackGround->Draw(pRT, GetClientRect(), TRUE);
	}
}