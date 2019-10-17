#include <windows.h>
#include "Arrange.h"
#include "PosBarBox.h"
#include "ArrEditorBox.h"
#include "../Sequencer.h"
#include "../MidiSphinx.h"
#include "../Resource.h"

#define IDC_HSCROLL    200
#define IDC_HZOOMHIGH  201
#define IDC_HZOOMLOW   202
#define IDC_VSCROLL    210
#define IDC_VZOOMLOW   211
#define IDC_VZOOMHIGH  212

#define IDC_POINTER    220
#define IDC_CUT        221
#define IDC_PASTE      222
#define IDC_ERASE      223
#define IDC_RENAME     224

static BOOL bPointer = FALSE;

extern CSequencer g_cSeq;
extern CMidiSphinx g_midi;
extern HINSTANCE g_hInst;
extern HWND hwndArrange;
extern HWND g_hMain;
extern HBITMAP g_hbmTool2;

const int cstHeightScroll = 16; // hauteur scroll bar
const int cstWidthBtn = 16; // largeur des 2 boutons associés au scroll bar
const RECT rcTool[5] = {{5,27,32,54},{5,54,32,81},{5,81,32,108},{5,108,32,135},{5,135,32,162}};

static CWinColumnTrack winColumnTrack;

void    Arrange_UpdateScrollBars(HWND);
TArrangeData * MakeArrangeData();
LRESULT Arrange_MsgCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgHScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgVScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgMdiActivate(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgLButtonDown(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgSize(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT Arrange_MsgPaint(HWND);
LRESULT Arrange_MsgUser(HWND);

/***************************************************************
* InitInstance:
*
***************************************************************/

WORD Arrange_InitInstance()
{
    WNDCLASSEX wincl;
    
    winColumnTrack.InitInstance();
    PosBarBox_InitControl(g_hInst);
    ArrEditorBox_InitControl(g_hInst);
    
    wincl.hInstance = g_hInst;
    wincl.lpszClassName = "ArrangementWndClass";
    wincl.lpfnWndProc = Arrange_WndProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);    
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;    
    wincl.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    
    return RegisterClassEx(&wincl);
}

/***************************************************************
* Create: création de fenêtre éditeur
*
***************************************************************/

HWND Arrange_Create(HWND hwndParent)
{
    if(hwndArrange)
    {
        SendMessage(hwndParent, WM_MDIACTIVATE, (WPARAM)hwndArrange, 0L);
        return hwndArrange;
    }
    
    TArrangeData * pData;
    
    pData = MakeArrangeData();
    if(!pData)
        return NULL;
    
    hwndArrange = CreateMDIWindow("ArrangementWndClass","Arrangement",
        MDIS_ALLCHILDSTYLES|(g_cSeq.IsMaximized() ? WS_MAXIMIZE : 0),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, g_hInst,(LPARAM)pData);
    
    if(!hwndArrange)
    {
        delete pData->pcolTrack;
        delete pData;
        return NULL;
    }
    ShowWindow(hwndArrange,  (g_cSeq.IsMaximized() ? SW_MAXIMIZE : SW_NORMAL));
    UpdateWindow(hwndArrange);   
    return hwndArrange;
}    


/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK Arrange_WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{        
    switch(message)
    {
        case WM_CREATE:
            return Arrange_MsgCreate(hwnd,message,wParam,lParam);
        case WM_HSCROLL:
            return Arrange_MsgHScroll(hwnd,message,wParam,lParam);
        case WM_VSCROLL:
            return Arrange_MsgVScroll(hwnd,message,wParam,lParam);
        case WM_COMMAND:
            return Arrange_MsgCommand(hwnd,message,wParam,lParam);
        case WM_LBUTTONDOWN:
            return Arrange_MsgLButtonDown(hwnd,message,wParam,lParam);
        case WM_PAINT:
            return Arrange_MsgPaint(hwnd);
        case WM_SIZE:
            return Arrange_MsgSize(hwnd,message,wParam,lParam);
        case WM_USER:
            return Arrange_MsgUser(hwnd);
        case WM_MDIACTIVATE:
            return Arrange_MsgMdiActivate(hwnd,message,wParam,lParam);
        case WM_CLOSE:
            return Arrange_MsgClose(hwnd,message,wParam,lParam);
        case WM_DESTROY:
            return Arrange_MsgDestroy(hwnd,message,wParam,lParam);
        default:
            return DefMDIChildProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/*****************************************************************
* MsgCreate : traitement de message WM_CREATE
*
*****************************************************************/

LRESULT Arrange_MsgCreate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    LPCREATESTRUCT lpcs;
    LPMDICREATESTRUCT lpmcs;
    lpcs = (LPCREATESTRUCT)lParam;
    lpmcs = (LPMDICREATESTRUCT)(lpcs->lpCreateParams);
    SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpmcs->lParam);
    
    TArrangeData * p = (TArrangeData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    // création fenetre editeur
    
    CreateWindowEx(0,ARREDITORBOX_CLASS,"", WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0, hwnd,(HMENU)198, g_hInst, p);
    
    // création fenetre barre de position
    
    CreateWindowEx(0,POSBARBOX_CLASS,"", WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0, hwnd,(HMENU)199, g_hInst,NULL);
    
    // création scroll bar horizontale et ses 2 boutons
    
    CreateWindowEx(0, "ScrollBar", "", WS_CHILD|WS_VISIBLE|SBS_HORZ,
        0, 0, 0, 0, hwnd, (HMENU)200, g_hInst, NULL);
    CreateWindowEx(0, "Button", "-", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)201, g_hInst, NULL);
    CreateWindowEx(0, "Button", "+", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)202, g_hInst, NULL);
    
    // création scroll bar verticale et ses 2 boutons
    
    CreateWindowEx(0, "ScrollBar", "", WS_CHILD|WS_VISIBLE|SBS_VERT,
        0, 0, 0, 0, hwnd, (HMENU)IDC_VSCROLL, g_hInst, NULL);
    CreateWindowEx(0, "Button", "-", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)IDC_VZOOMLOW, g_hInst, NULL);
    CreateWindowEx(0, "Button", "+", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)IDC_VZOOMHIGH, g_hInst, NULL);
        
    p->pcolTrack->Create(hwnd);
    
    g_midi.PrepareDefaultChannel(16);
    
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/*****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
*****************************************************************/

