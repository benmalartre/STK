#include <windows.h>
#include "ObjectMidi.h"

//////////////////////////////////////////////////////////////////////////
//  OBJETS DES SEQUENCES
//////////////////////////////////////////////////////////////////////////

CObjectEvent::CObjectEvent() :
    left(0),
    pRight(NULL),
    pLeft(NULL)
{
}

CObjectEvent::~CObjectEvent()
{
}

// Objet Note

CObjectEventNote::CObjectEventNote():
    note(0),
    velocity(80),
    right(0),
    bDisabled(FALSE),
    bSelected(FALSE)
{
}

CObjectEventNote::~CObjectEventNote()
{
}

void  CObjectEventNote::Write(HANDLE h)
{
    DWORD dwWritten;
    int   buf;
    buf = (int)GetType();
    WriteFile(h, &buf, 4, &dwWritten, NULL);
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &note, 4, &dwWritten, NULL);
    WriteFile(h, &velocity, 4, &dwWritten, NULL);
    WriteFile(h, &right, 4, &dwWritten, NULL);
    WriteFile(h, &bSelected, 4, &dwWritten, NULL);
}

int CObjectEventNote::WriteBuffer(TListBuffer* pList, int ref, int channel, WORD * pwStatus)
{
    WORD wState = (WORD)(0x90|(BYTE)channel);
    
    if((pwStatus)&&(*pwStatus==wState))
    {
        AddListBuffer(pList, (BYTE)note);
        if(ref==0)
            AddListBuffer(pList, (BYTE)velocity);
        else
            AddListBuffer(pList, 0);
        return 2;
    }
    else
    {
        AddListBuffer(pList, 0x90|(BYTE)channel);
        AddListBuffer(pList, (BYTE)note);
        if(ref==0)
            AddListBuffer(pList, (BYTE)velocity);
        else
            AddListBuffer(pList, 0);
        if(pwStatus)
            *pwStatus = wState;
        return 3;
    }
}    

// Objet Pitch Bend

CObjectEventPitchBend::CObjectEventPitchBend():
    value1(0),
    value2(64),
    bSelected(FALSE)
{
}

CObjectEventPitchBend::~CObjectEventPitchBend()
{
}

void  CObjectEventPitchBend::Write(HANDLE h)
{
    DWORD dwWritten;
    int   buf;
    buf = (int)GetType();
    WriteFile(h, &buf, 4, &dwWritten, NULL);
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &value1, 4, &dwWritten, NULL);
    WriteFile(h, &value2, 4, &dwWritten, NULL);
    WriteFile(h, &bSelected, 4, &dwWritten, NULL);
}

int CObjectEventPitchBend::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    WORD wState = (WORD)(0xE0|(BYTE)channel);
    
    if((pwStatus)&&(*pwStatus==wState))
    {
        AddListBuffer(pList, (BYTE)value1);
        AddListBuffer(pList, (BYTE)value2);
        return 2;
    }
    else
    {
        AddListBuffer(pList, 0xE0|(BYTE)channel);
        AddListBuffer(pList, (BYTE)value1);
        AddListBuffer(pList, (BYTE)value2);
        if(pwStatus)
            *pwStatus = wState;
        return 3;
    }
}

// Objet Key After Touch

CObjectEventKeyAfterTouch::CObjectEventKeyAfterTouch():
    note(0),
    velocity(0),
    bSelected(FALSE)
{
}

CObjectEventKeyAfterTouch::~CObjectEventKeyAfterTouch()
{
}

void  CObjectEventKeyAfterTouch::Write(HANDLE h)
{
    DWORD dwWritten;
    int   buf;
    buf = (int)GetType();
    WriteFile(h, &buf, 4, &dwWritten, NULL);
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &note, 4, &dwWritten, NULL);
    WriteFile(h, &velocity, 4, &dwWritten, NULL);
    WriteFile(h, &bSelected, 4, &dwWritten, NULL);
}

int CObjectEventKeyAfterTouch::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    WORD wState = (WORD)(0xA0|(BYTE)channel);
    
    if((pwStatus)&&(*pwStatus==wState))
    {
        AddListBuffer(pList, (BYTE)note);
        AddListBuffer(pList, (BYTE)velocity);
        return 2;
    }
    else
    {
        AddListBuffer(pList, 0xA0|(BYTE)channel);
        AddListBuffer(pList, (BYTE)note);
        AddListBuffer(pList, (BYTE)velocity);
        if(pwStatus)
            *pwStatus = wState;
        return 3;
    }
}

