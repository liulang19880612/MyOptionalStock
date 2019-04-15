#pragma once
class CChartTitleCtrl : public SWindow
{
	SOUI_CLASS_NAME(CChartTitleCtrl, L"charttitlectrl")
public:
	CChartTitleCtrl();
	virtual ~CChartTitleCtrl();
public:
	void OnPaint(IRenderTarget *pRT);
protected:
	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
	SOUI_MSG_MAP_END()
	
	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"backgroundskin", m_pSkinBackGround, TRUE)
	SOUI_ATTRS_END()
protected:
	ISkinObj*   m_pSkinBackGround;
};

