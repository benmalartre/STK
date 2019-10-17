#include <windows.h>
#include <commctrl.h>
#include "Resource.h"
#include "DlgWaveIn.h"

#define WM_USER_INFO    WM_USER+1
#define INFO_SUCCESS    1
#define INFO_INTERRUPT  2

extern HINSTANCE g_hInst;
extern char g_szWndTitle[];

const DWORD cstFrequency[3] = {44100,22050,11025};

/**********************************************************************
* Constructeur
*
**********************************************************************/

CDlgWaveIn::CDlgWaveIn():
    m_bStop(1),
    m_bGo(0),
    m_hDlg(NULL),
    m_hWaveIn(NULL),
    m_hFile(INVALID_HANDLE_VALUE)
{
}

/**********************************************************************
* Destructeur
*
**********************************************************************/

CDlgWaveIn::~CDlgWaveIn()
{
}

/*********************************************************************
* Create : 
*
*********************************************************************/

int CDlgWaveIn::Create(HWND hwndParent, CMidiSphinx * p, char * pFile)
{    
    m_pMidi = p;
    m_pFileName = pFile;
    
    if(!pFile)
        return 0;
    if(!m_mix.Init())
    {
        MessageBox(hwndParent, "  Carte son introuvable !!!", g_szWndTitle, 64);
        return 0;
    }
    // séléctionner le mixage stéreo du mixeur windows
    if(m_mix.GetStereoMixControl())
        m_mix.SetStereoMixValue(1);    
    
    // création de la boite de dialogue    
    return DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGWAVEIN),hwndParent,DlgProc,(LPARAM)this);
}

/*****************************************************
* DlgProc : traitement des messages de la boite de
* dialogue
*
*****************************************************/

BOOL CALLBACK CDlgWaveIn::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_INITDIALOG)
        SetWindowLong(hwnd,GWL_USERDATA,lParam);
    
    CDlgWaveIn *pDlg = (CDlgWaveIn*)GetWindowLong(hwnd,GWL_USERDATA);    
    if(!pDlg)
        return FALSE;
    
    switch(message)
    {
        case WM_INITDIALOG:
            return pDlg->MsgInitDialog(hwnd,wParam,lParam);
        case WM_TIMER:
            return pDlg->MsgTimer(hwnd);
        case WM_COMMAND:
            return pDlg->MsgCommand(hwnd,wParam,lParam);
        case WM_USER_INFO:
            return pDlg->MsgUser(hwnd,wParam);
        case WM_CTLCOLORSTATIC:
            return pDlg->MsgCtlColorStatic(hwnd,lParam);
        case MM_WIM_OPEN:
            return pDlg->MsgMmWimOpen(hwnd);
        case MM_WIM_DATA:
            return pDlg->MsgMmWimData(hwnd, lParam);
        case MM_WIM_CLOSE:
            return pDlg->MsgMmWimClose(hwnd);
        case WM_DESTROY:
            return pDlg->MsgDestroy(hwnd);
        case WM_SYSCOMMAND:
            return pDlg->MsgSysCommand(hwnd,wParam);
    }
    return FALSE;
}

/****************************************************************
* MsgInitDialog : traitement de message WM_INITDIALOG
*
****************************************************************/

BOOL CDlgWaveIn::MsgInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    int i;
    char szText[120];
    // sauvegarder le handle de la boite de dialogue
    
    m_hDlg = hwnd;
    
    // remplir le combobox taux d'échantillon    
    SendDlgItemMessage(hwnd, IDC_FREQUENCY, CB_RESETCONTENT, 0, 0);
    for(i=0; i<3; i++)
    {
        wsprintf(szText, " %i Hz", (int)cstFrequency[i]);
        SendDlgItemMessage(hwnd, IDC_FREQUENCY, CB_ADDSTRING, 0, (LPARAM)szText);
    }
    SendDlgItemMessage(hwnd, IDC_FREQUENCY, CB_SETCURSEL, 0, 0);
    
    // remplir le combobox résolution    
    SendDlgItemMessage(hwnd, IDC_RESOLUTION, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(hwnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)" 16 bits");
    SendDlgItemMessage(hwnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)"  8 bits");
    SendDlgItemMessage(hwnd, IDC_RESOLUTION, CB_SETCURSEL, 0, 0);
    
    // activer le mode stéréo    
    CheckDlgButton(hwnd, IDC_MODE, BST_CHECKED);
    
    // afficher la debut de postion du pointeur
    int start, end;
    m_pMidi->m_pSeq->TimeCodeToString(m_pMidi->m_dwCurrentTime, szText);
    SetDlgItemText(hwnd, IDC_STARTEDIT, szText);
    start = (int)m_pMidi->m_dwCurrentTime;
    // afficher la fin de position du pointeur
    if((DWORD)m_pMidi->m_pSeq->GetLastLocator()>m_pMidi->m_dwCurrentTime)
    {
        m_pMidi->m_pSeq->TimeCodeToString((DWORD)m_pMidi->m_pSeq->GetLastLocator(), szText);
        end = m_pMidi->m_pSeq->GetLastLocator();
    }
    else
        end = start+1;
            
    SetDlgItemText(hwnd, IDC_ENDEDIT, szText);
    
    // initialisation de ProgressBar
    SendDlgItemMessage(hwnd, IDC_PROGRESS, PBM_SETRANGE32, (WPARAM)start, (LPARAM)end);
    SendDlgItemMessage(hwnd, IDC_PROGRESS, PBM_SETPOS, (WPARAM)start, 0);
    SetTimer(hwnd, 1, 100, NULL);
    
    return FALSE;
}

