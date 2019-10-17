#include <windows.h>
#include <commctrl.h>
#include "Editor.h"
#include "KeyboardBox.h"
#include "MatrixBox.h"
#include "PosBarExBox.h"
#include "../Sequencer.h"

#define IDC_EDITOR_COMBO  153

#define IDC_HSCROLL    200
#define IDC_VSCROLL    210
#define IDC_HZOOMUP    201
#define IDC_HZOOMDOWN  202

extern CSequencer g_cSeq;
extern HINSTANCE g_hInst;
extern char g_szWndTitle[];

const int ctWidthBtn = 16;
const int ctHeightScroll = 16;

void    Editor_UpdateScrollBars(HWND);
TEditorData * MakeEditorData(TPattern*);
LRESULT Editor_MsgCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgHScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgVScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgMdiActivate(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgSize(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT Editor_MsgUser(HWND);

/***************************************************************
* InitInstance:
*
***************************************************************/

WORD Editor_InitInstance()
{
    WNDCLASS wcl;    
    
    KeyboardBox_InitControl(g_hInst);
    MatrixBox_InitControl(g_hInst);
    PosBarExBox_InitControl(g_hInst);
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = g_hInst;
    wcl.lpszClassName = "MdiEditorClass";
    wcl.lpfnWndProc = Editor_WndProc;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        
    return RegisterClass(&wcl);
}

/***************************************************************
* Create: création de fenêtre éditeur
*
***************************************************************/

HWND Editor_Create(HWND hwndParent)
{
    char szText[140];
    TEditorData * pData;
    TPattern * p = g_cSeq.GetPatternSelected();
    
    if(!p)
    {
        MessageBox(hwndParent, " Séléctionner une séquence avant d'ouvrir la   \n"
        " fenêtre éditeur . ", g_szWndTitle, MB_ICONINFORMATION);
        return NULL;
    }    
    
    if(p->id!=0)
    {
        SendMessage(hwndParent, WM_MDIACTIVATE, (WPARAM)GetDlgItem(hwndParent, p->id),0L);
        return GetDlgItem(hwndParent, p->id);
    }    
    pData = MakeEditorData(p);
    if(!pData)
        return NULL;
    
    wsprintf(szText, "Editeur: %s", p->szTitle);
    return CreateMDIWindow("MdiEditorClass",szText,
        MDIS_ALLCHILDSTYLES|(g_cSeq.IsMaximized() ? WS_MAXIMIZE : 0),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, g_hInst,(LPARAM)pData);
}    


/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK Editor_WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{        
    switch(message)
    {
        case WM_CREATE:
            return Editor_MsgCreate(hwnd,message,wParam,lParam);
        case WM_HSCROLL:
            return Editor_MsgHScroll(hwnd,message,wParam,lParam);
        case WM_VSCROLL:
            return Editor_MsgVScroll(hwnd,message,wParam,lParam);
        case WM_COMMAND:
            return Editor_MsgCommand(hwnd,message,wParam,lParam);
        case WM_SIZE:
            return Editor_MsgSize(hwnd,message,wParam,lParam);
        case WM_USER:
            return Editor_MsgUser(hwnd);
        case WM_MDIACTIVATE:
            return Editor_MsgMdiActivate(hwnd,message,wParam,lParam);
        case WM_CLOSE:
            return Editor_MsgClose(hwnd,message,wParam,lParam);
        case WM_DESTROY:
            return Editor_MsgDestroy(hwnd,message,wParam,lParam);
        default:
            return DefMDIChildProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/*****************************************************************
* MsgCreate : traitement de message WM_CREATE
*
*****************************************************************/

LRESULT Editor_MsgCreate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    char szText[8];
    LPCREATESTRUCT lpcs;
    LPMDICREATESTRUCT lpmcs;
    lpcs = (LPCREATESTRUCT)lParam;
    lpmcs = (LPMDICREATESTRUCT)(lpcs->lpCreateParams);
    SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpmcs->lParam);
    
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    p->pPtrn->id = (int)GetWindowLong(hwnd, GWL_ID);
    
    CreateWindowEx(0,KEYBOARDBOX_CLASS,"",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0,
            hwnd,(HMENU)150, g_hInst, p);
    
    CreateWindowEx(0,MATRIXBOX_CLASS,"",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0,
            hwnd,(HMENU)151, g_hInst, p);
    
    CreateWindowEx(0,POSBAREXBOX_CLASS,"",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0,
            hwnd,(HMENU)152, g_hInst, p);
    
    HWND hBuffer;
    hBuffer = CreateWindowEx(0,"Combobox","",
            WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
            60, 5, 50, 130,
            hwnd, (HMENU)IDC_EDITOR_COMBO, g_hInst, NULL);
    SendMessage(hBuffer, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), 1L);
    SendMessage(hBuffer, CB_RESETCONTENT, 0, 0);
    for(int i=0; i<7; i++)
    {
        wsprintf(szText, " %i", 1<<i);
        SendMessage(hBuffer, CB_ADDSTRING, 0, (LPARAM)szText);
    }
    SendMessage(hBuffer, CB_SETCURSEL, (WPARAM)g_cSeq.GetResolution(), 0);
    
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

LRESULT Editor_MsgCommand(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    int oldTick = p->cxTick;
    switch(LOWORD(wParam))
    {
        case IDC_HZOOMUP:
            SetFocus(GetDlgItem(hwnd, 151));
            p->cxTick = min(200, p->cxTick + 10);
            p->xo = (p->cxTick * p->xo)/oldTick;
            InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, 152), NULL, TRUE);
            Editor_UpdateScrollBars(hwnd);
            break;
        case IDC_HZOOMDOWN:
            SetFocus(GetDlgItem(hwnd, 151));
            p->cxTick = max(10, p->cxTick - 10);
            p->xo = (p->cxTick * p->xo)/oldTick;
            InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
            InvalidateRect(GetDlgItem(hwnd, 152), NULL, TRUE);
            Editor_UpdateScrollBars(hwnd);
            break;
        case IDC_EDITOR_COMBO:
            if(HIWORD(wParam)==CBN_SELCHANGE)
                g_cSeq.SetResolution(SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
    }    
    return 0;
}    

/******************************************************************
* 
*
******************************************************************/

LRESULT Editor_MsgMdiActivate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(hwnd ==(HWND)lParam)
        SetFocus(GetDlgItem(hwnd, 151));
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT Editor_MsgUser(HWND hwnd)
{
    InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, 152), NULL, TRUE);
    Editor_UpdateScrollBars(hwnd);
    return 0;
}    


