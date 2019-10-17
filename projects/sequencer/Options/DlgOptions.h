/*******************************************************************************
* Je regroupe ici les déclarations des classes utilisées dans le menu options
*
*******************************************************************************/

#ifndef _DLGOPTIONS_H_
#define _DLGOPTIONS_H_

#include "../MidiSphinx.h"

// la classe qui affiche la boite de dialogue pour le réglage de métronome

class CDlgClick
{
public:
    CDlgClick();
    ~CDlgClick();
    
    int Create(HWND, CSequencer*, TInit*);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgSysCommand(HWND,WPARAM);
    
    HWND        m_hDlg;
    CSequencer *m_pSeq;
    TInit      *m_pInit;
};

// classe pour la boite de dialogue Info Sequence

class CDlgInfoPattern
{
public:
    CDlgInfoPattern();
    ~CDlgInfoPattern();
    
    int Create(HWND, CSequencer*);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgSysCommand(HWND,WPARAM);
    
    HWND        m_hDlg;
    CSequencer *m_pSeq;
};

// classe pour la boite de dialogue Peripherique MIDI

class CDlgDevice
{
public:
    CDlgDevice();
    ~CDlgDevice();
    
    int Create(HWND, CMidiSphinx*);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgSysCommand(HWND,WPARAM);
    
    HWND         m_hDlg;
    CMidiSphinx *m_pMidi;
    int          *m_pDevIn, *m_pDevOut;
};

// classe pour la boite de dialogue Options d'enregistrement

class CDlgRecord
{
public:
    CDlgRecord();
    ~CDlgRecord();
    
    int Create(HWND, CMidiSphinx*);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgSysCommand(HWND,WPARAM);
    
    HWND         m_hDlg;
    CMidiSphinx *m_pMidi;
};

#endif
