#include <windows.h>
#include "PosBarExBox.h"
#include "../Sequencer.h"

extern CSequencer g_cSeq;
extern HBRUSH g_hBrushBarPos;


/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/


void    PosBarExBox_RepaintBox(HWND hwnd);
LRESULT CALLBACK PosBarExBox_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* TrackBox_InitControl : initialisation du registre de controle
*
**************************************************************************/

WORD WINAPI PosBarExBox_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = POSBAREXBOX_CLASS;
    wcl.lpfnWndProc = PosBarExBox_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = g_hBrushBarPos;
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* TrackBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK PosBarExBox_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT  rc;
    int id;
    
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    TEditorData * pData = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    id = (int)GetWindowLong(hwnd, GWL_ID);
    
    switch (message)
    {
        case WM_CREATE:
            break;
                            
        /*case WM_LBUTTONDOWN:
            x = LOWORD(lParam);
            if((x>5+origine+(cxTick*lastLoop/timeTick))&&
            (x<15+origine+(cxTick*lastLoop/timeTick)))
            {    
                SetCursor(LoadCursor(NULL,IDC_HAND));
                bSizePosLoop = TRUE;
            }    
            else if((x>10+origine+(cxTick*firstLoop/timeTick))&&
            (x<5+origine+(cxTick*lastLoop/timeTick)))
            {    
                SetCursor(g_hCursor);
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
                SetCursor(g_hCursor);
                if(x>10+origine+(cxTick*lastLoop/timeTick)+cxTick*2)
                {
                    firstLoop += timeTick*4;
                    lastLoop += timeTick*4;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                else if(x<10+origine+(cxTick*firstLoop/timeTick)-cxTick*2)
                {
                    firstLoop -= timeTick*4;
                    lastLoop -= timeTick*4;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            else if((wParam & MK_LBUTTON)&&(bSizePosLoop))
            {
                SetCursor(LoadCursor(NULL,IDC_HAND));
                if(x>10+origine+(cxTick*lastLoop/timeTick)+cxTick*2)
                {
                    lastLoop += timeTick*4;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                else if(x<10+origine+(cxTick*lastLoop/timeTick)-cxTick*2)
                {
                    lastLoop -= timeTick*4;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            else if((bMovePosLoop)||(bSizePosLoop))
            {
                SetCursor(LoadCursor(NULL,IDC_ARROW));
                bMovePosLoop = FALSE;
                bSizePosLoop = FALSE;
            }
            
            break;*/
        
        
        case WM_PAINT:
            PosBarExBox_RepaintBox(hwnd);
            break;
        /*    
        case WM_DESTROY:
            break;*/
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

/*************************************************************************
*
*
*************************************************************************/

void PosBarExBox_RepaintBox(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc, rect;
    char szText[8];
    HBRUSH hBrush, hBrushLoop;
    HPEN   hPen;
    int i, count = 0;
    int temp = 0;
    float fTick;
    
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    hBrush = CreateSolidBrush(RGB(240, 100, 0));
    hBrushLoop = CreateSolidBrush(RGB(250, 250, 0));
    hPen = CreatePen(PS_DOT, 1, RGB(64,64,64));
    GetClientRect(hwnd, &rc);     
    hdc = BeginPaint(hwnd, &ps);
    
    rect.left = 10+p->xo+(p->cxTick*p->pPtrn->left)/(int)g_cSeq.GetTimeTick();
    rect.right = 10+p->xo+(p->cxTick*p->pPtrn->right)/(int)g_cSeq.GetTimeTick();
    rect.top = 0;
    rect.bottom = rc.bottom;
    FillRect(hdc, &rect, hBrush);
    
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc,hPen);
    MoveToEx(hdc, 0, 20, NULL);
    LineTo(hdc, rc.right, 20);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    
    int buf;
    BYTE bySgt[4];
    
    i = 0;
    fTick = (float)p->cxTick/4.f;
    while((buf = 10 + (int)(fTick*i) + p->xo)<rc.right)
    {
        if(count==0)
        {
            g_cSeq.GetTimeSignature((DWORD)((fTick*i*g_cSeq.GetTimeTick())/p->cxTick), bySgt);
            count = bySgt[0] - 1;
            temp++;
            if(buf>=0)
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
            if(buf>=0)
            {
                MoveToEx(hdc, buf, 30, NULL);
                LineTo(hdc, buf, rc.bottom);
            }
        }
        i += 16/(1<<bySgt[1]);        
    }    
    
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    
    EndPaint(hwnd, &ps);
    DeleteObject(hBrush);
    DeleteObject(hBrushLoop);
    DeleteObject(hPen);
}


