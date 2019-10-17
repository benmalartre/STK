#ifndef _SEQUENCER_H_
#define _SEQUENCER_H_

#include "ObjectMidi.h"

#define  MAX_CHANNEL  16

#define ZONE_NULL   (UINT)0
#define ZONE_SIZING (UINT)1
#define ZONE_MOVING (UINT)2

#define FILTER_NOTE         1
#define FILTER_KEYPRESS     2
#define FILTER_CTRLCHANGE   4
#define FILTER_PRGMCHANGE   8
#define FILTER_CHANPRESS    16
#define FILTER_PITCHBEND    32
#define FILTER_SYSEX        64

#define FORMAT_0    1
#define FORMAT_1    2

struct TGlobal
{
    CObjectEvent          * pFirstMeta;
    CObjectEvent          * pLastMeta;
    CObjectEvent          * pFirstAll[MAX_CHANNEL];
    CObjectEvent          * pLastAll[MAX_CHANNEL];
    CObjectEvent          * pFirstSignature;
    CObjectEvent          * pLastSignature;
    CObjectMetaEventTempo * pFirstTempo;
    CObjectMetaEventTempo * pLastTempo;
    CObjectMetaEventText  * pTempNameTrack[MAX_CHANNEL];
    TControlUnity         * pFirstControl;
    TControlUnity         * pLastControl;
    TPattern              * pFirstPattern;
    TPattern              * pLastPattern;
    BOOL                    bOpen;
    BOOL                    bModify;
};

struct TOptionClick
{
    BOOL   bActiveM;
    BOOL   bActiveB;
    int    noteM;
    int    noteB;
    int    channelM;
    int    channelB;
    int    velocityM;
    int    velocityB;
}; 

struct TInit
{
    int           mod; // midiout device
    int           mid; // midiin device
    BOOL          bDemix; // demixage auto
    BOOL          bClick; // activation metronome pendant enregistrement
    BOOL          bFusion; // fusion des sequences créées
    BOOL          bQuantise; // activation de la qunatisation
    int           nQuantise; // valeur de la quantise
    UINT          uFilter; // parametre des filtres
    TOptionClick  optClick; // parametre de metronome
};
    
struct TTrackType
{
    int  channel;
    BOOL bMute;
};    

struct TEditorData
{
    TPattern  * pPtrn;   // sequence associé à la fenetre editeur
    int         xo;
    int         yo;
    int         cxTick;
    BOOL        bClose;
    RECT        rcPaint;    
};

struct TTempoMdiData
{
    int    xo;
    int    yo;
    int    cxTick;
    BOOL   bClose;
    RECT   rcPaint;
    POINT  oldCur;
};

// structure utilisée pour garder les valeurs courantes des controleurs volume et pan
struct TControlMidiCurrent
{
    int  vol;
    int  pan;
    BOOL bVol;
    BOOL bPan;
};

// structure utilisée pour marquer le niveau de led d'affichage
struct TLedLevel
{
    BYTE by1[16];
    BYTE by2[16];
    BYTE by3[16];
    BYTE by4[16];
};    

// structure utilisée pour referencer chaque objet dans une liste
struct TReferenceObject
{
    DWORD          dwTime;
    CObjectEvent * pObject;
    int            channel;
    int            reserved;
};    

// structure utilisée pour stocker la liste de tous les objets avant
// l'exportation des evenements en fichier MIDI
struct TListObject
{
    TReferenceObject * pRef;
    BOOL               bEnable;
    int                arraySize;
    int                count;
};    

