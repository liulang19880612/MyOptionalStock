#pragma once

#include "core/SimpleWnd.h"
#include "helper/MemDC.h"

class CDragView : public CSimpleWnd
{
public:
	CDragView(void);
    ~CDragView(void);

    static BOOL BeginDrag(HBITMAP hBmp,POINT ptHot ,COLORREF crKey, BYTE byAlpha,DWORD dwFlags);
    static void DragMove(POINT pt);
    static void EndDrag();
protected:

    void OnPaint(HDC dc);

    BEGIN_MSG_MAP_EX(CDragView)
        MSG_WM_PAINT(OnPaint)
    END_MSG_MAP()

    SOUI::CPoint m_ptHot;
    HBITMAP m_bmp;

    static CDragView * s_pCurDragWnd;
};