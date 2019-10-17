#include <windows.h>
#include <mmsystem.h>
#include "MidiSphinx.h"

#define   HORLOGE_TICKS  24

const BYTE cstProgramDefault[16]={0,33,3,4,48,50,11,28,16,0,7,45,61,73,83,97};

void CALLBACK TimerFunc(UINT, UINT, DWORD, DWORD, DWORD);
void CALLBACK Timer1Func(UINT, UINT, DWORD, DWORD, DWORD);
void CALLBACK MidiInFunc(HMIDIIN, UINT, DWORD, DWORD, DWORD);
int   __cdecl Compare(const void*, const void*);

/********************************************************************
* constructeur
*
********************************************************************/

CMidiSphinx::CMidiSphinx(TGlobal * p, TInit * pi):
    m_hMidiOut(0),
    m_hMidiIn(0),
    m_uTimerRes(0),
    m_uTimerId(0),
    m_uTimerClock(16),
    m_dwCurrentTime(0),
    m_dwCurrentClock(0),
    m_nCurrentTempo(120),
    m_pSeq(NULL),
    m_bSequenceGoing(FALSE),
    m_bRecording(FALSE),
    m_bStop(FALSE)
{
    m_byCurrentSignature[0] = 4;
    m_byCurrentSignature[1] = 2;
    m_byCurrentSignature[2] = 8;
    m_byCurrentSignature[3] = 24;
    m_pGlobal = p;
    m_pInit = pi;
    ZeroMemory(&m_ptRecord[0], sizeof(POINT)*MAX_CHANNEL);
}

/********************************************************************
* destructeur :
*
********************************************************************/

CMidiSphinx::~CMidiSphinx()
{
    Reset();
}       

BOOL CMidiSphinx::Open(CSequencer * p)
{
    m_pSeq = p;
    m_pSeq->GetTimeSignature(0, m_byCurrentSignature);
    m_nCurrentTempo = m_pSeq->GetTempo(0);
    
    midiInOpen(&m_hMidiIn, m_pInit->mid, (DWORD)MidiInFunc, (DWORD)this, CALLBACK_FUNCTION);
    return (BOOL)midiOutOpen(&m_hMidiOut, m_pInit->mod, 0, 0, 0);
}

BOOL CMidiSphinx::OpenMidiIn(int iDevice)
{
    return (BOOL)midiInOpen(&m_hMidiIn, iDevice, (DWORD)MidiInFunc, 
        (DWORD)this, CALLBACK_FUNCTION);
}    

void CMidiSphinx::Close()
{
    for(int i=0; i<MAX_CHANNEL; i++)
        if(m_mTrack[i].pEvent)
            free(m_mTrack[i].pEvent);
    FreeRecordEvent();
    if(m_hMidiOut)
    {
        midiOutClose(m_hMidiOut);
        m_hMidiOut = 0;
    }    
    if(m_hMidiIn)
    {    
        midiInClose(m_hMidiIn);
        m_hMidiIn = 0;
    }    
}

void CMidiSphinx::Start()
{
    if(m_hMidiIn)
        midiInStart(m_hMidiIn);
}

void CMidiSphinx::Reset()
{
    if(m_hMidiIn)
        midiInReset(m_hMidiIn);
}

void CMidiSphinx::PrepareDefaultChannel(int nTrack)
{
    if(!m_hMidiOut)
        return;
    
    DWORD dwMessage;    
    
    for(int i=0; i<nTrack; i++)
    {
        dwMessage = 0xC0|(BYTE)i|(cstProgramDefault[i]<<8);
        midiOutShortMsg(m_hMidiOut, dwMessage);
    }    
}

void CMidiSphinx::UpdatePrgmChange(int numTrack, int numProgram)
{
    DWORD dwMessage;
    
    if(m_hMidiOut)
    {
        dwMessage = 0xC0|(BYTE)m_pSeq->GetChannelTrack(numTrack)|((WORD)numProgram<<8);
        midiOutShortMsg(m_hMidiOut, dwMessage);
    }    
}

void CMidiSphinx::NoteOn(BYTE byChannel, BYTE byNote)
{
    if(!m_hMidiOut)
        return;
    
    DWORD dwMessage;
    
    dwMessage = 0x7F0090|byChannel|(byNote<<8);
    midiOutShortMsg(m_hMidiOut, dwMessage);
    m_byOldChannel = byChannel;
    m_byOldNote = byNote;
}

void CMidiSphinx::NoteOff()
{
    if((!m_hMidiOut)||(m_byOldChannel>127)||(m_byOldNote>127))
        return;
    
    DWORD dwMessage;
    
    dwMessage = 0x80|m_byOldChannel|(m_byOldNote<<8);
    midiOutShortMsg(m_hMidiOut, dwMessage);
}

void CMidiSphinx::SetProgramChange(BYTE byChannel, BYTE byPrgNumber)
{
    if((!m_hMidiOut)||(byChannel>=16)||(byPrgNumber>=128))
        return;
    
    DWORD dwMessage;
    dwMessage = 0xC0|(BYTE)byChannel|(byPrgNumber<<8);
    midiOutShortMsg(m_hMidiOut, dwMessage);
}    


