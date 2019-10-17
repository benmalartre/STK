#include <windows.h>
#include <commctrl.h>
#include "BlocCmd.h"
#include "MidiSphinx.h"
#include "Resource.h"

extern HWND  hwndBlocCmd;
extern HWND        g_hMdi,hwndArrange;
extern HINSTANCE   g_hInst;
extern TGlobal     g_base;
extern CMidiSphinx g_midi;

#define ID_TIMERBLOC 1

#define NB_COMMAND    11
#define ID_PLAY       0
#define ID_STOP       1
#define ID_LOOP       2
#define ID_CLICK      3    // metronome
#define ID_POINTER    4    // affichage position pointeur
#define ID_TEMPO      5    // affichage tempo
#define ID_SIGNATURE  6    // affichage signature
#define ID_REWIND     7
#define ID_RECORD     8
#define ID_NEXT       9
#define ID_BACK       10

#define NB_CTRL    3
#define IDC_TEMPO         20
#define IDC_NUM_SIGNATURE 21
#define IDC_DEN_SIGNATURE 22

const RECT rcRef[NB_COMMAND] = {{130,56,169,88},{89,56,128,88},{209,3,251,19},{209,23,251,39},
                    {100,5,180,25},{110,25,140,40},{150,25,180,40},{7,56,46,88},{212,56,248,88},
                    {171,56,210,88},{48,56,87,88}};

const RECT rcCtrl[NB_CTRL] = {{110,25,140,40},{145,25,160,40},{160,25,180,40}};

/***********************************************************
* Constructeur
*
***********************************************************/

CBlocCmd::CBlocCmd():
    m_dwOldTime(0),
    m_nOldTempo(0),
    m_oldFirstLocator(0),
    m_oldLastLocator(0)
{
    lstrcpy(m_szPointer, "0001.01.000");
    lstrcpy(m_szFirstLocator, "0001.01.000");
    lstrcpy(m_szLastLocator, "0120.01.000");
    lstrcpy(m_szTempo, "120");
    lstrcpy(m_szSignature, "4/4");
    for(int i=0; i<4; i++)
        m_oldSignature[i] = 0;
}

/***********************************************************
* Destructeur
*
***********************************************************/

CBlocCmd::~CBlocCmd()
{
}


/***************************************************************
* InitInstance:
*
***************************************************************/

WORD CBlocCmd::InitInstance()
{
    WNDCLASS wcl;    
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = g_hInst;
    wcl.lpszClassName = "BlocCmdClass";
    wcl.lpfnWndProc = WndProc;
    wcl.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    wcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
        
    return RegisterClass(&wcl);
}

/*******************************************************************
* Create : creation de fenetre de bloc de commande
*
*******************************************************************/

BOOL CBlocCmd::Create(HWND hwndParent)
{ 
    if(hwndBlocCmd)
        return FALSE;
    // creation de la fenetre
    
    RECT rc;
    int cyCaption, cxBorder, cyBorder;
    
    cyCaption = GetSystemMetrics(SM_CYCAPTION);
    cxBorder = GetSystemMetrics(SM_CXDLGFRAME);
    cyBorder = GetSystemMetrics(SM_CYDLGFRAME);
    
    GetWindowRect(hwndParent, &rc);
    
    hwndBlocCmd = CreateWindowEx(WS_EX_TOPMOST,"BlocCmdClass","Bloc de Commande",
            WS_POPUP|WS_CAPTION|WS_VISIBLE|WS_SYSMENU,
            rc.left + 300, rc.bottom - 250, 255+2*cxBorder, 96+cyCaption+2*cyBorder,
            hwndParent,NULL,g_hInst,this);
    
    if(hwndBlocCmd)
    {    
        UpdateWindow(hwndBlocCmd);
        return TRUE;
    }    
    
    return FALSE;
}


