#ifndef _OBJECTMIDI_H_
#define _OBJECTMIDI_H_

#define MIDI_NOTEON        ((BYTE)0x90)
#define MIDI_NOTEOFF       ((BYTE)0x80)
#define MIDI_KEYAFTERTOUCH ((BYTE)0xA0)
#define MIDI_CTRLCHANGE    ((BYTE)0xB0)
#define MIDI_PRGMCHANGE    ((BYTE)0xC0)
#define MIDI_CHANPRESS     ((BYTE)0xD0)
#define MIDI_PITCHBEND     ((BYTE)0xE0)
#define MIDI_META          ((BYTE)0xFF)

#define META_TEXT          ((BYTE)0x01)
#define META_COPYRIGHT     ((BYTE)0x02)
#define META_NAMETRACK     ((BYTE)0x03)
#define META_LYRIC         ((BYTE)0x05)
#define META_MARKER        ((BYTE)0x06)
#define META_CUEPOINT      ((BYTE)0x07)
#define META_TEMPO         ((BYTE)0x51)
#define META_SIGNATURE     ((BYTE)0x58)

#define CTRLCHANGE_VOLUME  0x07
#define CTRLCHANGE_PAN     0x0A

class CObjectEvent;

struct TControlUnity
{
    DWORD dwTime;
    BYTE  byTrack;
    BYTE  byStatus;
    BYTE  byData1;
    BYTE  byData2;
    TControlUnity * pLeft;
    TControlUnity * pRight;
};    

struct TListBuffer
{
    LPBYTE  pData;
    int     count;
    int     arraySize;
};

class CObjectEvent
{
public:
    CObjectEvent();
    virtual ~CObjectEvent();
    virtual BYTE GetType() = 0;
    virtual void Write(HANDLE)=0;
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*)=0;
    
    int  left;
    CObjectEvent * pRight;
    CObjectEvent * pLeft;
};    

// Objet Note

class CObjectEventNote : public CObjectEvent
{
public:
    CObjectEventNote();
    virtual ~CObjectEventNote();
    virtual BYTE GetType(){return MIDI_NOTEON;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    int  note;
    int  velocity;
    int  right;
    BOOL bDisabled;
    BOOL bSelected;
};

// Objet Pitch Bend

class CObjectEventPitchBend : public CObjectEvent
{
public:
    CObjectEventPitchBend();
    virtual ~CObjectEventPitchBend();
    virtual BYTE GetType(){return MIDI_PITCHBEND;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    int  value1;
    int  value2;
    BOOL bSelected;
};

// Objet Key After Touch

class CObjectEventKeyAfterTouch : public CObjectEvent
{
public:
    CObjectEventKeyAfterTouch();
    virtual ~CObjectEventKeyAfterTouch();
    virtual BYTE GetType(){return MIDI_KEYAFTERTOUCH;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    int  note;
    int  velocity;
    BOOL bSelected;
};


// Objet Channel Pressure

class CObjectEventChannelPressure : public CObjectEvent
{
public:
    CObjectEventChannelPressure();
    virtual ~CObjectEventChannelPressure();
    virtual BYTE GetType(){return MIDI_CHANPRESS;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    int  value;
    BOOL bSelected;
};

// Objet Controle Change

class CObjectEventControlChange : public CObjectEvent
{
public:
    CObjectEventControlChange();
    virtual ~CObjectEventControlChange();
    virtual BYTE GetType(){return MIDI_CTRLCHANGE;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    int  numController;
    int  value;
    BOOL bSelected;
    TControlUnity * pControl;
};

// Objet Programme Change

class CObjectEventProgramChange : public CObjectEvent
{
public:
    CObjectEventProgramChange();
    virtual ~CObjectEventProgramChange();
    virtual BYTE GetType(){return MIDI_PRGMCHANGE;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    int  numProgram;
    BOOL bSelected;
    TControlUnity * pControl;
};

// Objet Meta

class CObjectMetaEvent : public CObjectEvent
{
public:
    CObjectMetaEvent();
    virtual ~CObjectMetaEvent();
    virtual BYTE GetType(){return MIDI_META;}
    virtual BYTE GetMetaType() = 0;
    
