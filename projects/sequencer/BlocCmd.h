#ifndef _BLOCCOMMANDE_H_
#define _BLOCCOMMANDE_H_

class CBlocCmd
{
public:
    CBlocCmd();
    ~CBlocCmd();
    
    WORD InitInstance();
    BOOL Create(HWND);
    
private:
    static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
    
    LRESULT MsgCreate(HWND);
    LRESULT MsgTimer(HWND);
    LRESULT MsgCommand(HWND,WPARAM);
    LRESULT MsgLButtonDown(HWND,WPARAM,LPARAM);
    LRESULT MsgLButtonUp(HWND,WPARAM,LPARAM);
    LRESULT MsgLButtonDblClk(HWND,WPARAM,LPARAM);
    LRESULT MsgMouseMove(HWND,WPARAM,LPARAM);
    LRESULT MsgPaint(HWND);
    LRESULT MsgDestroy(HWND);
    
    BOOL    InterceptReturn(HWND, int, char*);
    LRESULT CmdTempo(HWND, WPARAM);
    LRESULT CmdNumSignature(HWND, WPARAM);
    LRESULT CmdDenSignature(HWND, WPARAM);
    
    HWND    m_hParent;
    char    m_szPointer[12];
    char    m_szLastLocator[12];
    char    m_szFirstLocator[12];
    char    m_szTempo[12];
    char    m_szSignature[12];
    DWORD   m_dwOldTime;
    int     m_nOldTempo;
    int     m_oldFirstLocator;
    int     m_oldLastLocator;
    BYTE    m_oldSignature[4];
    HBITMAP m_hbmBlocCmd;
    HBITMAP m_hbmBlocBtn;
    RECT    m_rect;
    POINT   m_ptCur;
    BOOL    m_bMove;
    int     m_nCommand;
    int     m_nDblClk;
    
    BOOL    m_bDrawStop;
    BOOL    m_bDrawPlay;
    BOOL    m_bDrawRewind;
    BOOL    m_bDrawLoop;
    BOOL    m_bDrawClick;
    BOOL    m_bDrawRecord;
    BOOL    m_bDrawNext;
    BOOL    m_bDrawBack;
};

#endif