/*********************************************************
* Begin : commencer la sequence
*
*********************************************************/

BOOL CMidiSphinx::Begin(BOOL bRecord)
{
    TIMECAPS tc;
    DWORD dwBuf;
    if(m_bSequenceGoing)
        return FALSE;
    End();
    
    ExtractListEvent();
    // recupère le détail de l'horloge
    
    timeGetDevCaps(&tc, sizeof(TIMECAPS));
    m_uTimerRes = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
    timeBeginPeriod(m_uTimerRes);
    
    SeekIndexListEvent(m_pSeq->GetTrackCount(), m_dwCurrentTime);
    m_nCurrentTempo = m_pSeq->GetTempo(m_dwCurrentTime);
    m_pSeq->GetTimeSignature(m_dwCurrentTime, m_byCurrentSignature);
    m_dwCurrentClock = m_pSeq->TimeCodeToTimeClock(m_dwCurrentTime);
    
    //initialisation
    WORD wBuf;
    for(int i=0; i<MAX_CHANNEL; i++)
    {
        if(bRecord)
        {
            m_ptRecord[i].x =  m_dwCurrentTime;
            m_ptRecord[i].y =  m_dwCurrentTime;
        }
        // initialisation des controleurs
        dwBuf = 0x79B0|(BYTE)i;
        midiOutShortMsg(m_hMidiOut, dwBuf);
        wBuf = (WORD)m_pSeq->GetPrgmChange(i, m_dwCurrentTime);
        dwBuf = 0xC0|(BYTE)m_pSeq->GetChannelTrack(i)|(wBuf<<8);
        midiOutShortMsg(m_hMidiOut, dwBuf);
    }
    
    m_uTimerClock = 10;
    m_dwTimerCount = timeGetTime();
    m_sendClick = 0;
    // crée un timer
    m_uTimerId = timeSetEvent(max(m_uTimerRes, m_uTimerClock), m_uTimerRes,
        TimerFunc, (DWORD)this, TIME_ONESHOT);
    
    if(m_uTimerId==0)
    {
        timeEndPeriod(m_uTimerRes);
        m_bStop = TRUE;
        return FALSE;
    }
    m_uTimerId1 = timeSetEvent(max(m_uTimerRes, m_uTimerClock), m_uTimerRes,
        Timer1Func, (DWORD)this, TIME_PERIODIC);
    
    if(m_uTimerId1==0)
    {
        timeKillEvent(m_uTimerId);
        m_uTimerId = 0;
        timeEndPeriod(m_uTimerRes);
        m_bStop = TRUE;
        return FALSE;
    }
    
    m_bSequenceGoing = TRUE;
    if(bRecord)
    {
        m_bRecording = TRUE;
        // recuperer l'état du bouton metronome
        m_bStateClick = m_pSeq->IsClicked();
        // on active ou non le bouton clic selon l'option d'enregistrement
        m_pSeq->SetClicked(m_pInit->bClick);
    }    
    m_bStop = FALSE;
    return TRUE;
}

/************************************************************
* End : arrête la sequence
*
************************************************************/

void CMidiSphinx::End()
{
    if(m_bSequenceGoing)
    {
        if(m_uTimerId)
        {
            timeKillEvent(m_uTimerId);
            m_uTimerId = 0;
        }    
        if(m_uTimerId1)
        {
            timeKillEvent(m_uTimerId1);
            m_uTimerId1 = 0;
        }    
        timeEndPeriod(m_uTimerRes);
        
        for(int i=0; i<MAX_CHANNEL; i++)
        {
            // on eteint tous les sons
            midiOutShortMsg(m_hMidiOut, 0x78B0|(BYTE)i);
            midiOutShortMsg(m_hMidiOut, 0x7BB0|(BYTE)i);            
            // on eteint tous les leds
            m_pSeq->ResetLed(i);
        }    
        // annule les notes
        midiOutReset(m_hMidiOut);
        
        m_bSequenceGoing = FALSE;
        if(m_bRecording)
        {
            m_bRecording = FALSE;
            // on remet l'état initial de bouton CLIC avant enregistrement
            m_pSeq->SetClicked(m_bStateClick);
            // extraire les evenements enregistrés
            if(m_pInit->bQuantise)
                ExtractRecordEvent(m_pInit->nQuantise);
            else
                ExtractRecordEvent(-1);
            // liberer les memoires alloués aux données enregistrées
            FreeRecordEvent();
        }    
    }
    m_bStop = TRUE;   
}

void CMidiSphinx::SetCurrentValue(DWORD dwTime)
{
    m_dwCurrentTime = dwTime;
    m_dwCurrentClock = m_pSeq->TimeCodeToTimeClock(dwTime);
    m_nCurrentTempo = m_pSeq->GetTempo(m_dwCurrentTime);
    m_pSeq->GetTimeSignature(m_dwCurrentTime, m_byCurrentSignature);
}

