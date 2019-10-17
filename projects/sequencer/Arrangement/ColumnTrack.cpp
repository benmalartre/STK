#include <windows.h>
#include "ColumnTrack.h"
#include "../MidiSphinx.h"
#include "../Resource.h"

extern HINSTANCE   g_hInst;
extern HBITMAP     g_hbmMute;
extern HBITMAP     g_hbmLed;
extern CMidiSphinx g_midi;
extern CSequencer  g_cSeq;

/***********************************************************
* Constructeur
*
***********************************************************/

CWinColumnTrack::CWinColumnTrack()
{
    m_hBrush = CreateSolidBrush(RGB(0, 80, 130));
    m_hWnd = NULL;
}

/***********************************************************
* Destructeur
*
***********************************************************/

CWinColumnTrack::~CWinColumnTrack()
{
    DeleteObject(m_hBrush);
}

/************************************************************
* InitInstance : initialisation de l'instance 
*
************************************************************/

WORD CWinColumnTrack::InitInstance()
{
    WNDCLASSEX wincl;
    
    wincl.hInstance = g_hInst;
    wincl.lpszClassName = "ColumnTrackWndClass";
    wincl.lpfnWndProc = WndProc;
    //wincl.style = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);    
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;    
    //wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wincl.hbrBackground = m_hBrush;
    
    return RegisterClassEx(&wincl);
}

/*******************************************************************
* Create : creation de fenetre
*
*******************************************************************/

BOOL CWinColumnTrack::Create(HWND hwndParent)
{
    HWND hwnd;
    RECT rc;
    
    GetClientRect(hwndParent, &rc);
    for(int i=0; i<16; i++)
    {
        m_countTimeLed[i] = 0;
        m_led[i] = 0;
    }    
    
    m_hWnd = CreateWindowEx(0,"ColumnTrackWndClass","",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            0, 0, 0, 0,
            hwndParent,(HMENU)IDC_COLUMNTRACK, g_hInst,this);
    
    if(!m_hWnd)
        return FALSE;
        
    return TRUE;
}

/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK CWinColumnTrack::WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    
    CWinColumnTrack *pHwnd = (CWinColumnTrack*)GetWindowLong(hwnd, GWL_USERDATA);
    if(!pHwnd)
        return DefWindowProc(hwnd,message,wParam,lParam);
    
    
    switch(message)
    {
        case WM_CREATE:
            return pHwnd->MsgCreate(hwnd,message,wParam,lParam);
        case WM_TIMER:
            return pHwnd->MsgTimer(hwnd,message,wParam,lParam);
        case WM_LBUTTONDOWN:
            return pHwnd->MsgLButtonDown(hwnd,wParam,lParam);
        case WM_LBUTTONDBLCLK:
            return pHwnd->MsgLButtonDblClk(hwnd,wParam,lParam);
        case WM_PAINT:
            return pHwnd->MsgPaint(hwnd);
        case WM_DESTROY:
            return pHwnd->MsgDestroy(hwnd);
        default:
            return DefWindowProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

HWND CWinColumnTrack::GetHandle()
{
    return m_hWnd;
}    


/*****************************************************************
* MsgCreate : traitement de message WM_CREATE
*
*****************************************************************/

LRESULT CWinColumnTrack::MsgCreate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    m_hWnd = hwnd;
    SetTimer(hwnd, 1, 16, NULL);
    for(int i=0; i<g_cSeq.GetTrackCount(); i++)
        g_cSeq.GetInstrumentName(i, g_midi.m_dwCurrentTime, m_szName[i]);
    return 0;
}

/*****************************************************************
* MsgTimer : traitement de message WM_TIMER
*
*****************************************************************/

LRESULT CWinColumnTrack::MsgTimer(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    RECT rc;
    BOOL bRedraw = FALSE;
    int i;
    
    for(i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        if(g_cSeq.m_led[i].by4[0]!=0)
        {
            m_countTimeLed[i] = 0;
            if(m_led[i]!=4)
            {
                m_led[i] = 4;
                bRedraw = TRUE;
            }    
        }
        else if(g_cSeq.m_led[i].by3[0]!=0)
        {
            m_countTimeLed[i] = 0;
            if(m_led[i]!=3)
            {
                m_led[i] = 3;
                bRedraw = TRUE;
            }    
        } 
        else if(g_cSeq.m_led[i].by2[0]!=0)
        {
            m_countTimeLed[i] = 0;
            if(m_led[i]!=2)
            {
                m_led[i] = 2;
                bRedraw = TRUE;
            }    
        }
        else if(g_cSeq.m_led[i].by1[0]!=0)
        {
            m_countTimeLed[i] = 0;
            if(m_led[i]!=1)
            {
                m_led[i] = 1;
                bRedraw = TRUE;
            }    
        }
        else if(m_led[i]!=0)
        {
            m_countTimeLed[i]++;
            if(m_countTimeLed[i]>8)
            {
                m_led[i]--;
                m_countTimeLed[i] = 0;
            }
            bRedraw = TRUE;
        }    
    }
    if(bRedraw)
    {
        GetClientRect(hwnd, &rc);
        rc.left += 30;
        rc.right = rc.left + 52;
        rc.top = g_cSeq.GetYOrigine();
        rc.bottom = rc.top + g_cSeq.GetTrackCount()*g_cSeq.GetCyTrack();
        InvalidateRect(hwnd, &rc, FALSE);
    }
    
    // met à jour l'affichage de nom de l'instrument
    char szBuffer[32];
    bRedraw = FALSE;
    for(i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        g_cSeq.GetInstrumentName(i, g_midi.m_dwCurrentTime, szBuffer);
        if(lstrcmp(szBuffer, m_szName[i]))
        {
            lstrcpy(m_szName[i], szBuffer);
            bRedraw = TRUE;
        }    
    }
    if(bRedraw)
    {
        GetClientRect(hwnd, &rc);
        rc.left += 100;
        rc.top = g_cSeq.GetYOrigine();
        rc.bottom = rc.top + g_cSeq.GetTrackCount()*g_cSeq.GetCyTrack();
        InvalidateRect(hwnd, &rc, TRUE);
    }   
    return 0;
}

