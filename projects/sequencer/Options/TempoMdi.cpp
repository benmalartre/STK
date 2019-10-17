#include <windows.h>
#include <commctrl.h>
#include "TempoMdi.h"
#include "TempoPosBar.h"
#include "TempoEditor.h"
#include "../Sequencer.h"

#define IDC_HSCROLL    200
#define IDC_VSCROLL    210
#define IDC_HZOOMUP    201
#define IDC_HZOOMDOWN  202

#define IDC_TEMPOPOSBAR  152
#define IDC_TEMPOEDITOR  151

extern CSequencer g_cSeq;
extern HINSTANCE g_hInst;
extern HWND g_hWndTempo;
extern char g_szWndTitle[];

const int cttWidthBtn = 16;
const int cttHeightScroll = 16;

void    TempoMdi_UpdateScrollBars(HWND);
TTempoMdiData * MakeTempoMdiData();
LRESULT TempoMdi_MsgCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgHScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgVScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgMdiActivate(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgSize(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT TempoMdi_MsgUser(HWND);

/***************************************************************
* InitInstance:
*
***************************************************************/

WORD TempoMdi_InitInstance()
{
    WNDCLASS wcl;
    TempoPosBarBox_InitControl(g_hInst);
    TempoEditor_InitControl(g_hInst);
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = g_hInst;
    wcl.lpszClassName = "MdiTempoWndClass";
    wcl.lpfnWndProc = TempoMdi_WndProc;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        
    return RegisterClass(&wcl);
}

/***************************************************************
* Create: création de fenêtre éditeur
*
***************************************************************/

HWND TempoMdi_Create(HWND hwndParent)
{
    if(g_hWndTempo)
    {
        SendMessage(hwndParent, WM_MDIACTIVATE, (WPARAM)g_hWndTempo, 0L);
        return g_hWndTempo;
    }
    TTempoMdiData * pData;
    pData = MakeTempoMdiData();
    if(!pData)
        return NULL;
        
    g_hWndTempo = CreateMDIWindow("MdiTempoWndClass","Editeur de tempo",
        MDIS_ALLCHILDSTYLES|(g_cSeq.IsMaximized() ? WS_MAXIMIZE : 0),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, g_hInst,(LPARAM)pData);
    return g_hWndTempo;
}    


/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK TempoMdi_WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{        
    switch(message)
    {
        case WM_CREATE:
            return TempoMdi_MsgCreate(hwnd,message,wParam,lParam);
        case WM_HSCROLL:
            return TempoMdi_MsgHScroll(hwnd,message,wParam,lParam);
        case WM_VSCROLL:
            return TempoMdi_MsgVScroll(hwnd,message,wParam,lParam);
        case WM_COMMAND:
            return TempoMdi_MsgCommand(hwnd,message,wParam,lParam);
        case WM_SIZE:
            return TempoMdi_MsgSize(hwnd,message,wParam,lParam);
        case WM_USER:
            return TempoMdi_MsgUser(hwnd);
        case WM_MDIACTIVATE:
            return TempoMdi_MsgMdiActivate(hwnd,message,wParam,lParam);
        case WM_CLOSE:
            return TempoMdi_MsgClose(hwnd,message,wParam,lParam);
        case WM_DESTROY:
            return TempoMdi_MsgDestroy(hwnd,message,wParam,lParam);
        default:
            return DefMDIChildProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/*****************************************************************
* MsgCreate : traitement de message WM_CREATE
*
*****************************************************************/

LRESULT TempoMdi_MsgCreate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    LPCREATESTRUCT lpcs;
    LPMDICREATESTRUCT lpmcs;
    lpcs = (LPCREATESTRUCT)lParam;
    lpmcs = (LPMDICREATESTRUCT)(lpcs->lpCreateParams);
    SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpmcs->lParam);
    
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
        
    CreateWindowEx(0,TEMPOPOSBARBOX_CLASS,"",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0,
            hwnd,(HMENU)IDC_TEMPOPOSBAR, g_hInst, p);
    
    CreateWindowEx(0,TEMPOEDITORBOX_CLASS,"",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0,
            hwnd,(HMENU)IDC_TEMPOEDITOR, g_hInst, p);
    
    // création scroll bar horizontale et ses 2 boutons
    
    CreateWindowEx(0, "ScrollBar", "",
        WS_CHILD|WS_VISIBLE|SBS_HORZ,
        0, 0, 0, 0, hwnd, (HMENU)IDC_HSCROLL, g_hInst, NULL);
    CreateWindowEx(0, "Button", "-",
        WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)IDC_HZOOMDOWN, g_hInst, NULL);
    CreateWindowEx(0, "Button", "+",
        WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)IDC_HZOOMUP, g_hInst, NULL);
    
    // création scroll bar verticale
    
    CreateWindowEx(0, "ScrollBar", "",
        WS_CHILD|WS_VISIBLE|SBS_VERT,
        0, 0, 0, 0, hwnd, (HMENU)IDC_VSCROLL, g_hInst, NULL);
    
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/*****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
*****************************************************************/

LRESULT TempoMdi_MsgCommand(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    int oldTick = p->cxTick;
    switch(LOWORD(wParam))
    {
        case IDC_HZOOMUP:
            SetFocus(GetDlgItem(hwnd, IDC_TEMPOEDITOR));
            p->cxTick = min(40, p->cxTick + 1);
            p->xo = (p->cxTick * p->xo)/oldTick;
            InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOPOSBAR), NULL, TRUE);
            TempoMdi_UpdateScrollBars(hwnd);
            break;
        case IDC_HZOOMDOWN:
            SetFocus(GetDlgItem(hwnd, IDC_TEMPOEDITOR));
            p->cxTick = max(1, p->cxTick - 1);
            p->xo = (p->cxTick * p->xo)/oldTick;
            InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOPOSBAR), NULL, TRUE);
            TempoMdi_UpdateScrollBars(hwnd);
            break;
    }
    return 0;
}    

