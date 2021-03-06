#pragma once

#include <atltime.h>

class CKeyboardWizardDlg;
class CIMChartDlg;
class CDrawToolDlg;

#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/CandleChart.h"


class CMainDlg : public SOUI::SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();
public:
	void ShowKeyboardWizard();
	void _GetCurrentValue();
public:
	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnImChart();
	void OnChangeValue();
	void OnDrawTool();
	void OnSize(UINT nType, SOUI::CSize size);
	BOOL OnEraseBkgnd(HDC dc);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//	消息处理
protected:
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_im_chart", OnImChart)
		EVENT_NAME_COMMAND(L"btn_change_value", OnChangeValue)
		EVENT_NAME_COMMAND(L"btn_draw_tool", OnDrawTool)
	EVENT_MAP_END()

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CHAR(OnChar)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
protected:
	BOOL                            m_bLayoutInited;       // 初始化完成标记
	CKeyboardWizardDlg*             m_pAutoPromptDlg;      // 自动提示
	CIMChartDlg*                    m_pIMChartDlg;         // 聊天工具
	CDrawToolDlg*                   m_pDrawToolDlg;        // 画图工具
	SArray<DC_KData>                m_arrDynaData; 
	SArray<DC_KData>                m_arrKData;
	int                             m_uValue1, m_uValue2, m_uValue3, m_uValue4;
};

