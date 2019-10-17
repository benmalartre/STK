#include <windows.h>
#include "../Resource.h"
#include "DlgOptions.h"

extern HINSTANCE g_hInst;
const char cstStrNote[12][4] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgClick::CDlgClick():
    m_hDlg(NULL),
    m_pSeq(NULL),
    m_pInit(NULL)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgClick::~CDlgClick()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgClick::Create(HWND hwndParent, CSequencer* p, TInit * pi)
{
    // sauvegarde     
    m_pSeq = p;
    m_pInit = pi;
    
    // création de la boite de dialogue    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGCLICK),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgClick::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgClick *pDlg = (CDlgClick*)GetWindowLong(hwnd,GWL_USERDATA);    
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

BOOL CDlgClick::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[16];
    int i;
    
    // sauvegarder le handle de la boite de dialogue    
    m_hDlg = hwnd;
    
    SendDlgItemMessage(hwnd, IDC_ACTIVE1, BM_SETCHECK, (WPARAM)m_pSeq->m_optClick.bActiveB, 0);
    SendDlgItemMessage(hwnd, IDC_ACTIVE2, BM_SETCHECK, (WPARAM)m_pSeq->m_optClick.bActiveM, 0);
    
    // remplir le combobox note
    
    SendDlgItemMessage(hwnd, IDC_NOTE1, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(hwnd, IDC_NOTE2, CB_RESETCONTENT, 0, 0);
    for(i=0; i<128; i++)
    {
        wsprintf(szText, " %s%i", cstStrNote[i%12], (int)(i/12) - 2);
        SendDlgItemMessage(hwnd, IDC_NOTE1, CB_ADDSTRING, 0, (LPARAM)szText);
        SendDlgItemMessage(hwnd, IDC_NOTE2, CB_ADDSTRING, 0, (LPARAM)szText);
    }
    SendDlgItemMessage(hwnd, IDC_NOTE1, CB_SETCURSEL, m_pSeq->m_optClick.noteB, 0);
    SendDlgItemMessage(hwnd, IDC_NOTE2, CB_SETCURSEL, m_pSeq->m_optClick.noteM, 0);
    
    // remplir le combobox velocité
    
    SendDlgItemMessage(hwnd, IDC_VELOCITY1, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(hwnd, IDC_VELOCITY2, CB_RESETCONTENT, 0, 0);
    for(i=0; i<128; i++)
    {
        wsprintf(szText, " %i", i);
        SendDlgItemMessage(hwnd, IDC_VELOCITY1, CB_ADDSTRING, 0, (LPARAM)szText);
        SendDlgItemMessage(hwnd, IDC_VELOCITY2, CB_ADDSTRING, 0, (LPARAM)szText);
    }
    SendDlgItemMessage(hwnd, IDC_VELOCITY1, CB_SETCURSEL, m_pSeq->m_optClick.velocityB, 0);
    SendDlgItemMessage(hwnd, IDC_VELOCITY2, CB_SETCURSEL, m_pSeq->m_optClick.velocityM, 0);
    
    // remplir le combobox canal
    
    SendDlgItemMessage(hwnd, IDC_CHANNEL1, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(hwnd, IDC_CHANNEL2, CB_RESETCONTENT, 0, 0);
    for(i=1; i<17; i++)
    {
        wsprintf(szText, " %i", i);
        SendDlgItemMessage(hwnd, IDC_CHANNEL1, CB_ADDSTRING, 0, (LPARAM)szText);
        SendDlgItemMessage(hwnd, IDC_CHANNEL2, CB_ADDSTRING, 0, (LPARAM)szText);
    }
    SendDlgItemMessage(hwnd, IDC_CHANNEL1, CB_SETCURSEL, m_pSeq->m_optClick.channelB, 0);
    SendDlgItemMessage(hwnd, IDC_CHANNEL2, CB_SETCURSEL, m_pSeq->m_optClick.channelM, 0);
    
    CheckDlgButton(hwnd, IDC_INCLUDE, BST_CHECKED);
    return FALSE;
}

/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgClick::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
        case IDOK:
            // changer les options de metronomes
            m_pSeq->m_optClick.bActiveB = (BOOL)SendDlgItemMessage(hwnd, IDC_ACTIVE1,
                BM_GETCHECK, 0, 0);
            m_pSeq->m_optClick.bActiveM = (BOOL)SendDlgItemMessage(hwnd, IDC_ACTIVE2,
                BM_GETCHECK, 0, 0);
            m_pSeq->m_optClick.noteB = SendDlgItemMessage(hwnd, IDC_NOTE1, CB_GETCURSEL, 0, 0);
            m_pSeq->m_optClick.noteM = SendDlgItemMessage(hwnd, IDC_NOTE2, CB_GETCURSEL, 0, 0);
            m_pSeq->m_optClick.channelB = SendDlgItemMessage(hwnd, IDC_CHANNEL1, 
                CB_GETCURSEL, 0, 0);
            m_pSeq->m_optClick.channelM = SendDlgItemMessage(hwnd, IDC_CHANNEL2, 
                CB_GETCURSEL, 0, 0);
            m_pSeq->m_optClick.velocityB = SendDlgItemMessage(hwnd, IDC_VELOCITY1, 
                CB_GETCURSEL, 0, 0);
            m_pSeq->m_optClick.velocityM = SendDlgItemMessage(hwnd, IDC_VELOCITY2, 
                CB_GETCURSEL, 0, 0);
            
            if(SendDlgItemMessage(hwnd, IDC_INCLUDE, BM_GETCHECK, 0, 0))
            {
                m_pInit->optClick.bActiveB = m_pSeq->m_optClick.bActiveB;
                m_pInit->optClick.bActiveM = m_pSeq->m_optClick.bActiveM;
                m_pInit->optClick.noteB = m_pSeq->m_optClick.noteB;
                m_pInit->optClick.noteM = m_pSeq->m_optClick.noteM;
                m_pInit->optClick.channelB = m_pSeq->m_optClick.channelB;
                m_pInit->optClick.channelM = m_pSeq->m_optClick.channelM;
                m_pInit->optClick.velocityB = m_pSeq->m_optClick.velocityB;
                m_pInit->optClick.velocityM = m_pSeq->m_optClick.velocityM;
            }    
            EndDialog(hwnd, 1);
            break;
    }                           
    return FALSE;
}

/****************************************************************
* MsgSysCommand : traitement de message WM_SYSCOMMAND
*
****************************************************************/

BOOL CDlgClick::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;  
    }
    return FALSE;
}
