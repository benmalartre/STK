#include <windows.h>
#include "TempoPosBar.h"
#include "../MidiSphinx.h"

//extern CMidiSphinx g_midi;
extern CSequencer g_cSeq;

/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/

void    TempoPosBarBox_RepaintBox(HWND hwnd);
LRESULT CALLBACK TempoPosBarBox_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* 
*
**************************************************************************/

WORD WINAPI TempoPosBarBox_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = TEMPOPOSBARBOX_CLASS;
    wcl.lpfnWndProc = TempoPosBarBox_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* TrackBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK TempoPosBarBox_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    
    switch (message)
    {
        case WM_PAINT:
            TempoPosBarBox_RepaintBox(hwnd);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}


/*************************************************************************
*
*
*************************************************************************/

void TempoPosBarBox_RepaintBox(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    char szText[8];
    HBRUSH hBrush;
    int i;
    
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    hBrush = CreateSolidBrush(RGB(240, 100, 0));
    GetClientRect(hwnd, &rc);     
    hdc = BeginPaint(hwnd, &ps);
    
    FillRect(hdc, &rc, hBrush);
    
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    int buf;
    int count = 0;
    int temp = 0;
    BYTE bySgt[4];
    
    i = 0;
    float fTick = (float)p->cxTick/4.f;
    while((buf = 10 + (int)(fTick*i) + p->xo)<rc.right)
    {
        if(count==0)
        {
            g_cSeq.GetTimeSignature((DWORD)((fTick*i*g_cSeq.GetTimeTick())/p->cxTick),
                bySgt);
            count = bySgt[0] - 1;
            temp++;
            if((buf>=0)&&(((p->cxTick<2)&&(temp%16==1))||
            ((p->cxTick<7)&&(p->cxTick>=2)&&(temp%4==1))||
            (p->cxTick>=7)))
            {
                MoveToEx(hdc, buf, 0, NULL);
                LineTo(hdc, buf, rc.bottom);
                wsprintf(szText, "%i", temp);
                TextOut(hdc, buf + 2, 0, szText, lstrlen(szText));
            }    
        }
        else
        {
            count--;
            if((buf>=0)&&(p->cxTick>12))
            {
                MoveToEx(hdc, buf, 18, NULL);
                LineTo(hdc, buf, rc.bottom);
            }
        }
        i += 16/(1<<bySgt[1]);        
    }
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    EndPaint(hwnd, &ps);
    DeleteObject(hBrush);
}
