#ifndef _DLGHELP_HEADER_H_
#define _DLGHELP_HEADER_H_

class CDlgHelp
{
public:
    CDlgHelp();
    ~CDlgHelp();
    
    int Create(HWND);
    
private:
    static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL MsgInitDialog(HWND, WPARAM, LPARAM);
    BOOL MsgCommand(HWND, WPARAM, LPARAM);
    BOOL MsgCtlColorStatic(HWND, WPARAM, LPARAM);
    BOOL MsgSysCommand(HWND,WPARAM);
    
    HWND        m_hDlg;
    int         m_index;
};

#endif
