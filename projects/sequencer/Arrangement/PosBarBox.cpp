#include <windows.h>
#include "PosBarBox.h"
#include "../MidiSphinx.h"

extern CMidiSphinx g_midi;
extern CSequencer g_cSeq;
extern HCURSOR g_hCursor[];
const int cstHeightLoopBar = 14;


/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/

void    PosBarBox_RepaintBox(HWND hwnd);
LRESULT CALLBACK PosBarBox_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* TrackBox_InitControl : initialisation du registre de controle
*
**************************************************************************/

WORD WINAPI PosBarBox_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = POSBARBOX_CLASS;
    wcl.lpfnWndProc = PosBarBox_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* TrackBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK PosBarBox_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT  rc;
    int id;
    int x, xx, y;
    static BOOL bMovePosLoop = FALSE;
    static BOOL bSizePosLoop = FALSE;
        
    id = (int)GetWindowLong(hwnd, GWL_ID);
    
    switch (message)
    {
        case WM_CREATE:
            break;
            
        case WM_SETFOCUS:
            PostMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)MAKELONG(id, POBXN_SETFOCUS), 
                (LPARAM)hwnd);
            break;
            
        case WM_KILLFOCUS:
            PostMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)MAKELONG(id, POBXN_KILLFOCUS), 
                (LPARAM)hwnd);
            break;
                    
        case WM_LBUTTONDOWN:
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            if(y>=cstHeightLoopBar)
            {
                SetCursor(LoadCursor(NULL,IDC_CROSS));
                if(!g_midi.m_bSequenceGoing)
                    g_midi.m_dwCurrentTime = (DWORD)g_cSeq.ScreenToTimeEvent(x);
            }    
            else if((x>5 + g_cSeq.GetXOrigine() + (g_cSeq.GetCxTick()*g_cSeq.GetLastLoop()/
            g_cSeq.GetTimeTick()))&&(x<15 + g_cSeq.GetXOrigine() + 
            (g_cSeq.GetCxTick()*g_cSeq.GetLastLoop()/g_cSeq.GetTimeTick()))&&
            (y<cstHeightLoopBar))
            {    
                SetCursor(LoadCursor(NULL,IDC_HAND));
                bSizePosLoop = TRUE;
            }    
            else if((x>10+ g_cSeq.GetXOrigine() +(g_cSeq.GetCxTick()*g_cSeq.GetFirstLoop()/
            (int)g_cSeq.GetTimeTick()))&&(x<5+ g_cSeq.GetXOrigine() +
            (g_cSeq.GetCxTick()*g_cSeq.GetLastLoop()/(int)g_cSeq.GetTimeTick()))&&
            (y<cstHeightLoopBar))
            {    
                SetCursor(g_hCursor[0]);
                bMovePosLoop = TRUE;
            }    
            break;
        
        case WM_LBUTTONUP:
            SetCursor(LoadCursor(NULL,IDC_ARROW));
            bMovePosLoop = FALSE;
            bSizePosLoop = FALSE;
            break;   
        
        case WM_MOUSEMOVE:
            x = LOWORD(lParam);
            
            if((wParam & MK_LBUTTON)&&(bMovePosLoop))
            {
                SetCursor(g_hCursor[0]);
                if(x>10+g_cSeq.GetXOrigine()+(g_cSeq.GetCxTick()*g_cSeq.GetLastLoop()/
                (int)g_cSeq.GetTimeTick())+g_cSeq.GetCxTick()*2)
                {
                    g_cSeq.MoveFirstLoop(4*(int)g_cSeq.GetTimeTick());
                    g_cSeq.MoveLastLoop(4*(int)g_cSeq.GetTimeTick());
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                else if(x<10+g_cSeq.GetXOrigine()+(g_cSeq.GetCxTick()*g_cSeq.GetFirstLoop()/
                (int)g_cSeq.GetTimeTick())-g_cSeq.GetCxTick()*2)
                {
                    g_cSeq.MoveFirstLoop(-4*(int)g_cSeq.GetTimeTick());
                    g_cSeq.MoveLastLoop(-4*(int)g_cSeq.GetTimeTick());
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            else if((wParam & MK_LBUTTON)&&(bSizePosLoop))
            {
                SetCursor(LoadCursor(NULL,IDC_HAND));
                if(x>10+g_cSeq.GetXOrigine()+(g_cSeq.GetCxTick()*g_cSeq.GetLastLoop()/
                (int)g_cSeq.GetTimeTick())+g_cSeq.GetCxTick()*2)
                {
                    g_cSeq.MoveLastLoop(4*(int)g_cSeq.GetTimeTick());
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                else if(x<10+g_cSeq.GetXOrigine()+(g_cSeq.GetCxTick()*g_cSeq.GetLastLoop()/
                (int)g_cSeq.GetTimeTick())-g_cSeq.GetCxTick()*2)
                {
                    g_cSeq.MoveLastLoop(-4*(int)g_cSeq.GetTimeTick());
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            else if((bMovePosLoop)||(bSizePosLoop))
            {
                SetCursor(LoadCursor(NULL,IDC_ARROW));
                bMovePosLoop = FALSE;
                bSizePosLoop = FALSE;
            }
            
            break;
        
        
        case WM_PAINT:
            PosBarBox_RepaintBox(hwnd);
            break;
        
                
        case POBXM_SETHSCROLL:
            g_cSeq.MoveXOrigine((int)lParam);
            InvalidateRect(hwnd, NULL, TRUE);
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

void PosBarBox_RepaintBox(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc, rect;
    char szText[8];
    HBRUSH hBrush, hBrushLoop;
    HPEN hPen;
    int i;
    
    hBrush = CreateSolidBrush(RGB(240, 100, 0));
    hBrushLoop = CreateSolidBrush(RGB(250, 250, 0));
    hPen = CreatePen(PS_DOT, 1, RGB(64,64,64));
    GetClientRect(hwnd, &rc);     
    hdc = BeginPaint(hwnd, &ps);
    
    FillRect(hdc, &rc, hBrush);
    rect.left = 10+ g_cSeq.GetXOrigine() +(g_cSeq.GetCxTick()*
        g_cSeq.GetFirstLoop()/g_cSeq.GetTimeTick());
    rect.right = 10+ g_cSeq.GetXOrigine() +(g_cSeq.GetCxTick()*
        g_cSeq.GetLastLoop()/g_cSeq.GetTimeTick());
    rect.top = 0;
    rect.bottom = cstHeightLoopBar;
    FillRect(hdc, &rect, hBrushLoop);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SelectObject(hdc, hPen);
    MoveToEx(hdc, 0, cstHeightLoopBar, NULL);
    LineTo(hdc, rc.right, cstHeightLoopBar);
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    int buf;
    int count = 0;
    int temp = 0;
    BYTE bySgt[4];
    
    i = 0;
    float fTick = (float)g_cSeq.GetCxTick()/4.f;
    while((buf = 10 + (int)(fTick*i) + g_cSeq.GetXOrigine())<rc.right)
    {
        if(count==0)
        {
            g_cSeq.GetTimeSignature((DWORD)((fTick*i*g_cSeq.GetTimeTick())/g_cSeq.GetCxTick()),
                bySgt);
            count = bySgt[0] - 1;
            temp++;
            if((buf>=0)&&(((g_cSeq.GetCxTick()<2)&&(temp%16==1))||
            ((g_cSeq.GetCxTick()<7)&&(g_cSeq.GetCxTick()>=2)&&(temp%4==1))||
            (g_cSeq.GetCxTick()>=7)))
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
            if((buf>=0)&&(g_cSeq.GetCxTick()>8))
            {
                MoveToEx(hdc, buf, 18, NULL);
                LineTo(hdc, buf, rc.bottom);
            }
        }
        i += 16/(1<<bySgt[1]);        
    }
    
    //SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    EndPaint(hwnd, &ps);
    DeleteObject(hPen);
    DeleteObject(hBrush);
    DeleteObject(hBrushLoop);
}


