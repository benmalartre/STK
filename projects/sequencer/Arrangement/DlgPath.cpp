#include <windows.h>
#include "../Resource.h"
#include "DlgPath.h"
#include "../MidiSphinx.h"

extern HINSTANCE   g_hInst;
extern CSequencer  g_cSeq;
extern CMidiSphinx g_midi;

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgPath::CDlgPath():
    m_hDlg(NULL),
    m_nSel(0)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgPath::~CDlgPath()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgPath::Create(HWND hwndParent, int numTrack, int sel)
{
    // sauvegarde 
    
    m_nSel = sel;
    m_numTrack = numTrack;
    
    // création de la boite de dialogue
    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGPATH),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgPath::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgPath *pDlg = (CDlgPath*)GetWindowLong(hwnd,GWL_USERDATA);    
    if(!pDlg)
        return FALSE;
    
    switch(message)
    {
        case WM_INITDIALOG:
            return pDlg->MsgInitDialog(hwnd,wParam,lParam);
            
        case WM_COMMAND:
            return pDlg->MsgCommand(hwnd,wParam,lParam);        
        
        case WM_LBUTTONDOWN:
            return pDlg->MsgLButtonDown(hwnd,wParam,lParam);
            
        case WM_LBUTTONUP:
            return pDlg->MsgLButtonUp(hwnd,wParam,lParam);
        
        case WM_PAINT:
            return pDlg->MsgPaint(hwnd);
        
        case WM_SYSCOMMAND:
            return pDlg->MsgSysCommand(hwnd,wParam);
    }
    return FALSE;
}

/****************************************************************
* MsgInitDialog : traitement de message WM_INITDIALOG
*
****************************************************************/

BOOL CDlgPath::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // sauvegarder le handle de la boite de dialogue
    
    m_hDlg = hwnd;
    return FALSE;
}

/****************************************************************
* MsgLButtonDown :
*
****************************************************************/

BOOL CDlgPath::MsgLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    RECT rc;
    int cx, cy;
    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    
    GetClientRect(hwnd, &rc);
    cy = (rc.bottom - rc.top)/32;
    cx = (rc.right - rc.left)/4;
    
    for(int i=0; i<128; i++)
    {
        rc.left = cx*(i/32);
        rc.right = rc.left + cx;
        rc.top = cy*(i%32);
        rc.bottom = rc.top + cy;
        if(PtInRect(&rc, pt))
        {
            m_nSel = i;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }    
    }
    return TRUE;
}

/****************************************************************
* MsgLButtonDown :
*
****************************************************************/

BOOL CDlgPath::MsgLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    g_cSeq.SetPrgmChange(m_numTrack, 0, m_nSel);
    g_midi.UpdatePrgmChange(m_numTrack, m_nSel);
    EndDialog(hwnd, 1);
    return TRUE;
}    


/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgPath::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    /*switch(LOWORD(wParam))
    {
        case IDCANCEL:
            EndDialog(hwnd, 0);
    } */                           
    return FALSE;
}

/****************************************************************
* MsgPaint
*
****************************************************************/

BOOL CDlgPath::MsgPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    char szText[64];
    int i, y, cx, cy;
    
    GetClientRect(hwnd, &rc);
    cy = (rc.bottom - rc.top)/32;
    cx = (rc.right - rc.left)/4;     
    
    hdc = BeginPaint(hwnd, &ps);
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    SetBkMode(hdc, TRANSPARENT);    
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    SetTextColor(hdc, 0);
    
    for(int i=0; i<128; i++)
    {
        rc.left = cx*(i/32);
        rc.right = rc.left + cx;
        rc.top = cy*(i%32);
        rc.bottom = rc.top + cy;
        wsprintf(szText, "%03i = %s", i, g_cSeq.m_szInstrument[i]);
        if(i==m_nSel)
        {
            FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
            SetTextColor(hdc, RGB(255,255,255));
            DrawText(hdc, szText, -1, &rc, DT_SINGLELINE|DT_VCENTER);
            SetTextColor(hdc, 0);
        }
        else
            DrawText(hdc, szText, -1, &rc, DT_SINGLELINE|DT_VCENTER);
    }
    
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    SetTextColor(hdc, 0);
    EndPaint(hwnd, &ps);
    return FALSE;
}    


/****************************************************************
* MsgSysCommand : traitement de message WM_SYSCOMMAND
*
****************************************************************/

BOOL CDlgPath::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;  
    }
    return FALSE;
}
