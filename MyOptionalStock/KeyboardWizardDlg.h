#pragma once

//////////////////////////////////////////////////////////////////////////
//  键盘精灵
//////////////////////////////////////////////////////////////////////////
#include "./ExternEvent/ExtendEvents.h"
#include "SMcListViewEx/SMCListViewEx.h"
struct StockInfo
{
	SOUI::SStringW code_id;
	SOUI::SStringW code_name;
	SOUI::SStringW code_exchange;
};
class CKeyboardWizardDlg : public SOUI::SHostWnd, public SOUI::SMcAdapterBase
{
public:
	CKeyboardWizardDlg();
	~CKeyboardWizardDlg();
public:
	void SetFirstChar(UINT nChar){ m_utFirstChar = nChar; };
protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam); 
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnCodeChange(SOUI::EventArgs * pEvt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected: 
	// 重写 SMcAdapterBase 接口
	virtual int getCount();
	virtual SOUI::SStringW GetColumnName(int iCol) const;
	virtual void getView(int position, SOUI::SWindow * pItem, pugi::xml_node xmlTemplate);
protected:
	BOOL                   m_bLayoutInited;
	std::vector<StockInfo> m_curSelectStock;
	UINT                   m_utFirstChar;
protected:
	EVENT_MAP_BEGIN()
		EVENT_NAME_HANDLER(L"edit_code", SOUI::EVT_RE_NOTIFY, OnCodeChange)
	EVENT_MAP_END()
	BEGIN_MSG_MAP_EX(CKeyboardWizardDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_KEYDOWN(OnKeyDown)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
};