LRESULT Arrange_MsgCommand(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int oldBuf;
    
    switch(LOWORD(wParam))
    {
        case IDC_VZOOMLOW:
            SetFocus(GetDlgItem(hwnd, 198));
            oldBuf = g_cSeq.GetCyTrack();
            g_cSeq.MoveCyTrack(-5);
            g_cSeq.SetYOrigine((g_cSeq.GetCyTrack()*g_cSeq.GetYOrigine())/oldBuf);
            InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
            InvalidateRect(GetDlgItem(hwnd, IDC_COLUMNTRACK), NULL, TRUE);
            Arrange_UpdateScrollBars(hwnd);
            break;
        case IDC_VZOOMHIGH:
            SetFocus(GetDlgItem(hwnd, 198));
            oldBuf = g_cSeq.GetCyTrack();
            g_cSeq.MoveCyTrack(5);
            g_cSeq.SetYOrigine((g_cSeq.GetCyTrack()*g_cSeq.GetYOrigine())/oldBuf);
            InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
            InvalidateRect(GetDlgItem(hwnd, IDC_COLUMNTRACK), NULL, TRUE);
            Arrange_UpdateScrollBars(hwnd);
            break;
        case 201:
            SetFocus(GetDlgItem(hwnd, 198));
            oldBuf = g_cSeq.GetCxTick();
            g_cSeq.MoveCxTick(-1);
            g_cSeq.SetXOrigine((g_cSeq.GetCxTick()*g_cSeq.GetXOrigine())/oldBuf);
            InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
            InvalidateRect(GetDlgItem(hwnd, 199), NULL, FALSE);
            Arrange_UpdateScrollBars(hwnd);
            break;
        case 202:
            SetFocus(GetDlgItem(hwnd, 198));
            oldBuf = g_cSeq.GetCxTick();
            g_cSeq.MoveCxTick(1);
            g_cSeq.SetXOrigine((g_cSeq.GetCxTick()*g_cSeq.GetXOrigine())/oldBuf);
            InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
            InvalidateRect(GetDlgItem(hwnd, 199), NULL, FALSE);
            Arrange_UpdateScrollBars(hwnd);
            break;
    }    
    return 0;
}    

/******************************************************************
* 
*
******************************************************************/