/*******************************************************************************
* AddListEvent : ajoute une liste dans les evenements
*
*******************************************************************************/

void CMidiSphinx::AddListEvent(int numTrack, TListEvent * p)
{
    // pour éviter l'extension de mémoire à chaque fois qu'on ajoute une liste,
    // on alloue un paquet de 100 listes
    
    if(!m_mTrack[numTrack].pEvent)
    {
        m_mTrack[numTrack].pEvent = (TListEvent*)malloc(100 * sizeof(TListEvent));
        if(m_mTrack[numTrack].pEvent)
            m_mTrack[numTrack].arraySize = 100;
        else
            return;
    }
    
    if(m_mTrack[numTrack].count >= m_mTrack[numTrack].arraySize)
    {
        m_mTrack[numTrack].pEvent = (TListEvent*)realloc(m_mTrack[numTrack].pEvent,
            (m_mTrack[numTrack].arraySize + 100) * sizeof(TListEvent));
        if(m_mTrack[numTrack].pEvent)
            m_mTrack[numTrack].arraySize += 100;
        else
            return;
    }
    
    CopyMemory(&m_mTrack[numTrack].pEvent[m_mTrack[numTrack].count], p, sizeof(TListEvent));
    m_mTrack[numTrack].count++;
}    

void CMidiSphinx::ExtractListEvent()
{
    TPattern * p;
    CObjectEvent * pe;
    CObjectEventNote * pn;
    CObjectEventPitchBend * pb;
    TListEvent buffer;
    
    if(m_bSequenceGoing)
        return;
    
    // libérer toutes les listes d'abord    
    FreeListEvent();
    
    // extraire les evenements dans chaque piste    
    for(int i=0; i<m_pSeq->GetTrackCount(); i++)
    {
        pe = m_pGlobal->pFirstAll[i];
        while(pe)
        {
            buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pe->left);
            if(pe->GetType()==MIDI_CTRLCHANGE)
            {
                buffer.dwMessage = 0xB0|(BYTE)m_pSeq->GetChannelTrack(i)|
                    (((CObjectEventControlChange*)pe)->numController<<8)|
                    (((CObjectEventControlChange*)pe)->value<<16);
                AddListEvent(i, &buffer);
            }
            else if(pe->GetType()==MIDI_PRGMCHANGE)
            {
                buffer.dwMessage = 0xC0|(BYTE)m_pSeq->GetChannelTrack(i)|
                    (((CObjectEventProgramChange*)pe)->numProgram<<8);
                AddListEvent(i, &buffer);
            }  
            pe = pe->pRight;
        }    
    }
    
    // extraire les evenements dans chaque sequence    
    p = m_pGlobal->pFirstPattern;
    while(p)
    {
        pe = p->pFirstEvent;
        while(pe)
        {
            // extraire les evenements Note
            if((m_pSeq->GetChannelTrack(p->numTrack)>=0)&&(pe->GetType()==MIDI_NOTEON))
            {
                pn = (CObjectEventNote*)pe;
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pn->left);
                buffer.dwMessage = 0x90|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|
                    (pn->note<<8)|(pn->velocity<<16);
                AddListEvent(p->numTrack, &buffer);
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pn->right);
                buffer.dwMessage = 0x80|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|(pn->note<<8);
                AddListEvent(p->numTrack, &buffer);
            }
            // extraire les evenements Pitch Bend
            else if((m_pSeq->GetChannelTrack(p->numTrack)>=0)&&(pe->GetType()==MIDI_PITCHBEND))
            {
                pb = (CObjectEventPitchBend*)pe;
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pb->left);
                buffer.dwMessage = 0xE0|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|
                    (pb->value1<<8)|(pb->value2<<16);
                AddListEvent(p->numTrack, &buffer);
            }
            // extraire les evenements KeyAfterTouch
            else if((m_pSeq->GetChannelTrack(p->numTrack)>=0)&&(pe->GetType()==MIDI_KEYAFTERTOUCH))
            {
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pe->left);
                buffer.dwMessage = 0xA0|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|
                    (((CObjectEventKeyAfterTouch*)pe)->note<<8)|
                    (((CObjectEventKeyAfterTouch*)pe)->velocity<<16);
                AddListEvent(p->numTrack, &buffer);
            }
            // extraire les evenements CtrlChange
            else if((m_pSeq->GetChannelTrack(p->numTrack)>=0)&&(pe->GetType()==MIDI_CTRLCHANGE))
            {
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pe->left);
                buffer.dwMessage = 0xB0|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|
                    (((CObjectEventControlChange*)pe)->numController<<8)|
                    (((CObjectEventControlChange*)pe)->value<<16);
                AddListEvent(p->numTrack, &buffer);
            }
            // extraire les evenements Channel Pressure
            else if((m_pSeq->GetChannelTrack(p->numTrack)>=0)&&(pe->GetType()==MIDI_CHANPRESS))
            {
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pe->left);
                buffer.dwMessage = 0xD0|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|
                    (((CObjectEventChannelPressure*)pe)->value<<8);
                AddListEvent(p->numTrack, &buffer);
            }
            // extraire les evenements Prgm Change
            else if((m_pSeq->GetChannelTrack(p->numTrack)>=0)&&(pe->GetType()==MIDI_PRGMCHANGE))
            {
                buffer.dwTimeEvent = m_pSeq->TimeCodeToTimeClock((DWORD)pe->left);
                buffer.dwMessage = 0xC0|(BYTE)m_pSeq->GetChannelTrack(p->numTrack)|
                    (((CObjectEventProgramChange*)pe)->numProgram<<8);
                AddListEvent(p->numTrack, &buffer);
            }
            pe = pe->pRight;
        }
        p = p->pRight;    
    }
    
    // maintenant trier suivant le temps d'evenement    
    SortListEvent(m_pSeq->GetTrackCount());
}    

