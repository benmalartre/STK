#ifndef _DLGWAVEIN_H_
#define _DLGWAVEIN_H_

#include "MidiSphinx.h"
#include "Mixer.h"

#define MAX_BUFFER 4
#define SIZE_BUFFER 44100

class CDlgWaveIn
{
public:
    CDlgWaveIn();
    ~CDlgWaveIn();
    
    int Create(HWND, CMidiSphinx*, char*);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgTimer(HWND);
    BOOL MsgDestroy(HWND);
    BOOL MsgUser(HWND, WPARAM);
    BOOL MsgSysCommand(HWND,WPARAM);
    BOOL MsgCtlColorStatic(HWND, LPARAM);
    BOOL MsgMmWimOpen(HWND);
    BOOL MsgMmWimData(HWND, LPARAM);
    BOOL MsgMmWimClose(HWND);
    BOOL CmdCancel(HWND);
    BOOL CmdOk(HWND);
    
    CMidiSphinx * m_pMidi;
    CMixer        m_mix;
    HWND          m_hDlg;
    HWAVEIN       m_hWaveIn;
    WAVEFORMATEX  m_wfx;
    WAVEHDR       m_whdr[MAX_BUFFER];
    short         m_buffer[MAX_BUFFER][SIZE_BUFFER/2];
    short         m_bufferTmp[SIZE_BUFFER/2];
    DWORD         m_dwWritten;
    DWORD         m_dwDataSize;
    char        * m_pFileName;
    HANDLE        m_hFile;
    BOOL          m_bStop;
    BOOL          m_bGo;    
};

#endif
