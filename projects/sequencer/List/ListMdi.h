#ifndef _MDI_LIST_HEADER_H_
#define _MDI_LIST_HEADER_H_

WORD    ListMdi_InitInstance();
HWND    ListMdi_Create(HWND);
LRESULT CALLBACK ListMdi_WndProc(HWND, UINT, WPARAM, LPARAM);
void    ListMdi_UpdateScrollBars(HWND);

#endif