/**************************************************************************
* SortListEvent : tri les listes des evenements midi
*  nbTrack(in) : nombre de pistes à trier
*
**************************************************************************/

void  CMidiSphinx::SortListEvent(int nbTrack)
{
    // pour éviter le tri pendant la lecture
    
    if(m_bSequenceGoing)
        return;
    
    for(int i=0; i<nbTrack; i++)
    {
        if(m_mTrack[i].pEvent)
        {
            qsort((void*)m_mTrack[i].pEvent, m_mTrack[i].count, sizeof(TListEvent), Compare);
        }    
    }    
}    

int  CMidiSphinx::SeekIndexCurrent(int numTrack, DWORD dwTime)
{
    int i;
    DWORD dwMsec = m_pSeq->TimeCodeToTimeClock(dwTime);
    
    if(!m_mTrack[numTrack].pEvent)
        return 0;
    
    // si les données sont moins importante, on fait une recherche sequentielle
    
    if(m_mTrack[numTrack].count<10)
    {
        for(i=0; i<m_mTrack[numTrack].count; i++)
        {
            if(dwMsec<=m_mTrack[numTrack].pEvent[i].dwTimeEvent)
                return i;
        }
        return i;    
    }
    
    // sinon on fait la recherche par dichotomie
    
    int k0, k1, k;
    k0 = 0;
    k1 = m_mTrack[numTrack].count - 1;
    k = (k1 - k0)/2;
    
    if(dwMsec<=m_mTrack[numTrack].pEvent[0].dwTimeEvent)
        return 0;
    if(dwMsec>m_mTrack[numTrack].pEvent[k1].dwTimeEvent)
        return (k1 + 1);
    
    do
    {
        if(dwMsec==m_mTrack[numTrack].pEvent[k].dwTimeEvent)
            return k;
        if(dwMsec<m_mTrack[numTrack].pEvent[k].dwTimeEvent)
            k1 = k;
        else if(dwMsec>m_mTrack[numTrack].pEvent[k].dwTimeEvent)
            k0 = k;
        
        k = ((k1 - k0)/2) + k0;
    }while(k!=k0);
    return k1;    
}    

void  CMidiSphinx::SeekIndexListEvent(int nbTrack, DWORD dwTime)
{
    int i;
    
    if(m_bSequenceGoing)
        return;
    
    for(int i=0; i<nbTrack; i++)
    {
        m_mTrack[i].indexCurrent = SeekIndexCurrent(i, dwTime);
    }
}        

/*******************************************************************************
* FreeListEvent : libère les mémoires allouées aux listes d'evenements
*
*******************************************************************************/

void  CMidiSphinx::FreeListEvent()
{
    if(m_bSequenceGoing)
        return;
    
    for(int i=0; i<MAX_CHANNEL; i++)
    {
        if(m_mTrack[i].pEvent)
            free(m_mTrack[i].pEvent);
        ZeroMemory(&m_mTrack[i], sizeof(TMidiTrack));
    }    
}    

/********************************************************************************
* TimerFunc : voici le coeur de la musique, c'est le timer qui gère le 
* generateur de son via la carte son
*
********************************************************************************/