struct TControl
{
    DWORD dwTimeTick;
    int   cxTick;
    int   cyTrack;
    int   xOrigine;     // point origine de scrollbar horizontal
    int   yOrigine;     // point d'origine de scrollbar vertical
    int   nResolution;
    int   nFirstLoop;
    int   nLastLoop;
    int   nFirstLocator;
    int   nLastLocator;
    int   nTrackCount;
    int   nTrackSelect;  // numero de piste selectionnée
    int   nVolume[MAX_CHANNEL];
    int   nPan[MAX_CHANNEL];
    BOOL  bPanActive[MAX_CHANNEL];
    BOOL  bLooped; // activer ou non le boucle de lecture
    BOOL  bClicked; // activer ou non le metronome
    BOOL  bGrid; // afficher grille ou non dans la fenetre arrangement
    BOOL  bBkWhite; // fond en blanc ou non la fenetre arrangement
    BOOL  bMaximize; // agrandir ou non les fenetre MDI
    BOOL  bInfoInclude; // inclure ou non les informations sequence
    
    TControl(): dwTimeTick(480),
                cxTick(10),
                cyTrack(25),
                xOrigine(0),
                yOrigine(0),
                nResolution(4),
                nFirstLoop(0),
                nLastLoop(1920),
                nFirstLocator(0),
                nLastLocator(228480),
                nTrackCount(16),
                nTrackSelect(-1),
                bLooped(FALSE),
                bClicked(FALSE),
                bGrid(TRUE),
                bBkWhite(FALSE),
                bMaximize(FALSE),
                bInfoInclude(TRUE)
                {}
};

class CSequencer
{
public:
    CSequencer(TGlobal *, TInit *, HWND *);
    ~CSequencer();
    
    void  SetGrid(BOOL);
    void  InverseGrid();
    void  InverseBkWhite();
    void  SetLooped(BOOL);
    void  SetClicked(BOOL);
    void  SetMaximized(BOOL);
    void  SetInfoInclude(BOOL);
    void  SetXOrigine(int);
    void  MoveXOrigine(int);
    void  SetYOrigine(int);
    void  MoveYOrigine(int);
    void  SetResolution(int);
    void  SetFirstLoop(int);
    void  MoveFirstLoop(int);
    void  SetLastLoop(int);
    void  MoveLastLoop(int);
    void  SetFirstLocator(int);
    void  SetLastLocator(int);
    void  SetTimeTick(int);
    void  SetCxTick(int);
    void  MoveCxTick(int);
    void  SetCyTrack(int);
    void  MoveCyTrack(int);
    void  SetMuteTrack(int, BOOL);
    void  InverseMuteTrack(int);
    void  SetChannelTrack(int, int);
    void  SetTrackSelect(int);
    void  SetVolumeTrack(int, int);
    void  MoveVolumeTrack(int, int);
    void  SetPanTrack(int, int);
    void  MovePanTrack(int, int);
    void  SetPanActive(int, BOOL);
    void  InversePanActive(int);
    
    int   GetXOrigine()const{return m_Ctrl.xOrigine;}
    int   GetYOrigine()const{return m_Ctrl.yOrigine;}
    DWORD GetTimeTick()const{return m_Ctrl.dwTimeTick;}
    int   GetCxTick()const{return m_Ctrl.cxTick;}
    int   GetCyTrack()const{return m_Ctrl.cyTrack;}
    int   GetResolution()const{return m_Ctrl.nResolution;}
    int   GetFirstLoop()const{return m_Ctrl.nFirstLoop;}
    int   GetLastLoop()const{return m_Ctrl.nLastLoop;}
    int   GetFirstLocator()const{return m_Ctrl.nFirstLocator;}
    int   GetLastLocator()const{return m_Ctrl.nLastLocator;}
    int   GetTrackCount()const{return m_Ctrl.nTrackCount;}
    int   GetTrackSelect()const{return m_Ctrl.nTrackSelect;}
    BOOL  IsGrid()const{return m_Ctrl.bGrid;}
    BOOL  IsLooped()const{return m_Ctrl.bLooped;}
    BOOL  IsClicked()const{return m_Ctrl.bClicked;}
    BOOL  IsBkWhite()const{return m_Ctrl.bBkWhite;}
    BOOL  IsMaximized()const{return m_Ctrl.bMaximize;}
    BOOL  IsMuteTrack(int)const;
    BOOL  IsInfoInclude()const{return m_Ctrl.bInfoInclude;}
    int   GetChannelTrack(int)const;
    int   ChannelToTrack(int)const;
    int   GetVolumeTrack(int n)const{return m_Ctrl.nVolume[n];}
    int   GetPanTrack(int n)const{return m_Ctrl.nPan[n];}
    BOOL  IsPanActive(int n)const{return m_Ctrl.bPanActive[n];}
    TPattern * GetPatternSelected();
    
    
    BOOL  Save(char*); // enregistre les sequences
    BOOL  Load(char*); // charge un fichier sequence
    char* GetFileName(); // retourne le nom de fichier courant
    char* GetFileImport(); // retourne le nom de fichier importé
    
