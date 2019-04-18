#include "stdafx.h"
#include "DrawToolDlg.h"


CDrawToolDlg::CDrawToolDlg(const SStringW&strXMLLayout) : SHostWnd(strXMLLayout)
{

}


CDrawToolDlg::~CDrawToolDlg()
{

}
void CDrawToolDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
}