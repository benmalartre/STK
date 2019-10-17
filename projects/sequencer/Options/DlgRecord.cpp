#include <windows.h>
#include "../Resource.h"
#include "DlgOptions.h"

extern HINSTANCE g_hInst;
const char szQuant[8][4] = {"1", "2", "4", "8", "16", "32", "64", "off"};

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgRecord::CDlgRecord():
    m_hDlg(NULL),
    m_pMidi(NULL)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgRecord::~CDlgRecord()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgRecord::Create(HWND hwndParent, CMidiSphinx* p)
{
    // sauvegarde 
    
    m_pMidi = p;
    
    // création de la boite de dialogue
    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGRECORD),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgRecord::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgRecord *pDlg = (CDlgRecord*)GetWindowLong(hwnd,GWL_USERDATA);    
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

BOOL CDlgRecord::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // sauvegarder le handle de la boite de dialogue    
    m_hDlg = hwnd;
    // initialisation des controles
    CheckDlgButton(hwnd, IDC_ONETRACK, m_pMidi->m_pInit->bDemix ? BST_UNCHECKED : BST_CHECKED);
    CheckDlgButton(hwnd, IDC_MORETRACK, m_pMidi->m_pInit->bDemix ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_ONCLICK, m_pMidi->m_pInit->bClick ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FUSION, m_pMidi->m_pInit->bFusion ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_ONQUANTISE, 
        m_pMidi->m_pInit->bQuantise ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(hwnd, IDC_CBQUANTISE), 
        m_pMidi->m_pInit->bQuantise ? TRUE : FALSE);
    CheckDlgButton(hwnd, IDC_FILTER_NOTE, 
        (m_pMidi->m_pInit->uFilter & FILTER_NOTE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FILTER_KEYPRESS, 
        (m_pMidi->m_pInit->uFilter & FILTER_KEYPRESS) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FILTER_CTRLCHANGE, 
        (m_pMidi->m_pInit->uFilter & FILTER_CTRLCHANGE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FILTER_PRGMCHANGE, 
        (m_pMidi->m_pInit->uFilter & FILTER_PRGMCHANGE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FILTER_CHANPRESS, 
        (m_pMidi->m_pInit->uFilter & FILTER_CHANPRESS) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FILTER_PITCHBEND, 
        (m_pMidi->m_pInit->uFilter & FILTER_PITCHBEND) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_FILTER_SYSEX, 
        (m_pMidi->m_pInit->uFilter & FILTER_SYSEX) ? BST_CHECKED : BST_UNCHECKED);
    // remplir combobox
    SendDlgItemMessage(hwnd, IDC_CBQUANTISE, CB_RESETCONTENT, 0, 0);
    for(int i=0; i<8; i++)
        SendDlgItemMessage(hwnd, IDC_CBQUANTISE, CB_ADDSTRING, 0, (LPARAM)szQuant[i]);
    SendDlgItemMessage(hwnd, IDC_CBQUANTISE, CB_SETCURSEL, m_pMidi->m_pInit->nQuantise, 0);
    return FALSE;
}

/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgRecord::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[256];
    int iSel;
    switch(LOWORD(wParam))
    {
        case IDC_ONQUANTISE:
            m_pMidi->m_pInit->bQuantise = SendDlgItemMessage(hwnd, IDC_ONQUANTISE,
                BM_GETCHECK, 0, 0);
            EnableWindow(GetDlgItem(hwnd, IDC_CBQUANTISE), 
                m_pMidi->m_pInit->bQuantise ? TRUE : FALSE);
            break;
        case IDOK:
            m_pMidi->m_pInit->nQuantise = SendDlgItemMessage(hwnd, IDC_CBQUANTISE, 
                CB_GETCURSEL, 0, 0);
            m_pMidi->m_pInit->bDemix = SendDlgItemMessage(hwnd, IDC_MORETRACK,
                BM_GETCHECK, 0, 0);
            m_pMidi->m_pInit->bClick = SendDlgItemMessage(hwnd, IDC_ONCLICK,
                BM_GETCHECK, 0, 0);
            m_pMidi->m_pInit->bFusion = SendDlgItemMessage(hwnd, IDC_FUSION,
                BM_GETCHECK, 0, 0);
            m_pMidi->m_pInit->uFilter = 0;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_NOTE, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_NOTE;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_KEYPRESS, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_KEYPRESS;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_CTRLCHANGE, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_CTRLCHANGE;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_PRGMCHANGE, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_PRGMCHANGE;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_CHANPRESS, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_CHANPRESS;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_PITCHBEND, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_PITCHBEND;
            if(SendDlgItemMessage(hwnd, IDC_FILTER_SYSEX, BM_GETCHECK, 0, 0))
                m_pMidi->m_pInit->uFilter |= FILTER_SYSEX;
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

BOOL CDlgRecord::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;  
    }
    return FALSE;
}
