#ifndef _TOOLTIP_CLASS_HEADER_
#define _TOOLTIP_CLASS_HEADER_

class CToolTip
{
public:
    CToolTip();
    ~CToolTip();
    
    void SetTipText(HWND, int, int, char*);
    void UpdateTipText(char*);
    static WORD InitInstance(HINSTANCE);
    
private:
    static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
    LRESULT MsgCreate(HWND);
    LRESULT MsgTimer(HWND);
    LRESULT MsgPaint(HWND);
    LRESULT MsgDestroy(HWND);
    
    HWND  m_hWnd, m_hParent;
    POINT m_pt;
    char  m_szText[80];
    int   m_count;
};    

#endif