/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK CBlocCmd::WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    
    CBlocCmd *pHwnd = (CBlocCmd*)GetWindowLong(hwnd, GWL_USERDATA);
    if(!pHwnd)
        return DefWindowProc(hwnd,message,wParam,lParam);
    
    
    switch(message)
    {
        case WM_CREATE:
            return pHwnd->MsgCreate(hwnd);
        case WM_TIMER:
            return pHwnd->MsgTimer(hwnd);
        case WM_COMMAND:
            return pHwnd->MsgCommand(hwnd, wParam);
        case WM_LBUTTONUP:
            return pHwnd->MsgLButtonUp(hwnd, wParam, lParam);
        case WM_PAINT:
            return pHwnd->MsgPaint(hwnd);
        case WM_LBUTTONDOWN:
            return pHwnd->MsgLButtonDown(hwnd,wParam,lParam);
        case WM_LBUTTONDBLCLK:
            return pHwnd->MsgLButtonDblClk(hwnd,wParam,lParam);
        case WM_MOUSEMOVE:
            return pHwnd->MsgMouseMove(hwnd,wParam,lParam);        
        case WM_DESTROY:
            return pHwnd->MsgDestroy(hwnd);        
        default:
            return DefWindowProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgCreate(HWND hwnd)
{
    HWND hBuffer;
    
    m_hbmBlocCmd = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BLOCCMD));
    m_hbmBlocBtn = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BLOCBTN));
    m_bMove = FALSE;
    m_bDrawStop = FALSE;
    m_bDrawRewind = FALSE;
    m_bDrawNext = FALSE;
    m_bDrawBack = FALSE;
    m_bDrawPlay = g_midi.m_bSequenceGoing;
    m_bDrawRecord = g_midi.m_bRecording;
    m_bDrawLoop = g_midi.m_pSeq->IsLooped();
    m_bDrawClick = g_midi.m_pSeq->IsClicked();
    m_nCommand = -1;
    SetTimer(hwnd, ID_TIMERBLOC, 40, NULL);
    
    // création controle pour modifier la valeur de tempo, j'ai mis ES_MULTILINE et 
    // ES_AUTOVSCROLL comme style pour intercepter le retour chariot
    
    hBuffer = CreateWindow("Edit", "", WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL|ES_NUMBER,
        rcCtrl[0].left, rcCtrl[0].top, rcCtrl[0].right - rcCtrl[0].left, 
        rcCtrl[0].bottom - rcCtrl[0].top, hwnd, (HMENU)IDC_TEMPO, g_hInst, NULL);
    SendMessage(hBuffer, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 1);
    
    // création controle pour modifier la valeur de numérateur de la signature    
    hBuffer = CreateWindow("Edit", "", WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL|ES_NUMBER,
        rcCtrl[1].left, rcCtrl[1].top, rcCtrl[1].right - rcCtrl[1].left, 
        rcCtrl[1].bottom - rcCtrl[1].top, hwnd, (HMENU)IDC_NUM_SIGNATURE, g_hInst, NULL);
    SendMessage(hBuffer, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 1);
    
    // création controle pour modifier la valeur de denominateur de la signature    
    hBuffer = CreateWindow("Edit", "", WS_CHILD|ES_MULTILINE|ES_AUTOVSCROLL|ES_NUMBER,
        rcCtrl[2].left, rcCtrl[2].top, rcCtrl[2].right - rcCtrl[2].left, 
        rcCtrl[2].bottom - rcCtrl[2].top, hwnd, (HMENU)IDC_DEN_SIGNATURE, g_hInst, NULL);
    SendMessage(hBuffer, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 1);
    
    SetFocus(GetParent(hwnd));
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgTimer(HWND hwnd)
{
    RECT rc;
    BOOL bRedraw = FALSE;
    
    SetRect(&rc, 0,0,0,0);
    // afficher signature si changement
    
    if((g_midi.m_byCurrentSignature[0]!=m_oldSignature[0])||
    (g_midi.m_byCurrentSignature[1]!=m_oldSignature[1]))
    {
        wsprintf(m_szSignature,"%i/%i", (int)g_midi.m_byCurrentSignature[0], 
            (int)(0x01<<g_midi.m_byCurrentSignature[1]));
        CopyMemory(m_oldSignature, g_midi.m_byCurrentSignature, sizeof(m_oldSignature));
        UnionRect(&rc, &rc, &rcRef[ID_SIGNATURE]);
        bRedraw = TRUE;
    }    
    
    // afficher position si changement
    if(m_dwOldTime!=g_midi.m_dwCurrentTime)
    {
        g_midi.m_pSeq->TimeCodeToString(g_midi.m_dwCurrentTime, m_szPointer);
        m_dwOldTime = g_midi.m_dwCurrentTime;
        UnionRect(&rc, &rc, &rcRef[ID_POINTER]);
        bRedraw = TRUE;
    }    
    
    // afficher tempo si changement
    if(m_nOldTempo!=g_midi.m_nCurrentTempo)
    {
        wsprintf(m_szTempo, "%3i", g_midi.m_nCurrentTempo);
        m_nOldTempo = g_midi.m_nCurrentTempo;
        UnionRect(&rc, &rc, &rcRef[ID_TEMPO]);
        bRedraw = TRUE;
    }
    
    if(bRedraw)
        InvalidateRect(hwnd, &rc, TRUE);
    
    // met à jour le bouton PLAY si pas de lecture    
    if((m_bDrawPlay)&&(!g_midi.m_bSequenceGoing))
    {
        m_bDrawPlay = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
    }
    else if((!m_bDrawPlay)&&(g_midi.m_bSequenceGoing))
    {
        m_bDrawPlay = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
    }
    // met à jour le bouton RECORD si pas d'enregistrement    
    if((m_bDrawRecord)&&(!g_midi.m_bRecording))
    {
        m_bDrawRecord = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
    }
    else if((!m_bDrawRecord)&&(g_midi.m_bRecording))
    {
        m_bDrawRecord = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
    }
    // met à jour le bouton CLIC    
    if((m_bDrawClick)&&(!g_midi.m_pSeq->IsClicked()))
    {
        m_bDrawClick = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_CLICK], TRUE);
    }
    else if((!m_bDrawClick)&&(g_midi.m_pSeq->IsClicked()))
    {
        m_bDrawClick = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_CLICK], TRUE);
    }
    // met à jour les locators
    if(m_oldFirstLocator!=g_midi.m_pSeq->GetFirstLocator())
    {
        m_oldFirstLocator = g_midi.m_pSeq->GetFirstLocator();
        g_midi.m_pSeq->TimeCodeToString((DWORD)m_oldFirstLocator, m_szFirstLocator);
        SetRect(&rc, 10,2,90,20);
        InvalidateRect(hwnd, &rc, TRUE);
    }    
    if(m_oldLastLocator!=g_midi.m_pSeq->GetLastLocator())
    {
        m_oldLastLocator = g_midi.m_pSeq->GetLastLocator();
        g_midi.m_pSeq->TimeCodeToString((DWORD)m_oldLastLocator, m_szLastLocator);
        SetRect(&rc, 10,25,90,42);
        InvalidateRect(hwnd, &rc, TRUE);
    }
    return 0;
}    