/******************************************************************
* 
*
******************************************************************/

LRESULT TempoMdi_MsgMdiActivate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(hwnd ==(HWND)lParam)
        SetFocus(GetDlgItem(hwnd, IDC_TEMPOEDITOR));
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT TempoMdi_MsgUser(HWND hwnd)
{
    InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOPOSBAR), NULL, TRUE);
    TempoMdi_UpdateScrollBars(hwnd);
    return 0;
}    


/******************************************************************
* 
*
******************************************************************/

LRESULT TempoMdi_MsgSize(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int cx, cy;
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    if(wParam == SIZE_MAXIMIZED)
        g_cSeq.SetMaximized(TRUE);
    else if((wParam == SIZE_RESTORED)&&(!p->bClose))
        g_cSeq.SetMaximized(FALSE);
    
    cx = LOWORD(lParam);
    cy = HIWORD(lParam);
    
    
    MoveWindow(GetDlgItem(hwnd, IDC_TEMPOEDITOR), 0, 25, cx - cttHeightScroll, 
        cy - 25 - cttHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_TEMPOPOSBAR), 0, 0, cx - cttHeightScroll, 25, TRUE);
    
    TempoMdi_UpdateScrollBars(hwnd);
    // scroll bar horizontale et les 2 boutons
    
    MoveWindow(GetDlgItem(hwnd, IDC_HSCROLL), 0, cy - cttHeightScroll, 
        cx - 2*cttWidthBtn - cttHeightScroll, cttHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_HZOOMDOWN), cx - 2*cttWidthBtn - cttHeightScroll, 
        cy - cttHeightScroll, cttWidthBtn, cttHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_HZOOMUP), cx - cttWidthBtn - cttHeightScroll, 
        cy - cttHeightScroll, cttWidthBtn, cttHeightScroll, TRUE);
    
    // scroll bar verticale et les 2 boutons
    
    MoveWindow(GetDlgItem(hwnd, IDC_VSCROLL), cx - cttHeightScroll, 0, cttHeightScroll, 
        cy - cttHeightScroll, TRUE);
    
    TempoMdi_UpdateScrollBars(hwnd);
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/************************************************************************
* 
*
************************************************************************/

