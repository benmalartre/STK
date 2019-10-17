#ifndef _DLGPATH_H_
#define _DLGPATH_H_

class CDlgPath
{
public:
    CDlgPath();
    ~CDlgPath();
    
    int Create(HWND, int, int);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgLButtonDown(HWND, WPARAM, LPARAM);
    BOOL MsgLButtonUp(HWND, WPARAM, LPARAM);
    BOOL MsgRButtonDown(HWND, WPARAM, LPARAM);
    BOOL MsgPaint(HWND);
    BOOL MsgSysCommand(HWND,WPARAM);
    
    HWND  m_hDlg;
    int   m_nSel;
    int   m_numTrack;
};

#endif
