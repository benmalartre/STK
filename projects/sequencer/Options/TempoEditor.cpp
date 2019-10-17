#include <windows.h>
#include "TempoEditor.h"
#include "../MidiSphinx.h"
#include "../ToolTip.h"

#define ID_TIMER3  3

extern CToolTip    g_toolTip;
extern CMidiSphinx g_midi;
extern CSequencer  g_cSeq;
extern TGlobal     g_base;
const int  cstTempoWide = 10;

/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/

void    TempoEditor_Repaint(HWND hwnd);
BOOL    TempoEditor_CreateObject(HWND, LPARAM);
UINT    TempoEditor_Select(HWND, LPARAM, CObjectMetaEventTempo **);
BOOL    TempoEditor_Size(HWND, LPARAM);
BOOL    TempoEditor_Move(HWND, LPARAM);
LRESULT CALLBACK TempoEditor_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* MatrixBox_InitControl : initialisation du registre de controle
*
**************************************************************************/

WORD WINAPI TempoEditor_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = TEMPOEDITORBOX_CLASS;
    wcl.lpfnWndProc = TempoEditor_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* MatrixBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK TempoEditor_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL bSizeTempo = FALSE;
    static BOOL bMoveTempo = FALSE;
    static CObjectMetaEventTempo * pReferenceTempo = NULL;
    RECT  rc, rcBuf;
    UINT uZone;
    char szText[80];
    
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    TTempoMdiData * pData = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    switch (message)
    {
        case WM_CREATE:
            GetClientRect(hwnd, &pData->rcPaint);
            pData->rcPaint.left = 8 + pData->xo +
                (((int)g_midi.m_dwCurrentTime*pData->cxTick)/(int)g_cSeq.GetTimeTick());
            pData->rcPaint.right = pData->rcPaint.left + 4;
            SetTimer(hwnd, ID_TIMER3, 30, NULL);
            break;
        
        case WM_TIMER:
            GetClientRect(hwnd, &rc);
            rc.left = 9 + pData->xo +
                (((int)g_midi.m_dwCurrentTime*pData->cxTick)/(int)g_cSeq.GetTimeTick());
            rc.right = rc.left + 3;
            if(EqualRect(&pData->rcPaint, &rc))
                break;
            UnionRect(&rcBuf, &rc, &pData->rcPaint);
            CopyRect(&pData->rcPaint, &rc);
            InvalidateRect(hwnd, &rcBuf, TRUE);
            break;
        
        
        case WM_KEYDOWN:
            if(LOWORD(wParam)==VK_DELETE)
            {
                g_cSeq.DeleteSelectedTempo();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        
        case WM_LBUTTONDOWN:
            SetFocus(hwnd);
            GetWindowRect(hwnd, &rcBuf);
            ClipCursor(&rcBuf);
            SetCapture(hwnd);
            uZone = TempoEditor_Select(hwnd, lParam, &pReferenceTempo);
            pData->oldCur.x = LOWORD(lParam);
            pData->oldCur.y = HIWORD(lParam);
            switch(uZone)
            {
                case ZONE_SIZING:
                    SetCursor(LoadCursor(NULL,IDC_HAND));
                    wsprintf(szText, "%i", pReferenceTempo->tempo);
                    g_toolTip.SetTipText(hwnd, LOWORD(lParam), HIWORD(lParam), szText);
                    bSizeTempo = TRUE;
                    break;
                case ZONE_MOVING:
                    SetCursor(LoadCursor(NULL,IDC_SIZEWE));
                    g_cSeq.TimeCodeToString((DWORD)pReferenceTempo->left, szText);
                    g_toolTip.SetTipText(hwnd, LOWORD(lParam), HIWORD(lParam), szText);
                    bMoveTempo = TRUE;
                    break;                    
            }
            break;
        
        case WM_LBUTTONUP:
            SetCursor(LoadCursor(NULL,IDC_ARROW));
            bSizeTempo = FALSE;
            bMoveTempo = FALSE;
            ClipCursor(NULL);
            ReleaseCapture();
            break;
        
        case WM_RBUTTONUP:
            SetFocus(hwnd);
            TempoEditor_CreateObject(hwnd, lParam);
            break;
        
        case WM_MOUSEMOVE:
            if((wParam & MK_LBUTTON)&&(bMoveTempo))
            {
                SetCursor(LoadCursor(NULL,IDC_SIZEWE));
                TempoEditor_Move(hwnd, lParam);
                g_cSeq.TimeCodeToString((DWORD)pReferenceTempo->left, szText);
                g_toolTip.UpdateTipText(szText);
            }
            else if((wParam & MK_LBUTTON)&&(bSizeTempo))
            {
                SetCursor(LoadCursor(NULL,IDC_HAND));
                TempoEditor_Size(hwnd, lParam);
                wsprintf(szText, "%i", pReferenceTempo->tempo);
                g_toolTip.UpdateTipText(szText);
            }
            else if((bSizeTempo)||(bMoveTempo))
            {
                SetCursor(LoadCursor(NULL,IDC_ARROW));
                bMoveTempo = FALSE;
                bSizeTempo = FALSE;
            }
            break;
        
        case WM_PAINT:
            TempoEditor_Repaint(hwnd);
            break;
        
        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER3);
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

void TempoEditor_Repaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HPEN hPen1, hPen2, hPenEvent;
    RECT rc;
    int i;
    
    TTempoMdiData * p = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
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
    while((buf = 10 + (int)(fTick*i) + p->xo)<rc.right)
    {
        if(count==0)
        {
            g_cSeq.GetTimeSignature((DWORD)((fTick*i*
                g_cSeq.GetTimeTick())/p->cxTick), bySgt);
            count = bySgt[0] - 1;
            temp++;
            if((buf>=0)&&(((p->cxTick<2)&&(temp%16==1))||(p->cxTick>=2)))
            {
                SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
                MoveToEx(hdc, buf, 0, NULL);
                LineTo(hdc, buf, p->yo + 300);
            }    
        }
        else
        {
            count--;
            if((buf>=0)&&(p->cxTick>12))
            {
                SelectObject(hdc,hPen1);
                MoveToEx(hdc, buf, 0, NULL);
                LineTo(hdc, buf, p->yo + 300);
            }
        }
        i += 16/(1<<bySgt[1]);
    }
    
    // on dessine les lignes
    
    SelectObject(hdc,hPen2);
    for(i=1; i<6; i++)
    {
        MoveToEx(hdc, 0, p->yo + 50*i , NULL);
        LineTo(hdc, rc.right, p->yo + 50*i);
    }    
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    MoveToEx(hdc, 0, p->yo + 300 , NULL);
    LineTo(hdc, rc.right, p->yo + 300);
    for(i=4; i<30; i++)
    {
        MoveToEx(hdc, 0, p->yo + 10*i , NULL);
        LineTo(hdc, 3, p->yo + 10*i);
    }    
    
    
    // dessine tous les tempos
    RECT rect;
    
    CObjectMetaEventTempo * pe;
    pe = g_base.pFirstTempo;
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    while(pe)
    {
        rect.left = 10 + p->xo + ((pe->left*p->cxTick)/(int)g_cSeq.GetTimeTick());
        rect.right = rect.left + cstTempoWide;
        rect.top = p->yo + 300 - pe->tempo;
        rect.bottom = p->yo + 300;
        if(pe->bSelected)
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        else
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
        FrameRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }
    
    // marque l'echelle
    
    char szText[12];
    SetTextColor(hdc, RGB(255,0,0));
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    for(i=1; i<6; i++)
    {
        wsprintf(szText, "%i", 300 - 50*i);
        TextOut(hdc, 2, p->yo + 50*i - 12, szText, lstrlen(szText));
    }
    SetTextColor(hdc, 0);
    int k;
    k = 10 + p->xo +
            (((int)g_midi.m_dwCurrentTime*p->cxTick)/(int)g_cSeq.GetTimeTick());
    
    SelectObject(hdc, hPenEvent);
    MoveToEx(hdc, k, 0, NULL);
    LineTo(hdc, k, rc.bottom);
    
    SelectObject(hdc,(HPEN)GetStockObject(BLACK_PEN));
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    EndPaint(hwnd, &ps);
    DeleteObject(hPen1);
    DeleteObject(hPen2);
    DeleteObject(hPenEvent);
}

