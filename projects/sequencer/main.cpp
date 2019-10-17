#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include "Resource.h"
#include "Sequencer.h"
#include "MidiSphinx.h"
#include "Arrangement/Arrange.h"
#include "Editor/Editor.h"
#include "List/ListMdi.h"
#include "BlocCmd.h"
#include "MixerMdi.h"
#include "ToolTip.h"
#include "DlgWaveIn.h"
#include "DlgHelp.h"
#include "Options/TempoMdi.h"
#include "Options/DlgOptions.h"

#define IDC_POSTCLOSE  41000

HWND  hwndBlocCmd = NULL;
HWND  hwndArrange = NULL;
HWND  g_hWndTempo = NULL;
HWND  g_hWndMixer = NULL;


void UpdateMenu();
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK FormatProc(HWND, UINT, WPARAM, LPARAM);

/****************************************************
* Les variables globales
*
****************************************************/

HINSTANCE  g_hInst;
HWND       g_hMdi;
HWND       g_hMain;
HCURSOR    g_hCursor[4];
HBITMAP    g_hClavier, g_hbmMute, g_hbmTool2;
HBITMAP    g_hbmSlider;
HBITMAP    g_hbmKnob1;
HBITMAP    g_hbmKnob2;
HBITMAP    g_hbmLed;
HBITMAP    g_hbmPan;
HBRUSH     g_hBrushBarPos;

TInit       g_cInit;
TGlobal     g_base;
CSequencer  g_cSeq(&g_base, &g_cInit, &g_hMdi);
CMidiSphinx g_midi(&g_base, &g_cInit);
CToolTip    g_toolTip;

char g_szClassName[] = "WindowsAppSphinx";
char g_szWndTitle[] = "Sphinx 1.0";

CBlocCmd      blocCmd;

/*************************************************************
* AboutHelp : messagebox pour l'aide "A propos de..."
*
*************************************************************/

void AboutHelp(HWND hwnd)
{
    MSGBOXPARAMS  mbp;
    
    mbp.cbSize      = sizeof(MSGBOXPARAMS);
    mbp.dwStyle     = MB_USERICON;
    mbp.hInstance   = g_hInst;
    mbp.hwndOwner   = hwnd;
    mbp.lpszIcon    = MAKEINTRESOURCE(IDI_SPHINX);
    mbp.lpszCaption = " A propos de...";
    mbp.lpszText    = "        Sphinx   Version 1.0  \n\n   Freeware         Mai   2005      \n\n\
          Auteur :  gagah1 \n\n       Créateur de Logiciel";
    
    MessageBoxIndirect(&mbp);
}

/***********************************************************
* DlgOpenFileSphinx : affichage de la boite de dialogue commune
* pour ouvrir un fichier
*  pFileName (out): reçoit le nom de fichier selectionné
* 
***********************************************************/

BOOL DlgOpenFileSphinx(HWND hwnd, char *pFileName)
{
    OPENFILENAME ofn;
    
    pFileName[0] = '\0';
    ZeroMemory(&ofn, sizeof(ofn));
    
    ofn.lStructSize   = sizeof(ofn);
    ofn.hwndOwner     = hwnd;
    ofn.lpstrFilter   = "Fichier Sequence Sphinx (*.inx)\0*.inx\0\0";
    ofn.lpstrFile     = pFileName;
    ofn.nMaxFile      = MAX_PATH;
    ofn.lpstrDefExt   = "inx";
    ofn.lpstrTitle    = "Ouvrir un fichier Sequence Sphinx...";
    ofn.Flags         = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
    if(GetOpenFileName(&ofn))
        return TRUE;
    return FALSE;
}

/***********************************************************
* DlgOpenMidiFile : affichage de la boite de dialogue commune
* pour ouvrir un fichier
*  pFileName (out): reçoit le nom de fichier selectionné
* 
***********************************************************/