    void  ReInit();
    void  GetInstrumentName(int, DWORD, char*);
    // tempo
    int   GetTempo(DWORD);
    void  SetTempo(DWORD, int);
    void  DeleteSelectedTempo();
    void  DeleteTempo(CObjectEvent*);
    // signature
    void  GetTimeSignature(DWORD, LPBYTE);
    void  SetTimeSignature(DWORD, const BYTE*);
    // meta texte
    void  SetTextEvent(BYTE, const void*, int);
    void  GetTextEvent(BYTE, char*);
    // ctrl change
    int   GetCtrlChange(BYTE, int, DWORD);
    void  SetCtrlChange(BYTE, int, DWORD, int);
    // prgm change
    int   GetPrgmChange(int, DWORD);
    void  SetPrgmChange(int, DWORD, int);
    // convertions
    int   GetTimeNoteIndex(DWORD);  // retourne l'index mesure d'un temps donné
    void  TimeCodeToString(DWORD, char*); // convertit le timeCode en mesure et temps
    DWORD TimeCodeToTimeClock(DWORD);
    DWORD TimeClockToTimeCode(DWORD);
    int   TimeCodeToBeat(DWORD);
    
    DWORD MeasureNextToTimeCode(DWORD);
    DWORD MeasureBackToTimeCode(DWORD);
    // leds
    void  ResetLed(int);
    void  AddLevelLed(int, BYTE, BYTE);
    void  RemoveLevelLed(int, BYTE);
    // liste des controles
    BOOL  AddControlUnity(CObjectEvent*, int);
    void  DeleteControlUnity(TControlUnity*);
    // patterns
    BOOL  CreatePattern(int, int);  // creation d'un nouveau sequence
    TPattern *  CreatePattern(TPatternValue*);
    BOOL  DuplicatePattern();
    TPattern *  SelectPattern(int, int, BOOL, UINT*); // seléctionner une sequence
    void  SelectAllPattern(int); // séléctionner les sequences de meme piste
    void  DeleteSelectedPattern(); // supprime une ou plusieures sequences séléctionnées
    BOOL  MergeSelectedPattern(); // fusionne une ou plusieures sequences séléctionnées
    BOOL  DivideSelectedPattern(int);
    BOOL  SizePattern(int); // redimensionne une ou plusieur sequence
    BOOL  MovePattern(int, int); // deplace une ou plusieures sequence
    void  UpdateIDPattern(int); // remet à jour les identifiants des sequences
    // notes
    BOOL  CreateNote(int, int, TEditorData *);
    CObjectEvent * CreateNote(TPattern *);
    CObjectEventNote * SelectNote(int, int, TEditorData *, BOOL, UINT*);
    void  SelectAllNote(int, TEditorData*);
    void  DeleteSelectedNote(TEditorData *);
    BOOL  SizeNote(int, CObjectEventNote *, TEditorData *);
    BOOL  MoveNote(int, int, CObjectEventNote *, TEditorData *);
    // pitch bend
    CObjectEvent * CreatePitchBend(TPattern *);
    // channel pressure
    CObjectEvent * CreateChannelPressure(TPattern *);
    // key after touch
    CObjectEvent * CreateKeyAfterTouch(TPattern *);
    // ctrl change
    CObjectEvent * CreateControlChange(TPattern *);
    // Prgm Change
    CObjectEvent * CreateProgramChange(TPattern*);
    