void CALLBACK TimerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    static int oldClick = 0;
    static DWORD dwOldMsgClick = 0x99;
    static BOOL  bResetClick = FALSE;
    int nClick, bufTempo;
    register int i, track, buffer;
    register DWORD dwBuf;
    DWORD dwMes;
    CMidiSphinx *   p = (CMidiSphinx*)dwUser;
    
    if(!p)
        return;    
    
    if(p->m_bStop)
        p->End();    
    
    // on lance le métronome s'il est actif
    
    if(bResetClick)
    {
        midiOutShortMsg(p->m_hMidiOut, dwOldMsgClick);
        bResetClick = FALSE;
    }    
    
    if(p->m_pSeq->IsClicked())
    {
        if((p->m_sendClick==2)&&(p->m_pSeq->m_optClick.bActiveM))
        {        
            dwOldMsgClick = 0x90|(BYTE)p->m_pSeq->m_optClick.channelM|
            (WORD)(p->m_pSeq->m_optClick.noteM << 8)|(p->m_pSeq->m_optClick.velocityM << 16);
            midiOutShortMsg(p->m_hMidiOut, dwOldMsgClick);
            dwOldMsgClick &= 0xFFFF;
            p->m_sendClick = 0;
            bResetClick = TRUE;
        }    
        else if((p->m_sendClick==1)&&(p->m_pSeq->m_optClick.bActiveB))
        {        
            dwOldMsgClick = 0x90|(BYTE)p->m_pSeq->m_optClick.channelB|
            (WORD)(p->m_pSeq->m_optClick.noteB << 8)|(p->m_pSeq->m_optClick.velocityB << 16);
            midiOutShortMsg(p->m_hMidiOut, dwOldMsgClick);
            dwOldMsgClick &= 0xFFFF;
            p->m_sendClick = 0;
            bResetClick = TRUE;
        }        
    }    
    
    // on recupère les evenements pour chaque piste
    
    for(i=0; i<p->m_pSeq->GetTrackCount(); i++)
    {
        if(p->m_pSeq->m_cCurrent[i].bVol)
        {
            buffer = min(127,(p->m_pSeq->m_cCurrent[i].vol* p->m_pSeq->GetVolumeTrack(i))/100);
            dwMes = 0x07B0|(BYTE)p->m_pSeq->GetChannelTrack(i)|(((DWORD)buffer)<<16);
            midiOutShortMsg(p->m_hMidiOut, dwMes);
            p->m_pSeq->m_cCurrent[i].bVol = FALSE;
        }
        if(p->m_pSeq->m_cCurrent[i].bPan)
        {
            if(p->m_pSeq->IsPanActive(i))
                dwMes = 0x0AB0|(BYTE)p->m_pSeq->GetChannelTrack(i)|
                    (((DWORD)p->m_pSeq->GetPanTrack(i))<<16);
            else
                dwMes = 0x0AB0|(BYTE)p->m_pSeq->GetChannelTrack(i)|
                    (((DWORD)p->m_pSeq->m_cCurrent[i].pan)<<16);
            
            midiOutShortMsg(p->m_hMidiOut, dwMes);
            p->m_pSeq->m_cCurrent[i].bPan = FALSE;
        }
        while((p->m_mTrack[i].pEvent)&&(p->m_mTrack[i].indexCurrent<p->m_mTrack[i].count)&&
        (p->m_mTrack[i].pEvent[p->m_mTrack[i].indexCurrent].dwTimeEvent <= p->m_dwCurrentClock))
        {
            dwBuf = p->m_mTrack[i].pEvent[p->m_mTrack[i].indexCurrent].dwMessage;
            if((dwBuf & 0xFFF0)==0x07B0)
            {
                p->m_pSeq->m_cCurrent[i].vol =  ((dwBuf&0xFF0000)>>16);
                buffer = min(127,(((dwBuf&0xFF0000)>>16)* p->m_pSeq->GetVolumeTrack(i))/100);
                
                midiOutShortMsg(p->m_hMidiOut,(dwBuf & 0xFFFF)|(((DWORD)buffer)<<16));
            }
            else if((dwBuf & 0xFFF0)==0x0AB0)
            {
                p->m_pSeq->m_cCurrent[i].pan =  ((dwBuf&0xFF0000)>>16);
                if(p->m_pSeq->IsPanActive(i))
                    midiOutShortMsg(p->m_hMidiOut,(dwBuf & 0xFFFF)|
                        (((DWORD)p->m_pSeq->GetPanTrack(i))<<16));
            }
            // on joue pas les notes si la piste est mutée
            
            else if(!p->m_pSeq->IsMuteTrack(i))
            {
                midiOutShortMsg(p->m_hMidiOut, dwBuf);
                if((dwBuf & 0xF0)==0x90)
                    p->m_pSeq->AddLevelLed(i, (dwBuf & 0xFF00)>>8, (dwBuf & 0xFF0000)>>16);
                else if((dwBuf & 0xF0)==0x80)
                    p->m_pSeq->RemoveLevelLed(i, (dwBuf & 0xFF00)>>8);
            }    
            p->m_mTrack[i].indexCurrent++;
        }
        
        // si la piste est mutée, on annule toutes les notes jouées
        
        if(p->m_pSeq->IsMuteTrack(i))
        {
            midiOutShortMsg(p->m_hMidiOut, 0x7BB0|(BYTE)p->m_pSeq->GetChannelTrack(i));
            p->m_pSeq->ResetLed(i);
        }    
    }
    
    dwBuf = timeGetTime();
    p->m_dwCurrentClock +=  dwBuf - p->m_dwTimerCount;
    p->m_dwTimerCount = dwBuf;
    
    p->m_uTimerId = timeSetEvent(max(p->m_uTimerRes, p->m_uTimerClock), p->m_uTimerRes,
        TimerFunc, dwUser, TIME_ONESHOT);
    
    if(p->m_uTimerId==0)
        p->End();
}