BOOL DlgOpenMidiFile(HWND hwnd, char *pFileName)
{
    OPENFILENAME ofn;
    
    pFileName[0] = '\0';
    ZeroMemory(&ofn, sizeof(ofn));
    
    ofn.lStructSize   = sizeof(ofn);
    ofn.hwndOwner     = hwnd;
    ofn.lpstrFilter   = "Fichier Midi (*.mid)\0*.mid\0\0";
    ofn.lpstrFile     = pFileName;
    ofn.nMaxFile      = MAX_PATH;
    ofn.lpstrDefExt   = "mid";
    ofn.lpstrTitle    = "Importer un fichier Midi...";
    ofn.Flags         = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
    if(GetOpenFileName(&ofn))
        return TRUE;
    return FALSE;
}


/***********************************************************
* DlgSaveFileSphinx : affichage de la boite de dialogue 
* commune pour sauvegarder en fichier INX
* pFileName (out): reçoit le nom de fichier saisi
* 
***********************************************************/

BOOL DlgSaveFileSphinx(HWND hwnd, char *pFileName)
{
    OPENFILENAME ofn;
    
    pFileName[0] = '\0';
    ZeroMemory(&ofn, sizeof(ofn));
    
    ofn.lStructSize   = sizeof(ofn);
    ofn.hwndOwner     = hwnd;
    ofn.lpstrFilter   = "Fichier Sequence Sphinx (*.inx)\0*.inx\0\0";
    ofn.lpstrFile     = pFileName;
    ofn.nMaxFile      = MAX_PATH;
    ofn.lpstrDefExt   = "inx";
    ofn.lpstrTitle    = "Enregistrer sous...";
    ofn.Flags         = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    if(GetSaveFileName(&ofn))
        return TRUE;
    return FALSE;
}

/***********************************************************
* DlgSaveMidiFile : affichage de la boite de dialogue 
* commune pour sauvegarder en fichier MIDI
* pFileName (out): reçoit le nom de fichier saisi
* 
***********************************************************/

BOOL DlgSaveMidiFile(HWND hwnd, char *pFileName)
{
    OPENFILENAME ofn;
    
    pFileName[0] = '\0';
    ZeroMemory(&ofn, sizeof(ofn));
    
    ofn.lStructSize   = sizeof(ofn);
    ofn.hwndOwner     = hwnd;
    ofn.lpstrFilter   = "Fichier Midi (*.mid)\0*.mid\0\0";
    ofn.lpstrFile     = pFileName;
    ofn.nMaxFile      = MAX_PATH;
    ofn.lpstrDefExt   = "mid";
    ofn.lpstrTitle    = "Exporter en fichier MIDI sous...";
    ofn.Flags         = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    if(GetSaveFileName(&ofn))
        return TRUE;
    return FALSE;
}

/***********************************************************
* DlgSaveFileWave : affichage de la boite de dialogue 
* commune pour sauvegarder en fichier WAVE
* pFileName (out): reçoit le nom de fichier saisi
* 
***********************************************************/

BOOL DlgSaveFileWave(HWND hwnd, char *pFileName)
{
    OPENFILENAME ofn;
    
    pFileName[0] = '\0';
    ZeroMemory(&ofn, sizeof(ofn));
    
    ofn.lStructSize   = sizeof(ofn);
    ofn.hwndOwner     = hwnd;
    ofn.lpstrFilter   = "Fichier Wave (*.wav)\0*.wav\0\0";
    ofn.lpstrFile     = pFileName;
    ofn.nMaxFile      = MAX_PATH;
    ofn.lpstrDefExt   = "wav";
    ofn.lpstrTitle    = "Convertir sous...";
    ofn.Flags         = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    if(GetSaveFileName(&ofn))
        return TRUE;
    return FALSE;
}

/******************************************************************
* GetShortFileName : modifie le nom de fichier complet en nom 
* court sans le chemin
*
******************************************************************/

char* GetShortFileName(char * longFileName)
{
    char * p1, *p2;
    
    p1 = p2 = longFileName;
    
    while(*p1 != '\0')
    {
        if(*p1 == '\\')
            p2 = p1 + 1;
        p1++;
    }
    return p2;
}

