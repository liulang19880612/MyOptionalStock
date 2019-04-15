#include "stdafx.h"
#include "souistd.h"
#include "DragView.h"


CDragView * CDragView:: s_pCurDragWnd=NULL;

CDragView::CDragView(void):m_bmp(NULL)
{
}

CDragView::~CDragView(void)
{
}

void CDragView::OnPaint( HDC dc )
{
    PAINTSTRUCT ps;
    dc = ::BeginPaint(m_hWnd,&ps);
    if(m_bmp)
    {
        SOUI::CRect rc;
        GetClientRect(rc);
        HDC hMemDC = CreateCompatibleDC(dc);
        HGDIOBJ hOldBmp=SelectObject(hMemDC,m_bmp);
        BitBlt(dc,0,0,rc.Width(),rc.Height(),hMemDC,0,0,SRCCOPY);
        ::SelectObject(hMemDC,hOldBmp);
        ::DeleteDC(hMemDC);
    }
    ::EndPaint(m_hWnd,&ps);
}

BOOL CDragView::BeginDrag( HBITMAP hBmp,POINT ptHot ,COLORREF crKey, BYTE byAlpha,DWORD dwFlags)
{
    if(s_pCurDragWnd) return FALSE;
    s_pCurDragWnd=new CDragView;
    BITMAP bm;
    GetObject(hBmp,sizeof(bm),&bm);

    if(!s_pCurDragWnd->Create(NULL,WS_POPUP,WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,0,0,bm.bmWidth,bm.bmHeight,0,NULL))
    {
        delete s_pCurDragWnd;
        s_pCurDragWnd=NULL;
        return FALSE;
    }
    
    s_pCurDragWnd->ModifyStyleEx(0,WS_EX_LAYERED);

    if(bm.bmBitsPixel==32)
    {
        HDC dc=s_pCurDragWnd->GetDC();
        CMemDC memdc(dc,hBmp);
        BLENDFUNCTION bf={AC_SRC_OVER,0,byAlpha,AC_SRC_ALPHA};
		SOUI::CPoint pt(0, 0);
		CSize sz(bm.bmWidth, bm.bmHeight);
		SOUI::CPoint pt1(0, 0);
        s_pCurDragWnd->UpdateLayeredWindow(dc,&pt,&sz,memdc,&pt1,crKey,&bf,LWA_ALPHA);
        s_pCurDragWnd->ReleaseDC(dc);
    }else
    {
        s_pCurDragWnd->SetLayeredWindowAttributes(crKey,byAlpha,dwFlags);
        s_pCurDragWnd->m_bmp=hBmp;
        s_pCurDragWnd->Invalidate();
    }
    s_pCurDragWnd->m_ptHot=ptHot;
    return TRUE;
}


void CDragView::DragMove( POINT pt )
{
    SASSERT(s_pCurDragWnd);
    s_pCurDragWnd->SetWindowPos(HWND_TOPMOST,pt.x-s_pCurDragWnd->m_ptHot.x,pt.y-s_pCurDragWnd->m_ptHot.y,0,0,SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOOWNERZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
}

void CDragView::EndDrag()
{
    SASSERT(s_pCurDragWnd);
    s_pCurDragWnd->DestroyWindow();
    delete s_pCurDragWnd;
    s_pCurDragWnd=NULL;
}