LRESULT Arrange_MsgLButtonDown(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    RECT rc;
    BOOL b = FALSE;
    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    
    TArrangeData * p = (TArrangeData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(!p)
        return 0;
        
    for(int i=0; i<5; i++)
    {
        if(PtInRect(&rcTool[i], pt))
        {
            p->modeCursor = i;
            b = TRUE;
            break;
        }    
    }
    if(!b)
        return 0;
    
    SetRect(&rc, 3, 25, 35, 165);
    InvalidateRect(hwnd, &rc, FALSE);
    
    return 0;    
}

/******************************************************************
* 
*
******************************************************************/

LRESULT Arrange_MsgPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc, hmem;
    RECT rc;
    int i;
    
    TArrangeData * p = (TArrangeData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(!p)
        return 0;
    
    hdc = BeginPaint(hwnd, &ps);
    hmem = CreateCompatibleDC(hdc);    
    SetRect(&rc, 3, 25, 35, 165);
    FrameRect(hdc, &rc, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
    SelectObject(hmem, g_hbmTool2);    
    BitBlt(hdc, 5, 27, 28, 136, hmem, 0, 0, SRCCOPY);
    
    i = p->modeCursor;    
    if(i<5)
    {
        BitBlt(hdc, rcTool[i].left, rcTool[i].top, rcTool[i].right - rcTool[i].left, 
                rcTool[i].bottom - rcTool[i].top, hmem, 27, 27*i, SRCCOPY);
    }    
    
    DeleteDC(hmem);
    EndPaint(hwnd, &ps);
    return 0;
}


/******************************************************************
* 
*
******************************************************************/

LRESULT Arrange_MsgMdiActivate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    BOOL b;
    if(hwnd ==(HWND)lParam)
    {
        b = TRUE;
        SetFocus(GetDlgItem(hwnd, 198));
    }    
    else
        b = FALSE;
    
    EnableMenuItem(GetMenu(g_hMain), IDM_GRID, b ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(GetMenu(g_hMain), IDM_WHITEBACKGROUND, b ? MF_ENABLED : MF_GRAYED);
    DrawMenuBar(g_hMain);
    
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT Arrange_MsgUser(HWND hwnd)
{
    InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
    InvalidateRect(GetDlgItem(hwnd, 199), NULL, FALSE);
    Arrange_UpdateScrollBars(hwnd);
    return 0;
}    


/******************************************************************
* 
*
******************************************************************/

LRESULT Arrange_MsgSize(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int cx, cy;
    cx = LOWORD(lParam);
    cy = HIWORD(lParam);
    if(wParam == SIZE_MAXIMIZED)
        g_cSeq.SetMaximized(TRUE);
    else if(wParam == SIZE_RESTORED)
        g_cSeq.SetMaximized(FALSE);
        
    MoveWindow(GetDlgItem(hwnd, IDC_COLUMNTRACK), 40, 25, 240, cy - 25, TRUE);
    MoveWindow(GetDlgItem(hwnd, 198), 280, 25, cx - 280 - cstHeightScroll,
        cy - 25 - cstHeightScroll, TRUE);
    
    
    MoveWindow(GetDlgItem(hwnd, 199), 280, 0, cx - 280 - cstHeightScroll, 25, TRUE);
    
    // scroll bar horizontale et les 2 boutons
    
    MoveWindow(GetDlgItem(hwnd, 200), 280, cy - cstHeightScroll, 
        cx - 280 - 2*cstWidthBtn - cstHeightScroll, cstHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, 201), cx - 2*cstWidthBtn - cstHeightScroll, 
        cy - cstHeightScroll, cstWidthBtn, cstHeightScroll, TRUE);
    MoveWindow(GetDlgItem(hwnd, 202), cx - cstWidthBtn - cstHeightScroll, 
        cy - cstHeightScroll, cstWidthBtn, cstHeightScroll, TRUE);
    
    // scroll bar verticale et les 2 boutons
    
    MoveWindow(GetDlgItem(hwnd, IDC_VSCROLL), cx - cstHeightScroll, 0, cstHeightScroll, 
        cy - cstHeightScroll - 2*cstWidthBtn, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_VZOOMLOW), cx - cstHeightScroll, 
        cy - cstHeightScroll - 2*cstWidthBtn, cstHeightScroll, cstWidthBtn, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_VZOOMHIGH), cx - cstHeightScroll, 
        cy - cstHeightScroll - cstWidthBtn, cstHeightScroll, cstWidthBtn, TRUE);    
    
    Arrange_UpdateScrollBars(hwnd);
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/************************************************************************
* 
*
************************************************************************/

LRESULT Arrange_MsgHScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, 200), SB_CTL, &info);
    
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
    SetScrollPos(GetDlgItem(hwnd, 200), SB_CTL, x ,TRUE);
    g_cSeq.SetXOrigine(-x);
    InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
    InvalidateRect(GetDlgItem(hwnd, 199), NULL, TRUE);
    return 0;
}

