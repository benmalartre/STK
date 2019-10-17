#include <windows.h>
#include "../Resource.h"
#include "DlgOptions.h"

extern HINSTANCE g_hInst;

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgDevice::CDlgDevice():
    m_hDlg(NULL),
    m_pMidi(NULL),
    m_pDevIn(NULL),
    m_pDevOut(NULL)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgDevice::~CDlgDevice()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgDevice::Create(HWND hwndParent, CMidiSphinx* p)
{
    // sauvegarde 
    
    m_pMidi = p;
    
    // création de la boite de dialogue
    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGDEVICE),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgDevice::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgDevice *pDlg = (CDlgDevice*)GetWindowLong(hwnd,GWL_USERDATA);    
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

BOOL CDlgDevice::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    DWORD dwDevice;
    MIDIOUTCAPS moc;
    MIDIINCAPS  mic;
    char szText[256];
    int i, j, iSel;
    
    // sauvegarder le handle de la boite de dialogue    
    m_hDlg = hwnd;
    
    if(m_pDevIn)
        free(m_pDevIn);
    if(m_pDevOut)
        free(m_pDevOut);
    
    // recuperer le nombre de periphérique MIDIOUT du système
    dwDevice = midiOutGetNumDevs();    
    m_pDevOut = (int*)malloc((dwDevice+1)*sizeof(int));
    if(!m_pDevOut)
        return FALSE;
    
    // recuperer le nom de chaque périphérique s'il en existe
    SendDlgItemMessage(hwnd, IDC_CBMIDIOUT, CB_RESETCONTENT, 0, 0);
    iSel = 0;
    for(i=-1, j=0; i<(int)dwDevice; i++)
    {
        if(!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS)))
        {
            SendDlgItemMessage(hwnd, IDC_CBMIDIOUT, CB_ADDSTRING, 0, (LPARAM)moc.szPname);
            m_pDevOut[j] = i;
            if(m_pMidi->m_pInit->mod==i)
                iSel = j;
            j++;
        }    
    }
    if(j>0)
        SendDlgItemMessage(hwnd, IDC_CBMIDIOUT, CB_SETCURSEL, iSel, 0);    
    
    // recuperer le nombre de periphérique MIDIIN du système
    dwDevice = midiInGetNumDevs();
    m_pDevIn = (int*)malloc(dwDevice*sizeof(int));
    if((!m_pDevIn)&&(dwDevice))
        return FALSE;
    
    // recuperer le nom de chaque périphérique s'il en existe
    SendDlgItemMessage(hwnd, IDC_CBMIDIIN, CB_RESETCONTENT, 0, 0);
    iSel = 0;
    for(i=0, j=0; i<(int)dwDevice; i++)
    {
        if(!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS)))
        {
            SendDlgItemMessage(hwnd, IDC_CBMIDIIN, CB_ADDSTRING, 0, (LPARAM)mic.szPname);
            m_pDevIn[j] = i;
            if(m_pMidi->m_pInit->mid==i)
                iSel = j;
            j++;
        }    
    }
    if(j>0)
        SendDlgItemMessage(hwnd, IDC_CBMIDIIN, CB_SETCURSEL, iSel, 0);    
    
    return FALSE;
}

/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgDevice::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[256];
    int iSel;
    switch(LOWORD(wParam))
    {
        case IDOK:
            if(m_pMidi->m_bSequenceGoing)
            {
                MessageBox(hwnd, " Avant de réinitier le périphérique MIDI, vous  \n"
                " devez d'arreter la lecture ou l'enregistrement. ", "Sphinx", 64);
                break;
            }    
            // reinitier le périphérique MIDIOUT
            if(m_pDevOut)
            {
                iSel = SendDlgItemMessage(hwnd, IDC_CBMIDIOUT, CB_GETCURSEL, 0, 0);
                if(m_pMidi->m_hMidiOut)
                    midiOutClose(m_pMidi->m_hMidiOut);
                midiOutOpen(&m_pMidi->m_hMidiOut, m_pDevOut[iSel], 0, 0, 0);
                m_pMidi->m_pInit->mod = m_pDevOut[iSel];            
            }
            // reinitier le périphérique MIDIOUT
            if(m_pDevIn)
            {
                iSel = SendDlgItemMessage(hwnd, IDC_CBMIDIIN, CB_GETCURSEL, 0, 0);
                if(m_pMidi->m_hMidiIn)
                {                
                    m_pMidi->Reset();
                    midiInClose(m_pMidi->m_hMidiIn);
                }    
                m_pMidi->OpenMidiIn(m_pDevOut[iSel]);
                m_pMidi->Start();
                m_pMidi->m_pInit->mid = m_pDevIn[iSel];
            }
            if(m_pDevIn)
                free(m_pDevIn);
            if(m_pDevOut)
                free(m_pDevOut);
            EndDialog(hwnd, 1);
            break;
        
        case IDCANCEL:
            if(m_pDevIn)
                free(m_pDevIn);
            if(m_pDevOut)
                free(m_pDevOut);
            EndDialog(hwnd, 0);
            break;
    }                           
    return FALSE;
}

/****************************************************************
* MsgSysCommand : traitement de message WM_SYSCOMMAND
*
****************************************************************/

BOOL CDlgDevice::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            if(m_pDevIn)
                free(m_pDevIn);
            if(m_pDevOut)
                free(m_pDevOut);
            EndDialog(hwnd, 0);
            return TRUE;  
    }
    return FALSE;
}
