#include "StdAfx.h"
#include "SMiniblink.h"
#include <Imm.h>
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"msimg32.lib")
namespace SOUI
{
	//////////////////////////////////////////////////////////////////////////
	// SWkeLoader


	SWkeLoader::SWkeLoader()
	{
		wkeInit();
	}


	SWkeLoader::~SWkeLoader()
	{
		wkeShutdown();
	}

	//////////////////////////////////////////////////////////////////////////
	// SWkeWebkit

	SWkeWebkit::SWkeWebkit(void) :m_pWebView(NULL)
	{
	}

	SWkeWebkit::~SWkeWebkit(void)
	{
	}

	void SWkeWebkit::OnPaint(IRenderTarget *pRT)
	{
		CRect rcClip;
		pRT->GetClipBox(&rcClip);
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcInvalid;
		rcInvalid.IntersectRect(&rcClip, &rcClient);
		HDC hdc = pRT->GetDC();
		if (GetStyle().m_byAlpha != 0xff)
		{
			BLENDFUNCTION bf = { AC_SRC_OVER,0,GetStyle().m_byAlpha,AC_SRC_ALPHA };
			AlphaBlend(hdc, rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height(), wkeGetViewDC((wkeWebView)m_pWebView), rcInvalid.left - rcClient.left, rcInvalid.top - rcClient.top, rcInvalid.Width(), rcInvalid.Height(), bf);
		}
		else
		{
			BitBlt(hdc, rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height(), wkeGetViewDC((wkeWebView)m_pWebView), rcInvalid.left - rcClient.left, rcInvalid.top - rcClient.top, SRCCOPY);
		}
		pRT->ReleaseDC(hdc);
	}

	void SWkeWebkit::OnSize(UINT nType, CSize size)
	{
		__super::OnSize(nType, size);
		wkeResize(m_pWebView, size.cx, size.cy);
	}