/**********************************************************************************
*
*
**********************************************************************************/

void CALLBACK Timer1Func(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    int buf;
    CMidiSphinx *   p = (CMidiSphinx*)dwUser;    
    
    if((!p)||(p->m_bStop))
        return ;
    
    p->m_dwCurrentTime = p->m_pSeq->TimeClockToTimeCode(p->m_dwCurrentClock);
    if(p->m_dwCurrentTime>(DWORD)p->m_pSeq->GetLastLocator())
        p->End();    
    
    // on recupère le time signature courant
    p->m_pSeq->GetTimeSignature(p->m_dwCurrentTime, p->m_byCurrentSignature);
    
    buf = p->m_pSeq->TimeCodeToBeat(p->m_dwCurrentTime);
    if(buf)
        p->m_sendClick = buf;
    // on recupère le tempo
    buf = p->m_pSeq->GetTempo(p->m_dwCurrentTime);
    if(p->m_nCurrentTempo!=buf)
        p->m_nCurrentTempo = buf;
}    

/***************************************************************************
* 
*
***************************************************************************/

void CALLBACK MidiInFunc(HMIDIIN hMidi, UINT uMsg, DWORD dwInst, DWORD dw1, DWORD dw2)
{
    register DWORD dwMes;
    register int channel, value;
    register TListEvent buffer;
    CMidiSphinx *   p = (CMidiSphinx*)dwInst;    
    
    switch(uMsg)
    {
        case MIM_DATA:
            // recuperer les messages entrants
            if(p->m_pInit->bDemix)
                dwMes = dw1;
            else if(p->m_pSeq->GetTrackSelect()>=0)
            {
                channel = p->m_pSeq->GetChannelTrack(p->m_pSeq->GetTrackSelect());
                dwMes = (dw1 & 0xFFFFF0)|(BYTE)channel;
            }
            else
                dwMes = 0;
            
            // filtre les evenements selon les option d'enregistrement
            p->FilterEvents(&dwMes);
            
            if(dwMes==0)
                break;
            
            // envoyer l'evenements recuperés
            midiOutShortMsg(p->m_hMidiOut, dwMes);
            if(p->m_bRecording)
            {
                channel = (int)(dwMes & 0x0F);
                // met à jour le segment des sequences
                p->m_ptRecord[channel].y = (LONG)p->m_dwCurrentTime;
                buffer.dwTimeEvent = p->m_dwCurrentTime;
                buffer.dwMessage = dwMes;
                // stocker les evenements 
                p->AddRecordEvent(&buffer);
            }
            break;
        case MIM_CLOSE:
            break;
    }    
}
    
/***************************************************************************
* Compare : comparaison de 2 elements du ListEvent pour le tri au quicksort
*
***************************************************************************/

int __cdecl Compare(const void* t1, const void* t2)
{
    int delta = (int)((TListEvent*)t1)->dwTimeEvent - (int)((TListEvent*)t2)->dwTimeEvent;
    if(delta==0)
    {
        if(((((TListEvent*)t1)->dwMessage & 0xF0)==MIDI_PRGMCHANGE)&&
        ((((TListEvent*)t2)->dwMessage & 0xF0)!=MIDI_PRGMCHANGE))
            return -1;
        if(((((TListEvent*)t1)->dwMessage & 0xF0)==MIDI_CTRLCHANGE)&&
        ((((TListEvent*)t2)->dwMessage & 0xF0)!=MIDI_CTRLCHANGE))
            return -1;
        // ici si la note on et la note off ont le meme timeEvent, on doit traiter la
        // note off d'abord avant la note on
        if(((((TListEvent*)t1)->dwMessage & 0xF0)==0x80)&&
        ((((TListEvent*)t2)->dwMessage & 0xF0)==0x90))
            return -1;
        
        if(((((TListEvent*)t1)->dwMessage & 0xF0)!=MIDI_PRGMCHANGE)&&
        ((((TListEvent*)t2)->dwMessage & 0xF0)==MIDI_PRGMCHANGE))
            return 1;
        if(((((TListEvent*)t1)->dwMessage & 0xF0)!=MIDI_CTRLCHANGE)&&
        ((((TListEvent*)t2)->dwMessage & 0xF0)==MIDI_CTRLCHANGE))
            return 1;
        if(((((TListEvent*)t1)->dwMessage & 0xF0)==0x90)&&
        ((((TListEvent*)t2)->dwMessage & 0xF0)==0x80))
            return 1;
    }    
    return delta;
}

/*********************************************************************************
*
*
*********************************************************************************/