/*******************************************************************************
* TempoEditor_CreateObject :
*
*******************************************************************************/

BOOL  TempoEditor_CreateObject(HWND hwnd, LPARAM lParam)
{
    int i, x, y;
    DWORD dwBuf, dwDelta;
    
    TTempoMdiData * pData = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    x = max(0, LOWORD(lParam) - 10 - pData->xo);
    dwBuf = (DWORD)((x*(int)g_cSeq.GetTimeTick())/pData->cxTick);
            
    y = HIWORD(lParam);
    if(y>300+pData->yo)
        return FALSE;
    
    dwDelta = (DWORD)((cstTempoWide*(int)g_cSeq.GetTimeTick())/pData->cxTick);
    
    CObjectMetaEventTempo * pe;
    
    // deséléctionner les tempos séléctionnés avant 
    pe = g_base.pFirstTempo;
    while(pe)
    {
        pe->bSelected = FALSE;
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }    
    
    // chercher si on a déjà un objet tempo à la meme emplacement 
    pe = g_base.pFirstTempo;
    while(pe)
    {
        if(((DWORD)pe->left<=dwBuf)&&((DWORD)pe->left+dwDelta>=dwBuf))
        {
            pe->bSelected = TRUE;
            InvalidateRect(hwnd, NULL, TRUE);
            return TRUE;
        }    
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }
    
    // sinon on crée un nouveau objet tempo
    g_cSeq.SetTempo(dwBuf, 120);
    InvalidateRect(hwnd, NULL, TRUE);
    return TRUE;
}

/********************************************************************************
*
*
********************************************************************************/