/****************************************************************
* MsgTimer : traitement de message WM_TIMER
*
****************************************************************/

BOOL CDlgWaveIn::MsgTimer(HWND hwnd)
{
    // met à jour le ProgressBar selon le progressement de l'enregistrement
    SendDlgItemMessage(hwnd, IDC_PROGRESS, PBM_SETPOS, (WPARAM)(int)m_pMidi->m_dwCurrentTime, 0);
    
    // arrete l'enregistrement si depasse le dernier location de pointeur
    if(((DWORD)m_pMidi->m_pSeq->GetLastLocator()<=m_pMidi->m_dwCurrentTime)&&(m_bGo))
    {
        KillTimer(hwnd, 1);
        waveInReset(m_hWaveIn);
        m_bStop = TRUE;
        PostMessage(hwnd, WM_USER_INFO, (WPARAM)INFO_SUCCESS, 0);
    }    
    return TRUE;
}

/****************************************************************
* MsgUser : traitement de message WM_USER
*
****************************************************************/

BOOL CDlgWaveIn::MsgUser(HWND hwnd, WPARAM wParam)
{
    switch(wParam)
    {
        case INFO_INTERRUPT:
            if((MessageBox(hwnd, " Enregistrement interrompu!  Voulez-vous garder  \n"
            " l'enregistrement effectué? ", g_szWndTitle, 68)==IDNO)&&(m_pFileName))
                DeleteFile(m_pFileName);
            break;
        case INFO_SUCCESS:
            SetDlgItemText(hwnd, IDCANCEL, "Fermer");
            MessageBox(hwnd, " Enregistrement reussi!  ", g_szWndTitle, MB_ICONINFORMATION);            
            if(!m_bGo)
                EndDialog(hwnd, 0);
            break;        
    }    
    return TRUE;
}    


/****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
****************************************************************/

BOOL CDlgWaveIn::MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
        case IDCANCEL:
            return CmdCancel(hwnd);
        case IDOK:
            return CmdOk(hwnd);
    }
    return FALSE;
}

/****************************************************************
* MsgDestroy : traitement de message WM_DESTROY
*
****************************************************************/

BOOL CDlgWaveIn::MsgDestroy(HWND hwnd)
{    
    KillTimer(hwnd, 1);
    m_mix.Close();
    return TRUE;
}


/****************************************************************
* MsgSysCommand : traitement de message WM_SYSCOMMAND
*
****************************************************************/

BOOL CDlgWaveIn::MsgSysCommand(HWND hwnd, WPARAM wParam)
{    
    switch(LOWORD(wParam))
    {
        case SC_CLOSE:
            CmdCancel(hwnd);
            return TRUE;
        case SC_SCREENSAVE:
            return TRUE;
    }
    return FALSE;
}

/****************************************************************
* MsgCtlColorStatic : traitement de message WM_CTLCOLORSTATIC
*
****************************************************************/

BOOL CDlgWaveIn::MsgCtlColorStatic(HWND hwnd, LPARAM lParam)
{
    int id = GetWindowLong((HWND)lParam, GWL_ID);
    // modifie la couleur des controles static en blanc
    if((id==IDC_STARTEDIT)||(id==IDC_ENDEDIT))
        return (BOOL)GetStockObject(WHITE_BRUSH);
    return FALSE;
}

/****************************************************************
* CmdCancel : traitement de commande IDCANCEL
*
****************************************************************/

BOOL CDlgWaveIn::CmdCancel(HWND hwnd)
{
    m_pMidi->End();
    
    if(m_bGo)
    {
        waveInReset(m_hWaveIn);
        m_bStop = TRUE;
        SetDlgItemText(hwnd, IDCANCEL, "Fermer");
        PostMessage(hwnd, WM_USER_INFO, (WPARAM)INFO_INTERRUPT, 0);
    }
    else
        EndDialog(hwnd, 0);
    return FALSE;
}

/****************************************************************
* CmdOk : traitement de commande IDOK
*
****************************************************************/

