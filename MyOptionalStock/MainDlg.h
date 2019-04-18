#pragma once

#include <atltime.h>

class CKeyboardWizardDlg;
class CIMChartDlg;

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
	void OnSize(UINT nType, SOUI::CSize size);
	BOOL OnEraseBkgnd(HDC dc);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//	��Ϣ����
protected:
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_im_chart", OnImChart)
		EVENT_NAME_COMMAND(L"btn_change_value", OnChangeValue)
	EVENT_MAP_END()

	//HostWnd��ʵ������Ϣ����
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
	BOOL                            m_bLayoutInited;       // ��ʼ����ɱ��
	CKeyboardWizardDlg*             m_pAutoPromptDlg;      // �Զ���ʾ
	CIMChartDlg*                    m_pIMChartDlg;         // ���칤��
	SArray<DC_KData>                m_arrDynaData; 
	SArray<DC_KData>                m_arrKData;
	int                             m_uValue1, m_uValue2, m_uValue3, m_uValue4;
};