LRESULT CWinColumnTrack::MsgLButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
    RECT rc, rect, rc1;
    POINT pt;
    
    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    
    GetClientRect(hwnd, &rc);
    for(int i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        rect.left = rc.left;
        rect.right = rc.right;
        rect.top = i*g_cSeq.GetCyTrack() + g_cSeq.GetYOrigine();
        rect.bottom = (i+1)*g_cSeq.GetCyTrack() + g_cSeq.GetYOrigine() + 1;
        
        rc1.left = rc.left + 30;
        rc1.right = rc1.left + 15;
        rc1.top = rect.top + (g_cSeq.GetCyTrack() - 15)/2;
        rc1.bottom = rc1.top + 15;
        
        if(PtInRect(&rc1, pt))
        {
            g_cSeq.InverseMuteTrack(i);
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        }
        else if(PtInRect(&rect, pt))
        {
            g_cSeq.SetTrackSelect(i);
            InvalidateRect(hwnd, NULL, TRUE);
            g_cSeq.SelectAllPattern(i);
            SendMessage(GetParent(hwnd), WM_USER, 0, 0);            
            break;
        }
        else
        {
            g_cSeq.SetTrackSelect(-1);
            InvalidateRect(hwnd, NULL, TRUE);
        }       
    }
    return 0;
}    

LRESULT CWinColumnTrack::MsgLButtonDblClk(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
    RECT rc, rect;
    POINT pt;
    
    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    
    GetClientRect(hwnd, &rc);
    for(int i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        rect.left = rc.left;
        rect.right = rc.right;
        rect.top = i*g_cSeq.GetCyTrack() + g_cSeq.GetYOrigine();
        rect.bottom = (i+1)*g_cSeq.GetCyTrack() + g_cSeq.GetYOrigine() + 1;
        if(PtInRect(&rect, pt))
        {
            // j'ai mis Sleep pour attendre l'action de double clique avant d'ouvrir
            // la boite de dialogue sinon l'action LBUTTONUP est activé directement
            // à la boite de dialogue si la fenetre se superpose à l'emplacement du
            // curseur
            Sleep(200);
            m_cPath.Create(hwnd, i, g_cSeq.GetPrgmChange(i, 0));
            InvalidateRect(hwnd, &rect, TRUE);
            break;
        }    
    }
    return 0;
}



LRESULT CWinColumnTrack::MsgPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc, hmem;
    RECT rc, rect;
    char szText[64];
    int i, y;    
    
    GetClientRect(hwnd, &rc);     
    hdc = BeginPaint(hwnd, &ps);
    hmem = CreateCompatibleDC(hdc);
    
    SetBkMode(hdc, TRANSPARENT);    
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SetTextColor(hdc, RGB(250,220,200));
    
    for(i=0; i<g_cSeq.GetTrackCount(); i++)
    {
        // on dessine une rectangle pour marquer les pistes
        rect.left = rc.left;
        rect.right = rc.right;
        rect.top = i*g_cSeq.GetCyTrack() + g_cSeq.GetYOrigine();
        rect.bottom = (i+1)*g_cSeq.GetCyTrack() + g_cSeq.GetYOrigine() + 1;
        if(i==g_cSeq.GetTrackSelect())
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
        FrameRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        
        y = rect.top + (g_cSeq.GetCyTrack() - 15)/2;
        
        // on affiche les boutons MUTE
        SelectObject(hmem, g_hbmMute);
        if(g_cSeq.IsMuteTrack(i))
            BitBlt(hdc, rect.left + 30, y, 15, 15, hmem, 0, 0, SRCCOPY);
        else
            BitBlt(hdc, rect.left + 30, y, 15, 15, hmem, 0, 15, SRCCOPY);
        
        // on affiche les leds
        SelectObject(hmem, g_hbmLed);
        BitBlt(hdc, rect.left + 58, y+4, 24, 8, hmem, 24, m_led[i]*8, SRCCOPY);
        
        // on affiche le numéro des pistes
        
        wsprintf(szText, "%i", i+1);
        rect.right = rc.left + 20;
        DrawText(hdc, szText, -1, &rect, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
        
        // on affiche le nom de l'instrument
        
        wsprintf(szText, "%s", m_szName[i]);
        rect.left = rc.left + 100;
        rect.right = rc.right;
        DrawText(hdc, szText, -1, &rect, DT_SINGLELINE|DT_VCENTER);
        
    }
    
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    SetTextColor(hdc, 0);
    DeleteDC(hmem);
    EndPaint(hwnd, &ps);
    return 0;
}



LRESULT CWinColumnTrack::MsgDestroy(HWND hwnd)
{
    KillTimer(hwnd, 1);
    return 0;
}