void CMidiSphinx::FilterEvents(DWORD * pdw)
{
    register BYTE bStatus;
    bStatus = (BYTE)((*pdw) & 0xF0);
    switch(bStatus)
    {
        case MIDI_NOTEON:
        case MIDI_NOTEOFF:
            if(!(m_pInit->uFilter & FILTER_NOTE))
                *pdw = 0;
            break;
        case MIDI_KEYAFTERTOUCH:
            if(!(m_pInit->uFilter & FILTER_KEYPRESS))
                *pdw = 0;
            break;
        case MIDI_CTRLCHANGE:
            if(!(m_pInit->uFilter & FILTER_CTRLCHANGE))
                *pdw = 0;
            break;
        case MIDI_PRGMCHANGE:
            if(!(m_pInit->uFilter & FILTER_PRGMCHANGE))
                *pdw = 0;
            break;
        case MIDI_CHANPRESS:
            if(!(m_pInit->uFilter & FILTER_CHANPRESS))
                *pdw = 0;
            break;
        case MIDI_PITCHBEND:
            if(!(m_pInit->uFilter & FILTER_PITCHBEND))
                *pdw = 0;
            break;
    }    
}

/*******************************************************************************
* AddRecordEvent : ajoute une liste dans les evenements
*
*******************************************************************************/

void CMidiSphinx::AddRecordEvent(TListEvent * p)
{
    // pour éviter l'extension de mémoire à chaque fois qu'on ajoute une liste,
    // on alloue un paquet de 100 listes
    
    if(!m_trackRecord.pEvent)
    {
        m_trackRecord.pEvent = (TListEvent*)malloc(100 * sizeof(TListEvent));
        if(m_trackRecord.pEvent)
            m_trackRecord.arraySize = 100;
        else
            return;
    }
    
    TListEvent * pBuf;
    if(m_trackRecord.count >= m_trackRecord.arraySize)
    {
        pBuf = (TListEvent*)realloc(m_trackRecord.pEvent,
            (m_trackRecord.arraySize + 100) * sizeof(TListEvent));
        if(pBuf)
        {
            m_trackRecord.pEvent = pBuf;
            m_trackRecord.arraySize += 100;
        }    
        else
            return;
    }
    
    CopyMemory(&m_trackRecord.pEvent[m_trackRecord.count], p, sizeof(TListEvent));
    m_trackRecord.count++;
}

/*******************************************************************************
* FreeRecordEvent : libère les mémoires allouées aux listes d'evenements
*
*******************************************************************************/

void  CMidiSphinx::FreeRecordEvent()
{
    if(m_trackRecord.pEvent)
        free(m_trackRecord.pEvent);
    ZeroMemory(&m_trackRecord, sizeof(TMidiTrack));
}

/*********************************************************************************
*
*
*********************************************************************************/

