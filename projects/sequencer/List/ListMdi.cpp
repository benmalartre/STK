#define  _WIN32_IE 0x401
#include <windows.h>
#include <commctrl.h>
#include "ListMdi.h"
#include "../Sequencer.h"
#ifndef _STANDARD_HEADER_H_
#include "../Standard.h"
#endif
#define ID_LIST    100

struct TInfoEvent
{
    CObjectEvent * pRef;
    DWORD dwPos;
    DWORD dwLength;
    BYTE  byStatus;
    BYTE  byChannel;
    BYTE  byData1;
    BYTE  byData2;
};    

struct TListMdiData
{
    TPattern  * pPtrn;   // sequence associé à la fenetre editeur
    int         xo;
    int         yo;
    BOOL        bClose;    
};

const char cstStrNote12[12][4] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

extern CSequencer g_cSeq;
extern HINSTANCE g_hInst;
extern char g_szWndTitle[];

void         ListMdi_Data1ToString(BYTE, BYTE, char*);
void         ListMdi_Data2ToString(BYTE, BYTE, char*);
void         ListMdi_StatusToString(BYTE, char*);
void         ListMdi_LengthToString(BYTE, DWORD, char*);
void         ListMdi_Information(TInfoEvent*, char*);
void         ListMdi_CmdFillList(HWND);
int CALLBACK ListMdi_Compare(LPARAM, LPARAM, LPARAM);

TInfoEvent * MakeInfoEvent(CObjectEvent*, int);
TListMdiData * MakeListMdiData(TPattern*);