/************************************************************************
* 
*
************************************************************************/

LRESULT Arrange_MsgVScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info);
    
    int y;
    y = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINEDOWN: y += 10; break;
        case SB_LINEUP: y -= 10; break;
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
    g_cSeq.SetYOrigine(-y);
    InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLUMNTRACK), NULL, TRUE);
    return 0;
}

/*****************************************************************
* MsgClose : traitement message WM_CLOSE
*
*****************************************************************/

LRESULT Arrange_MsgClose(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TArrangeData * p = (TArrangeData*)GetWindowLong(hwnd, GWL_USERDATA);
    // on recupère l'id de la fenetre arrangement
    p->idRef = (int)GetWindowLong(hwnd, GWL_ID);
    return DefMDIChildProc(hwnd,message,wParam,lParam);
}


/******************************************************************
* 
*
******************************************************************/

LRESULT Arrange_MsgDestroy(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TArrangeData * p = (TArrangeData*)GetWindowLong(hwnd, GWL_USERDATA);
    // on met à jour tous les identifiants des fenetres de sequences
    g_cSeq.UpdateIDPattern(p->idRef);
    // n'oublie pas de mettre le handle en NULL
    hwndArrange = NULL;
    delete p->pcolTrack;
    delete p;
    return DefMDIChildProc(hwnd,message,wParam,lParam);
}

/****************************************************************************
* MakeArrangeData : création d'une donnée associé à la fenetre arrangement
*
****************************************************************************/

TArrangeData * MakeArrangeData()
{
    TArrangeData * p;
    
    p = new TArrangeData;
    if(!p)
        return NULL;
    
    ZeroMemory(p, sizeof(TArrangeData));
    
    p->pcolTrack = new CWinColumnTrack;
    if(!p->pcolTrack)
    {
        delete p;
        return NULL;
    }    
    
    return p;
}

/******************************************************************
* 
*
******************************************************************/

void Arrange_UpdateScrollBars(HWND hwnd)
{
    RECT rc;
    int cx, cy;
    int cxModel, cyModel;
    
    GetClientRect(GetDlgItem(hwnd, 198), &rc);
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
    
    cxModel = 10 + 40 + (g_cSeq.GetLastLocator()*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick();
    cyModel = 50 + g_cSeq.GetCyTrack()*g_cSeq.GetTrackCount();
    
    SCROLLINFO info;
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    
    GetScrollInfo(GetDlgItem(hwnd, 200), SB_CTL, &info);
    if(cx>=cxModel)
    {
        cx = cxModel;        
        SetScrollPos(GetDlgItem(hwnd, 200), SB_CTL, 0 ,TRUE);
        g_cSeq.SetXOrigine(0);
        InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
        InvalidateRect(GetDlgItem(hwnd, 199), NULL, FALSE);
    }
    else
    {
        SetScrollPos(GetDlgItem(hwnd, 200), SB_CTL, -g_cSeq.GetXOrigine() ,TRUE);
    }    
    
    info.nMin = 0;
    info.nMax = cxModel-1;
    info.nPage = cx;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(GetDlgItem(hwnd, 200), SB_CTL, &info, TRUE);
    
    info.fMask = SIF_ALL;
    
    GetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info);
    if(cy>=cyModel)
    {
        cy = cyModel;        
        SetScrollPos(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, 0 ,TRUE);
        g_cSeq.SetYOrigine(0);
        InvalidateRect(GetDlgItem(hwnd, 198), NULL, FALSE);
        InvalidateRect(GetDlgItem(hwnd, IDC_COLUMNTRACK), NULL, TRUE);
    }
    else
    {
        SetScrollPos(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, -g_cSeq.GetYOrigine() ,TRUE);
    }    
    
    info.nMin = 0;
    info.nMax = cyModel-1;
    info.nPage = cy;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info, TRUE);
}