LRESULT TempoMdi_MsgHScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, &info);
    
    int x;
    x = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINERIGHT: x += 40; break;
        case SB_LINELEFT: x -= 40; break;
        case SB_PAGERIGHT: x += info.nPage; break;
        case SB_PAGELEFT: x -= info.nPage; break;
        case SB_THUMBTRACK: x = info.nTrackPos; break;
        case SB_THUMBPOSITION: x = info.nPos; break;
    }
    if(x>info.nMax)
        x = info.nMax;
    if(x<0)
        x = 0;
    SetScrollPos(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, x ,TRUE);
    p->xo = - x;
    InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOPOSBAR), NULL, TRUE);
    return 0;
}

/************************************************************************
* 
*
************************************************************************/

LRESULT TempoMdi_MsgVScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info);
    
    int y;
    y = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINEDOWN: y += 20; break;
        case SB_LINEUP: y -= 20; break;
        case SB_PAGEDOWN: y += info.nPage; break;
        case SB_PAGEUP: y -= info.nPage; break;
        case SB_THUMBTRACK: y = info.nTrackPos; break;
        case SB_THUMBPOSITION: y = info.nPos; break;
    }
    if(y>info.nMax)
        y = info.nMax;
    if(y<0)
        y = 0;
    SetScrollPos(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, y ,TRUE);
    p->yo = -y;
    InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, TRUE);
    return 0;
}

/******************************************************************
* 
*
******************************************************************/

LRESULT TempoMdi_MsgClose(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(p)
        p->bClose = TRUE;
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT TempoMdi_MsgDestroy(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(p)
        delete p;
    g_hWndTempo = NULL;
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/****************************************************************************
* MakeTrackData : création d'une donnée associé à chaque controle créé
*
****************************************************************************/

TTempoMdiData * MakeTempoMdiData()
{
    TTempoMdiData * p;
    
    p = new TTempoMdiData;
    if(!p)
        return NULL;
    
    ZeroMemory(p, sizeof(TTempoMdiData));
    p->cxTick = 10;
    
    return p;
}

/******************************************************************
* 
*
******************************************************************/

void TempoMdi_UpdateScrollBars(HWND hwnd)
{
    RECT rc;
    int cx, cy;
    int cxModel;
    int cyModel = 330;
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    cxModel = 10 + 40 + (g_cSeq.GetLastLocator()*p->cxTick)/(int)g_cSeq.GetTimeTick();
    GetClientRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), &rc);
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
    
    SCROLLINFO info;
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, &info);
    if(cx>=cxModel)
    {
        cx = cxModel;        
        SetScrollPos(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, 0 ,TRUE);
        p->xo = 0;
        InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, FALSE);
        InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOPOSBAR), NULL, TRUE);
    }
    else
    {
        SetScrollPos(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, - p->xo ,TRUE);
    }
    info.nMin = 0;
    info.nMax = cxModel-1;
    info.nPage = cx;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, &info, TRUE);
    
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info);
    if(cy>=cyModel)
    {
        cy = cyModel;        
        SetScrollPos(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, 0 ,TRUE);
        p->yo = 0;
        InvalidateRect(GetDlgItem(hwnd, IDC_TEMPOEDITOR), NULL, TRUE);
    }
    else
    {
        SetScrollPos(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, - p->yo ,TRUE);
        UpdateWindow(GetDlgItem(hwnd, IDC_VSCROLL));
    }    
    
    info.nMin = 0;
    info.nMax = cyModel-1;
    info.nPage = cy;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info, TRUE);
    
}
