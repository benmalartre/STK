#ifndef _WIN_COLUMNTRACK_H_
#define _WIN_COLUMNTRACK_H_

#include "DlgPath.h"

#define IDC_COLUMNTRACK  230

class CWinColumnTrack;

struct TArrangeData
{
    CWinColumnTrack * pcolTrack;
    int               idRef;
    int               modeCursor;
};

class CWinColumnTrack
{
public:
    CWinColumnTrack();
    ~CWinColumnTrack();
    
    WORD InitInstance();
    BOOL Create(HWND);
    HWND GetHandle();
    HWND    m_hWnd;
    
private:
    static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
    
    LRESULT MsgCreate(HWND,UINT,WPARAM,LPARAM);
    LRESULT MsgTimer(HWND,UINT,WPARAM,LPARAM);
    LRESULT MsgLButtonDown(HWND,WPARAM,LPARAM);
    LRESULT MsgLButtonDblClk(HWND,WPARAM,LPARAM);
    LRESULT MsgPaint(HWND);
    LRESULT MsgDestroy(HWND);
    
    HBRUSH   m_hBrush;
    int      m_countTimeLed[16];
    int      m_led[16];
    CDlgPath m_cPath;
    char     m_szName[16][32];
};

#endif