BOOL CDlgWaveIn::CmdOk(HWND hwnd)
{
    DWORD dwTaux;
    WORD  wChannels, wBits;
    
    dwTaux = cstFrequency[SendDlgItemMessage(hwnd, IDC_FREQUENCY, CB_GETCURSEL, 0, 0)];
    if(SendDlgItemMessage(hwnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)==0)
        wBits = (WORD)16;
    else
        wBits = (WORD)8;
    
    if(IsDlgButtonChecked(hwnd, IDC_MODE))
        wChannels = 2;
    else
        wChannels = 1;
    
    m_pMidi->ExtractListEvent();
    
    m_wfx.cbSize          = 0;
    m_wfx.wFormatTag      = WAVE_FORMAT_PCM;
    m_wfx.nSamplesPerSec  = dwTaux;
    m_wfx.nChannels       = wChannels;
    m_wfx.wBitsPerSample  = wBits;
    m_wfx.nBlockAlign     = (m_wfx.nChannels*m_wfx.wBitsPerSample)/8;
    m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec*m_wfx.nBlockAlign;
    
    if(waveInOpen(&m_hWaveIn, 0, &m_wfx, (DWORD)hwnd, 0, CALLBACK_WINDOW)!=MMSYSERR_NOERROR)
    {
        MessageBox(hwnd, " Erreur d'enregistrement! ", " Erreur", MB_ICONERROR);
        return FALSE;
    }
    
    EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
    SetDlgItemText(hwnd, IDCANCEL, "Annuler");
    
    return TRUE;
}

BOOL CDlgWaveIn::MsgMmWimOpen(HWND hwnd)
{
    DWORD dwBuf = 0;
    
    m_hFile = CreateFile(m_pFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if(m_hFile==INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, " On ne peut pas ouvrir le fichier  ", "Erreur", 16);
        waveInClose(m_hWaveIn);
        return FALSE;
    }
    
    // L'entête du fichier wave
    
    WriteFile(m_hFile, "RIFF", 4, &m_dwWritten, NULL);
    WriteFile(m_hFile, &dwBuf, 4, &m_dwWritten, NULL);
    WriteFile(m_hFile, "WAVE", 4, &m_dwWritten, NULL);
    WriteFile(m_hFile, "fmt ", 4, &m_dwWritten, NULL);
    dwBuf = sizeof(PCMWAVEFORMAT);
    WriteFile(m_hFile, &dwBuf, 4, &m_dwWritten, NULL);
    WriteFile(m_hFile, &m_wfx, dwBuf, &m_dwWritten, NULL);
    WriteFile(m_hFile, "data", 4, &m_dwWritten, NULL);
    m_dwDataSize = 0;
    WriteFile(m_hFile, &m_dwDataSize, 4, &m_dwWritten, NULL);
    
    for(int i=0; i<MAX_BUFFER; i++)
    {
        ZeroMemory(&m_whdr[i], sizeof(WAVEHDR));
        m_whdr[i].lpData         = (LPSTR)m_buffer[i];
        m_whdr[i].dwBufferLength = m_wfx.nSamplesPerSec;
        m_whdr[i].dwFlags        = WHDR_DONE;
        
        waveInPrepareHeader(m_hWaveIn, &m_whdr[i], sizeof(WAVEHDR));
        waveInAddBuffer(m_hWaveIn, &m_whdr[i], sizeof(WAVEHDR));
    }
    
    m_bGo = TRUE;
    m_bStop = FALSE;
    waveInStart(m_hWaveIn);
    m_pMidi->Begin(FALSE);
    return TRUE;
}

BOOL CDlgWaveIn::MsgMmWimClose(HWND hwnd)
{
    DWORD dwBuf = 0;
    
    if(m_bGo)
        for(int i=0; i<MAX_BUFFER; i++)
            waveInUnprepareHeader(m_hWaveIn, &m_whdr[i], sizeof(WAVEHDR));
    
    m_bGo = FALSE;
    
    if(m_hFile!=INVALID_HANDLE_VALUE)
    {
        dwBuf = 20 + sizeof(PCMWAVEFORMAT) + m_dwDataSize;
        SetFilePointer(m_hFile, 4, NULL, FILE_BEGIN);
        WriteFile(m_hFile, &dwBuf, 4, &m_dwWritten, NULL);
        SetFilePointer(m_hFile, 24 + sizeof(PCMWAVEFORMAT), NULL, FILE_BEGIN);
        WriteFile(m_hFile, &m_dwDataSize, 4, &m_dwWritten, NULL);
        CloseHandle(m_hFile);
    }    
    m_hFile = INVALID_HANDLE_VALUE;
    return TRUE;
}    


BOOL CDlgWaveIn::MsgMmWimData(HWND hwnd, LPARAM lParam)
{
    PWAVEHDR p = (PWAVEHDR)lParam;
    
    WriteFile(m_hFile, p->lpData, p->dwBytesRecorded, &m_dwWritten, NULL);
    m_dwDataSize += m_dwWritten;
    
    if(m_bStop)
        waveInClose(m_hWaveIn);
    else
        waveInAddBuffer(m_hWaveIn, p, sizeof(WAVEHDR));
    return TRUE;
}    
