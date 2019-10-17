#ifndef _MIDI_SPHINX_H_
#define _MIDI_SPHINX_H_

#include "Sequencer.h"

struct TListEvent
{
    DWORD dwTimeEvent;
    DWORD dwMessage;
};


struct TMidiTrack
{
    TListEvent * pEvent;
    int          arraySize;
    int          count;
    int          indexCurrent;
    int          indexStart;
    int          indexFirstLoop;
    int          indexLastLoop;
        TMidiTrack():
            pEvent(0),
            arraySize(0),
            count(0),
            indexCurrent(0),
            indexStart(0),
            indexFirstLoop(0),
            indexLastLoop(0)
            {}
};

class CMidiSphinx
{
public:
    CMidiSphinx(TGlobal*, TInit*);
    ~CMidiSphinx();    
    
    BOOL  Open(CSequencer*);
    BOOL  OpenMidiIn(int);
    void  Close();
    void  Start(); 
    void  Reset();
    void  UpdatePrgmChange(int, int);
    void  PrepareDefaultChannel(int);
    void  NoteOn(BYTE, BYTE);
    void  NoteOff();
    
    BOOL  Begin(BOOL);
    void  End();
    
    void  ExtractListEvent();
    void  SetProgramChange(BYTE, BYTE);
    void  SetCurrentValue(DWORD);

    HMIDIOUT   m_hMidiOut;
    HMIDIIN    m_hMidiIn;
    UINT       m_uTimerRes;
    UINT       m_uTimerId;
    UINT       m_uTimerId1;
    UINT       m_uTimerClock;
    DWORD      m_dwTimerCount;
    BOOL       m_bSequenceGoing;
    BOOL       m_bRecording;
    BOOL       m_bStop;
    
    TMidiTrack  m_mTrack[MAX_CHANNEL];    
    DWORD       m_dwCurrentTime;
    DWORD       m_dwCurrentClock;
    int         m_nCurrentTempo;
    BYTE        m_byCurrentSignature[4];
    
    int         m_sendClick;
    
    CSequencer *m_pSeq;
    TInit      *m_pInit;
    
    // pour l'enregistrement
    void  FilterEvents(DWORD*); // filtre les evenements selon l'option d'enregistrement
    void  AddRecordEvent(TListEvent*);
    void  FreeRecordEvent();
    
    POINT       m_ptRecord[MAX_CHANNEL]; // les segments min et max de sequence d'enregistrement
    TMidiTrack  m_trackRecord; // liste des evenements entrants durant l'enregistrement
    BOOL        m_bStateClick; // etat de bouton click avant enregistrement
    
private:
    int   SeekIndexCurrent(int, DWORD);
    void  SeekIndexListEvent(int, DWORD);
    void  SortListEvent(int);
    void  AddListEvent(int, TListEvent*);
    void  FreeListEvent();
    
    // pour l'enregistrement
    BOOL  ExtractRecordEvent(int);
    int   RoundTimeEvent(DWORD, int);

private:
    BYTE       m_byOldNote;
    BYTE       m_byOldChannel;
    TGlobal  * m_pGlobal;    
};

#endif
