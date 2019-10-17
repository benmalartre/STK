#include <windows.h>
#include "ArrEditorBox.h"
#include "ColumnTrack.h"
#include "../MidiSphinx.h"
#include "../ToolTip.h"

#define ID_TIMER1  1

extern CToolTip    g_toolTip;
extern CMidiSphinx g_midi;
extern CSequencer g_cSeq;
extern HCURSOR g_hCursor[];
extern TGlobal g_base;
extern HWND  hwndArrange;

/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/

void    RepaintArrEditorBox(HWND hwnd);
LRESULT CALLBACK ArrEditorBox_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* TrackBox_InitControl : initialisation du registre de controle
*
**************************************************************************/

WORD WINAPI ArrEditorBox_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = ARREDITORBOX_CLASS;
    wcl.lpfnWndProc = ArrEditorBox_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* TrackBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK ArrEditorBox_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT  rc, rcBuf;
    int id, i;
    BOOL bRedraw = FALSE;
    char szText[32];
    static BOOL bMovePattern = FALSE;
    static BOOL bSizePattern = FALSE;
    static BOOL bCopyPattern = FALSE;
    static RECT rcPaint;
    static int  oldRecordArr[MAX_CHANNEL];
    
    id = (int)GetWindowLong(hwnd, GWL_ID);
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    TArrangeData * pData = (TArrangeData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    
    switch (message)
    {
        case WM_CREATE:
            GetClientRect(hwnd, &rcPaint);
            rcPaint.left = 9 + g_cSeq.GetXOrigine() +
                (((int)g_midi.m_dwCurrentTime*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
            rcPaint.right = rcPaint.left + 3;
            SetTimer(hwnd, ID_TIMER1, 30, NULL);
            break;
            
        case WM_TIMER:
            if(g_midi.m_bRecording)
            {
                for(i=0; i<MAX_CHANNEL; i++)
                {
                    if(oldRecordArr[i]!=g_midi.m_ptRecord[i].y)
                    {
                        oldRecordArr[i] = g_midi.m_ptRecord[i].y;
                        bRedraw = TRUE;
                    }    
                }    
                if(bRedraw)
                {
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                }    
            }    
            GetClientRect(hwnd, &rc);
            rc.left = 8 + g_cSeq.GetXOrigine() +
                (((int)g_midi.m_dwCurrentTime*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
            
            // pour que le pointeur reste dans la fenetre arrangement pendant la lecture
            
            if((rc.left>rc.right - 1)&&(hwndArrange)&&(g_midi.m_bSequenceGoing))
            {
                SendMessage(hwndArrange, WM_HSCROLL, (WPARAM)SB_PAGERIGHT, 0);
                break;
            }
                
            rc.right = rc.left + 4;
            if(EqualRect(&rcPaint, &rc))
                break;
            UnionRect(&rcBuf, &rc, &rcPaint);
            CopyRect(&rcPaint, &rc);
            InvalidateRect(hwnd, &rcBuf, FALSE);
            break;
            
        case WM_SETCURSOR:
            if(LOWORD(lParam)==HTCLIENT)
            {
                switch(pData->modeCursor)
                {
                    case 1:
                    case 2:
                    case 3:
                        SetCursor(g_hCursor[pData->modeCursor]);
                        break;
                    case 4:
                        SetCursor(LoadCursor(NULL, IDC_IBEAM));
                        break;
                    default:
                        SetCursor(LoadCursor(NULL, IDC_ARROW));
                        break;
                }
                return 0;
            }
            return DefWindowProc(hwnd,message,wParam,lParam);
        
        case WM_SETFOCUS:
            PostMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)MAKELONG(id, AEBXN_SETFOCUS), 
                (LPARAM)hwnd);
            break;
            
        case WM_KILLFOCUS:
            PostMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)MAKELONG(id, AEBXN_KILLFOCUS), 
                (LPARAM)hwnd);
            break;
        
        case WM_KEYDOWN:
            if(LOWORD(wParam)==VK_DELETE)
            {
                g_cSeq.DeleteSelectedPattern();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            
            break;
        
        case WM_LBUTTONDOWN:
            UINT uZone;
            SetFocus(hwnd);
            GetWindowRect(hwnd, &rcBuf);
            ClipCursor(&rcBuf);
            SetCapture(hwnd);            
            if(pData->modeCursor==0) // mode pointeur
            {
                g_cSeq.SelectPattern(LOWORD(lParam), HIWORD(lParam), 
                    wParam & MK_SHIFT, &uZone);
                switch(uZone)
                {
                    case ZONE_SIZING:
                        SetCursor(LoadCursor(NULL,IDC_HAND));
                        bSizePattern = TRUE;
                        break;
                    case ZONE_MOVING:
                        SetCursor(g_hCursor[0]);
                        bMovePattern = TRUE;
                        break;                    
                }    
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if((pData->modeCursor==2)||(pData->modeCursor==3)) // mode fusion ou gomme
            {
                g_cSeq.SelectPattern(LOWORD(lParam), HIWORD(lParam), 
                    wParam & MK_SHIFT, NULL);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if(pData->modeCursor==1) // mode coupe
            {
                g_cSeq.SelectPattern(LOWORD(lParam), HIWORD(lParam), 
                    wParam & MK_SHIFT, NULL);                
                g_cSeq.TimeCodeToString((DWORD)g_cSeq.ScreenToTimeEvent(LOWORD(lParam)), szText);
                g_toolTip.SetTipText(hwnd, LOWORD(lParam), HIWORD(lParam), szText);                
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if(pData->modeCursor==4) // mode editeur nom de sequence
            {
                g_cSeq.SelectPattern(LOWORD(lParam), HIWORD(lParam), FALSE, NULL);                
                InvalidateRect(hwnd, NULL, FALSE);
            } 
            break;
        
        case WM_LBUTTONUP:
            if(pData->modeCursor==0) // mode pointeur
            {
                SetCursor(LoadCursor(NULL,IDC_ARROW));
                bMovePattern = FALSE;
                bSizePattern = FALSE;
                bCopyPattern = FALSE;
            }
            else if(pData->modeCursor==2) // mode fusion
            {
                g_cSeq.MergeSelectedPattern();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if(pData->modeCursor==3) // mode gomme
            {
                g_cSeq.DeleteSelectedPattern();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if(pData->modeCursor==1) // mode coupe
            {
                g_cSeq.DivideSelectedPattern(LOWORD(lParam));
                InvalidateRect(hwnd, NULL, FALSE);
            }
            ClipCursor(NULL);
            ReleaseCapture();
            break;        
        
        case WM_RBUTTONUP: // creation d'une nouvelle sequence
            SetFocus(hwnd);
            if(pData->modeCursor==0) // mode pointeur
                if(g_cSeq.CreatePattern(LOWORD(lParam), HIWORD(lParam)))
                    InvalidateRect(hwnd, NULL, FALSE);
                
            break;
        
        case WM_MOUSEMOVE:
            if(pData->modeCursor==0) // mode pointeur
            {
                if((wParam & MK_LBUTTON)&&(wParam & MK_CONTROL)&&(bMovePattern)&&(!bCopyPattern))
                {
                    SetCursor(g_hCursor[0]);
                    bCopyPattern = TRUE;
                    if(g_cSeq.DuplicatePattern())
                        InvalidateRect(hwnd, NULL, TRUE);
                }    
                else if((wParam & MK_LBUTTON)&&(bMovePattern))
                {
                    SetCursor(g_hCursor[0]);
                    if(g_cSeq.MovePattern(LOWORD(lParam), HIWORD(lParam)))
                        InvalidateRect(hwnd, NULL, TRUE);
                }
                else if((wParam & MK_LBUTTON)&&(bSizePattern))
                {
                    SetCursor(LoadCursor(NULL,IDC_HAND));
                    if(g_cSeq.SizePattern(LOWORD(lParam)))
                        InvalidateRect(hwnd, NULL, TRUE);
                }
                else if((bMovePattern)||(bSizePattern))
                {
                    SetCursor(LoadCursor(NULL,IDC_ARROW));
                    bMovePattern = FALSE;
                    bSizePattern = FALSE;
                }
            }
            else if((wParam & MK_LBUTTON)&&(pData->modeCursor==1)) // mode coupe
            {
                g_cSeq.TimeCodeToString((DWORD)g_cSeq.ScreenToTimeEvent(LOWORD(lParam)), szText);
                g_toolTip.UpdateTipText(szText);
            }        
            break;
        
        case WM_PAINT:
            RepaintArrEditorBox(hwnd);
            break;
        
        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER1);
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

void RepaintArrEditorBox(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    HPEN hPen, hPenEvent;
    HBRUSH hBrush;
    int i;
    
    hBrush = CreateSolidBrush(RGB(243, 240, 203));
    hPen = CreatePen(PS_DOT, 1, RGB(64,64,64));
    hPenEvent = CreatePen(PS_SOLID, 3, RGB(127,127,0));
    GetClientRect(hwnd, &rc);       
    hdc = BeginPaint(hwnd, &ps);
    SetBkMode(hdc, TRANSPARENT);
    // en cas de modification de couleur de fond de l'editeur
    
    if(!g_cSeq.IsBkWhite())
        FillRect(hdc, &rc, hBrush);
    else
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // affichage des grilles
    
    if(g_cSeq.IsGrid())
    {
        // tracer les colonnes de grilles
                
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
                g_cSeq.GetTimeSignature((DWORD)((fTick*i*
                    g_cSeq.GetTimeTick())/g_cSeq.GetCxTick()), bySgt);
                count = bySgt[0] - 1;
                temp++;
                if((buf>=0)&&(((g_cSeq.GetCxTick()<2)&&(temp%16==1))||(g_cSeq.GetCxTick()>=2)))
                {
                    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
                    MoveToEx(hdc, buf, 0, NULL);
                    LineTo(hdc, buf, rc.bottom);
                }    
            }
            else
            {
                count--;
                if((buf>=0)&&(g_cSeq.GetCxTick()>8))
                {
                    SelectObject(hdc,hPen);
                    MoveToEx(hdc, buf, 0, NULL);
                    LineTo(hdc, buf, rc.bottom);
                }
            }
            i += 16/(1<<bySgt[1]);        
        }
        
        SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    
        // tracer les lignes de grilles
    
        for(i=0; i<=g_cSeq.GetTrackCount(); i++)
        {
            MoveToEx(hdc, 0, g_cSeq.GetYOrigine() + g_cSeq.GetCyTrack()*i, NULL);
            LineTo(hdc, rc.right, g_cSeq.GetYOrigine() + g_cSeq.GetCyTrack()*i);
        }
    }
    
    TPattern * p;
    RECT rect;
    
    // on dessine toutes les sequences
    
    p = g_base.pFirstPattern;
    SelectObject(hdc,(HPEN)GetStockObject(WHITE_PEN));
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SetTextColor(hdc, RGB(255,255,255));
    while(p)
    {
        rect.left = 10 + g_cSeq.GetXOrigine() +
            ((p->left*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
        rect.right = 9 + g_cSeq.GetXOrigine() +
            ((p->right*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
        rect.top = g_cSeq.GetYOrigine() + p->numTrack*g_cSeq.GetCyTrack()+ 3;
        rect.bottom = rect.top + g_cSeq.GetCyTrack() - 3;
        if(p->bSelected)
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        else
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
        
        // cadre de la sequence
        
        MoveToEx(hdc, rect.left, rect.top, NULL);
        LineTo(hdc, rect.right, rect.top);
        LineTo(hdc, rect.right, rect.bottom);
        
        // affichage du nom de la sequence
        
        DrawText(hdc, p->szTitle, -1, &rect, DT_SINGLELINE|DT_VCENTER);
        
        p = p->pRight;
    }
    
    // affichage de sequence d'enregistrement
    
    for(i=0; i<MAX_CHANNEL; i++)
    {
        if((g_midi.m_bRecording)&&(g_midi.m_ptRecord[i].x<g_midi.m_ptRecord[i].y))
        {
            rect.left = 10 + g_cSeq.GetXOrigine() +
                ((g_midi.m_ptRecord[i].x*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
            rect.right = 9 + g_cSeq.GetXOrigine() +
                ((g_midi.m_ptRecord[i].y*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
            rect.top = g_cSeq.GetYOrigine() + i*g_cSeq.GetCyTrack()+ 3;
            rect.bottom = rect.top + g_cSeq.GetCyTrack() - 3;
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        }    
    }
    
    
    // affichage du pointeur
    
    int k;
    k = 10 + g_cSeq.GetXOrigine() +
            (((int)g_midi.m_dwCurrentTime*g_cSeq.GetCxTick())/(int)g_cSeq.GetTimeTick());
    
    SelectObject(hdc, hPenEvent);
    MoveToEx(hdc, k, 0, NULL);
    LineTo(hdc, k, rc.bottom);
    
    SetTextColor(hdc, 0);
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));    
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    EndPaint(hwnd, &ps);
    DeleteObject(hPen);
    DeleteObject(hPenEvent);
    DeleteObject(hBrush);
}