    void  DeleteAll();
    int   ScreenToTimeEvent(int);   // convertit l'abcisse de l'ecran en mesure temps
    
private:
    
    void  UpdateTimeSignature();
    BOOL  CopyAllEvent(TPattern * , TPattern * );
    TPattern* CreateSamePattern(TPattern *);    
    int   ScreenToNumTrack(int);
    TPattern * ScreenToPattern(int, int, UINT*); // retourne l'adresse d'une sequence pointée 
    void  DeletePattern(TPattern *, BOOL);
    void  DeselectedAllPattern();
    BOOL  IsCutNote(TPattern*, int);
    BOOL  MergePattern(TPattern *); // fusionne une sequence avec celle de droite
    BOOL  DividePattern(TPattern*, int);
    
    void  DeselectedAllNote(TEditorData *);
    int   ScreenToTimeEvent(int, TEditorData *);
    int   ScreenToNoteValue(int, TEditorData *);
    CObjectEventNote * ScreenToNoteObject(int, int, TEditorData *, UINT *);
    void  DeleteNote(CObjectEvent *, TPattern *);
    
    void  DeleteAllEvent(TPattern *); // supprime tous les evenements dans une sequence
    
    void  DeleteEvent(CObjectEvent *, TPattern *);
    
private:
    HWND     * m_pHMdi;    
    TPattern * m_pPatternSelected;
    POINT      m_oldPtCur; // anciennes coordonnées du curseur
    TTrackType m_tTrack[MAX_CHANNEL]; // pour savoir le n° canaux affécté à la piste
    TGlobal  * m_pGlobal;
    TInit    * m_pInit;
    char       m_szFileName[MAX_PATH]; // nom de fichier courant
    char       m_szFileImport[MAX_PATH]; // nom de fichier midi importé
    
public:
    char       m_szInstrument[128][32]; // tableau pour les noms des instruments courants
    TControlMidiCurrent m_cCurrent[MAX_CHANNEL];
    TLedLevel           m_led[MAX_CHANNEL];
    TControl            m_Ctrl;
    TOptionClick        m_optClick;
    
/********************************************************************
* Import et export des fichiers MIDI
*
********************************************************************/

public:
    BOOL   ImportMidiFile(char*);
    BOOL   ExportMidiFile(char*, int);
    
private:
    WORD   WordSwap(WORD); // pour convertir les données ENDIAN
    DWORD  DwordSwap(DWORD);
    DWORD  GetVarLen(LPBYTE *);
    BOOL   CollectMidiData(LPBYTE*, DWORD);
    BOOL   CollectMidiMeta(LPBYTE*, DWORD);
    BOOL   CollectMidiNote(LPBYTE*, DWORD, BOOL);
    BOOL   CollectMidiPitchBend(LPBYTE*, DWORD, BOOL);
    BOOL   CollectMidiKeyAfterTouch(LPBYTE*, DWORD, BOOL);
    BOOL   CollectMidiChannelPressure(LPBYTE*, DWORD, BOOL);
    BOOL   CollectMidiPrgmChange(LPBYTE*, DWORD, BOOL);
    BOOL   CollectMidiCtrlChange(LPBYTE*, DWORD, BOOL);
    
    void  ExtractListObject(int);
    BOOL  AddListObject(TListObject*, TReferenceObject*);
    void  SortListObject(TListObject*);
    void  FreeListObject();
    
    void  CreateTempNameTrack(int, char*);
    void  RecoverTempNameTrack();
    void  FreeTempNameTrack();
    
    TListObject m_mList[MAX_CHANNEL];
    TListObject m_mListMeta;
    TPattern  * m_pOldPattern;
    BYTE        m_byStatus;
    char        m_szNameTrack[256];
    int         m_oldChannel;
    int         m_maxLenght;
};    

#endif