// Objet Channel Pressure

CObjectEventChannelPressure::CObjectEventChannelPressure():
    value(0),
    bSelected(FALSE)
{
}

CObjectEventChannelPressure::~CObjectEventChannelPressure()
{
}

void  CObjectEventChannelPressure::Write(HANDLE h)
{
    DWORD dwWritten;
    int   buf;
    buf = (int)GetType();
    WriteFile(h, &buf, 4, &dwWritten, NULL);
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &value, 4, &dwWritten, NULL);
    WriteFile(h, &bSelected, 4, &dwWritten, NULL);
}

int CObjectEventChannelPressure::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    WORD wState = (WORD)(0xD0|(BYTE)channel);
    
    if((pwStatus)&&(*pwStatus==wState))
    {
        AddListBuffer(pList, (BYTE)value);
        return 1;
    }
    else
    {
        AddListBuffer(pList, 0xD0|(BYTE)channel);
        AddListBuffer(pList, (BYTE)value);
        if(pwStatus)
            *pwStatus = wState;
        return 2;
    }
}

// Objet Controle Change

CObjectEventControlChange::CObjectEventControlChange():
    numController(7),
    value(100),
    bSelected(FALSE),
    pControl(NULL)
{
}

CObjectEventControlChange::~CObjectEventControlChange()
{
}

void  CObjectEventControlChange::Write(HANDLE h)
{
    DWORD dwWritten;
    int   buf;
    buf = (int)GetType();
    WriteFile(h, &buf, 4, &dwWritten, NULL);    
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &numController, 4, &dwWritten, NULL);
    WriteFile(h, &value, 4, &dwWritten, NULL);
    WriteFile(h, &bSelected, 4, &dwWritten, NULL);
}

int CObjectEventControlChange::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    int buf;
    if(numController==CTRLCHANGE_VOLUME)
        buf = min(127,(value*ref)/100);
    else if((numController==CTRLCHANGE_PAN)&&(ref>=256))
        buf = ref - 256;
    else
        buf = value;
    
    WORD wState = (WORD)(0xB0|(BYTE)channel);
    
    if((pwStatus)&&(*pwStatus==wState))
    {
        AddListBuffer(pList, (BYTE)numController);
        AddListBuffer(pList, (BYTE)buf);
        return 2;
    }
    else
    {
        AddListBuffer(pList, 0xB0|(BYTE)channel);
        AddListBuffer(pList, (BYTE)numController);
        AddListBuffer(pList, (BYTE)buf);
        if(pwStatus)
            *pwStatus = wState;
        return 3;
    }
}

// Objet Programme Change

CObjectEventProgramChange::CObjectEventProgramChange():
    numProgram(0),
    pControl(NULL)
{
}

CObjectEventProgramChange::~CObjectEventProgramChange()
{
}

void  CObjectEventProgramChange::Write(HANDLE h)
{
    DWORD dwWritten;
    int   buf;
    buf = (int)GetType();
    WriteFile(h, &buf, 4, &dwWritten, NULL);    
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &numProgram, 4, &dwWritten, NULL);
    WriteFile(h, &bSelected, 4, &dwWritten, NULL);
}

int CObjectEventProgramChange::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    WORD wState = (WORD)(0xC0|(BYTE)channel);
    
    if((pwStatus)&&(*pwStatus==wState))
    {
        AddListBuffer(pList, (BYTE)numProgram);
        return 1;
    }
    else
    {
        AddListBuffer(pList, 0xC0|(BYTE)channel);
        AddListBuffer(pList, (BYTE)numProgram);
        if(pwStatus)
            *pwStatus = wState;
        return 2;
    }
}

// Objet Meta

CObjectMetaEvent::CObjectMetaEvent():
    dataLen(0),
    pData(NULL)
{
}

CObjectMetaEvent::~CObjectMetaEvent()
{
}    

// Objet Tempo

CObjectMetaEventTempo::CObjectMetaEventTempo():
    tempo(120),
    bSelected(FALSE)
{
    dataLen = 3;
    pData = new BYTE[3];
    SetTempo(120);
}

CObjectMetaEventTempo::~CObjectMetaEventTempo()
{
    if(pData)
        delete pData;
}