	void wkePaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdcSrc, int x, int y, int cx, int cy)
	{
		SWkeWebkit *_THIS = (SWkeWebkit*)param;
		CRect rcClient;
		_THIS->GetClientRect(&rcClient);
		CRect rcInvalid(CPoint(x, y), CSize(cx, cy));
		rcInvalid.OffsetRect(rcClient.TopLeft());
		_THIS->InvalidateRect(rcInvalid);
	}

	void wkeURLChangedCallback(wkeWebView webView, void* param, const wkeString url)
	{
		SStringT strUrl = wkeToStringW(url);
		int abc;
	}


	int SWkeWebkit::OnCreate(void *)
	{
		m_pWebView = wkeCreateWebView();
		wkeSetHandle(m_pWebView, this->GetContainer()->GetHostHwnd());
		if (!m_pWebView) return 1;
		wkeSetNavigationToNewWindowEnable((wkeWebView)m_pWebView, false);
		wkeOnPaintUpdated((wkeWebView)m_pWebView, wkePaintUpdatedCallback, this);
		wkeOnURLChanged(m_pWebView, wkeURLChangedCallback, NULL);
		wkeLoadURL(m_pWebView, S_CT2A(m_strUrl, CP_UTF8));
		return 0;
	}

	void SWkeWebkit::OnDestroy()
	{
		if (m_pWebView)
		{
			wkeSetHandle(m_pWebView, nullptr);
			wkeDestroyWebView((wkeWebView)m_pWebView);
		}
	}
	LRESULT SWkeWebkit::OnMouseEvent(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
		{
			SetFocus();
			SetCapture();
		}
		else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
		{
			ReleaseCapture();
		}

		CRect rcClient;
		GetClientRect(&rcClient);

		int x = GET_X_LPARAM(lParam) - rcClient.left;
		int y = GET_Y_LPARAM(lParam) - rcClient.top;

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		SetMsgHandled(wkeFireMouseEvent(m_pWebView, message, x, y, flags));
		return 0;
	}

	LRESULT SWkeWebkit::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		if (wParam == VK_F12)
		{
			wkeSetDebugConfig(m_pWebView, "showDevTools", u8"E:\\doc\\Visual Studio 2017\\Projects\\SouiAndMb\\Debug\\front_end\\inspector.html");
		}
		SetMsgHandled(wkeFireKeyDownEvent(m_pWebView, wParam, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(wkeFireKeyUpEvent(m_pWebView, wParam, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		CRect rc;
		GetWindowRect(&rc);
		pt.x -= rc.left;
		pt.y -= rc.top;

		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;

		SetMsgHandled(wkeFireMouseWheelEvent(m_pWebView, pt.x, pt.y, delta, flags));

		return 0;
	}

	LRESULT SWkeWebkit::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		unsigned int charCode = wParam;
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		SetMsgHandled(wkeFireKeyPressEvent(m_pWebView, charCode, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnImeStartComposition(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		wkeRect caret = wkeGetCaret(m_pWebView);

		CRect rcClient;
		GetClientRect(&rcClient);

		CANDIDATEFORM form;
		form.dwIndex = 0;
		form.dwStyle = CFS_EXCLUDE;
		form.ptCurrentPos.x = caret.x + rcClient.left;
		form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
		form.rcArea.top = caret.y + rcClient.top;
		form.rcArea.bottom = caret.y + caret.h + rcClient.top;
		form.rcArea.left = caret.x + rcClient.left;
		form.rcArea.right = caret.x + caret.w + rcClient.left;
		COMPOSITIONFORM compForm;
		compForm.ptCurrentPos = form.ptCurrentPos;
		compForm.rcArea = form.rcArea;
		compForm.dwStyle = CFS_POINT;

		HWND hWnd = GetContainer()->GetHostHwnd();
		HIMC hIMC = ImmGetContext(hWnd);
		ImmSetCandidateWindow(hIMC, &form);
		ImmSetCompositionWindow(hIMC, &compForm);
		ImmReleaseContext(hWnd, hIMC);
		return 0;
	}

	void SWkeWebkit::OnSetFocus(SWND wndOld)
	{
		__super::OnSetCursor(wndOld);
		wkeSetFocus(m_pWebView);
	}

	void SWkeWebkit::OnKillFocus(SWND wndFocus)
	{
		wkeKillFocus(m_pWebView);
		__super::OnKillFocus(wndFocus);
	}

	BOOL SWkeWebkit::OnSetCursor(const CPoint &pt)
	{
		int curInf = wkeGetCursorInfoType((wkeWebView)m_pWebView);

		HCURSOR hCursor = GETRESPROVIDER->LoadCursor(wkeCursor(curInf));
		SetCursor(hCursor);
		return TRUE;
	}

#define	WkeCursorInfoMiddlePanning			20
#define	WkeCursorInfoEastPanning			21
#define	WkeCursorInfoNorthPanning			22
#define	WkeCursorInfoNorthEastPanning		23
#define	WkeCursorInfoNorthWestPanning		24	
#define	WkeCursorInfoSouthPanning			25
#define	WkeCursorInfoSouthEastPanning		26
#define	WkeCursorInfoSouthWestPanning		27
#define	WkeCursorInfoWestPanning			28
#define	WkeCursorInfoMove					29
#define	WkeCursorInfoVerticalText			30
#define	WkeCursorInfoCell					31
#define	WkeCursorInfoContextMenu			32
#define	WkeCursorInfoAlias					33
#define	WkeCursorInfoProgress				34
#define	WkeCursorInfoNoDrop					35
#define	WkeCursorInfoCopy					36
#define	WkeCursorInfoNone					37
#define	WkeCursorInfoNotAllowed				38
#define	WkeCursorInfoZoomIn					39
#define	WkeCursorInfoZoomOut				40
#define	WkeCursorInfoGrab					41
#define	WkeCursorInfoGrabbing				42
#define	TypeCustom							43

	LPCTSTR SWkeWebkit::wkeCursor(int wekInfId)
	{
		/*
		TypePointer,0
		TypeCross,1
		TypeHand,2
		TypeIBeam,3
		TypeWait,4
		TypeHelp,5
		TypeEastResize,6
		TypeNorthResize,7
		TypeNorthEastResize,8
		TypeNorthWestResize,9
		TypeSouthResize,10
		TypeSouthEastResize,11
		TypeSouthWestResize,12
		TypeWestResize,13
		TypeNorthSouthResize,14
		TypeEastWestResize,15
		TypeNorthEastSouthWestResize,16
		TypeNorthWestSouthEastResize,17
		TypeColumnResize,18
		TypeRowResize,19
		TypeMiddlePanning,20
		TypeEastPanning,
		TypeNorthPanning,
		TypeNorthEastPanning,
		TypeNorthWestPanning,
		TypeSouthPanning,
		TypeSouthEastPanning,
		TypeSouthWestPanning,
		TypeWestPanning,
		TypeMove,
		TypeVerticalText,
		TypeCell,
		TypeContextMenu,
		TypeAlias,
		TypeProgress,
		TypeNoDrop,
		TypeCopy,
		TypeNone,
		TypeNotAllowed,
		TypeZoomIn,
		TypeZoomOut,
		TypeGrab,
		TypeGrabbing,
		TypeCustom
		*/
		LPCTSTR curResStr = NULL;
		switch (wekInfId)
		{
		case WkeCursorInfoPointer:
			curResStr = IDC_ARROW; break;
		case WkeCursorInfoCross:
			curResStr = IDC_CROSS; break;
		case WkeCursorInfoHand:
			curResStr = IDC_HAND; break;
		case WkeCursorInfoIBeam:
			curResStr = IDC_IBEAM; break;
		case WkeCursorInfoWait:
			curResStr = IDC_WAIT; break;
		case WkeCursorInfoHelp:
			curResStr = IDC_HELP; break;
		case WkeCursorInfoEastResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoNorthResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoNorthEastResize:
			curResStr = IDC_SIZENESW; break;
		case WkeCursorInfoNorthWestResize:
			curResStr = IDC_SIZENWSE; break;
		case WkeCursorInfoSouthResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoSouthEastResize:
			curResStr = IDC_SIZENWSE; break;
		case WkeCursorInfoSouthWestResize:
			curResStr = IDC_SIZENESW; break;
		case WkeCursorInfoWestResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoNorthSouthResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoEastWestResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoNorthEastSouthWestResize:
			curResStr = IDC_SIZENESW; break;
		case WkeCursorInfoNorthWestSouthEastResize:
			curResStr = IDC_SIZENWSE; break;
		case WkeCursorInfoColumnResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoRowResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoMove:
			curResStr = IDC_SIZEALL; break;
		default:curResStr = IDC_ARROW; break;
		}
		return curResStr;
	}

	BOOL SWkeWebkit::OnAttrUrl(SStringW strValue, BOOL bLoading)
	{
		m_strUrl = strValue;
		if (!bLoading&&m_pWebView)
			wkeLoadURL(m_pWebView, S_CT2A(m_strUrl, CP_UTF8));
		return !bLoading;
	}
}

