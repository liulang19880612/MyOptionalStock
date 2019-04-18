#pragma once

class CDrawToolDlg : public SHostWnd
{
public:
	CDrawToolDlg(const SStringW&strXMLLayout);
	~CDrawToolDlg();
public:
	void OnLButtonUp(UINT nFlags, CPoint point);
public:
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
};