LRESULT ListMdi_MsgCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgHScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgVScroll(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgNotify(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgMdiActivate(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgSize(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgClose(HWND, UINT, WPARAM, LPARAM);
LRESULT ListMdi_MsgDestroy(HWND, UINT, WPARAM, LPARAM);

/***************************************************************
* InitInstance:
*
***************************************************************/

WORD ListMdi_InitInstance()
{
    WNDCLASS wcl;    
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = g_hInst;
    wcl.lpszClassName = "MdiListClass";
    wcl.lpfnWndProc = ListMdi_WndProc;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcl.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        
    return RegisterClass(&wcl);
}

/***************************************************************
* Create: création de fenêtre éditeur
*
***************************************************************/

HWND ListMdi_Create(HWND hwndParent)
{
    char szText[140];
    TListMdiData * pData;
    TPattern * p = g_cSeq.GetPatternSelected();
    
    if(!p)
    {
        MessageBox(hwndParent, " Séléctionner une séquence avant d'ouvrir la   \n"
        " fenêtre liste d'évenement . ", g_szWndTitle, MB_ICONINFORMATION);
        return NULL;
    }    
    
    if(p->idList!=0)
    {
        SendMessage(hwndParent, WM_MDIACTIVATE, (WPARAM)GetDlgItem(hwndParent, p->idList),0L);
        return GetDlgItem(hwndParent, p->idList);
    }    
    pData = MakeListMdiData(p);
    if(!pData)
        return NULL;
    
    wsprintf(szText, "Liste d'évenement: %s", p->szTitle);
    return CreateMDIWindow("MdiListClass",szText,
        MDIS_ALLCHILDSTYLES|(g_cSeq.IsMaximized() ? WS_MAXIMIZE : 0),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, g_hInst,(LPARAM)pData);
}    


/*************************************************************
* WndProc : traitement des messages
*
*************************************************************/

LRESULT CALLBACK ListMdi_WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{        
    switch(message)
    {
        case WM_CREATE:
            return ListMdi_MsgCreate(hwnd,message,wParam,lParam);
        /*case WM_HSCROLL:
            return Editor_MsgHScroll(hwnd,message,wParam,lParam);
        case WM_VSCROLL:
            return Editor_MsgVScroll(hwnd,message,wParam,lParam);
        case WM_COMMAND:
            return Editor_MsgCommand(hwnd,message,wParam,lParam);*/
        case WM_SIZE:
            return ListMdi_MsgSize(hwnd,message,wParam,lParam);
        case WM_NOTIFY:
            return ListMdi_MsgNotify(hwnd,message,wParam,lParam);
        //case WM_MDIACTIVATE:
            //return ListMdi_MsgMdiActivate(hwnd,message,wParam,lParam);
        case WM_CLOSE:
            return ListMdi_MsgClose(hwnd,message,wParam,lParam);
        case WM_DESTROY:
            return ListMdi_MsgDestroy(hwnd,message,wParam,lParam);
        default:
            return DefMDIChildProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/*****************************************************************
* MsgCreate : traitement de message WM_CREATE
*
*****************************************************************/

LRESULT ListMdi_MsgCreate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    HWND hBuffer;
    LPCREATESTRUCT lpcs;
    LPMDICREATESTRUCT lpmcs;
    lpcs = (LPCREATESTRUCT)lParam;
    lpmcs = (LPMDICREATESTRUCT)(lpcs->lpCreateParams);
    SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpmcs->lParam);
    
    TListMdiData * p = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    p->pPtrn->idList = (int)GetWindowLong(hwnd, GWL_ID);
    
    // Ajout de controle ListView pour stocker les evenements
    
    hBuffer = CreateWindowEx (WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, 
        WS_CHILD|WS_VISIBLE|LVS_REPORT,//|LVS_SINGLESEL|LVS_NOLABELWRAP,
        0, 0, 0, 0, hwnd, (HMENU)ID_LIST, g_hInst, NULL);    
    
    ListView_SetExtendedListViewStyleEx(hBuffer, 0, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
    
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
    lvc.fmt = LVCFMT_RIGHT;
    lvc.cx = 80;
    lvc.pszText = "Position";
    ListView_InsertColumn(hBuffer, 0, &lvc);
    lvc.pszText = "Durée";
    ListView_InsertColumn(hBuffer, 1, &lvc);
    
    lvc.cx = 100;
    lvc.fmt = LVCFMT_CENTER;
    lvc.pszText = "Status";
    ListView_InsertColumn(hBuffer, 2, &lvc);
    
    lvc.cx = 40;
    lvc.pszText = "Canal";
    ListView_InsertColumn(hBuffer, 3, &lvc);
    
    lvc.fmt = LVCFMT_RIGHT;
    lvc.pszText = "Num";
    ListView_InsertColumn(hBuffer, 4, &lvc);
    lvc.pszText = "Val";
    ListView_InsertColumn(hBuffer, 5, &lvc);
    
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 300;    
    lvc.pszText = "Information";
    ListView_InsertColumn(hBuffer, 6, &lvc);
    
    
    ListMdi_CmdFillList(hwnd);
    
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/*****************************************************************
* MsgCommand : traitement de message WM_COMMAND
*
*****************************************************************/

LRESULT ListMdi_MsgCommand(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TListMdiData * p = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    return 0;
}    

/******************************************************************
* 
*
******************************************************************/

LRESULT ListMdi_MsgNotify(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    char text[256];    
    LVITEM lvi;
    NMLVDISPINFO* pDispInfo;
    NMITEMACTIVATE * pnItem;
    NMLVKEYDOWN * pnkd;
    TInfoEvent * p;
        
    TListMdiData * pMdi = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    switch(wParam)
    {
        case ID_LIST:
            if(((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
            {
                pDispInfo = (NMLVDISPINFO*)lParam;
                if(pDispInfo->item.mask & LVIF_TEXT)
                {
                    p = (TInfoEvent*)(pDispInfo->item.lParam);
                    
                    switch(pDispInfo->item.iSubItem)
                    {
                        case 0:
                            g_cSeq.TimeCodeToString(p->dwPos, pDispInfo->item.pszText);
                            break;
                        case 1:
                            ListMdi_LengthToString(p->byStatus, p->dwLength,
                                pDispInfo->item.pszText);
                            break;
                        case 2:
                            ListMdi_StatusToString(p->byStatus, pDispInfo->item.pszText);
                            break;
                        case 3:
                            itoa((int)p->byChannel, pDispInfo->item.pszText, 10);
                            break;
                        case 4:
                            ListMdi_Data1ToString(p->byStatus,p->byData1,pDispInfo->item.pszText);
                            break;
                        case 5:
                            ListMdi_Data2ToString(p->byStatus,p->byData2,pDispInfo->item.pszText);
                            break;
                        case 6:
                            ListMdi_Information(p, pDispInfo->item.pszText);
                            break;
                    }
                }
            }
            break;
    }
    return 0; 
}



/******************************************************************
* 
*
******************************************************************/

LRESULT ListMdi_MsgMdiActivate(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    /*if(hwnd ==(HWND)lParam)
        SetFocus(GetDlgItem(hwnd, 151));*/
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT ListMdi_MsgSize(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int cx, cy;
    TListMdiData * p = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    if(wParam == SIZE_MAXIMIZED)
        g_cSeq.SetMaximized(TRUE);
    else if((wParam == SIZE_RESTORED)&&(!p->bClose))
        g_cSeq.SetMaximized(FALSE);
    
    cx = LOWORD(lParam);
    cy = HIWORD(lParam);
    //ListMdi_UpdateScrollBars(hwnd);
    MoveWindow(GetDlgItem(hwnd, ID_LIST), 0, 0, cx, cy, TRUE);
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/************************************************************************
* 
*
************************************************************************/

LRESULT ListMdi_MsgHScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    /*TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, &info);
    
    int x;
    x = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINERIGHT: x += 40; break;
        case SB_LINELEFT: x -= 40; break;
        case SB_PAGERIGHT: x += info.nPage; break;
        case SB_PAGELEFT: x -= info.nPage; break;
        case SB_THUMBTRACK: x = info.nTrackPos; break;
        case SB_THUMBPOSITION: x = info.nPos; break;
    }
    if(x>info.nMax)
        x = info.nMax;
    if(x<0)
        x = 0;
    SetScrollPos(GetDlgItem(hwnd, IDC_HSCROLL), SB_CTL, x ,TRUE);
    p->xo = - x - (p->cxTick*p->pPtrn->left)/(int)g_cSeq.GetTimeTick();
    InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, 152), NULL, TRUE);*/
    return 0;
}

/************************************************************************
* 
*
************************************************************************/

LRESULT ListMdi_MsgVScroll(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    SCROLLINFO info;
    /*TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, &info);
    
    int y;
    y = info.nPos;
    switch(LOWORD(wParam))
    {
        case SB_LINEDOWN: y += 20; break;
        case SB_LINEUP: y -= 20; break;
        case SB_PAGEDOWN: y += info.nPage; break;
        case SB_PAGEUP: y -= info.nPage; break;
        case SB_THUMBTRACK: y = info.nTrackPos; break;
        case SB_THUMBPOSITION: y = info.nPos; break;
    }
    if(y>info.nMax)
        y = info.nMax;
    if(y<0)
        y = 0;
    SetScrollPos(GetDlgItem(hwnd, IDC_VSCROLL), SB_CTL, y ,TRUE);
    p->yo = -y;
    InvalidateRect(GetDlgItem(hwnd, 151), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, 150), NULL, TRUE);*/
    return 0;
}

/******************************************************************
* 
*
******************************************************************/

LRESULT ListMdi_MsgClose(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TListMdiData * p = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    if(p)
        p->bClose = TRUE;
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/******************************************************************
* 
*
******************************************************************/

LRESULT ListMdi_MsgDestroy(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    TListMdiData * p = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    // liberer les memoires allouées à la listView
    LVITEM lvi;
    int count = ListView_GetItemCount(GetDlgItem(hwnd, ID_LIST));
    for(lvi.iItem=0; lvi.iItem<count; lvi.iItem++)
    {
        lvi.mask = LVIF_PARAM;
        ListView_GetItem(GetDlgItem(hwnd, ID_LIST), &lvi);
        if(lvi.lParam)
            free((LPVOID)lvi.lParam);
    }
    
    if(p)
    {
        g_cSeq.UpdateIDPattern(p->pPtrn->idList);
        p->pPtrn->idList = 0;
        delete p;
    }
    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/****************************************************************************
* MakeListMdiData : création d'une donnée associé à chaque controle créé
*
****************************************************************************/

TListMdiData * MakeListMdiData(TPattern * pSel)
{
    TListMdiData * p;
    
    p = new TListMdiData;
    if(!p)
        return NULL;
    
    ZeroMemory(p, sizeof(TListMdiData));
    p->pPtrn = pSel;
    
    return p;
}

/******************************************************************
* 
*
******************************************************************/

TInfoEvent * MakeInfoEvent(CObjectEvent * pObj, int numTrack)
{
    if(!pObj)
        return NULL;
    
    TInfoEvent * p = (TInfoEvent*)malloc(sizeof(TInfoEvent));
    if(!p)
        return NULL;
    ZeroMemory(p, sizeof(TInfoEvent));
    p->dwPos = (DWORD)pObj->left;
    p->byStatus = (BYTE)pObj->GetType();
    p->byChannel = (BYTE)g_cSeq.GetChannelTrack(numTrack);
    p->pRef = pObj;
    switch(p->byStatus)
    {
        case MIDI_NOTEON:
            p->dwLength = (DWORD)(((CObjectEventNote*)pObj)->right - pObj->left);
            p->byData1 = (BYTE)((CObjectEventNote*)pObj)->note;
            p->byData2 = (BYTE)((CObjectEventNote*)pObj)->velocity;
            break;
        case MIDI_KEYAFTERTOUCH:
            p->byData1 = (BYTE)((CObjectEventKeyAfterTouch*)pObj)->note;
            p->byData2 = (BYTE)((CObjectEventKeyAfterTouch*)pObj)->velocity;
            break;
        case MIDI_CTRLCHANGE:
            p->byData1 = (BYTE)((CObjectEventControlChange*)pObj)->numController;
            p->byData2 = (BYTE)((CObjectEventControlChange*)pObj)->value;
            break;
        case MIDI_PRGMCHANGE:
            p->byData1 = (BYTE)((CObjectEventProgramChange*)pObj)->numProgram;
            break;
        case MIDI_CHANPRESS:
            p->byData1 = (BYTE)((CObjectEventChannelPressure*)pObj)->value;
            break;
        case MIDI_PITCHBEND:
            p->byData1 = (BYTE)((CObjectEventPitchBend*)pObj)->value1;
            p->byData2 = (BYTE)((CObjectEventPitchBend*)pObj)->value2;
            break;
    }
    return p;    
}

/******************************************************************
* 
*
******************************************************************/

void    ListMdi_CmdFillList(HWND hwnd)
{
    TListMdiData * pData = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    TInfoEvent * pInfo;
    LVITEM lvi;
    BOOL bSort = FALSE;
    CObjectEvent * p = pData->pPtrn->pFirstEvent;
    
    while(p)
    {
        pInfo = MakeInfoEvent(p, pData->pPtrn->numTrack);
        // ajouter l'evenement dans la ListView
        if(pInfo)
        {
            if(!bSort) 
                bSort = TRUE;
            lvi.mask = LVIF_TEXT|LVIF_PARAM;
            lvi.iItem = ListView_GetItemCount(GetDlgItem(hwnd, ID_LIST));
            lvi.iSubItem = 0;
            lvi.lParam = (LPARAM)pInfo;
            lvi.pszText = LPSTR_TEXTCALLBACK;
            ListView_InsertItem(GetDlgItem(hwnd, ID_LIST), &lvi);
        }
        p = p->pRight;
    }
    if(bSort)
        ListView_SortItems(GetDlgItem(hwnd, ID_LIST), ListMdi_Compare, 0);    
}    

int CALLBACK ListMdi_Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    int i;
    int result = 0;
    
    TInfoEvent *p1, *p2;
    p1 = (TInfoEvent*)lParam1;
    p2 = (TInfoEvent*)lParam2;
    
    if(p1->dwPos > p2->dwPos)
        result = 1;
    else if(p1->dwPos < p2->dwPos)
        result = -1;
    return result;
}

void  ListMdi_StatusToString(BYTE status, char * pText)
{
    if(!pText)
        return;
    
    switch(status)
    {
        case MIDI_NOTEON:
            lstrcpy(pText, "Note");
            return;
        case MIDI_KEYAFTERTOUCH:
            lstrcpy(pText, "Key Pressure");
            return;
        case MIDI_CTRLCHANGE:
            lstrcpy(pText, "Controller");
            return;
        case MIDI_PRGMCHANGE:
            lstrcpy(pText, "Program");
            return;
        case MIDI_CHANPRESS:
            lstrcpy(pText, "Channel Pressure");
            return;
        case MIDI_PITCHBEND:
            lstrcpy(pText, "Pitch Bend");
            return;
        default:
            lstrcpy(pText, "-----");
    }    
}    

void  ListMdi_Data1ToString(BYTE status, BYTE data, char * pText)
{
    if(!pText)
        return;
    
    if(status==MIDI_NOTEON)
    {
        wsprintf(pText, "%s%i", cstStrNote12[data%12], (int)(data/12) - 2);
        return;
    }
    itoa((int)data, pText, 10);   
}    

void  ListMdi_Data2ToString(BYTE status, BYTE data, char * pText)
{
    if(!pText)
        return;
    if((status==MIDI_PRGMCHANGE)||(status==MIDI_CHANPRESS))
    {
        lstrcpy(pText, "--");
        return;
    }
    itoa((int)data, pText, 10);    
}    

void  ListMdi_LengthToString(BYTE status, DWORD length, char * pText)
{
    if(!pText)
        return;
    int k;
    
    if(status==MIDI_NOTEON)
    {
        k = (int)(length/g_cSeq.GetTimeTick());        
        wsprintf(pText, "%02i.%03i", k, ((int)length)%(int)g_cSeq.GetTimeTick());
        return;
    }
    lstrcpy(pText, "--");   
}

void  ListMdi_Information(TInfoEvent * pData, char * pText)
{
    if(!pText)
        return;
    if(!pData)
    {
        pText[0] = 0;
        return;
    }    
    
    if(pData->byStatus==MIDI_PRGMCHANGE)
    {
        lstrcpy(pText, szInstrumentName[pData->byData1]);
        return;
    }
    else if(pData->byStatus==MIDI_CTRLCHANGE)
    {
        lstrcpy(pText, szControllerName[pData->byData1]);
        return;
    }
    pText[0] = 0;    
}

/******************************************************************
* 
*
******************************************************************/

void ListMdi_UpdateScrollBars(HWND hwnd)
{
    RECT rc;
    int cx, cy;
    int cxModel = 680;
    int cyModel = 50;
    TListMdiData * p = (TListMdiData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    GetClientRect(hwnd, &rc);
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
    
    SCROLLINFO info;
    
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_HORZ, &info);
    if(cx>=cxModel)
    {
        cx = cxModel;        
        SetScrollPos(hwnd, SB_HORZ, 0 ,TRUE);
        p->xo = 0;
        InvalidateRect(hwnd, NULL, TRUE);
    }
    else
    {
        SetScrollPos(hwnd, SB_HORZ, - p->xo ,TRUE);
    }
    info.nMin = 0;
    info.nMax = cxModel-1;
    info.nPage = cx;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
    
    info.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_VERT, &info);
    if(cy>=cyModel)
    {
        cy = cyModel;        
        SetScrollPos(hwnd, SB_VERT, 0 ,TRUE);
        p->yo = 0;
        InvalidateRect(hwnd, NULL, TRUE);
    }
    else
    {
        SetScrollPos(hwnd, SB_VERT, - p->yo ,TRUE);
        UpdateWindow(hwnd);
    }    
    
    info.nMin = 0;
    info.nMax = cyModel-1;
    info.nPage = cy;
    info.fMask = SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
}
