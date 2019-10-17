#include <windows.h>
#include "../Resource.h"
#include "DlgOptions.h"

extern HINSTANCE g_hInst;

/*************************************************************************
* RTrim : fonction qui supprime le caractère espace à la fin d'une chaine
*
*************************************************************************/

void RTrim(char * pText)
{
    if(!pText)
        return;
    
    int i = lstrlen(pText) - 1;
    while((i>=0)&&(pText[i]==' '))
        i--;
    pText[i+1] = 0;
}

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgInfoPattern::CDlgInfoPattern():
    m_hDlg(NULL),
    m_pSeq(NULL)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgInfoPattern::~CDlgInfoPattern()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgInfoPattern::Create(HWND hwndParent, CSequencer* p)
{
    // sauvegarde 
    
    m_pSeq = p;
    
    // création de la boite de dialogue
    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGINFO),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgInfoPattern::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgInfoPattern *pDlg = (CDlgInfoPattern*)GetWindowLong(hwnd,GWL_USERDATA);    
    if(!pDlg)
        return FALSE;
    
    switch(message)
    {
        case WM_INITDIALOG:
            return pDlg->MsgInitDialog(hwnd,wParam,lParam);
            
        case WM_COMMAND:
            return pDlg->MsgCommand(hwnd,wParam,lParam);
        
        case WM_SYSCOMMAND:
            return pDlg->MsgSysCommand(hwnd,wParam);
    }
    return FALSE;
}

/****************************************************************
* MsgInitDialog : traitement de message WM_INITDIALOG
*
****************************************************************/

BOOL CDlgInfoPattern::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[256];
    int i;
    
    // sauvegarder le handle de la boite de dialogue    
    m_hDlg = hwnd;
    
    m_pSeq->GetTextEvent(META_LYRIC, szText);
    SetDlgItemText(hwnd, IDC_LYRIC, szText);
    
    m_pSeq->GetTextEvent(META_TEXT, szText);
    SetDlgItemText(hwnd, IDC_TEXT, szText);
    
    m_pSeq->GetTextEvent(META_COPYRIGHT, szText);
    SetDlgItemText(hwnd, IDC_COPYRIGHT, szText);
    
    m_pSeq->GetTextEvent(META_CUEPOINT, szText);
    SetDlgItemText(hwnd, IDC_CUEPOINT, szText);
    
    m_pSeq->GetTextEvent(META_MARKER, szText);
    SetDlgItemText(hwnd, IDC_MARKER, szText);
    
    SendDlgItemMessage(hwnd, IDC_INCLUDE, BM_SETCHECK, (WPARAM)m_pSeq->IsInfoInclude(), 0);
    
    SetFocus(GetDlgItem(hwnd,IDOK));
    return FALSE;
}

/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgInfoPattern::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[256];
    BOOL b;
    switch(LOWORD(wParam))
    {
        case IDOK:
            GetDlgItemText(hwnd, IDC_LYRIC, szText, 255);
            RTrim(szText);
            m_pSeq->SetTextEvent(META_LYRIC, szText, lstrlen(szText));
            
            GetDlgItemText(hwnd, IDC_TEXT, szText, 255);
            RTrim(szText);
            m_pSeq->SetTextEvent(META_TEXT, szText, lstrlen(szText));
            
            GetDlgItemText(hwnd, IDC_COPYRIGHT, szText, 255);
            RTrim(szText);
            m_pSeq->SetTextEvent(META_COPYRIGHT, szText, lstrlen(szText));
            
            GetDlgItemText(hwnd, IDC_MARKER, szText, 255);
            RTrim(szText);
            m_pSeq->SetTextEvent(META_MARKER, szText, lstrlen(szText));
            
            GetDlgItemText(hwnd, IDC_CUEPOINT, szText, 255);
            RTrim(szText);
            m_pSeq->SetTextEvent(META_CUEPOINT, szText, lstrlen(szText));
            
            b = (BOOL)SendDlgItemMessage(hwnd, IDC_INCLUDE, BM_GETCHECK, 0, 0);
            m_pSeq->SetInfoInclude(b);
            EndDialog(hwnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
    }                           
    return FALSE;
}

/****************************************************************
* MsgSysCommand : traitement de message WM_SYSCOMMAND
*
****************************************************************/

BOOL CDlgInfoPattern::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;  
    }
    return FALSE;
}