/******************************************************************
* UpdateMenu : mis à jour des menus
*
******************************************************************/

void UpdateMenu()
{
    HMENU hMenu = GetMenu(g_hMain);
    EnableMenuItem(hMenu, 1, g_base.bOpen ? MF_BYPOSITION|MF_ENABLED : MF_BYPOSITION|MF_GRAYED);   
    EnableMenuItem(hMenu, 2, g_base.bOpen ? MF_BYPOSITION|MF_ENABLED : MF_BYPOSITION|MF_GRAYED);
    EnableMenuItem(hMenu, 3, g_base.bOpen ? MF_BYPOSITION|MF_ENABLED : MF_BYPOSITION|MF_GRAYED);   
    EnableMenuItem(hMenu, 4, g_base.bOpen ? MF_BYPOSITION|MF_ENABLED : MF_BYPOSITION|MF_GRAYED);
    
    EnableMenuItem(hMenu, IDM_FILESAVE, (g_base.bOpen&&g_base.bModify) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_FILESAVEAS, g_base.bOpen ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_FILECLOSE, g_base.bOpen ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_FILEWAVEIN, g_base.bOpen ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_FILEEXPORT, g_base.bOpen ? MF_ENABLED : MF_GRAYED);
    
    CheckMenuItem(hMenu, IDM_WHITEBACKGROUND,g_cSeq.IsBkWhite() ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_GRID, g_cSeq.IsGrid() ? MF_CHECKED : MF_UNCHECKED);
    DrawMenuBar(g_hMain);
}

/*******************************************************************************
* UpdateTitle : met à jour le titre de la fenetre principale
*
*******************************************************************************/

void UpdateTitle()
{
    char szBuffer[300];    
    
    if(!g_base.bOpen)
        lstrcpy(szBuffer, g_szWndTitle);    
    else if(lstrlen(g_cSeq.GetFileName())!=0)
        wsprintf(szBuffer, "%s  :  %s ", g_szWndTitle, GetShortFileName(g_cSeq.GetFileName()));
    else if(lstrlen(g_cSeq.GetFileImport())!=0)
        wsprintf(szBuffer, "%s  :  %s ", g_szWndTitle, 
            GetShortFileName(g_cSeq.GetFileImport()));
    else 
        wsprintf(szBuffer, "%s  :  sans titre ", g_szWndTitle);
    
    SetWindowText(g_hMain, szBuffer);   
}

/***************************************************************
* MsgSaveModified : fonction qui affiche une boite de dialogue
* avant la fermeture de l'application si le contenu du fichier
* a changé
*  retour : vrai si on clique sur OUI ou NON , faux si on
*           clique sur ANNULER
*
***************************************************************/

BOOL  MsgSaveModified(HWND hwnd)
{
    int ret;
    
    if((g_base.bOpen)&&(g_base.bModify))
    {
        ret = MessageBox(hwnd, "     Enregistrer les modifications .  ", g_szWndTitle, 0x33);
        if(ret==IDYES)
        {
            SendMessage(hwnd, WM_COMMAND, (WPARAM)IDM_FILESAVE, 0);
            return TRUE;
        }
        else if(ret==IDNO)
            return TRUE;
        else
            return FALSE;
    }
    return TRUE;
}

/******************************************************************
* CompareFileExtension : compare l'extension du fichier pFile
*  par rapport à l'extension pExt
*
******************************************************************/

BOOL CompareFileExtension(const char* pFile, const char* pExt)
{
    if((!pFile)||(!pExt))
        return FALSE;
    
    char szBuffer[MAX_PATH];
    char szExt[16];
    szBuffer[0] = 0;
    
    for(int i=lstrlen(pFile)-1; i>=0; i--)
    {
        if(pFile[i]=='.')
        {
            lstrcpy(szBuffer, &pFile[i]);
            break;
        }    
    }
    
    if(szBuffer[0]==0)
        return FALSE;
    lstrcpy(szExt, pExt);
    CharLower(szExt);
    CharLower(szBuffer);
    if(lstrcmp(szBuffer, szExt))
        return FALSE;
    return TRUE;
}    

