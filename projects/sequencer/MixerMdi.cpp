#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include "MixerMdi.h"
#include "MidiSphinx.h"
#include "ToolTip.h"

extern CToolTip    g_toolTip;
extern CSequencer  g_cSeq;
extern CMidiSphinx g_midi;
extern HINSTANCE   g_hInst;
extern HWND        g_hWndMixer;
extern HBITMAP     g_hbmSlider;
extern HBITMAP     g_hbmKnob1;
extern HBITMAP     g_hbmKnob2;
extern HBITMAP     g_hbmLed;
extern HBITMAP     g_hbmMute;
extern HBITMAP     g_hbmPan;

struct TMixerMdiData
{
    int   xo;
    int   yo;
    int   volNumber;
    int   panNumber;
    BOOL  bClose;
    POINT oldPt;
    char  szOldInstr[MAX_CHANNEL][32];
    int   led[MAX_CHANNEL];
};    


void    MixerMdi_UpdateScrollBars(HWND);
TMixerMdiData * MakeMixerMdiData();
LRESULT MixerMdi_MsgCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgTimer(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgLButtonDown(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgLButtonUp(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgMouseMove(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgMdiActivate(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgHScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgVScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgSize(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT MixerMdi_MsgDestroy(HWND, UINT, WPARAM, LPARAM);


/***************************************************************
* InitInstance:
*
***************************************************************/

WORD MixerMdi_InitInstance()
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = g_hInst;
    wcl.lpszClassName = "MdiMixerWndClass";
    wcl.lpfnWndProc = MixerMdi_WndProc;
    wcl.style = CS_DBLCLKS|CS_HREDRAW;
    wcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
        
    return RegisterClass(&wcl);
}

/***************************************************************
* Create: création de fenêtre mixeur
*
***************************************************************/

HWND MixerMdi_Create(HWND hwndParent)
{
    if(g_hWndMixer)
    {
        SendMessage(hwndParent, WM_MDIACTIVATE, (WPARAM)g_hWndMixer, 0L);
        return g_hWndMixer;
    }
    
    TMixerMdiData * pData;
    pData = MakeMixerMdiData();
    if(!pData)
        return NULL;
    
    g_hWndMixer = CreateMDIWindow("MdiMixerWndClass","Mixeur",
        MDIS_ALLCHILDSTYLES|WS_HSCROLL|WS_VSCROLL|(g_cSeq.IsMaximized() ? WS_MAXIMIZE : 0),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, g_hInst, (LPARAM)pData);
    return g_hWndMixer;
}

/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK MixerMdi_WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{        
    switch(message)
    {
        case WM_CREATE:
            return MixerMdi_MsgCreate(hwnd,message,wParam,lParam);
        case WM_TIMER:
            return MixerMdi_MsgTimer(hwnd,message,wParam,lParam);
        case WM_LBUTTONDOWN:
            return MixerMdi_MsgLButtonDown(hwnd,message,wParam,lParam);
        case WM_LBUTTONUP:
            return MixerMdi_MsgLButtonUp(hwnd,message,wParam,lParam);
        case WM_MOUSEMOVE:
            return MixerMdi_MsgMouseMove(hwnd,message,wParam,lParam);
        case WM_PAINT:
            return MixerMdi_MsgPaint(hwnd,message,wParam,lParam);
        case WM_HSCROLL:
            return MixerMdi_MsgHScroll(hwnd,message,wParam,lParam);
        case WM_VSCROLL:
            return MixerMdi_MsgVScroll(hwnd,message,wParam,lParam);
        case WM_SIZE:
            return MixerMdi_MsgSize(hwnd,message,wParam,lParam);
        /*case WM_MDIACTIVATE:
            return MixerMdi_MsgMdiActivate(hwnd,message,wParam,lParam);*/
        case WM_CLOSE:
            return MixerMdi_MsgClose(hwnd,message,wParam,lParam);
        case WM_DESTROY:
            return MixerMdi_MsgDestroy(hwnd,message,wParam,lParam);
        default:
            return DefMDIChildProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/*************************************************************************
*
*
*************************************************************************/

LRESULT MixerMdi_MsgCreate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    LPCREATESTRUCT lpcs;
    LPMDICREATESTRUCT lpmcs;
    lpcs = (LPCREATESTRUCT)lParam;
    lpmcs = (LPMDICREATESTRUCT)(lpcs->lpCreateParams);
    SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpmcs->lParam);
    SetTimer(hwnd, 1, 10, NULL); // timer pour le led
    SetTimer(hwnd, 2, 100, NULL); // timer pour mettre à jour le nom de l'instrument
    
    TMixerMdiData * p = (TMixerMdiData*)lpmcs->lParam;
    if(p)
    {
        for(int i=0; i<g_cSeq.GetTrackCount(); i++)
            g_cSeq.GetInstrumentName(i, 0, p->szOldInstr[i]);
    }    
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/*************************************************************************
*
*
*************************************************************************/

LRESULT MixerMdi_MsgTimer(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    char szText[32];
    RECT rc;
    BOOL bRedraw = FALSE;
    static int countTimeLed[16];
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    if(wParam==1) // 1er timer
    {
    for(int i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        if(g_cSeq.m_led[i].by4[0]!=0)
        {
            countTimeLed[i] = 0;
            if(p->led[i]!=4)
            {
                p->led[i] = 4;
                bRedraw = TRUE;
            }    
        }
        else if(g_cSeq.m_led[i].by3[0]!=0)
        {
            countTimeLed[i] = 0;
            if(p->led[i]!=3)
            {
                p->led[i] = 3;
                bRedraw = TRUE;
            }    
        } 
        else if(g_cSeq.m_led[i].by2[0]!=0)
        {
            countTimeLed[i] = 0;
            if(p->led[i]!=2)
            {
                p->led[i] = 2;
                bRedraw = TRUE;
            }    
        }
        else if(g_cSeq.m_led[i].by1[0]!=0)
        {
            countTimeLed[i] = 0;
            if(p->led[i]!=1)
            {
                p->led[i] = 1;
                bRedraw = TRUE;
            }    
        }
        else if(p->led[i]!=0)
        {
            countTimeLed[i]++;
            if(countTimeLed[i]>8)
            {
                p->led[i]--;
                countTimeLed[i] = 0;
            }
            bRedraw = TRUE;
        }    
    }
    if(bRedraw)
    {
        rc.left = 10 + p->xo;
        rc.right = rc.left + 60*g_cSeq.GetTrackCount();
        rc.top = 435 + p->yo;
        rc.bottom = rc.top + 15;
        InvalidateRect(hwnd, &rc, FALSE);
    }
    }
    else if(wParam==2) // 2è timer
    {
        for(int i=0; i<g_cSeq.GetTrackCount(); i++)
        {
            g_cSeq.GetInstrumentName(i, g_midi.m_dwCurrentTime, szText);
            if(lstrcmp(szText, p->szOldInstr[i]))
            {
                lstrcpy(p->szOldInstr[i], szText);
                if(i%2)
                {
                    rc.left = 10 + 60*i + p->xo - 60;
                    rc.right = 9 + 60*(i+1) + p->xo;
                    rc.bottom = 500 + p->yo;
                    rc.top = rc.bottom - 24;
                    InvalidateRect(hwnd, &rc, TRUE);
                }
                else
                {
                    rc.left = 10 + 60*i + p->xo;
                    rc.right = 9 + 60*(i+1) + p->xo + 60;
                    rc.top = 10 + p->yo;
                    rc.bottom = rc.top + 24;
                    InvalidateRect(hwnd, &rc, TRUE);
                }    
            }   
        }
    }        
          
    return 0;
}    


/*************************************************************************
*
*
*************************************************************************/

LRESULT MixerMdi_MsgPaint(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc, hMem;
    HPEN hPen1, hPen2;
    RECT rc, rect, rc1;
    char szText[100];
    int i;
    int x0, y0, x1, y1;
    float angle;
    
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    hPen1 = CreatePen(PS_SOLID, 1, RGB(54,54,54));
    hPen2 = CreatePen(PS_SOLID, 1, RGB(194,194,194));
    GetClientRect(hwnd, &rc);       
    
    hdc = BeginPaint(hwnd, &ps);
    hMem = CreateCompatibleDC(hdc);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SetTextColor(hdc, RGB(54,54,54));
    
    for(i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        //g_cSeq.GetInstrumentName(i, 0, szText);
        if(i%2)
        {
            rect.top = 35 + p->yo;
            rect.bottom = 500 + p->yo;
            rect.left = 10 + 60*i + p->xo;
            rect.right = 9 + 60*(i+1) + p->xo;
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            
            rc1.left = rect.left - 60;
            rc1.right = rect.right;
            rc1.bottom = rect.bottom;
            rc1.top = rc1.bottom - 24;
            // affichage nom de piste
            FillRect(hdc, &rc1, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            DrawText(hdc, p->szOldInstr[i], -1, &rc1, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
            
            SelectObject(hdc, hPen1);
            // gauche
            MoveToEx(hdc, rect.left, rect.top, NULL);
            LineTo(hdc, rect.left, rc1.top);
            // bas
            MoveToEx(hdc, rect.right, rc1.bottom, NULL);
            LineTo(hdc, rc1.left, rc1.bottom);
            // morceau bas coté gauche
            LineTo(hdc, rc1.left, rc1.top);
            
            SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
            // droite
            MoveToEx(hdc, rect.right, rect.bottom, NULL);
            LineTo(hdc, rect.right, rect.top);
            // haut
            LineTo(hdc, rect.left, rect.top);
            // bas intermediaire
            MoveToEx(hdc, rc1.left, rc1.top, NULL);
            LineTo(hdc, rect.left, rc1.top);
        }
        else
        {
            rect.top = 10 + p->yo;
            rect.bottom = 475 + p->yo;
            rect.left = 10 + 60*i + p->xo;
            rect.right = 9 + 60*(i+1) + p->xo;
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            
            rc1.left = rect.left;
            rc1.right = rect.right + 60;
            rc1.top = rect.top;
            rc1.bottom = rc1.top + 24;
            // affichage nom de piste
            FillRect(hdc, &rc1, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            DrawText(hdc, p->szOldInstr[i], -1, &rc1, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
            
            SelectObject(hdc, hPen1);
            // gauche
            MoveToEx(hdc, rect.left, rect.top, NULL);
            LineTo(hdc, rect.left, rect.bottom);
            // bas
            LineTo(hdc, rect.right, rect.bottom);
            // haut intermediaire
            MoveToEx(hdc, rect.right, rc1.bottom, NULL);
            LineTo(hdc, rc1.right, rc1.bottom);
            
            SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
            // droite
            MoveToEx(hdc, rect.right, rc1.bottom, NULL);
            LineTo(hdc, rect.right, rect.bottom);
            // haut
            MoveToEx(hdc, rect.left, rc1.top, NULL);
            LineTo(hdc, rc1.right, rc1.top);
            // morceau haut coté droite 
            LineTo(hdc, rc1.right, rc1.bottom);
        }
        rect.bottom = 475 + p->yo;
        rect.top = 35 + p->yo;
        
        rc1.left = rect.left;
        rc1.right = rect.right;
        rc1.bottom = rect.bottom;
        rc1.top = rc1.bottom - 16;
        // affichage numéro piste
        itoa(i+1, szText, 10);
        DrawText(hdc, szText, -1, &rc1, DT_SINGLELINE|DT_CENTER);
        // affichage glisseur volume
        SelectObject(hMem, g_hbmSlider);
        BitBlt(hdc, rect.left + 18, rect.top + 225, 25, 159, hMem, 0, 0, SRCCOPY);
        // affichage bouton volume
        SelectObject(hMem, g_hbmKnob1);
        BitBlt(hdc, rect.left + 21, rect.top + 350 - g_cSeq.GetVolumeTrack(i), 
            20, 40, hMem, 0, 0, SRCCOPY);
        
        SelectObject(hMem, g_hbmKnob2);
        // affichage balance
        BitBlt(hdc, rect.left + 8, rect.top + 160, 44, 38, hMem, 0, 0, SRCCOPY);
        //affichage effets
        BitBlt(hdc, rect.left + 8, rect.top + 20, 44, 38, hMem, 0, 0, SRCCOPY);
        BitBlt(hdc, rect.left + 8, rect.top + 70, 44, 38, hMem, 0, 0, SRCCOPY);
        
        // affichage bouton activation Balance
        SelectObject(hMem, g_hbmPan);
        if(g_cSeq.IsPanActive(i))
            BitBlt(hdc, rect.left + 15, rect.top + 140, 30, 15, hMem, 0, 0, SRCCOPY);
        else
            BitBlt(hdc, rect.left + 15, rect.top + 140, 30, 15, hMem, 0, 15, SRCCOPY);
        
        // affichage Bouton MUTE
        SelectObject(hMem, g_hbmMute);
        if(g_cSeq.IsMuteTrack(i))
            BitBlt(hdc, rect.left + 8, rect.top + 400, 15, 15, hMem, 0, 30, SRCCOPY);
        else
            BitBlt(hdc, rect.left + 8, rect.top + 400, 15, 15, hMem, 0, 45, SRCCOPY);
        
        // affichage LED
        SelectObject(hMem, g_hbmLed);
        BitBlt(hdc, rect.left + 30, rect.top + 403, 24, 8, hMem, 0, 8*p->led[i], SRCCOPY);
        
        // marquage niveau balance
        SelectObject(hdc, (HPEN)GetStockObject(BLACK_PEN));
        x0 = rect.left + 30;
        y0 = rect.top + 179;
        angle = ((5.f*M_PI)/4.f - ((3.f*M_PI*(float)g_cSeq.GetPanTrack(i))/254.f));
        x1 = x0 + (int)(11*cos(angle));
        y1 = y0 - (int)(11*sin(angle));
        MoveToEx(hdc, x0, y0, NULL);
        LineTo(hdc, x1, y1);
        
        // marquage niveau effet1
        x0 = rect.left + 30;
        y0 = rect.top + 39;
        angle = (5.f*M_PI)/4.f ;//- ((3.f*M_PI*(float)g_cSeq.GetEffet1Track(i))/254.f));
        x1 = x0 + (int)(11*cos(angle));
        y1 = y0 - (int)(11*sin(angle));
        MoveToEx(hdc, x0, y0, NULL);
        LineTo(hdc, x1, y1);
        
        // marquage niveau effet2
        x0 = rect.left + 30;
        y0 = rect.top + 89;
        angle = (5.f*M_PI)/4.f ;//- ((3.f*M_PI*(float)g_cSeq.GetEffet1Track(i))/254.f));
        x1 = x0 + (int)(11*cos(angle));
        y1 = y0 - (int)(11*sin(angle));
        MoveToEx(hdc, x0, y0, NULL);
        LineTo(hdc, x1, y1);
    }
    
    DeleteDC(hMem);
    SetTextColor(hdc, 0);
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    EndPaint(hwnd, &ps);
    DeleteObject(hPen1);
    DeleteObject(hPen2);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT MixerMdi_MsgLButtonDown(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    RECT rect;
    char szText[40];
    int muteNumber = -1;
    int panActive = -1;
    
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    p->oldPt.x = LOWORD(lParam);
    p->oldPt.y = HIWORD(lParam);
    SetCapture(hwnd);
    p->volNumber = -1;
    p->panNumber = -1;
    for(int i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        // verifier si c'est le volume
        rect.left = 31 + 60*i + p->xo;
        rect.right = rect.left + 20;
        rect.top = 385 - g_cSeq.GetVolumeTrack(i) + p->yo;
        rect.bottom = rect.top + 40;
        if(PtInRect(&rect, p->oldPt))
        {
            p->volNumber = i;
            break;
        }
        // verifier si c'est le balance
        rect.left = 26 + 60*i + p->xo;
        rect.right = rect.left + 28;
        rect.top = 200 + p->yo;
        rect.bottom = rect.top + 28;
        if(PtInRect(&rect, p->oldPt))
        {
            p->panNumber = i;
            break;
        }
        // verifier si c'est le bouton MUTE
        rect.left = 18 + 60*i + p->xo;
        rect.right = rect.left + 15;
        rect.top = 435 + p->yo;
        rect.bottom = rect.top + 15;
        if(PtInRect(&rect, p->oldPt))
        {
            muteNumber = i;
            break;
        }
        // verifier si c'est le bouton PAN ACTIVE
        rect.left = 25 + 60*i + p->xo;
        rect.right = rect.left + 30;
        rect.top = 175 + p->yo;
        rect.bottom = rect.top + 15;
        if(PtInRect(&rect, p->oldPt))
        {
            panActive = i;
            break;
        } 
    }
    if(p->volNumber!=-1)
    {
        SetCursor(LoadCursor(NULL,IDC_HAND));
        wsprintf(szText, "Volume: %i", g_cSeq.GetVolumeTrack(p->volNumber));
        g_toolTip.SetTipText(hwnd, LOWORD(lParam), HIWORD(lParam), szText);
    }
    else if(p->panNumber!=-1)
    {
        SetCursor(LoadCursor(NULL,IDC_HAND));
        wsprintf(szText, "Balance: %i", g_cSeq.GetPanTrack(p->panNumber) - 63);
        g_toolTip.SetTipText(hwnd, LOWORD(lParam), HIWORD(lParam), szText);
    }
    else if(muteNumber!=-1)
    {
        SetCursor(LoadCursor(NULL,IDC_HAND));
        g_cSeq.InverseMuteTrack(muteNumber);
        InvalidateRect(hwnd, &rect, FALSE);
    }
    else if(panActive!=-1)
    {
        SetCursor(LoadCursor(NULL,IDC_HAND));
        g_cSeq.InversePanActive(panActive);
        InvalidateRect(hwnd, &rect, FALSE);
    }      
    return 0;
}

/******************************************************************
* 
*
******************************************************************/

LRESULT MixerMdi_MsgLButtonUp(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    SetCursor(LoadCursor(NULL,IDC_ARROW));
    p->volNumber = -1;
    p->panNumber = -1;
    ReleaseCapture();
    return 0;
}    

/******************************************************************
* 
*
******************************************************************/

LRESULT MixerMdi_MsgMouseMove(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    char szText[40];
    RECT rect;
    int x,y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);
    
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    if((wParam & MK_LBUTTON)&&(p->volNumber!=-1))
    {
        SetCursor(LoadCursor(NULL,IDC_HAND));
        g_cSeq.MoveVolumeTrack(p->volNumber, p->oldPt.y - y);
        p->oldPt.y = y;
        rect.left = 31 + p->volNumber*60 + p->xo;
        rect.right = rect.left + 20;
        rect.top = 258 + p->yo;
        rect.bottom = 425 + p->yo;
        InvalidateRect(hwnd, &rect, FALSE);
        wsprintf(szText, "Volume: %i", g_cSeq.GetVolumeTrack(p->volNumber));
        g_toolTip.UpdateTipText(szText);
    }
    else if((wParam & MK_LBUTTON)&&(p->panNumber!=-1))
    {
        SetCursor(LoadCursor(NULL,IDC_HAND));
        g_cSeq.MovePanTrack(p->panNumber, p->oldPt.y - y);
        p->oldPt.y = y;        
        rect.left = 26 + p->panNumber*60 + p->xo;
        rect.right = rect.left + 28;
        rect.top = 200 + p->yo;
        rect.bottom = rect.top + 28;
        InvalidateRect(hwnd, &rect, FALSE);
        wsprintf(szText, "Balance: %i", g_cSeq.GetPanTrack(p->panNumber) - 63);
        g_toolTip.UpdateTipText(szText);
    }
    else if((p->volNumber!=-1)||(p->panNumber!=-1))
    {
        SetCursor(LoadCursor(NULL,IDC_ARROW));
        p->volNumber = -1;
        p->panNumber = -1;
    }
    return 0;
}    

/************************************************************************
* 
*
************************************************************************/

LRESULT MixerMdi_MsgHScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_HORZ, &info);
    
    int x;
    x = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINERIGHT: x += 60; break;
        case SB_LINELEFT: x -= 60; break;
        case SB_PAGERIGHT: x += info.nPage; break;
        case SB_PAGELEFT: x -= info.nPage; break;
        case SB_THUMBTRACK: x = info.nTrackPos; break;
        case SB_THUMBPOSITION: x = info.nPos; break;
    }
    if(x>info.nMax)
        x = info.nMax;
    if(x<0)
        x = 0;
    SetScrollPos(hwnd, SB_HORZ, x ,TRUE);
    p->xo = - x;
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
}

/************************************************************************
* 
*
************************************************************************/

LRESULT MixerMdi_MsgVScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_VERT, &info);
    
    int y;
    y = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINEDOWN: y += 40; break;
        case SB_LINEUP: y -= 40; break;
        case SB_PAGEDOWN: y += info.nPage; break;
        case SB_PAGEUP: y -= info.nPage; break;
        case SB_THUMBTRACK: y = info.nTrackPos; break;
        case SB_THUMBPOSITION: y = info.nPos; break;
    }
    if(y>info.nMax)
        y = info.nMax;
    if(y<0)
        y = 0;
    SetScrollPos(hwnd, SB_VERT, y ,TRUE);
    p->yo = -y;
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
}
    
/******************************************************************
* 
*
******************************************************************/

LRESULT MixerMdi_MsgSize(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(wParam == SIZE_MAXIMIZED)
        g_cSeq.SetMaximized(TRUE);
    else if((wParam == SIZE_RESTORED)&&(!p->bClose))
        g_cSeq.SetMaximized(FALSE);
    
    MixerMdi_UpdateScrollBars(hwnd);
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT MixerMdi_MsgClose(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);    
    if(p)
        p->bClose = TRUE;
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT MixerMdi_MsgDestroy(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    KillTimer(hwnd, 1);
    KillTimer(hwnd, 2);
    if(p)
        delete p;
    
    g_hWndMixer = NULL;
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/****************************************************************************
* MakeMixerMdiData : création d'une donnée associé à chaque fenetre créé
*
****************************************************************************/

TMixerMdiData * MakeMixerMdiData()
{
    TMixerMdiData * p;
    
    p = new TMixerMdiData;
    if(!p)
        return NULL;
    
    ZeroMemory(p, sizeof(TMixerMdiData));
    p->volNumber = -1;
    p->panNumber = -1;
    
    return p;
}

/******************************************************************
* 
*
******************************************************************/

void MixerMdi_UpdateScrollBars(HWND hwnd)
{
    RECT rc;
    int cx, cy;
    int cxModel = g_cSeq.GetTrackCount()*60 + 40;
    int cyModel = 530;
    TMixerMdiData * p = (TMixerMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    GetClientRect(hwnd, &rc);
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
    
    SCROLLINFO info;
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_HORZ, &info);
    if(cx>=cxModel)
    {
        cx = cxModel;        
        SetScrollPos(hwnd, SB_HORZ, 0 ,TRUE);
        p->xo = 0;
        InvalidateRect(hwnd, NULL, TRUE);
    }
    else
    {
        SetScrollPos(hwnd, SB_HORZ, - p->xo ,TRUE);
    }
    info.nMin = 0;
    info.nMax = cxModel-1;
    info.nPage = cx;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
    
    info.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_VERT, &info);
    if(cy>=cyModel)
    {
        cy = cyModel;        
        SetScrollPos(hwnd, SB_VERT, 0 ,TRUE);
        p->yo = 0;
        InvalidateRect(hwnd, NULL, TRUE);
    }
    else
    {
        SetScrollPos(hwnd, SB_VERT, - p->yo ,TRUE);
        UpdateWindow(hwnd);
    }    
    
    info.nMin = 0;
    info.nMax = cyModel-1;
    info.nPage = cy;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
    
}
