#include <windows.h>
#include "ToolTip.h"

extern HINSTANCE   g_hInst;

CToolTip::CToolTip():
    m_hWnd(0)
{
}

CToolTip::~CToolTip()
{
}

/***************************************************************
* InitInstance:
*
***************************************************************/

WORD CToolTip::InitInstance(HINSTANCE hInst)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInst;
    wcl.lpszClassName = "ToolTipClassSpecialGagah1";
    wcl.lpfnWndProc = WndProc;
    wcl.style = CS_HREDRAW|CS_VREDRAW;
    wcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK CToolTip::WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    
    CToolTip *pHwnd = (CToolTip*)GetWindowLong(hwnd, GWL_USERDATA);
    if(!pHwnd)
        return DefWindowProc(hwnd,message,wParam,lParam);
    
    
    switch(message)
    {
        case WM_CREATE:
            pHwnd->MsgCreate(hwnd);
        case WM_TIMER:
            return pHwnd->MsgTimer(hwnd);
        case WM_PAINT:
            return pHwnd->MsgPaint(hwnd);       
        case WM_DESTROY:
            return pHwnd->MsgDestroy(hwnd);     
        default:
            return DefWindowProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

void CToolTip::SetTipText(HWND hParent, int x, int y, char * pText)
{
    if(m_hWnd)
        DestroyWindow(m_hWnd);
    
    m_hParent = hParent;
    
    m_pt.x = x;
    m_pt.y = y;
    ClientToScreen(hParent, &m_pt);
    m_pt.x += 20;
    m_pt.y -= 20;
    
    if(pText)
        lstrcpy(m_szText, pText);
    else
        m_szText[0] = 0;
    
    CreateWindowEx(WS_EX_TOPMOST,"ToolTipClassSpecialGagah1","",
        WS_POPUP|WS_VISIBLE|WS_BORDER,
        m_pt.x, m_pt.y, 60, 20,
        hParent , NULL, g_hInst, this);
    SetFocus(hParent);
}

void CToolTip::UpdateTipText(char * pText)
{
    HDC hdc;
    TEXTMETRIC tm;
    int cx;
    
    if(pText)
        lstrcpy(m_szText, pText);
    else
        m_szText[0] = 0;
    
    m_count = 0;
    
    if(!m_hWnd)
    {
        CreateWindowEx(WS_EX_TOPMOST,"ToolTipClassSpecialGagah1","",
            WS_POPUP|WS_VISIBLE|WS_BORDER,
            m_pt.x, m_pt.y, 20, 20,
            m_hParent , NULL, g_hInst, this);
        SetFocus(m_hParent);
    }
    else
    {
        hdc = GetDC(m_hWnd);
        SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
        GetTextMetrics(hdc, &tm);
        cx = tm.tmAveCharWidth * lstrlen(m_szText) + 20;
        ReleaseDC(m_hWnd, hdc);
        MoveWindow(m_hWnd, m_pt.x, m_pt.y, cx, 20, TRUE);
        InvalidateRect(m_hWnd, NULL, TRUE);
    }        
}    

LRESULT CToolTip::MsgCreate(HWND hwnd)
{
    HDC hdc;
    TEXTMETRIC tm;
    int cx;
    
    m_hWnd = hwnd;
    SetTimer(hwnd, 1, 50, NULL);
    m_count = 0;
    hdc = GetDC(hwnd);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    GetTextMetrics(hdc, &tm);
    cx = tm.tmAveCharWidth * lstrlen(m_szText) + 20;
    ReleaseDC(hwnd, hdc);
    MoveWindow(hwnd, m_pt.x, m_pt.y, cx, 20, TRUE);
    return 0;
}

LRESULT CToolTip::MsgTimer(HWND hwnd)
{
    m_count++;
    if(m_count>40)
        DestroyWindow(hwnd);
    return 0;
}

LRESULT CToolTip::MsgDestroy(HWND hwnd)
{
    m_count = 0;
    KillTimer(hwnd, 1);
    m_hWnd = NULL;
    return 0;
}

LRESULT CToolTip::MsgPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HBRUSH hBrush;
    RECT rc;
  
    hBrush = CreateSolidBrush(RGB(243, 240, 203));
    GetClientRect(hwnd, &rc);
  
    hdc = BeginPaint(hwnd, &ps);
    FillRect(hdc, &rc, hBrush);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SetTextColor(hdc, 0);
    DrawText(hdc, m_szText, -1, &rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
  
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
  
    EndPaint(hwnd, &ps);
    DeleteObject(hBrush);
    return 0;
}