/******************************************************************************
* LoadOptions : charge les options
*
******************************************************************************/

void LoadOptions()
{
    char szIniFile[MAX_PATH];
    GetModuleFileName(g_hInst, szIniFile, MAX_PATH);
    PathRenameExtension(szIniFile, ".ini");
    
    GetPrivateProfileStruct("Options", "General", &g_cInit, sizeof(TInit), szIniFile);
}

/******************************************************************************
* SaveOptions : sauvegarde les options
*
******************************************************************************/

void SaveOptions()
{
    char szIniFile[MAX_PATH];
    GetModuleFileName(g_hInst, szIniFile, MAX_PATH);
    PathRenameExtension(szIniFile, ".ini");
    
    WritePrivateProfileStruct("Options", "General", &g_cInit, sizeof(TInit), szIniFile);
}


/*****************************************************
* WinMain : point d'entrée de l'application
*
*****************************************************/

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszArgument, int nFunsterStil)
{
    MSG messages;
    WNDCLASSEX wincl;
    
    g_hBrushBarPos = CreateSolidBrush(RGB(207,239,255));
    InitCommonControls();
    g_hInst = hThisInstance;
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = g_szClassName;
    wincl.lpfnWndProc = WinProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);    
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (g_hInst, MAKEINTRESOURCE(IDI_SPHINX));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDM_MENU);
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;    
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    
    if (!RegisterClassEx (&wincl))
        return 0;
        
    if(!Arrange_InitInstance())
        return 0;
    
    if(!Editor_InitInstance())
        return 0;
    
    if(!blocCmd.InitInstance())
        return 0;
    
    if(!MixerMdi_InitInstance())
        return 0;
    
    if(!ListMdi_InitInstance())
        return 0;
        
    if(!TempoMdi_InitInstance())
        return 0;
    
    if(!CToolTip::InitInstance(g_hInst))
        return 0;
    
    g_cInit.mod = -1;  // MIDIMAPPER
    g_cInit.mid = 0;
    g_cInit.bDemix = TRUE;
    g_cInit.bClick = FALSE;
    g_cInit.bFusion = FALSE;
    g_cInit.bQuantise = FALSE;
    g_cInit.nQuantise = 7;
    g_cInit.uFilter = 63; // tous les filtres sauf sysEx
    
    // initialisation options métronome
    g_cInit.optClick.bActiveB = TRUE;
    g_cInit.optClick.bActiveM = TRUE;
    g_cInit.optClick.noteB = 37;
    g_cInit.optClick.noteM = 40;
    g_cInit.optClick.channelB = 9;
    g_cInit.optClick.channelM = 9;
    g_cInit.optClick.velocityB = 100;
    g_cInit.optClick.velocityM = 110;    
    
    // charge les options
    LoadOptions();
    
    g_cSeq.ReInit();
    g_midi.Open(&g_cSeq);
    
    g_hMain = CreateWindowEx(0, g_szClassName, g_szWndTitle,
        WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_MAXIMIZE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        HWND_DESKTOP, NULL, hThisInstance, NULL);
    
    ShowWindow (g_hMain, SW_MAXIMIZE);
    UpdateWindow(g_hMain);
    
    // si un fichier est à ouvrir    
    if(__argc>1)
    {
        if(((CompareFileExtension(__argv[1], ".mid"))&&(g_cSeq.ImportMidiFile(__argv[1])))||
        ((CompareFileExtension(__argv[1], ".inx"))&&(g_cSeq.Load(__argv[1]))))
        {
            g_midi.SetCurrentValue(0);
            g_midi.Start();
            g_base.bOpen = TRUE;
            UpdateMenu();
            UpdateTitle();
            blocCmd.Create(g_hMain);
            Arrange_Create(g_hMdi);
        }
        else
        {
            MessageBox(g_hMain, "  On ne peut pas ouvrir ce fichier .   ", 
                g_szWndTitle, MB_ICONINFORMATION);
        }
    }
    
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if(!TranslateMDISysAccel(g_hMdi, &messages))
        {
            {
                TranslateMessage(&messages);        
                DispatchMessage(&messages);
            }
        }
    }
    // ferme les midi
    g_midi.Close();
    
    // sauvegarde les options
    SaveOptions();
    return messages.wParam;
}