UINT  TempoEditor_Select(HWND hwnd, LPARAM lParam, CObjectMetaEventTempo ** ppRef)
{
    int i, x, y;
    DWORD dwBuf, dwDelta;
    CObjectMetaEventTempo * pe;
    
    // deséléctionner les tempos séléctionnés auparavant 
    pe = g_base.pFirstTempo;
    while(pe)
    {
        pe->bSelected = FALSE;
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }
    
    TTempoMdiData * pData = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    x = max(0, LOWORD(lParam) - 10 - pData->xo);
    dwBuf = (DWORD)((x*(int)g_cSeq.GetTimeTick())/pData->cxTick);
            
    y = HIWORD(lParam);
    // si la coordonnée y depasse le cadre , on traite pas
    if(y>300+pData->yo)
    {
        if(ppRef)
            *ppRef = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        return ZONE_NULL;
    }    
    
    dwDelta = (DWORD)((cstTempoWide*(int)g_cSeq.GetTimeTick())/pData->cxTick);
    
    // chercher si on a déjà un objet tempo à la meme emplacement 
    pe = g_base.pFirstTempo;
    while(pe)
    {
        if(((DWORD)pe->left<=dwBuf)&&((DWORD)pe->left+dwDelta>=dwBuf))
        {
            if(y>=305+pData->yo-pe->tempo)
            {
                pe->bSelected = TRUE;
                if(ppRef)
                    *ppRef = pe;
                InvalidateRect(hwnd, NULL, TRUE);
                return ZONE_MOVING;
            }
            else if((y<305+pData->yo-pe->tempo)&&(y>=290+pData->yo-pe->tempo))
            {
                pe->bSelected = TRUE;
                if(ppRef)
                    *ppRef = pe;
                InvalidateRect(hwnd, NULL, TRUE);
                return ZONE_SIZING;
            }
        }        
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }
    
    if(ppRef)
        *ppRef = NULL;
    InvalidateRect(hwnd, NULL, TRUE);
    return ZONE_NULL;
}

/********************************************************************************
*
*
********************************************************************************/

BOOL  TempoEditor_Size(HWND hwnd, LPARAM lParam)
{
    int i, y, tempo;
    RECT rc;
    CObjectMetaEventTempo * pe;
    
    // recupérer le tempo selectionné
    
    pe = g_base.pFirstTempo;
    while(pe)
    {
        if(pe->bSelected)
            break;
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }
    
    // si pas de tempo selectionné, on sort de la fonction
    
    if(!pe)
        return FALSE;
    
    TTempoMdiData * pData = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    y = HIWORD(lParam);
    
    tempo = pe->tempo - y + pData->oldCur.y;
    pData->oldCur.x = LOWORD(lParam);
    pData->oldCur.y = y;
    
    if(tempo==pe->tempo)
        return FALSE;
    
    if(tempo>pe->tempo)
        rc.top = pData->yo + 300 - tempo;
    else
        rc.top = pData->yo + 300 - pe->tempo;
    rc.bottom = pData->yo + 300;    
    rc.left = 10 + pData->xo + ((pe->left*pData->cxTick)/(int)g_cSeq.GetTimeTick());
    rc.right = rc.left + cstTempoWide;
    
    pe->SetTempo(tempo);
    InvalidateRect(hwnd, &rc, TRUE);
    
    return TRUE;
}

/********************************************************************************
*
*
********************************************************************************/

BOOL  TempoEditor_Move(HWND hwnd, LPARAM lParam)
{
    int  x;
    RECT rc;
    CObjectMetaEventTempo * pe;
    
    // recupérer le tempo selectionné
    
    pe = g_base.pFirstTempo;
    while(pe)
    {
        if(pe->bSelected)
            break;
        pe = (CObjectMetaEventTempo *)pe->pRight;
    }
    
    // si pas de tempo selectionné, on sort de la fonction
    
    if(!pe)
        return FALSE;
    
    TTempoMdiData * pData = (TTempoMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    x = LOWORD(lParam);    
    
    
    //x = max(0, x - 10 - pData->xo);
    int k;    
    k = max(0, pe->left + ((x - pData->oldCur.x)*(int)g_cSeq.GetTimeTick())/pData->cxTick);
    pData->oldCur.x = x;
    pData->oldCur.y = HIWORD(lParam);
    
    if(k==pe->left)
        return FALSE;
    
    if(k<pe->left)
    {
        rc.left = 10 + pData->xo + ((k*pData->cxTick)/(int)g_cSeq.GetTimeTick());
        rc.right = cstTempoWide + 10 + pData->xo + 
            ((pe->left*pData->cxTick)/(int)g_cSeq.GetTimeTick());
    }
    else
    {
        rc.left = 10 + pData->xo + ((pe->left*pData->cxTick)/(int)g_cSeq.GetTimeTick());
        rc.right = cstTempoWide + 10 + pData->xo + 
            ((k*pData->cxTick)/(int)g_cSeq.GetTimeTick());
    }        
    rc.top = pData->yo + 300 - pe->tempo;
    rc.bottom = pData->yo + 300;
    pe->left = k;
    InvalidateRect(hwnd, &rc, TRUE);
    
    return TRUE;
}
