#include <windows.h>
#include "Resource.h"
#include "DlgHelp.h"

extern HINSTANCE g_hInst;

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgHelp::CDlgHelp():
    m_hDlg(NULL)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgHelp::~CDlgHelp()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgHelp::Create(HWND hwndParent)
{
    // création de la boite de dialogue
    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGHELP),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgHelp::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgHelp *pDlg = (CDlgHelp*)GetWindowLong(hwnd,GWL_USERDATA);    
    if(!pDlg)
        return FALSE;
    
    switch(message)
    {
        case WM_INITDIALOG:
            return pDlg->MsgInitDialog(hwnd,wParam,lParam);            
        case WM_COMMAND:
            return pDlg->MsgCommand(hwnd,wParam,lParam);
        case WM_CTLCOLORSTATIC:
            return pDlg->MsgCtlColorStatic(hwnd,wParam,lParam);
        case WM_SYSCOMMAND:
            return pDlg->MsgSysCommand(hwnd,wParam);
    }
    return FALSE;
}

/****************************************************************
* MsgInitDialog : traitement de message WM_INITDIALOG
*
****************************************************************/

BOOL CDlgHelp::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // sauvegarder le handle de la boite de dialogue    
    m_hDlg = hwnd;
    RECT rc;
    char szText[1024];
    
    GetClientRect(hwnd, &rc);
    m_index = 0;
    MoveWindow(GetDlgItem(hwnd, IDC_EDITHELP), 0, 0, rc.right, rc.bottom - 28, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDCANCEL), rc.right - 140, rc.bottom - 25, 120, 22, TRUE);
    SendDlgItemMessage(hwnd,IDC_EDITHELP,WM_SETFONT,(WPARAM)GetStockObject(ANSI_FIXED_FONT),1L);
    LoadString(g_hInst, 30100, szText, 1024);
    SetDlgItemText(hwnd, IDC_EDITHELP, szText);
    return FALSE;
}

/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgHelp::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[1024];
    if((LOWORD(wParam)>=IDM_HELP_FIRST)&&(LOWORD(wParam)<=IDM_HELP_LAST))
    {
        LoadString(g_hInst, LOWORD(wParam)+10000, szText, 1024);
        SetDlgItemText(hwnd, IDC_EDITHELP, szText);
        m_index = LOWORD(wParam);
    }
    else
    {
        switch(LOWORD(wParam))
        {
            case IDM_HELP_BACK:
                if(m_index==0)
                    m_index = IDM_HELP_LAST;
                else
                {
                    m_index--;
                    if(m_index<IDM_HELP_FIRST)
                    {
                        m_index = 0;
                        LoadString(g_hInst, 30100, szText, 1024);
                        SetDlgItemText(hwnd, IDC_EDITHELP, szText);
                        break;
                    }
                }
                SendMessage(hwnd, WM_COMMAND, (WPARAM)m_index, 0);
                break;
            case IDM_HELP_NEXT:
                if(m_index==0)
                    m_index = IDM_HELP_FIRST;
                else
                {
                    m_index++;
                    if(m_index>IDM_HELP_LAST)
                    {
                        m_index = 0;
                        LoadString(g_hInst, 30100, szText, 1024);
                        SetDlgItemText(hwnd, IDC_EDITHELP, szText);
                        break;
                    }
                }
                SendMessage(hwnd, WM_COMMAND, (WPARAM)m_index, 0);
                break;
            case IDCANCEL:
                EndDialog(hwnd, 0);
                break;
            default:
                LoadString(g_hInst, 30100, szText, 1024);
                SetDlgItemText(hwnd, IDC_EDITHELP, szText);
                break;
        }    
    }                           
    return FALSE;
}

/****************************************************************
* MsgCtlColorStatic : traitement de message WM_CTLCOLORSTATIC
*
****************************************************************/

BOOL CDlgHelp::MsgCtlColorStatic(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    return (BOOL)GetStockObject(WHITE_BRUSH);
}    

/****************************************************************
* MsgSysCommand : traitement de message WM_SYSCOMMAND
*
****************************************************************/

BOOL CDlgHelp::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;  
    }
    return FALSE;
}