/***************************************************************
* WinProc : traitement de message de fenetre principale
*
***************************************************************/

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hChild;
    MDICREATESTRUCT mcs;
    char szFileBuf[MAX_PATH];
    static CDlgWaveIn * pwi;
    int i;
    
    switch (message)
    {
        case WM_CREATE:
            CLIENTCREATESTRUCT ccs;
            
            ccs.hWindowMenu = GetSubMenu(GetMenu(hwnd), 4);
            ccs.idFirstChild = 10000;
            
            g_hCursor[0] = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_HAND2));
            g_hCursor[1] = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_CHISEL));
            g_hCursor[2] = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_STICK));
            g_hCursor[3] = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_RUBBER));
            g_hClavier = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CLAVIER));
            g_hbmMute = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_MUTE));
            g_hbmTool2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TOOLBOX2));
            g_hbmSlider = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_SLIDER));
            g_hbmKnob1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KNOB1));
            g_hbmKnob2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KNOB2));
            g_hbmLed = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LED));
            g_hbmPan = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PAN));
            
            g_hMdi = CreateWindowEx (WS_EX_CLIENTEDGE, "mdiclient", NULL,
            WS_CHILD|WS_CLIPCHILDREN|WS_VSCROLL|WS_HSCROLL,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hwnd, NULL, g_hInst, (LPVOID)&ccs);
            
            ShowWindow(g_hMdi, SW_MAXIMIZE);
            //blocCmd.Create(hwnd);
            //Arrange_Create(g_hMdi);            
            g_base.bModify = FALSE;
            
            SetFocus(hwnd);
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_FILENEW:
                    if(!MsgSaveModified(hwnd))
                        break;
                    if(g_base.bOpen)
                        SendMessage(hwnd, WM_COMMAND, (WPARAM)IDC_POSTCLOSE, 0);
                    g_cSeq.ReInit();
                    g_midi.SetCurrentValue(0);
                    g_midi.Start();
                    g_base.bOpen = TRUE;
                    UpdateMenu();
                    UpdateTitle();
                    blocCmd.Create(hwnd);
                    Arrange_Create(g_hMdi);
                    break;
                case IDM_FILEOPEN:
                    if(!MsgSaveModified(hwnd))
                        break;
                    if(g_base.bOpen)
                        SendMessage(hwnd, WM_COMMAND, (WPARAM)IDC_POSTCLOSE, 0);
                    g_cSeq.ReInit();
                    if(!DlgOpenFileSphinx(hwnd, szFileBuf))
                        break;
                    if(!g_cSeq.Load(szFileBuf))
                    {
                        MessageBox(hwnd, "  On ne peut pas ouvrir ce fichier .   ", 
                            g_szWndTitle, MB_ICONINFORMATION);
                        break;
                    }
                    g_midi.SetCurrentValue(0);
                    g_midi.Start();
                    g_base.bOpen = TRUE;
                    UpdateMenu();
                    UpdateTitle();
                    blocCmd.Create(hwnd);
                    Arrange_Create(g_hMdi);   
                    break;
                case IDM_FILEIMPORT:
                    if(!MsgSaveModified(hwnd))
                        break;
                    if(g_base.bOpen)
                        SendMessage(hwnd, WM_COMMAND, (WPARAM)IDC_POSTCLOSE, 0);
                    g_cSeq.ReInit();
                    if(!DlgOpenMidiFile(hwnd, szFileBuf))
                        break;
                    if(!g_cSeq.ImportMidiFile(szFileBuf))
                    {
                        MessageBox(hwnd, "  On ne peut pas ouvrir ce fichier .   ", 
                            g_szWndTitle, MB_ICONINFORMATION);
                        break;
                    }
                    g_midi.SetCurrentValue(0);
                    g_midi.Start();
                    g_base.bOpen = TRUE;
                    UpdateMenu();
                    UpdateTitle();
                    blocCmd.Create(hwnd);
                    Arrange_Create(g_hMdi);
                    break;
                case IDM_FILESAVE:
                    if(lstrlen(g_cSeq.GetFileName())==0)
                        SendMessage(hwnd, WM_COMMAND, (WPARAM)IDM_FILESAVEAS, 0L);
                    else
                        g_cSeq.Save(g_cSeq.GetFileName());
                    break;
                case IDM_FILESAVEAS:
                    if(!DlgSaveFileSphinx(hwnd, szFileBuf))
                        break;
                    if(g_cSeq.Save(szFileBuf))
                        UpdateTitle();
                    break;
                case IDM_FILEEXPORT:
                    if(!DlgSaveMidiFile(hwnd, szFileBuf))
                        break;
                    int format;
                    format = DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DLGFORMAT),hwnd,
                        FormatProc,0);
                    if(format==0)
                        break;
                    if(g_cSeq.ExportMidiFile(szFileBuf, format))
                        MessageBeep(1);
                    break;
                case IDM_FILECLOSE:
                    if(!MsgSaveModified(hwnd))
                        break;
                case IDC_POSTCLOSE:
                    if(g_midi.m_bSequenceGoing)
                        g_midi.End();
                    if(hwndArrange)
                    {
                        SendMessage(hwndArrange, WM_CLOSE, 0, 0);
                        hwndArrange = NULL;
                    }
                    if(hwndBlocCmd)
                    {
                        SendMessage(hwndBlocCmd, WM_CLOSE, 0, 0);
                        hwndBlocCmd = NULL;
                    }
                    if(g_hWndTempo)
                    {
                        SendMessage(g_hWndTempo, WM_CLOSE, 0, 0);
                        g_hWndTempo = NULL;
                    }
                    if(g_hWndMixer)
                    {
                        SendMessage(g_hWndMixer, WM_CLOSE, 0, 0);
                        g_hWndMixer = NULL;
                    }
                    g_cSeq.DeleteAll();
                    g_midi.Reset();
                    g_base.bOpen = FALSE;
                    g_base.bModify = FALSE;
                    UpdateMenu();
                    UpdateTitle();       
                    break;
                case IDM_FILEWAVEIN:
                    if(!DlgSaveFileWave(hwnd, szFileBuf))
                        break;
                    pwi = new CDlgWaveIn;
                    if(!pwi)
                        break;
                    if(hwndBlocCmd)
                        ShowWindow(hwndBlocCmd, SW_HIDE);
                    pwi->Create(hwnd, &g_midi, szFileBuf);
                    delete pwi;
                    pwi = NULL;
                    if(hwndBlocCmd)
                        ShowWindow(hwndBlocCmd, SW_NORMAL);
                    break;
                case IDM_FILEEXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_GRID:
                    g_cSeq.InverseGrid();
                    CheckMenuItem(GetMenu(hwnd), IDM_GRID, 
                        g_cSeq.IsGrid() ? MF_CHECKED : MF_UNCHECKED);
                    InvalidateRect(GetDlgItem(hwndArrange, 198), NULL, TRUE);
                    break;
                case IDM_WHITEBACKGROUND:
                    g_cSeq.InverseBkWhite();
                    CheckMenuItem(GetMenu(hwnd), IDM_WHITEBACKGROUND, 
                        g_cSeq.IsBkWhite() ? MF_CHECKED : MF_UNCHECKED);
                    InvalidateRect(GetDlgItem(hwndArrange, 198), NULL, TRUE);
                    break;
                case IDM_TEMPO:
                    TempoMdi_Create(g_hMdi);
                    break;
                case IDM_CLICK:
                    CDlgClick * pDlgClick;
                    pDlgClick = new CDlgClick;
                    if(!pDlgClick)
                        break;
                    pDlgClick->Create(hwnd, &g_cSeq, &g_cInit);
                    delete pDlgClick;
                    break;
                case IDM_INFOPATTERN:
                    CDlgInfoPattern * pDlgInfo;
                    pDlgInfo = new CDlgInfoPattern;
                    if(!pDlgInfo)
                        break;
                    pDlgInfo->Create(hwnd, &g_cSeq);
                    delete pDlgInfo;
                    break;
                case IDM_MIDIDEVICE:
                    CDlgDevice * pDlgDevice;
                    pDlgDevice = new CDlgDevice;
                    if(!pDlgDevice)
                        break;
                    pDlgDevice->Create(hwnd, &g_midi);
                    delete pDlgDevice;
                    break;
                case IDM_MIDIOPTION:
                    CDlgRecord * pDlgRecord;
                    pDlgRecord = new CDlgRecord;
                    if(!pDlgRecord)
                        break;
                    pDlgRecord->Create(hwnd, &g_midi);
                    delete pDlgRecord;
                    break;
                case IDM_WNDARRANGE:
                    Arrange_Create(g_hMdi);
                    break;
                case IDM_WNDCOMMAND:
                    blocCmd.Create(hwnd);
                    break;
                case IDM_WNDLIST:
                    ListMdi_Create(g_hMdi);
                    break;
                case IDM_WNDMATRIX:
                    Editor_Create(g_hMdi);
                    break;
                case IDM_WNDMIXER:
                    MixerMdi_Create(g_hMdi);
                    break;
                case IDM_ABOUTHELP:
                    AboutHelp(hwnd);
                    break;
                case IDM_HELPFILE:
                    CDlgHelp * pDlgHelp;
                    pDlgHelp = new CDlgHelp;
                    if(!pDlgHelp)
                        break;
                    pDlgHelp->Create(hwnd);
                    delete pDlgHelp;
                    break;
                default:
                    return DefFrameProc(hwnd, g_hMdi, message, wParam, lParam);
            }    
            break;
        case WM_DESTROY:
            DeleteObject(g_hBrushBarPos);
            DeleteObject(g_hClavier);
            DeleteObject(g_hbmMute);
            DeleteObject(g_hbmTool2);
            DeleteObject(g_hbmSlider);
            DeleteObject(g_hbmKnob1);
            DeleteObject(g_hbmKnob2);
            DeleteObject(g_hbmLed);
            DeleteObject(g_hbmPan);
            for(i=0; i<4; i++)
                DeleteObject(g_hCursor[i]);
            PostQuitMessage (0);
            break;
        default:
            return DefFrameProc(hwnd, g_hMdi, message, wParam, lParam);
    }
    return 0;
}

/*****************************************************
* FormatProc : traitement des messages de la boite de
* dialogue Format utilisé avant enregistrement de
* fichier MIDI
*
*****************************************************/

BOOL CALLBACK FormatProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
            SendDlgItemMessage(hwnd, IDC_FORMAT, CB_RESETCONTENT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_FORMAT, CB_ADDSTRING, 0, (LPARAM)" Format 0");
            SendDlgItemMessage(hwnd, IDC_FORMAT, CB_ADDSTRING, 0, (LPARAM)" Format 1");
            SendDlgItemMessage(hwnd, IDC_FORMAT, CB_SETCURSEL, 1, 0);
            return TRUE;
            
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
                case IDOK:
                    if(SendDlgItemMessage(hwnd, IDC_FORMAT, CB_GETCURSEL, 0, 0)==0)
                        EndDialog(hwnd, FORMAT_0);
                    else
                        EndDialog(hwnd, FORMAT_1);
                    return TRUE;
            }    
            break;
        
        case WM_SYSCOMMAND:
            if(LOWORD(wParam)==SC_CLOSE)
            {
                EndDialog(hwnd, 0);
                return TRUE;  
            }
            break;
    }
    return FALSE;
}
