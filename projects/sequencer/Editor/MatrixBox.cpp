#include <windows.h>
#include "MatrixBox.h"
#include "../MidiSphinx.h"

#define ID_TIMER2  2

extern CMidiSphinx g_midi;
extern CSequencer g_cSeq;
extern HCURSOR g_hCursor[];

const int cstHeightGrid[7] = {0,15,34,54,70,85,104};

/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/

void    MatrixBox_Repaint(HWND hwnd);
LRESULT CALLBACK MatrixBox_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* MatrixBox_InitControl : initialisation du registre de controle
*
**************************************************************************/

WORD WINAPI MatrixBox_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = MATRIXBOX_CLASS;
    wcl.lpfnWndProc = MatrixBox_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* MatrixBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK MatrixBox_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL bSizeNote = FALSE;
    static BOOL bMoveNote = FALSE;
    static CObjectEventNote * pReferenceNote = NULL;
    RECT  rc, rcBuf;
    HWND  hParent;
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
            GetClientRect(hwnd, &pData->rcPaint);
            pData->rcPaint.left = 9 + pData->xo +
                (((int)g_midi.m_dwCurrentTime*pData->cxTick)/(int)g_cSeq.GetTimeTick());
            pData->rcPaint.right = pData->rcPaint.left + 3;
            SetTimer(hwnd, ID_TIMER2, 30, NULL);
            break;
        
        case WM_TIMER:
            GetClientRect(hwnd, &rc);
            rc.left = 8 + pData->xo +
                (((int)g_midi.m_dwCurrentTime*pData->cxTick)/(int)g_cSeq.GetTimeTick());
            
            // pour que le pointeur reste dans la fenetre arrangement pendant la lecture
            
            hParent = GetParent(hwnd);
            
            if((rc.left>rc.right - 1)&&(hParent)&&(g_midi.m_bSequenceGoing)&&
            ((int)g_midi.m_dwCurrentTime<pData->pPtrn->right))
            {
                SendMessage(hParent, WM_HSCROLL, (WPARAM)SB_PAGERIGHT, 0);
                GetClientRect(hwnd, &pData->rcPaint);
                break;
            }            
            rc.right = rc.left + 4;
            if(EqualRect(&pData->rcPaint, &rc))
                break;
            UnionRect(&rcBuf, &rc, &pData->rcPaint);
            CopyRect(&pData->rcPaint, &rc);
            InvalidateRect(hwnd, &rcBuf, TRUE);
            break;
        
        
        case WM_KEYDOWN:
            if(LOWORD(wParam)==VK_DELETE)
            {
                g_cSeq.DeleteSelectedNote(pData);
                InvalidateRect(hwnd, NULL, TRUE);
            }    
            break;
        
        case WM_LBUTTONDOWN:
            UINT uZone;
            SetFocus(hwnd);
            pReferenceNote = g_cSeq.SelectNote(LOWORD(lParam), HIWORD(lParam), pData,
                wParam & MK_SHIFT, &uZone);
            switch(uZone)
            {
                case ZONE_SIZING:
                    SetCursor(LoadCursor(NULL,IDC_HAND));
                    bSizeNote = TRUE;
                    break;
                case ZONE_MOVING:
                    SetCursor(g_hCursor[0]);
                    bMoveNote = TRUE;
                    break;                    
            }
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        
        case WM_LBUTTONUP:
            SetCursor(LoadCursor(NULL,IDC_ARROW));
            pReferenceNote = NULL;
            bSizeNote = FALSE;
            bMoveNote = FALSE;
            break;
        
        case WM_RBUTTONUP:
            SetFocus(hwnd);
            if(g_cSeq.CreateNote(LOWORD(lParam), HIWORD(lParam), pData))
                InvalidateRect(hwnd, NULL, FALSE);
            else
                MessageBeep(1);
            break;
        
        case WM_MOUSEMOVE:
            if((wParam & MK_LBUTTON)&&(bMoveNote))
            {
                SetCursor(g_hCursor[0]);
                if(g_cSeq.MoveNote(LOWORD(lParam), HIWORD(lParam), pReferenceNote, pData))
                    InvalidateRect(hwnd, NULL, TRUE);
            }
            else if((wParam & MK_LBUTTON)&&(bSizeNote))
            {
                SetCursor(LoadCursor(NULL,IDC_HAND));
                if(g_cSeq.SizeNote(LOWORD(lParam), pReferenceNote, pData))
                    InvalidateRect(hwnd, NULL, TRUE);
            }
            else if((bSizeNote)||(bMoveNote))
            {
                SetCursor(LoadCursor(NULL,IDC_ARROW));
                bMoveNote = FALSE;
                bSizeNote = FALSE;
            }
            break;
        
        case WM_PAINT:
            MatrixBox_Repaint(hwnd);
            break;
        
        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER2);
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