/******************************************************************
* 
*
******************************************************************/

LRESULT Editor_MsgSize(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int cx, cy;
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    if(wParam == SIZE_MAXIMIZED)
        g_cSeq.SetMaximized(TRUE);
    else if((wParam == SIZE_RESTORED)&&(!p->bClose))
        g_cSeq.SetMaximized(FALSE);
    
    cx = LOWORD(lParam);
    cy = HIWORD(lParam);
    
    MoveWindow(GetDlgItem(hwnd, 150), 50, 35, 65, cy - 35 - ctHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, 151), 115, 35, cx - 115 - ctHeightScroll, 
        cy - 35 - ctHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, 152), 115, 0, cx - 115 - ctHeightScroll, 35, TRUE);
    
    Editor_UpdateScrollBars(hwnd);
    // scroll bar horizontale et les 2 boutons
    
    MoveWindow(GetDlgItem(hwnd, IDC_HSCROLL), 0, cy - ctHeightScroll, 
        cx - 2*ctWidthBtn - ctHeightScroll, ctHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_HZOOMDOWN), cx - 2*ctWidthBtn - ctHeightScroll, 
        cy - ctHeightScroll, ctWidthBtn, ctHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_HZOOMUP), cx - ctWidthBtn - ctHeightScroll, 
        cy - ctHeightScroll, ctWidthBtn, ctHeightScroll, TRUE);
    
    // scroll bar verticale et les 2 boutons
    
    MoveWindow(GetDlgItem(hwnd, IDC_VSCROLL), cx - ctHeightScroll, 0, ctHeightScroll, 
        cy - ctHeightScroll, TRUE);
    
    Editor_UpdateScrollBars(hwnd);
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/************************************************************************
* 
*
************************************************************************/

LRESULT Editor_MsgHScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
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
    p->xo = - x - (p->cxTick*p->pPtrn->left)/(int)g_cSeq.GetTimeTick();
    InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, 152), NULL, TRUE);
    return 0;
}

/************************************************************************
* 
*
************************************************************************/

LRESULT Editor_MsgVScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
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
    InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, 150), NULL, TRUE);
    return 0;
}

/******************************************************************
* 
*
******************************************************************/

LRESULT Editor_MsgClose(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(p)
        p->bClose = TRUE;
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT Editor_MsgDestroy(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(p)
    {
        g_cSeq.UpdateIDPattern(p->pPtrn->id);
        p->pPtrn->id = 0;
        delete p;
    }    
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/****************************************************************************
* MakeTrackData : création d'une donnée associé à chaque controle créé
*
****************************************************************************/

TEditorData * MakeEditorData(TPattern * pSel)
{
    TEditorData * p;
    
    p = new TEditorData;
    if(!p)
        return NULL;
    
    ZeroMemory(p, sizeof(TEditorData));
    p->cxTick = 100;
    p->pPtrn = pSel;
    p->yo = -400;
    p->xo = -(p->cxTick*pSel->left)/(int)g_cSeq.GetTimeTick();
    
    return p;
}

/******************************************************************
* 
*
******************************************************************/

void Editor_UpdateScrollBars(HWND hwnd)
{
    RECT rc;
    int cx, cy;
    int cxModel;
    int cyModel = 1080;
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    cxModel = 10 + (p->cxTick*(p->pPtrn->right - p->pPtrn->left))/(int)g_cSeq.GetTimeTick() +
        p->cxTick * 4;
        
    GetClientRect(GetDlgItem(hwnd, 151), &rc);
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
        p->xo = -(p->cxTick*p->pPtrn->left)/(int)g_cSeq.GetTimeTick();
        InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
        InvalidateRect(GetDlgItem(hwnd, 152), NULL, TRUE);
    }
    else
    {
        SetScrollPos(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, 
        -(p->cxTick*p->pPtrn->left)/(int)g_cSeq.GetTimeTick()- p->xo ,TRUE);
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
        InvalidateRect(GetDlgItem(hwnd, 150), NULL, FALSE);
        InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
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