    int    dataLen;
    BYTE * pData;
};

// Objet Meta Tempo

class CObjectMetaEventTempo : public CObjectMetaEvent
{
public:
    CObjectMetaEventTempo();
    virtual ~CObjectMetaEventTempo();
    virtual BYTE GetMetaType(){return META_TEMPO;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    void SetTempo(int);
    int tempo;
    BOOL bSelected;
};    

// Objet Meta Signature

class CObjectMetaEventTimeSignature : public CObjectMetaEvent
{
public:
    CObjectMetaEventTimeSignature();
    virtual ~CObjectMetaEventTimeSignature();
    virtual BYTE GetMetaType(){return META_SIGNATURE;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    void SetTimeSignature(BYTE, BYTE, BYTE, BYTE);
    void GetTimeSignature(LPBYTE);
    int  timeNoteIndex;      // index de mesure pour référence
};

// Objet Meta Texte, Copyright, Paroles, Marqueur et Spécial

class CObjectMetaEventText : public CObjectMetaEvent
{
public:
    CObjectMetaEventText(BYTE);
    virtual ~CObjectMetaEventText();
    virtual BYTE GetMetaType(){return byType;}
    virtual void Write(HANDLE);
    virtual int WriteBuffer(TListBuffer*, int, int, WORD*);
    
    void SetText(char*, int);
    void GetText(char*);
    BYTE byType;
};


struct TPatternValue
{
    int  numTrack;
    int  left;
    int  right;
    BOOL bSelected;
    BOOL bRecord;
    char szTitle[256];
};

struct TPattern
{
    int  numTrack;
    int  left;
    int  right;
    BOOL bSelected;
    BOOL bRecord;
    char szTitle[256];
    TPattern * pLeft;
    TPattern * pRight;
    int  id;
    int  idList;    
    CObjectEvent * pFirstEvent;
    CObjectEvent * pLastEvent;
};

/*******************************************************************************
* AddListBuffer : ajoute un evenement dans la liste 
*
*******************************************************************************/

static BOOL AddListBuffer(TListBuffer * pList, BYTE byData)
{
    // pour éviter l'extension de mémoire à chaque fois qu'on ajoute une liste,
    // on alloue un paquet de 100 listes
    
    if(!pList->pData)
    {
        pList->pData = (LPBYTE)malloc(100 * sizeof(BYTE));
        if(pList->pData)
            pList->arraySize = 100;
        else
            return FALSE;
    }
    
    LPBYTE pBuf;
    
    if(pList->count >= pList->arraySize)
    {
        pBuf = (LPBYTE)realloc(pList->pData,
            (pList->arraySize + 100) * sizeof(BYTE));
        if(pBuf)
        {
            pList->pData = pBuf;
            pList->arraySize += 100;
        }    
        else
            return FALSE;
    }
    
    pList->pData[pList->count] = byData;
    pList->count++;
    return TRUE;
}


static void FreeListBuffer(TListBuffer * pList)
{
    if((pList)&&(pList->pData))
    {
        free(pList->pData);
        ZeroMemory(pList, sizeof(TListBuffer));
    }    
}
 
/*******************************************************************************
* AddListDeltaTime : ajoute la durée d'evenement suivant dans la liste 
*
*******************************************************************************/

static BOOL AddListDeltaTime(TListBuffer * pList, int value)
{
    DWORD dwValue = (DWORD)value;
    DWORD dwBuf = dwValue & 0x7F;
    BYTE  byBuf;
    
    while(dwValue >>= 7)
    {
        dwBuf <<= 8;
        dwBuf |= ((dwValue & 0x7F) | 0x80);
    }
    
    while(1)
    {
        byBuf = (BYTE)(dwBuf & 0xFF);
        AddListBuffer(pList, byBuf);
        if(dwBuf & 0x80)
            dwBuf >>= 8;
        else
            break;
    }    
    return TRUE;
}    

/*******************************************************************************
* AddListEndOfTrack : ajoute la fin de bloc de piste
*
*******************************************************************************/

static BOOL AddListEndOfTrack(TListBuffer * pList)
{
    AddListBuffer(pList, 0xFF);
    AddListBuffer(pList, 0x2F);
    AddListBuffer(pList, 0);
    return TRUE;
}    
#endif