BOOL CMidiSphinx::ExtractRecordEvent(int nQuant)
{
    if(!m_trackRecord.pEvent)
        return FALSE;
    TPattern * p = NULL;
    CObjectEvent * pe = NULL;
    CObjectEvent * pn;
    TPatternValue patv;
    int i, nChannel, val1, val2;
    
    for(i=0; i<m_trackRecord.count; i++)
    {
        // recuperer le numero de canal MIDI
        nChannel = (int)(m_trackRecord.pEvent[i].dwMessage & 0x0F);
    
        // chercher si ce canal est déjà utilisé
        p = m_pGlobal->pFirstPattern;
        while(p)
        {
            if((p->numTrack == nChannel)&&(p->bRecord))
                break;
            p = p->pRight;
        }
        // si on trouve pas on en crée un.
        if(!p)
        {
            patv.numTrack = nChannel;
            patv.left = (int)m_trackRecord.pEvent[i].dwTimeEvent;
            patv.right = (int)m_trackRecord.pEvent[i].dwTimeEvent;
            patv.bSelected = FALSE;
            patv.bRecord = TRUE;            
            wsprintf(patv.szTitle, "MIDI %i", nChannel+1);
            p = m_pSeq->CreatePattern(&patv);
            if(!p)
                return FALSE;            
        }
        
        BYTE byStatus = (BYTE)(m_trackRecord.pEvent[i].dwMessage & 0xF0);
        switch(byStatus)
        {
            case MIDI_KEYAFTERTOUCH:
                if(p->right<(int)m_trackRecord.pEvent[i].dwTimeEvent)
                    p->right = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                pe = m_pSeq->CreateKeyAfterTouch(p);
                if(!pe)
                    return FALSE;
                pe->left = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                val1 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF00)>>8);
                val2 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF0000)>>16);
                ((CObjectEventKeyAfterTouch*)pe)->note = val1;
                ((CObjectEventKeyAfterTouch*)pe)->velocity = val2;
                ((CObjectEventKeyAfterTouch*)pe)->bSelected = FALSE;
                break;
            case MIDI_CHANPRESS:
                if(p->right<(int)m_trackRecord.pEvent[i].dwTimeEvent)
                    p->right = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                pe = m_pSeq->CreateChannelPressure(p);
                if(!pe)
                    return FALSE;
                pe->left = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                val1 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF00)>>8);
                ((CObjectEventChannelPressure*)pe)->value = val1;
                ((CObjectEventChannelPressure*)pe)->bSelected = FALSE;
                break;
            case MIDI_PRGMCHANGE:
                if(p->right<(int)m_trackRecord.pEvent[i].dwTimeEvent)
                    p->right = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                pe = m_pSeq->CreateProgramChange(p);
                if(!pe)
                    return FALSE;
                pe->left = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                val1 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF00)>>8);
                ((CObjectEventProgramChange*)pe)->numProgram = val1;
                ((CObjectEventProgramChange*)pe)->bSelected = FALSE;
                if(pe->left==0)
                    m_pSeq->SetPrgmChange(p->numTrack, 0, val1);
                // ajoute dans la liste controle
                m_pSeq->AddControlUnity(pe, p->numTrack);
                break;
            case MIDI_CTRLCHANGE:
                if(p->right<(int)m_trackRecord.pEvent[i].dwTimeEvent)
                    p->right = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                pe = m_pSeq->CreateControlChange(p);
                if(!pe)
                    return FALSE;
                pe->left = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                val1 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF00)>>8);
                val2 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF0000)>>16);
                ((CObjectEventControlChange*)pe)->numController = val1;
                ((CObjectEventControlChange*)pe)->value = val2;
                ((CObjectEventControlChange*)pe)->bSelected = FALSE;
                // ajoute dans la liste controle
                m_pSeq->AddControlUnity(pe, p->numTrack);
                break;
            case MIDI_PITCHBEND:
                if(p->right<(int)m_trackRecord.pEvent[i].dwTimeEvent)
                    p->right = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                pe = m_pSeq->CreatePitchBend(p);
                if(!pe)
                    return FALSE;
                pe->left = (int)m_trackRecord.pEvent[i].dwTimeEvent;
                val1 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF00)>>8);
                val2 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF0000)>>16);
                ((CObjectEventPitchBend*)pe)->value1 = val1;
                ((CObjectEventPitchBend*)pe)->value2 = val2;
                ((CObjectEventPitchBend*)pe)->bSelected = FALSE;
                break;
            case MIDI_NOTEON:
            case MIDI_NOTEOFF:
                val1 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF00)>>8);
                val2 = (int)((m_trackRecord.pEvent[i].dwMessage & 0xFF0000)>>16);
                if(val2==0)
                    byStatus = MIDI_NOTEOFF;
                int lenghtNote = RoundTimeEvent(m_trackRecord.pEvent[i].dwTimeEvent, nQuant) +
                    (int)m_pSeq->GetTimeTick()/24;
                if(byStatus==MIDI_NOTEON)
                {                    
                    if(p->right<lenghtNote)
                        p->right = lenghtNote;
                    if(p->left>RoundTimeEvent(m_trackRecord.pEvent[i].dwTimeEvent, nQuant))
                        p->left = RoundTimeEvent(m_trackRecord.pEvent[i].dwTimeEvent, nQuant);
                    pe = m_pSeq->CreateNote(p);
                    pe->left = RoundTimeEvent(m_trackRecord.pEvent[i].dwTimeEvent, nQuant);
                    ((CObjectEventNote*)pe)->right = lenghtNote;
                    ((CObjectEventNote*)pe)->note = val1;
                    ((CObjectEventNote*)pe)->velocity = val2;
                    ((CObjectEventNote*)pe)->bSelected = FALSE;
                    ((CObjectEventNote*)pe)->bDisabled = TRUE;
                }
                else
                {
                    pe = p->pFirstEvent;
                    while(pe)
                    {
                        if((((CObjectEventNote*)pe)->note==val1)&&(pe->GetType()==MIDI_NOTEON)&&
                        (((CObjectEventNote*)pe)->bDisabled))
                            pn = pe;
                        pe = pe->pRight;    
                    }
                    if(pn)
                    {
                        if(pn->left<(int)m_trackRecord.pEvent[i].dwTimeEvent)
                            ((CObjectEventNote*)pn)->right = 
                                (int)m_trackRecord.pEvent[i].dwTimeEvent;
                        else
                            ((CObjectEventNote*)pn)->right = pn->left + 1;
                        
                        if(p->right<((CObjectEventNote*)pn)->right)
                            p->right = ((CObjectEventNote*)pn)->right;                        
                        ((CObjectEventNote*)pn)->bDisabled = FALSE;
                    }    
                }
                break;
        } // end switch  
    } // end for
    
    p = m_pGlobal->pFirstPattern;
    while(p)
    {
        if(p->bRecord)
            p->bRecord = FALSE;
        p = p->pRight;
    }    
    return TRUE;
}

/*********************************************************************************
* RoundTimeEvent : arrondi le timeCode selon l'index de quantise utilisé
*
*********************************************************************************/

int CMidiSphinx::RoundTimeEvent(DWORD dwTime, int nQuant)
{
    if((nQuant>=7)||(nQuant<0))
        return (int)dwTime;
    
    int rep = ((int)m_pSeq->GetTimeTick()*4)/(1<<nQuant);
    int ret = (int)dwTime/rep;
    if(((int)dwTime%rep)>(rep/2))
        ret++;
    
    return (ret*rep);
}    
