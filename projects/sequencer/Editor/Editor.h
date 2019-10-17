#ifndef _MDI_EDITOR_H_
#define _MDI_EDITOR_H_


WORD    Editor_InitInstance();
HWND    Editor_Create(HWND);
LRESULT CALLBACK Editor_WndProc(HWND, UINT, WPARAM, LPARAM);
void    Editor_UpdateScrollBars(HWND);

#endif