void MatrixBox_Repaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HPEN hPen1, hPen2, hPenEvent;
    RECT rc;
    int i;
    
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    if(!p)
        return;
    hPen1 = CreatePen(PS_DOT, 1, RGB(164,164,164));
    hPen2 = CreatePen(PS_SOLID, 1, RGB(194,194,194));
    hPenEvent = CreatePen(PS_SOLID, 3, RGB(127,127,0));
    GetClientRect(hwnd, &rc);       
    hdc = BeginPaint(hwnd, &ps);
    SetBkMode(hdc, TRANSPARENT);
    
    int buf, count=0;
    int temp=0;
    float fTick;
    BYTE bySgt[4];
    
    // dessine les colonnes
    
    i = 0;
    fTick = (float)p->cxTick/4.f;
    for(i=0; (buf = 10 + (int)(fTick*i) + p->xo)<rc.right; i++)
    {
        if((count==0)&&(temp<=0))
        {
            g_cSeq.GetTimeSignature((DWORD)((fTick*i*g_cSeq.GetTimeTick())/p->cxTick), bySgt);
            count = bySgt[0] - 1;
            temp = (16/(1<<bySgt[1])) - 1;
            SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
        }
        else
        {
            if(temp<=0)
            {
                count--;
                temp = (16/(1<<bySgt[1])) - 1;
                SelectObject(hdc,hPen1);
            }
            else
            {
                SelectObject(hdc,hPen2);
                temp--;
                if(p->cxTick<=20)
                    continue;
            }
        }
        if(buf>=0)
        {
            MoveToEx(hdc, buf, 0, NULL);
            LineTo(hdc, buf, rc.bottom);
        }
    }
    
    // on dessine les lignes
    
    
    for(i=0; p->yo + i*120<rc.bottom; i++)
    {
        SelectObject(hdc,hPen2);
        
        for(int j=1; j<7; j++)
        {
            MoveToEx(hdc, 0, p->yo + i*120 + cstHeightGrid[j], NULL);
            LineTo(hdc, rc.right, p->yo + i*120 + cstHeightGrid[j]);
        }    
        SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
        MoveToEx(hdc, 0, p->yo + i*120 , NULL);
        LineTo(hdc, rc.right, p->yo + i*120);
        MoveToEx(hdc, 0, p->yo + i*120 + 70, NULL);
        LineTo(hdc, rc.right, p->yo + i*120 + 70);
    }    
    
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    
    // dessine toutes les notes
    
    CObjectEvent * pe;
    CObjectEventNote * pn;
    RECT rect;
        
    pe = p->pPtrn->pFirstEvent;
    
    while(pe)
    {
        if(pe->GetType()==MIDI_NOTEON)
        {
        pn = (CObjectEventNote*)pe;
        rect.left = 10 + p->xo + ((pn->left*p->cxTick)/(int)g_cSeq.GetTimeTick());
        rect.right = 9 + p->xo + ((pn->right*p->cxTick)/(int)g_cSeq.GetTimeTick());
        rect.top = p->yo + (119 - pn->note)*10 + 1;
        rect.bottom = rect.top + 9;
        if(pn->bSelected)
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        else
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
        }    
        
        // cadre de la note
        /*
        MoveToEx(hdc, rect.left, rect.top, NULL);
        LineTo(hdc, rect.right, rect.top);
        LineTo(hdc, rect.right, rect.bottom);*/
        
        pe = pe->pRight;
    }
    
    int k;
    k = 10 + p->xo +
            (((int)g_midi.m_dwCurrentTime*p->cxTick)/(int)g_cSeq.GetTimeTick());
    // affichage pointeur de position    
    SelectObject(hdc, hPenEvent);
    MoveToEx(hdc, k, 0, NULL);
    LineTo(hdc, k, rc.bottom);
    
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    
    EndPaint(hwnd, &ps);
    DeleteObject(hPen1);
    DeleteObject(hPen2);
    DeleteObject(hPenEvent);
}