/*************************************************************************************
*
*
*************************************************************************************/
    
LRESULT CBlocCmd::MsgPaint(HWND hwnd)
{
  PAINTSTRUCT ps;
  HDC hdc, hmem;
  char szText[64];
  
  hdc = BeginPaint(hwnd, &ps);
  hmem = CreateCompatibleDC(hdc);
  SelectObject(hmem, m_hbmBlocCmd);
  BitBlt(hdc, 0, 0, 255, 96, hmem, 0, 0, SRCCOPY);
  SelectObject(hmem, m_hbmBlocBtn);
  
  if(m_bDrawStop)
      BitBlt(hdc, 89, 56, 39, 32, hmem, 83, 1, SRCCOPY);
  if(m_bDrawPlay)
      BitBlt(hdc, 130, 56, 39, 32, hmem, 124, 1, SRCCOPY);
  if(m_bDrawRewind)
      BitBlt(hdc, 7, 56, 39, 32, hmem, 1, 1, SRCCOPY);
  if(m_bDrawLoop)
      BitBlt(hdc, 209, 3, 42, 16, hmem, 248, 0, SRCCOPY);
  if(m_bDrawClick)
      BitBlt(hdc, 209, 23, 42, 16, hmem, 248, 19, SRCCOPY);
  if(m_bDrawRecord)
      BitBlt(hdc, 212, 56, 39, 32, hmem, 206, 1, SRCCOPY);
  if(m_bDrawNext)
      BitBlt(hdc, 171, 56, 39, 32, hmem, 165, 1, SRCCOPY);
  if(m_bDrawBack)
      BitBlt(hdc, 48, 56, 39, 32, hmem, 42, 1, SRCCOPY);
  
  DeleteDC(hmem);
  SetBkMode(hdc, TRANSPARENT);
  SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
  SetTextColor(hdc, RGB(111,223,0));
  
  // affichage locateurs
  TextOut(hdc, 10, 2, m_szFirstLocator, lstrlen(m_szFirstLocator));
  TextOut(hdc, 10, 25, m_szLastLocator, lstrlen(m_szLastLocator));
  
  // affichage tempo
  TextOut(hdc, rcRef[ID_TEMPO].left, rcRef[ID_TEMPO].top, m_szTempo, lstrlen(m_szTempo));
  
  // affichage signature
  TextOut(hdc, rcRef[ID_SIGNATURE].left, rcRef[ID_SIGNATURE].top, m_szSignature, 
      lstrlen(m_szSignature));
  
  // affichage pointeur
  SetTextColor(hdc, RGB(220,223,0));
  SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
  TextOut(hdc, 100, 4, m_szPointer, lstrlen(m_szPointer));
  
  SetTextColor(hdc, 0);
  
  EndPaint(hwnd, &ps);
  return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    m_ptCur.x = LOWORD(lParam);
    m_ptCur.y = HIWORD(lParam);
    
    m_nCommand = -1;
    SetCapture(hwnd);
    for(int i=0; i<NB_COMMAND; i++)
    {
        if(PtInRect(&rcRef[i], m_ptCur))
        {
            m_nCommand = i;
            break;
        }    
    }
    if(m_nCommand==-1)
        return 0;
    switch(m_nCommand)
    {
        case ID_STOP:
            m_bDrawStop = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_STOP], TRUE);
            break;
        case ID_PLAY:
            m_bDrawPlay = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
            break;
        case ID_REWIND:
            m_bDrawRewind = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_REWIND], TRUE);
            break;
        case ID_NEXT:
            m_bDrawNext = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_NEXT], TRUE);
            break;
        case ID_BACK:
            m_bDrawBack = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_BACK], TRUE);
            break;
        case ID_LOOP:
            m_bDrawLoop = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_LOOP], TRUE);
            break;
        case ID_CLICK: // metronome
            m_bDrawClick = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_CLICK], TRUE);
            break;
        case ID_RECORD:
            m_bDrawRecord = TRUE;
            InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
            break;
    }
    //SetFocus(GetParent(hwnd));
    return 0;    
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    DWORD dwBuf;
    
    if((PtInRect(&rcRef[ID_PLAY], pt))&&(m_nCommand==ID_PLAY))
    {
        g_midi.Begin(FALSE);
        m_bDrawPlay = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
    }
    else if((PtInRect(&rcRef[ID_RECORD], pt))&&(m_nCommand==ID_RECORD))
    {
        g_midi.Begin(TRUE);
        m_bDrawRecord = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
    }
    else if((PtInRect(&rcRef[ID_STOP], pt))&&(m_nCommand==ID_STOP))
    {
        g_midi.End();
        if(m_bDrawPlay)
        {
            m_bDrawPlay = FALSE;
            InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
        }
        if(m_bDrawRecord)
        {
            m_bDrawRecord = FALSE;
            InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
        }    
    }
    else if((PtInRect(&rcRef[ID_REWIND], pt))&&(m_nCommand==ID_REWIND))
    {
        g_midi.End();
        if(m_bDrawPlay)
        {
            m_bDrawPlay = FALSE;
            InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
        }
        // remet à zéro le timeCode
        g_midi.SetCurrentValue(0);
        // affiche la première page de fenetre arrangement
        g_midi.m_pSeq->SetXOrigine(0);
        if(hwndArrange)
            SendMessage(hwndArrange, WM_USER, 0, 0);
    }
    else if((PtInRect(&rcRef[ID_NEXT], pt))&&(m_nCommand==ID_NEXT))
    {
        if(!g_midi.m_bSequenceGoing)
        {
            dwBuf = g_midi.m_pSeq->MeasureNextToTimeCode(g_midi.m_dwCurrentTime);
            g_midi.SetCurrentValue(dwBuf);
        }
    }
    else if((PtInRect(&rcRef[ID_BACK], pt))&&(m_nCommand==ID_BACK))
    {
        if(!g_midi.m_bSequenceGoing)
        {
            dwBuf = g_midi.m_pSeq->MeasureBackToTimeCode(g_midi.m_dwCurrentTime);
            g_midi.SetCurrentValue(dwBuf);
        }
    }   
    else if((PtInRect(&rcRef[ID_LOOP], pt))&&(m_nCommand==ID_LOOP))
    {
        m_bDrawLoop = !g_midi.m_pSeq->IsLooped();
        g_midi.m_pSeq->SetLooped(m_bDrawLoop);
        InvalidateRect(hwnd, &rcRef[ID_LOOP], TRUE);
    }
    else if((PtInRect(&rcRef[ID_CLICK], pt))&&(m_nCommand==ID_CLICK))
    {
        m_bDrawClick = !g_midi.m_pSeq->IsClicked();
        g_midi.m_pSeq->SetClicked(m_bDrawClick);
        InvalidateRect(hwnd, &rcRef[ID_CLICK], TRUE);
    }
    m_nCommand = -1;
    
    if(m_bDrawStop)
    {
        m_bDrawStop = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_STOP], TRUE);
    }
    if(m_bDrawRewind)
    {
        m_bDrawRewind = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_REWIND], TRUE);
    }
    if(m_bDrawNext)
    {
        m_bDrawNext = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_NEXT], TRUE);
    }
    if(m_bDrawBack)
    {
        m_bDrawBack = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_BACK], TRUE);
    }  
    ReleaseCapture();
    SetFocus(GetParent(hwnd));
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgLButtonDblClk(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    char szText[128];
    m_ptCur.x = LOWORD(lParam);
    m_ptCur.y = HIWORD(lParam);
    
    m_nDblClk = -1;
    
    // éviter de modifier les parametre pendant la lecture
    if(g_midi.m_bSequenceGoing)
        return 0;
    
    for(int i=0; i<NB_CTRL; i++)
    {
        if(PtInRect(&rcCtrl[i], m_ptCur))
        {
            m_nDblClk = i;
            break;
        }    
    }
    if(m_nDblClk==-1)
        return 0;
    switch(m_nDblClk+20)
    {
        case IDC_TEMPO:
            ShowWindow(GetDlgItem(hwnd, m_nDblClk+20), SW_NORMAL);
            SetFocus(GetDlgItem(hwnd, m_nDblClk+20));
            wsprintf(szText, "%i", g_midi.m_nCurrentTempo);
            SetDlgItemText(hwnd, IDC_TEMPO, szText);
            SendDlgItemMessage(hwnd, IDC_TEMPO, EM_SETSEL, 0, lstrlen(szText));
            break;
        case IDC_NUM_SIGNATURE:
            ShowWindow(GetDlgItem(hwnd, m_nDblClk+20), SW_NORMAL);
            SetFocus(GetDlgItem(hwnd, m_nDblClk+20));
            wsprintf(szText, "%i", g_midi.m_byCurrentSignature[0]);
            SetDlgItemText(hwnd, IDC_NUM_SIGNATURE, szText);
            SendDlgItemMessage(hwnd, IDC_NUM_SIGNATURE, EM_SETSEL, 0, lstrlen(szText));
            break;
        case IDC_DEN_SIGNATURE:
            ShowWindow(GetDlgItem(hwnd, m_nDblClk+20), SW_NORMAL);
            SetFocus(GetDlgItem(hwnd, m_nDblClk+20));
            wsprintf(szText, "%i", 1<<g_midi.m_byCurrentSignature[1]);
            SetDlgItemText(hwnd, IDC_DEN_SIGNATURE, szText);
            SendDlgItemMessage(hwnd, IDC_DEN_SIGNATURE, EM_SETSEL, 0, lstrlen(szText));
            break;
    }
    return 0;    
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    
    // on verifie si le curseur est dans le rectangle STOP
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_STOP)&&
    (PtInRect(&rcRef[ID_STOP], pt))&&(!m_bDrawStop))
    {
        m_bDrawStop = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_STOP], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_STOP)&&
    (!PtInRect(&rcRef[ID_STOP], pt))&&(m_bDrawStop))
    {
        m_bDrawStop = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_STOP], TRUE);
    }
    
    // on verifie si le curseur est dans le rectangle REWIND
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_REWIND)&&
    (PtInRect(&rcRef[ID_REWIND], pt))&&(!m_bDrawRewind))
    {
        m_bDrawRewind = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_REWIND], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_REWIND)&&
    (!PtInRect(&rcRef[ID_REWIND], pt))&&(m_bDrawRewind))
    {
        m_bDrawRewind = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_REWIND], TRUE);
    }
    
    // on verifie si le curseur est dans le rectangle NEXT
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_NEXT)&&
    (PtInRect(&rcRef[ID_NEXT], pt))&&(!m_bDrawNext))
    {
        m_bDrawNext = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_NEXT], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_NEXT)&&
    (!PtInRect(&rcRef[ID_NEXT], pt))&&(m_bDrawNext))
    {
        m_bDrawNext = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_NEXT], TRUE);
    }
    
    // on verifie si le curseur est dans le rectangle BACK
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_BACK)&&
    (PtInRect(&rcRef[ID_BACK], pt))&&(!m_bDrawBack))
    {
        m_bDrawBack = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_BACK], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_BACK)&&
    (!PtInRect(&rcRef[ID_BACK], pt))&&(m_bDrawBack))
    {
        m_bDrawBack = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_BACK], TRUE);
    }    
    
    // on verifie si le curseur est dans le rectangle PLAY
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_PLAY)&&
    (PtInRect(&rcRef[ID_PLAY], pt))&&(!m_bDrawPlay))
    {
        m_bDrawPlay = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_PLAY)&&
    (!PtInRect(&rcRef[ID_PLAY], pt))&&(m_bDrawPlay))
    {
        m_bDrawPlay = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_PLAY], TRUE);
    }
    
    // on verifie si le curseur est dans le rectangle RECORD
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_RECORD)&&
    (PtInRect(&rcRef[ID_RECORD], pt))&&(!m_bDrawRecord))
    {
        m_bDrawRecord = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_RECORD)&&
    (!PtInRect(&rcRef[ID_RECORD], pt))&&(m_bDrawRecord))
    {
        m_bDrawRecord = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_RECORD], TRUE);
    }
    
    // on verifie si le curseur est dans le rectangle BOUCLE
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_LOOP)&&
    (PtInRect(&rcRef[ID_LOOP], pt))&&(!m_bDrawLoop))
    {
        m_bDrawLoop = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_LOOP], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_LOOP)&&
    (!PtInRect(&rcRef[ID_LOOP], pt))&&(m_bDrawLoop)&&(!g_midi.m_pSeq->IsLooped()))
    {
        m_bDrawLoop = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_LOOP], TRUE);
    }
    
    // on verifie si le curseur est dans le rectangle CLIC
    
    if((wParam & MK_LBUTTON)&&(m_nCommand==ID_CLICK)&&
    (PtInRect(&rcRef[ID_CLICK], pt))&&(!m_bDrawClick))
    {
        m_bDrawClick = TRUE;
        InvalidateRect(hwnd, &rcRef[ID_CLICK], TRUE);
    }
    else if((wParam & MK_LBUTTON)&&(m_nCommand==ID_CLICK)&&
    (!PtInRect(&rcRef[ID_CLICK], pt))&&(m_bDrawClick)&&(!g_midi.m_pSeq->IsClicked()))
    {
        m_bDrawClick = FALSE;
        InvalidateRect(hwnd, &rcRef[ID_CLICK], TRUE);
    }
    return 0;
}    

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgCommand(HWND hwnd, WPARAM wParam)
{
    char szText[256];
    switch(LOWORD(wParam))
    {
        case IDC_TEMPO:
            return CmdTempo(hwnd, wParam);
        case IDC_NUM_SIGNATURE:
            return CmdNumSignature(hwnd, wParam);
        case IDC_DEN_SIGNATURE:
            return CmdDenSignature(hwnd, wParam);
    }
    return 0;
}    

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::MsgDestroy(HWND hwnd)
{
    KillTimer(hwnd, ID_TIMERBLOC);
    hwndBlocCmd = NULL;
    DeleteObject(m_hbmBlocCmd);
    DeleteObject(m_hbmBlocBtn);
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::CmdTempo(HWND hwnd, WPARAM wParam)
{
    char szText[128];
    int tempo;
    
    if(HIWORD(wParam)==EN_CHANGE)
    {
        if(InterceptReturn(hwnd, IDC_TEMPO, szText))
        {
            tempo = min(255, max(30, atoi(szText)));
            ShowWindow(GetDlgItem(hwnd, IDC_TEMPO), SW_HIDE);
            g_midi.m_pSeq->SetTempo(g_midi.m_dwCurrentTime, tempo);
            g_midi.m_nCurrentTempo = tempo;
            SetFocus(GetParent(hwnd));
        }    
    }
    else if(HIWORD(wParam)==EN_KILLFOCUS)
        ShowWindow(GetDlgItem(hwnd, IDC_TEMPO), SW_HIDE);
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::CmdNumSignature(HWND hwnd, WPARAM wParam)
{
    char szText[128];
    BYTE bySgt[4];
    int num;
    
    if(HIWORD(wParam)==EN_CHANGE)
    {
        if(InterceptReturn(hwnd, IDC_NUM_SIGNATURE, szText))
        {
            num = min(16, max(1, atoi(szText)));
            ShowWindow(GetDlgItem(hwnd, IDC_NUM_SIGNATURE), SW_HIDE);
            g_midi.m_pSeq->GetTimeSignature(g_midi.m_dwCurrentTime, bySgt);
            bySgt[0] = (BYTE)num;
            g_midi.m_pSeq->SetTimeSignature(g_midi.m_dwCurrentTime, bySgt);
            g_midi.m_byCurrentSignature[0] = (BYTE)num;
            SetFocus(GetParent(hwnd));
            // met à jour la fenetre arrangement si elle est ouverte
            if(hwndArrange)
                SendMessage(hwndArrange, WM_USER, 0,0);
            
            // met à jour les fenetres editeurs si elles sont ouvertes
            TPattern * p = g_base.pFirstPattern;
            while(p)
            {
                if(p->id!=0)
                    SendMessage(GetDlgItem(g_hMdi, p->id), WM_USER, 0,0);
                p = p->pRight;
            }
        }    
    }
    else if(HIWORD(wParam)==EN_KILLFOCUS)
        ShowWindow(GetDlgItem(hwnd, IDC_NUM_SIGNATURE), SW_HIDE);
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

LRESULT CBlocCmd::CmdDenSignature(HWND hwnd, WPARAM wParam)
{
    char szText[128];
    BYTE bySgt[4];
    int den;
    
    if(HIWORD(wParam)==EN_CHANGE)
    {
        if(InterceptReturn(hwnd, IDC_DEN_SIGNATURE, szText))
        {
            den = atoi(szText);
            if(den<=2)
                den = 1;
            else if(den<=4)
                den = 2;
            else if(den<=8)
                den = 3;
            else
                den = 4;
            ShowWindow(GetDlgItem(hwnd, IDC_DEN_SIGNATURE), SW_HIDE);
            g_midi.m_pSeq->GetTimeSignature(g_midi.m_dwCurrentTime, bySgt);
            bySgt[1] = (BYTE)den;
            g_midi.m_pSeq->SetTimeSignature(g_midi.m_dwCurrentTime, bySgt);
            g_midi.m_byCurrentSignature[1] = (BYTE)den;
            SetFocus(GetParent(hwnd));
            // met à jour la fenetre arrangement si elle est ouverte
            if(hwndArrange)
                SendMessage(hwndArrange, WM_USER, 0,0);
            
            // met à jour les fenetres editeurs si elles sont ouvertes
            TPattern * p = g_base.pFirstPattern;
            while(p)
            {
                if(p->id!=0)
                    SendMessage(GetDlgItem(g_hMdi, p->id), WM_USER, 0,0);
                p = p->pRight;
            }
        }    
    }
    else if(HIWORD(wParam)==EN_KILLFOCUS)
        ShowWindow(GetDlgItem(hwnd, IDC_DEN_SIGNATURE), SW_HIDE);
    return 0;
}

/*************************************************************************************
*
*
*************************************************************************************/

BOOL CBlocCmd::InterceptReturn(HWND hwnd, int id, char * lpszText)
{
    int i, j, len;
    BOOL bSuccess = FALSE;
    if(!lpszText)
        return FALSE;
    
    GetDlgItemText(hwnd, id, lpszText, 128);
    len = lstrlen(lpszText);
    for(i=0; i<len; i++)
    {
        if(lpszText[i]=='\r')
        {
            bSuccess = TRUE;
            break;
        }    
    }
    if(bSuccess)
    {
        j = i;
        for(i+=2;i<len;i++, j++)
        {
            lpszText[j] = lpszText[i];
        }
        lpszText[len-2] = '\0';
        return TRUE;
    }        
    
    return FALSE;
}    