void  CObjectMetaEventTempo::SetTempo(int t)
{
    int buf;
    buf = min(250, max(30, t)); // pour éviter la division par zero
    tempo = buf;
    buf = 60000000/buf;
    
    pData[0] = (BYTE)((buf & 0xFF0000)>>16);
    pData[1] = (BYTE)((buf & 0xFF00)>>8);
    pData[2] = (BYTE)(buf & 0xFF);
}

void  CObjectMetaEventTempo::Write(HANDLE h)
{
    DWORD dwWritten;
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, &tempo, 4, &dwWritten, NULL);
}    

int CObjectMetaEventTempo::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    AddListBuffer(pList, 0xFF);
    AddListBuffer(pList, 0x51);
    AddListBuffer(pList, 0x03);
    for(int i=0; i<3; i++)
        AddListBuffer(pList, pData[i]);
    if(pwStatus)
        *pwStatus = 0;
    return 6;
}

// Objet Time Signature

CObjectMetaEventTimeSignature::CObjectMetaEventTimeSignature()
{
    dataLen = 4;
    pData = new BYTE[4];
    if(pData)
    {
        pData[0] = (BYTE)4;
        pData[1] = (BYTE)2;
        pData[2] = (BYTE)24;
        pData[3] = (BYTE)8;
    }
    timeNoteIndex = 0;    
}

CObjectMetaEventTimeSignature::~CObjectMetaEventTimeSignature()
{
    if(pData)
        delete pData;
}

void CObjectMetaEventTimeSignature::SetTimeSignature(BYTE n, BYTE d, BYTE c, BYTE b)
{
    if(!pData)
        pData = new BYTE[4];
    if(pData)
    {
        pData[0] = n;
        pData[1] = d;
        pData[2] = c;
        pData[3] = b;
    }
}    

void CObjectMetaEventTimeSignature::GetTimeSignature(LPBYTE pSignature)
{
    if((pSignature)&&(pData))
    {
        pSignature[0] = pData[0];
        pSignature[1] = pData[1];
        pSignature[2] = pData[2];
        pSignature[3] = pData[3];
    }    
}

void CObjectMetaEventTimeSignature::Write(HANDLE h)
{
    DWORD dwWritten;
    WriteFile(h, &left, 4, &dwWritten, NULL);
    WriteFile(h, pData, 4, &dwWritten, NULL);
}

int CObjectMetaEventTimeSignature::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    AddListBuffer(pList, 0xFF);
    AddListBuffer(pList, 0x58);
    AddListBuffer(pList, 0x04);
    for(int i=0; i<4; i++)
        AddListBuffer(pList, pData[i]);
    if(pwStatus)
        *pwStatus = 0;
    return 7;
}

// Objet Texte, Copyright, paroles, marqueur, spécial

CObjectMetaEventText::CObjectMetaEventText(BYTE type)
{
    byType = type;
}

CObjectMetaEventText::~CObjectMetaEventText()
{
    if(pData)
        delete pData;
}

void CObjectMetaEventText::SetText(char * pText, int taille)
{
    if(!pText)
        return;
    if(pData)
        delete pData;
    
    dataLen = min(taille, 255);
    pData = new BYTE[dataLen];
    if(!pData)
    {
        dataLen = 0;
        return;
    }
    CopyMemory(pData, pText, dataLen);
}    

void CObjectMetaEventText::GetText(char * pText)
{
    if(!pText)
        return;
    if(!pData)
    {
        pText[0] = 0;
        return;
    }
    CopyMemory(pText, pData, dataLen);
    pText[dataLen] = 0; // terminaison NULL pour chaine de caractère
}       

void CObjectMetaEventText::Write(HANDLE h)
{
    DWORD dwWritten;
    if(!pData)
        return;
    int   buf;
    buf = (int)byType;
    WriteFile(h, &buf, 4, &dwWritten, NULL);  
    WriteFile(h, &left, 4, &dwWritten, NULL);    
    WriteFile(h, pData, dataLen, &dwWritten, NULL);
}

int CObjectMetaEventText::WriteBuffer(TListBuffer* pList, int ref, 
    int channel, WORD * pwStatus)
{
    if(!pData)
        return 0;
    AddListBuffer(pList, 0xFF);
    AddListBuffer(pList, byType);
    AddListBuffer(pList, (BYTE)(dataLen & 0xFF));
    for(int i=0; i<dataLen; i++)
        AddListBuffer(pList, pData[i]);
    if(pwStatus)
        *pwStatus = 0;
    return (3 + dataLen);
}
