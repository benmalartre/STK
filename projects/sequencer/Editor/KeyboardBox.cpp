#include <windows.h>
#include "KeyboardBox.h"
#include "../Sequencer.h"
#include "../MidiSphinx.h"

extern CMidiSphinx g_midi;
extern HBITMAP g_hClavier;

/**************************************************************************
* Déclarations des fonctions
*
**************************************************************************/

BYTE    KeyboardBox_GetNotePlaying(int x, int y, TEditorData * p);
void    KeyboardBox_Repaint(HWND hwnd);
LRESULT CALLBACK KeyboardBox_Procedure(HWND, UINT, WPARAM, LPARAM);


/**************************************************************************
* KeyboardBox_InitControl : initialisation du registre de controle
*
**************************************************************************/

WORD WINAPI KeyboardBox_InitControl(HINSTANCE hInstance)
{
    WNDCLASS wcl;
    
    ZeroMemory(&wcl, sizeof(WNDCLASS));
    wcl.hInstance = hInstance;
    wcl.lpszClassName = KEYBOARDBOX_CLASS;
    wcl.lpfnWndProc = KeyboardBox_Procedure;
    wcl.style = CS_DBLCLKS;
    wcl.hCursor = LoadCursor(NULL, IDC_HAND);
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        
    return RegisterClass(&wcl);
}

/**************************************************************************
* KeyboardBox_Procedure : traitement des messages
*
**************************************************************************/

LRESULT CALLBACK KeyboardBox_Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT  rc;
    int id;
    if(message==WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hwnd, GWL_USERDATA,(LONG)lpcs->lpCreateParams);
    }
    TEditorData * pData = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    id = (int)GetWindowLong(hwnd, GWL_ID);
    
    switch (message)
    {
        case WM_CREATE:    
            break;
        
        case WM_LBUTTONDOWN:
            BYTE byNote;
            SetCapture(hwnd);
            byNote = KeyboardBox_GetNotePlaying(LOWORD(lParam), HIWORD(lParam), pData);            
            if(byNote<128)
            {
                g_midi.NoteOn((BYTE)pData->pPtrn->numTrack, byNote);
                g_midi.m_pSeq->SelectAllNote((int)byNote, pData);
                SendMessage(GetParent(hwnd), WM_USER, 0, 0); 
            }    
            break;
        
        case WM_LBUTTONUP:
            g_midi.NoteOff();
            ReleaseCapture();
            break;
        
        case WM_PAINT:
            KeyboardBox_Repaint(hwnd);
            break;
        
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}



/*************************************************************************
*
*
*************************************************************************/

void KeyboardBox_Repaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc, hdcMem;
    RECT rc;
    char szText[8];
    int i;
    
    TEditorData * p = (TEditorData*)GetWindowLong(hwnd, GWL_USERDATA);
    
    GetClientRect(hwnd, &rc);       
    hdc = BeginPaint(hwnd, &ps);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
    hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, g_hClavier);
    for(i=0; (p->yo + i*120<rc.bottom)&&(i<10); i++)
    {    
        BitBlt(hdc, 0, p->yo + i*120, 65, 120, hdcMem, 0, 0, SRCCOPY);
        wsprintf(szText, "c %i", 7-i);
        TextOut(hdc, 42, p->yo + 104 + i*120, szText, lstrlen(szText));
    }    
            
    DeleteDC(hdcMem);
    SelectObject(hdc, (HFONT)GetStockObject(SYSTEM_FONT));
    EndPaint(hwnd, &ps);
}

BYTE KeyboardBox_GetNotePlaying(int x, int y, TEditorData * p)
{
    const BYTE mat1[8] = {11,9,7,5,4,2,0,0};
    int octave;
    BYTE note;
    
    octave = 9 - (y - p->yo)/120;
    if(x>40)
        note = mat1[((y - p->yo)%120)/17];
    else
        note = (BYTE)(11 - ((y - p->yo)%120)/10);
    if(octave<0)
        return (BYTE)0xFF;
    return ((BYTE)(octave*12)+ note);
}    

